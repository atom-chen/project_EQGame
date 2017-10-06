%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 任务
%%% @end
%%% Created :  1 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_task).

%% API
-export([start/1, handle_cast/2]).
-export([give/2, complete/2, is_exist/2, is_exist_complete_task/2, get_task_list/1, give_task_id/2]).

%% test
-export([test/0]).

-include("common_def.hrl").
-include("router.hrl").
-include("packet_def.hrl").
-include("enum_def.hrl").
-include("tplt_def.hrl").
-include("sys_msg.hrl").
-include("player_data.hrl").

%% 最大任务数量
-define(MAXTASKCOUNT, 30).

start(Account) ->
    [router:make_event_source(?msg_req_farm_task_list, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_farm_complete_task, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% handle call
%%%===================================================================


%%%===================================================================
%%% handle cast
%%%===================================================================
%% 请求获取任务列表
handle_cast({#msg{event=?msg_req_farm_task_list}, #req_task_list{}}, State) ->
    get_task_list(State),
    {noreply, State};
%% 请求完成任务
handle_cast({_Msg, #req_farm_complete_task{task_id=TaskId}}, State) ->
    {_Reply, NState} = complete(TaskId, State),
    {noreply, NState}.
%%%===================================================================
%%% API
%%%===================================================================
get_task_list(State) ->
    Account = farm:get_account(State),
    PlayerTask = farm:get_task(State),
    ActiveList = farm_task_data:get_active_list(PlayerTask),
    case length(ActiveList) > 0 of
	true ->
	    net_helper:farm2client(Account, #notify_farm_task_list{task_list=ActiveList});
	_ ->
	    net_helper:farm2client(Account, #notify_farm_task_list{task_list=[]})
    end.

is_exist_complete_task(TaskId, State) ->
    PlayerTask = farm:get_task(State),
    CompleteList = farm_task_data:get_complete_list(PlayerTask),
    is_exist(TaskId, 4, CompleteList).

is_exist(TaskId, State) ->
    Task = farm:get_task(State),
    ActiveList = farm_task_data:get_active_list(Task),
    is_exist(TaskId, 4, ActiveList).

%% 是否存在任务
-spec is_exist(integer(), integer(), list()) -> atom().
is_exist(TaskId, N, TaskList) ->
    case lists:keyfind(TaskId, N, TaskList) of
	false ->
	    false;
	_ ->
	    true
    end.

%% 任务是否已满
-spec is_full(list(), integer()) -> atom().
is_full([], Count) ->
    Count >= ?MAXTASKCOUNT;
is_full([_Task|TaskList], Count) ->
    is_full(TaskList, Count + 1).

%% 任务是否到期
-spec is_due_date(integer(), tuple()) -> atom().
is_due_date(TaskId, Task) ->
    TaskTplt = get_task_tplt(TaskId),
    CurrDate = datetime:localtime(),
    CreateDate = Task#player_task.create_date,
    TimeLimit = TaskTplt#farm_task_tplt.time_limit,
    case TimeLimit > 0 of %% 如果设置了时间限制
	true ->
	    case datetime:diff_time(CreateDate, CurrDate) + TimeLimit > 0 of
		true ->
		    true;
		_ ->
		    false
	    end;
	false ->
	    false
    end.

%% 接受任务, 
%% 返回值:
%% ok:接收任务成功
%% {fail, task_is_full}:该玩家所能接受的任务已满
%% {fail, doing_task}:该任务已经接受，并且正在进行中
%% {fail, completed_task}:该任务已经接受，并且已经完成
give(TaskId, State) ->
    Account = farm:get_account(State),
    case wallow:in_normal_time(State) of
	true ->
	    Mod = list_to_atom("farm_task_" ++ integer_to_list(TaskId)),
	    case Mod:can_give(State) of
		true ->
		    {Reply, NState} = do_give(Account, TaskId, Mod, State),
		    Task = farm:get_task(NState),
		    ActiveList = farm_task_data:get_active_list(Task),
		    net_helper:farm2client(Account, #notify_farm_task_list{task_list=ActiveList}),
		    {Reply, NState};
		{false, Reason} ->
		    farm_sys_msg:send(Account, ?msg_task_script, Reason),
		    {false, State}
	    end;
	false ->
	    farm_sys_msg:send(Account, ?err_wallow_farm_give_task),
	    {false, State}
    end.

-spec do_give(atom(), integer(), atom(), tuple()) -> any().
do_give(Account, TaskId, Mod, State) when is_atom(Account), is_integer(TaskId) ->
    TaskTplt = get_task_tplt(TaskId),
    PlayerTask = farm:get_task(State),
    IsRepeat = get_tplt_is_repeat(TaskTplt),
    case get_tplt_task_id(TaskTplt) > 0 of %% 模板表是否存在该任务
	true -> 
	    case IsRepeat == 1 of
		true ->
		    do_give(Account, TaskId, PlayerTask, TaskTplt, Mod, State); 
		false ->
		    case is_exist_complete_task(TaskId, State) of
			true ->
			    farm_sys_msg:send(Account, ?err_farm_task_not_repeat_get),
			    {fasle, State};
			false ->
			    do_give(Account, TaskId, PlayerTask, TaskTplt, Mod, State)
		    end
	    end;
	false ->
	    {false, State}
    end.
do_give(Account, TaskId, PlayerTask, TaskTplt, Mod, State) ->
    ActiveList = farm_task_data:get_active_list(PlayerTask),
    case is_exist(TaskId, 4, ActiveList) of
	true ->
	    farm_sys_msg:send(Account, ?err_farm_task_is_exist),
	    {false, State};
	false -> %% 不存在该任务则创建新的任务
	    case is_full(ActiveList, 0) of
		true -> %% 任务已满
		    farm_sys_msg:send(Account, ?err_farm_task_is_full),
		    {false, State};
		false -> %% 创建新的任务
		    NState = Mod:do_give(State),
		    Id = guid:make(?st_task),
		    Now = datetime:localtime(),
		    Task = #player_task{id=Id, account=Account, task_id=TaskId, step_index=0,
					track = 0, status=?ACTIVE,
					create_date=Now},
		    NPlayerTask = farm_task_data:set_active_list(PlayerTask, [Task|ActiveList]),
		    db_common:write(NPlayerTask),
		    do_notify_task(Account, ?msg_task_give, TaskTplt),
		    net_helper:farm2client(Account, #notify_give_farm_task{task=Task}),
		    {true, farm:set_task(NPlayerTask, NState)}
	    end
    end.

merge_item([0], [0], _MergeList) ->
    [];
merge_item([], [], MergeList) ->
    MergeList;
merge_item([ItemId|ItemList], [Count|CountList], MergeList) ->
    NMergeList = merge_item_count(ItemId, Count, []) ++ MergeList,
    merge_item(ItemList, CountList, NMergeList).

merge_item_count(_ItemId, 0, ItemList) ->
    ItemList;
merge_item_count(ItemId, Count, ItemList) ->
    Item = item:make_item(ItemId),
    NItemList = [Item | ItemList],
    merge_item_count(ItemId, Count - 1, NItemList).

%% 完成任务
complete(TaskId, State) ->
    Account = farm:get_account(State),
    case wallow:in_normal_time(State) of
	true ->
	    Mod = list_to_atom("farm_task_" ++ integer_to_list(TaskId)),
	    case Mod:can_complete(State) of
		true ->
		    do_complete(Account, TaskId, Mod, State);
		{false, Reason} ->
		    farm_sys_msg:send(Account, ?msg_task_script, Reason),
		    {false, State}
	    end;
	false ->
	    farm_sys_msg:send(Account, ?err_wallow_farm_complete_task),
	    {false, State}
    end.

-spec do_complete(atom(), integer(), atom(), tuple()) -> tuple().
do_complete(Account, TaskId, Mod, State) ->
    TaskTplt = get_task_tplt(TaskId),
    PlayerTask = farm:get_task(State),
    ActiveList = farm_task_data:get_active_list(PlayerTask),
    case lists:keyfind(TaskId, 4, ActiveList) of
	false ->
	    farm_sys_msg:send(Account, ?err_farm_not_exist_task),
	    {false, State};
	Task ->
	    case is_due_date(TaskId, Task) of %% 任务是否到期
		true ->
		    farm_sys_msg:send(Account, ?err_farm_task_has_expried),
		    {false, State};
		false ->
		    do_complete(Account, TaskTplt, Task, Mod, State)
	    end
    end.
do_complete(Account, TaskTplt, Task, Mod, State) ->
    NPlayerTask = farm:get_task(State),
    ActiveList = farm_task_data:get_active_list(NPlayerTask),
    CompleteList = farm_task_data:get_complete_list(NPlayerTask),
    NCompleteList = [Task | CompleteList],
    NNPlayerTask = farm_task_data:set_complete_list(NPlayerTask, NCompleteList),

    NActiveList = lists:delete(Task, ActiveList),
    NNNPlayerTask = farm_task_data:set_active_list(NNPlayerTask, NActiveList),

    NState = farm:set_task(NNNPlayerTask, State),
    
    %% 奖励玩家金币
    Farm = player_farm:get_farm_data(Account),
    Coin = player_farm:get_coin(Farm),
    NCoin = Coin + TaskTplt#farm_task_tplt.reward_game_coin,
    NFarm = player_farm:set_coin(NCoin, Farm),

    %% 奖励玩家经验
    Exp = player_farm:get_exp(NFarm),
    NExp = Exp + TaskTplt#farm_task_tplt.exp,
    NNFarm = player_farm:add_exp(NFarm, NExp),

    %% 奖励玩家物品
    ItemList = merge_item(TaskTplt#farm_task_tplt.fixed_reward_items, 
			  TaskTplt#farm_task_tplt.fixed_reward_items_count,
			  []),
    Depot = farm_depot:read(Account),
    NDepot = farm_depot:add_item(Depot, ItemList),

    db_farm:write(NNFarm, NDepot),
    db_common:write(NNNPlayerTask),
    farm_coin:send_coin(Account, NCoin),
    player_farm:notify_farm_data(Account, Account, NNFarm),
    net_helper:farm2client(Account, #notify_task_complete{id=Task#player_task.id}),
    NNState = Mod:do_complete(NState),
    {true, NNState}.

give_task_id(TaskId, State) ->
    Tasks = farm:get_task(State),
    ActiveList = farm_task_data:get_active_list(Tasks),
    Task = lists:keyfind(TaskId, #player_task.task_id, ActiveList),
    Task#player_task.id.

%%%===================================================================
%%% Internal functions
%%%===================================================================
do_notify_task(Account, Type, TaskTplt) ->
    farm_sys_msg:send(Account, Type, [TaskTplt#farm_task_tplt.title]).

%% do_notify_reward_items(Account, TaskTplt) ->
%%     ItemList = TaskTplt#farm_task_tplt.fixed_reward_items,
%%     ItemCount = TaskTplt#farm_task_tplt.fixed_reward_items_count,
%%     do_notify_reward_items(Account, ItemList, ItemCount).

%% do_notify_reward_items(_Account, [], []) ->
%%     [];
%% do_notify_reward_items(Account, [Item|ItemList], [Count|CountList]) ->
%%     case Item > 0 of
%% 	true ->
%% 	    #item_tplt{name=Name} = tplt:get_data2(item_tplt, Item),
%% 	    farm_sys_msg:send(Account, ?msg_task_reward_item, [Name, Count]),
%% 	    do_notify_reward_items(Account, ItemList, CountList);
%% 	false ->
%% 	    do_notify_reward_items(Account, ItemList, CountList)
%%     end.

%% do_notify_reward_coin(Account, TaskTplt) ->
%%     do_notify_reward_game_coin(Account, TaskTplt),
%%     do_notify_reward_eq_coin(Account, TaskTplt).

%% do_notify_reward_game_coin(Account, TaskTplt) ->
%%     Coin = TaskTplt#farm_task_tplt.reward_game_coin,
%%     case Coin > 0 of
%% 	true ->
%% 	    farm_sys_msg:send(Account, ?msg_task_reward_game_coin, [Coin]);
%% 	false ->
%% 	    ok
%%     end.    

%% do_notify_reward_eq_coin(Account, TaskTplt) ->
%%     Coin = TaskTplt#farm_task_tplt.reward_eq_coin,
%%     case Coin > 0 of
%% 	true ->
%% 	    farm_sys_msg:send(Account, ?msg_task_reward_eq_coin, [Coin]);
%% 	false ->
%% 	    ok
%%     end.    

%% 获取任务Id
-spec get_task_tplt(integer()) -> tuple().
get_task_tplt(TaskId) ->
    tplt:get_data(farm_task_tplt, TaskId).

get_tplt_task_id(TaskTplt) ->
   TaskTplt#farm_task_tplt.task_id.

get_tplt_is_repeat(TaskTplt) ->
    TaskTplt#farm_task_tplt.is_repeat.


%%%===================================================================
%%% Test case
%%%===================================================================
test() ->
    router:cast(ddd, ?msg_req_task_list, #req_task_list{}).
