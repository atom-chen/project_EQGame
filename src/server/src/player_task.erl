%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 任务
%%% @end
%%% Created :  1 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_task).

%% API
-export([start/1, handle_call/3, handle_cast/2]).
-export([get_flag/2, set_flag/3, get_max_task_count/0]).
-export([give/2, complete/3, is_exist/2, is_exist_complete_task/2, get_all_once_task/1,
	 get_task_list/1, give_task_id/2]).

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
-define(show, 1).
-define(hidden, 0).
% 任务可放弃
-define(can_give_up, 1).

start(Account) ->
    [router:make_event_source(?msg_req_task_list, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_track_task, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_give_up_task, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_cancel_track_task, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_get_track_list, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_complete_task, Account, {self(), ?MODULE}),
     router:make_event_target(give_task, Account, {self(), ?MODULE}),
     router:make_event_target(complete_task, Account, {self(), ?MODULE}),
     router:make_event_target(give_chain_task, Account, {self(), ?MODULE}),
     router:make_event_target(complete_chain_task, Account, {self(), ?MODULE}),
     router:make_event_target(task_is_exist, Account, {self(), ?MODULE}),
     router:make_event_target(task_is_full, Account, {self(), ?MODULE}),
     router:make_event_target(get_task_flag, Account, {self(), ?MODULE}),
     router:make_event_target(set_task_flag, Account, {self(), ?MODULE}),
     router:make_event_target(hand_task_props, Account, {self(), ?MODULE}),
     router:make_event_target(task_is_complete, Account, {self(), ?MODULE}),
     router:make_event_target(is_exist_complete_task, Account, {self(), ?MODULE}),
     router:make_event_target(get_task_list, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% handle call
%%%===================================================================
%% 任务是否存在
handle_call({#msg{event=task_is_exist}, TaskId}, _From, State) ->
    Task = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(Task),
    IsExist = is_exist(TaskId, 4, ActiveList),
    {reply, IsExist, State};
%% 任务是否已满
handle_call(#msg{event=task_is_full}, _From, State) ->
    Task = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(Task),
    IsFull = is_full(ActiveList, 0),
    {reply, IsFull, State};
%% 获取任务列表
handle_call(#msg{event=get_task_list}, _From, State) ->
    Task = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(Task),
    {reply, ActiveList, State};
%% 接受任务
handle_call({#msg{event=give_task}, TaskId}, _From, State) ->
    {Reply, NNState} = give(TaskId, State),
    {reply, Reply, NNState};
%% 接受一个链式任务
handle_call({#msg{event=give_chain_task}, TaskId}, _From, State) ->
    {Reply, NState} = give_chain_task(TaskId, State),
    {reply, Reply, NState};
handle_call({#msg{event=complete_chain_task}, {ChainTaskId, ItemId}}, _From, State) ->
    {Reply, NState} = complete_chain_task(ChainTaskId, ItemId, State),
    {reply, Reply, NState};
%% 查看任务标记
handle_call({#msg{event=get_task_flag}, TaskId}, _From, State) ->
    StepIndex = get_flag(TaskId, State),
    {reply, StepIndex, State};
%% 改变任务标记
handle_call({#msg{event=set_task_flag}, {TaskId, StepIndex}}, _From, State) ->
    Task = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(Task),
    NActiveList = set_flag(TaskId, StepIndex, ActiveList),
    NTask = player_task_data:set_active_list(Task, NActiveList),
    Ret = db_common:write(NTask),
    {reply, Ret, State};
%% 上交道具
handle_call({#msg{event=hand_task_props}, TaskId}, _From, State) ->
    Account = player_data:get_account(State),
    {Ret, NPlayerData} = 
    case hand_props(Account, TaskId, State) of
	{true, PlayerData} ->
	    {true, PlayerData};
	{false, not_enough_props, _PropList} ->
	    {{false, ?err_task_not_enough_props}, State}
    end,
    {reply, Ret, NPlayerData};
%% 完成任务
handle_call({#msg{event=complete_task}, {TaskId, ItemId}}, _From, State) ->
    {Reply, NNState} = complete(TaskId, ItemId, State),
    {reply, Reply, NNState};
%% 任务是否完成
handle_call({#msg{event=task_is_complete}, TaskId}, _From, State) ->
    PlayerTask = player_data:get_task(State),
    CompleteList = player_task_data:get_complete_list(PlayerTask),
    IsComplete = is_exist(TaskId, 4, CompleteList),
    {reply, IsComplete, State};
%% 是否存在已经完成的任务
handle_call({#msg{event=is_exist_complete_task}, TaskId}, _From, State) ->
    IsComplete = is_exist_complete_task(TaskId, State),
    {reply, IsComplete, State}.

%%%===================================================================
%%% handle cast
%%%===================================================================
%% 请求获取任务列表
handle_cast({#msg{event=?msg_req_task_list}, #req_task_list{}}, State) ->
    NState = get_task_list(State),
    {noreply, NState};
%% 请求跟踪任务
handle_cast({#msg{event=?msg_req_track_task}, #req_track_task{id=Id}}, State) ->
    Account = player_data:get_account(State),
    PlayerTask = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(PlayerTask),
    case lists:keyfind(Id, 2, ActiveList) of
	false ->
	    false;
	Task ->
	    NTask = Task#player_task{track = 1},
	    NActiveList = lists:keyreplace(Id, 2, ActiveList, NTask),
	    NPlayerTask = player_task_data:set_active_list(PlayerTask, NActiveList),
	    db_common:write(NPlayerTask),
	    net_helper:send2client(Account, #notify_track_task{id=Id})
    end,
    {noreply, State};
%% 取消跟踪任务
handle_cast({#msg{event=?msg_req_cancel_track_task}, #req_cancel_track_task{id=Id}}, State) ->
    Account = player_data:get_account(State),
    PlayerTask = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(PlayerTask),
    case lists:keyfind(Id, 2, ActiveList) of
	false ->
	    false;
	Task ->
	    NTask = Task#player_task{track = 0},
	    NActiveList = lists:keyreplace(Id, 2, ActiveList, NTask),
	    NPlayerTask = player_task_data:set_active_list(PlayerTask, NActiveList),
	    db_common:write(NPlayerTask),
	    net_helper:send2client(Account, #notify_cancel_track_task{id=Id})
    end,
    {noreply, State};
%% 请求完成任务
handle_cast({_Msg, #req_complete_task{task_id=TaskId}}, State) ->
    {_Reply, NState} = complete(TaskId, 0, State),
    {noreply, NState};
%% 请求放弃任务
handle_cast({#msg{event=?msg_req_give_up_task}, #req_give_up_task{id=Id}}, State) ->
    Account = player_data:get_account(State),
    PlayerTask = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(PlayerTask),
    NState = 
	case lists:keyfind(Id, 2, ActiveList) of
	    false ->
		State;
	    Task ->
		TaskId = Task#player_task.task_id,
		TaskTplt = get_task_tplt(TaskId),
		case TaskTplt#player_task_tplt.is_give_up == ?can_give_up of
		    true ->
			NTask = Task#player_task{status=?GIVEUP},
			NActiveList = lists:delete(Task, ActiveList),
			NPlayerTask = player_task_data:set_active_list(PlayerTask, NActiveList),
			CancelList = player_task_data:get_cancel_list(NPlayerTask),
			NCancelList = [NTask | CancelList],
			NNPlayerTask = player_task_data:set_cancel_list(NPlayerTask, NCancelList),
			db_common:write(NNPlayerTask),
			net_helper:send2client(Account, #notify_give_up_task{id=Id}),
			player_data:set_task(NNPlayerTask, State);
		    false ->
			sys_msg:send(Account, ?err_task_not_give_up),
			State
		end
	end,
    {noreply, NState};
%% 请求获取任务列表
handle_cast({#msg{event=?msg_req_get_track_list}, #req_get_track_list{}}, State) ->
    Account = player_data:get_account(State),
    PlayerTask = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(PlayerTask),
    Fun = 
	fun(#player_task{track=Track}) -> 
		Track == 1
	end,
    NActiveList = lists:filter(Fun, ActiveList),
    net_helper:send2client(Account, #notify_get_track_list{task_list=NActiveList}),
    {noreply, State}.
%%%===================================================================
%%% API
%%%===================================================================
get_task_list(State) ->
    Account = player_data:get_account(State),
    PlayerTask = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(PlayerTask),
    FirstOnceTask = 15000,
    case is_exist_complete_task(FirstOnceTask, State) or is_exist(FirstOnceTask, State) of
	true -> 
	    net_helper:send2client(Account, #notify_task_list{task_list=ActiveList}),
	    State;
	_ -> 
	    NActiveList = get_all_once_task(Account) ++ ActiveList,
	    NPlayerTask = player_task_data:set_active_list(PlayerTask, NActiveList),
	    db_common:write(NPlayerTask),
	    net_helper:send2client(Account, #notify_task_list{task_list=NActiveList}),
	    player_data:set_task(NPlayerTask, State)
    end.


get_all_once_task(Account) ->
    List = tplt:get_all_data(player_task_tplt),
    [make_task(Account, TaskId) || #player_task_tplt{task_id=TaskId, kind=Kind} <- List, 
	       (Kind =:= ?tk_once) or (Kind =:= ?tk_rate_once)].


is_exist_complete_task(TaskId, State) ->
    PlayerTask = player_data:get_task(State),
    CompleteList = player_task_data:get_complete_list(PlayerTask),
    is_exist(TaskId, 4, CompleteList).

is_exist(TaskId, State) ->
    Task = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(Task),
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
is_full([Task|TaskList], Count) ->
    TaskId = Task#player_task.task_id,
    TaskTplt = get_task_tplt(TaskId),
    case TaskTplt#player_task_tplt.is_show == ?show of
	true ->
	    is_full(TaskList, Count + 1);
	false ->
	    is_full(TaskList, Count)
	end.

%% 上交道具
-spec hand_props(atom(), integer(), tuple()) -> tuple().
hand_props(Account, TaskId, State) when is_atom(Account), is_integer(TaskId), is_tuple(State) ->
    TaskTplt = get_task_tplt(TaskId),
    RequireItem = TaskTplt#player_task_tplt.require_items,
    RequireItemCount = TaskTplt#player_task_tplt.require_items_count,
    RequireItemList = player_task_item:get_item_list(RequireItem, RequireItemCount, []),
    case can_do_hand_props(RequireItem) of
	true ->
	    do_hand_props(RequireItem, RequireItemList, RequireItemCount, State);
	false ->
	    {true, State}
    end.

can_do_hand_props([Item|_RequireItem]) ->
    Item /= 0.

do_hand_props(RequireItem, RequireItemList, RequireItemCount, State) ->
    Bag = player_data:get_pack(?pt_bag, State),
    case player_pack:check_item_count(RequireItemList, Bag, []) of
	true ->
	    ItemList = player_task_item:get_item_list_1(RequireItem, RequireItemCount, []),
	    NState = player_pack:del_item(ItemList, State),
	    {true, NState};
	{false, ErrorList} ->
	    PropList = create_not_enough_props(ErrorList, ""),
	    {false, not_enough_props, PropList}
    end.

create_not_enough_props([], PropList) ->
    PropList;
create_not_enough_props([{ItemId, Count}|ErrorList], PropList) ->
    #item_tplt{name=Name}=tplt:get_data(item_tplt, ItemId),
    NName = binary_to_list(Name),
    NPropList = PropList ++ "," ++ NName ++ "x" ++ integer_to_list(Count),
    create_not_enough_props(ErrorList, NPropList).

%% 任务是否到期
-spec is_due_date(integer(), tuple()) -> atom().
is_due_date(TaskId, Task) ->
    TaskTplt = get_task_tplt(TaskId),
    CurrDate = datetime:localtime(),
    CreateDate = Task#player_task.create_date,
    TimeLimit = TaskTplt#player_task_tplt.time_limit,
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
    Account = player_data:get_account(State),
    case wallow:in_normal_time(State) of
	true ->
	    Mod = list_to_atom("task_" ++ integer_to_list(TaskId)),
	    case Mod:can_give(State) of
		true ->
		    {Reply, NState} = do_give(Account, TaskId, Mod, State),
		    {Reply, NState};
		{false, Reason} ->
		    sys_msg:send(Account, ?msg_task_script, Reason),
		    {false, State}
	    end;
	false ->
	    sys_msg:send(Account, ?err_wallow_give_task),
	    {false, State}
    end.

-spec do_give(atom(), integer(), atom(), tuple()) -> any().
do_give(Account, TaskId, Mod, State) when is_atom(Account), is_integer(TaskId) ->
    TaskTplt = get_task_tplt(TaskId),
    PlayerTask = player_data:get_task(State),
    IsRepeat = get_tplt_is_repeat(TaskTplt),
    case get_tplt_task_id(TaskTplt) > 0 of %% 模板表是否存在该任务
	true -> 
	    case IsRepeat == 1 of
		true ->
		    do_give(Account, TaskId, PlayerTask, TaskTplt, Mod, State); 
		false ->
		    case is_exist_complete_task(TaskId, State) of
			true ->
			    sys_msg:send(Account, ?err_task_not_repeat_get),
			    {fasle, State};
			false ->
			    do_give(Account, TaskId, PlayerTask, TaskTplt, Mod, State)
		    end
	    end;
	false ->
	    {false, State}
    end.
do_give(Account, TaskId, PlayerTask, TaskTplt, Mod, State) ->
    ActiveList = player_task_data:get_active_list(PlayerTask),
    case is_exist(TaskId, 4, ActiveList) of
	true ->
	    sys_msg:send(Account, ?err_task_is_exist),
	    {false, State};
	false -> %% 不存在该任务则创建新的任务
	    case ((TaskTplt#player_task_tplt.is_show == ?show) and is_full(ActiveList, 0)) of
		true -> %% 任务已满
		    sys_msg:send(Account, ?err_task_is_full),
		    {false, State};
		false -> %% 创建新的任务
		    Props = TaskTplt#player_task_tplt.props,
		    PropsCount = TaskTplt#player_task_tplt.props_count,

		    Pack = player_data:get_pack(?pt_bag, State),
		    case can_add_item(Props, PropsCount, Pack) of
			true ->
			    NState = Mod:do_give(State),
			    Items = merge_item(Props, PropsCount, []),
			    case player_pack:add_item2(Account, Items, NState) of
				{ok, NNState} ->
				    Task = make_task(Account, TaskId),
				    NTask = daily_task:init(Task, TaskTplt),

				    NPlayerTask = player_task_data:set_active_list(PlayerTask, [NTask|ActiveList]),
				    db_common:write(NPlayerTask),
				    do_notify_task(Account, ?msg_task_give, TaskTplt),
				    net_helper:send2client(Account, #notify_give_task{task=Task}),

				    {true, player_data:set_task(NPlayerTask, NNState)};
				{error, _} ->
				    sys_msg:send(Account, ?msg_task_bag_is_full),
				    {false, State}
			    end;
			false ->
			    sys_msg:send(Account, ?msg_task_bag_is_full),
			    {false, State}
		    end
	    end
    end.

make_task(Account, TaskId) ->
    Id = guid:make(?st_task),
    Now = datetime:localtime(),
    Task = #player_task{id=Id, account=Account, task_id=TaskId, step_index=0,
			track = 0, status=?ACTIVE,
			create_date=Now},
    Task.


can_add_item([0], [0], _Pack) ->
    true;
can_add_item([], [], _Pack) ->
    true;
can_add_item([ItemId|ItemList], [Count|CountList], Pack) ->
    case pack:find_available_grid(ItemId, Count, Pack) of
	full ->
	    false;
	_Index ->
	    can_add_item(ItemList, CountList, Pack)
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

give_chain_task(TaskId, State) ->
    Account = player_data:get_account(State),
    ChainTaskTplt = get_chain_task_tplt(TaskId),
    Task = player_data:get_task(State),
    ActiveChainList = player_task_data:get_active_chain_list(Task),
    IsRepeat = get_chain_task_tplt_is_repeat(ChainTaskTplt),
    do_give_chain_task(Account, TaskId, IsRepeat, ActiveChainList, Task, ChainTaskTplt, State).

do_give_chain_task(Account, TaskId, IsRepeat, ActiveChainList, Task, ChainTaskTplt, State) ->
    case IsRepeat == 1 of %% 验证是否可重复接链式任务
	true ->
	    do_give_chain_task(Account, TaskId, ActiveChainList, Task, ChainTaskTplt, State);
	false ->
	    CompleteChainList = player_task_data:get_complete_chain_list(Task),
	    case is_exist(TaskId, 3, CompleteChainList) of %% 是否存在已经完成的链式任务
		true ->
		    sys_msg:send(Account, ?err_task_not_repeat_get),
		    {false, State};
		false ->
		    do_give_chain_task(Account, TaskId, ActiveChainList, Task, ChainTaskTplt, State)
	    end
    end.
do_give_chain_task(Account, ChainTaskId, ActiveChainList, Task, ChainTaskTplt, State) ->
    case is_exist(ChainTaskId, 3, ActiveChainList) of
	true ->
	    sys_msg:send(Account, ?err_task_is_exist),
	    {false, State};
	false ->
	    [OneTaskId | TaskList] = get_chain_task_tplt_tasks(ChainTaskTplt),
	    case give(OneTaskId, State) of
		{true, NState} ->
		    ChainTask = create_give_chain_task(ChainTaskId, OneTaskId, TaskList),
		    NActiveChainList = [ChainTask | ActiveChainList],
		    NTask = player_task_data:set_active_chain_list(Task, NActiveChainList),
		    db_common:write(NTask),
		    {true, player_data:set_task(NTask, NState)};
		{false, NState} ->
		    {false, NState}
	    end
    end.

%% 创建一个链式任务
-spec create_give_chain_task(integer(), integer(), list()) -> tuple().
create_give_chain_task(ChainTaskId, OneTaskId, TaskList) ->
    InstId = guid:make(?st_task),
    Now = datetime:localtime(),
    chain_task_data:new(InstId, ChainTaskId, TaskList, OneTaskId, Now).

complete_chain_task(ChainTaskId, ItemId, State) ->
    Account = player_data:get_account(State),
    PlayerTask = player_data:get_task(State),
    ActiveChainList = player_task_data:get_active_chain_list(PlayerTask),
    case lists:keyfind(ChainTaskId, 3, ActiveChainList) of
	false ->
	    sys_msg:send(Account, ?err_not_exist_task),
	    {false, State};
	ChainTask ->
	    CurrTaskId = chain_task_data:get_curr_task_id(ChainTask),
	    case complete(CurrTaskId, ItemId, State) of
		{true, NState} ->
		    case chain_task_data:get_task_list(ChainTask) of
			[] ->
			    NActiveChainList = lists:delete(ChainTask, ActiveChainList),
			    NPlayerTask = player_task_data:set_active_chain_list(PlayerTask, NActiveChainList),
			    NNPlayerTask = player_task_data:set_complete_chain_list(NPlayerTask, ChainTask),
			    db_common:write(NNPlayerTask),
			    {true, player_data:set_task(NNPlayerTask, NState)};
			[OneTaskId | TaskList] ->
			    case give(OneTaskId, State) of
				{true, NState} ->
				    NChainTask = chain_task_data:set_curr_task_id(ChainTask, OneTaskId),
				    NNChainTask = chain_task_data:set_task_list(NChainTask, TaskList),
				    NActiveChainList = lists:keyreplace(ChainTaskId, 3, ActiveChainList, NNChainTask),
				    NPlayerTask = player_task_data:set_active_chain_list(PlayerTask, NActiveChainList),
				    db_common:write(NPlayerTask),
				    {true, player_data:set_task(NPlayerTask, NState)};
				{false, NState} ->
				    {false, NState}
			    end
		    end;
		{false, State} ->
		    {false, State}
	    end
    end.

%% 获取当前任务的步骤的索引
-spec get_flag(integer(), list()|tuple()) -> integer().
get_flag(TaskId, State) when is_record(State, player_data) ->
    Task = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(Task),
    get_flag(TaskId, ActiveList);
get_flag(TaskId, TaskList) when is_list(TaskList) ->
    case lists:keyfind(TaskId, 4, TaskList) of
	false ->
	    0;
	Task ->
	    Task#player_task.step_index
    end.


%% 改变当前任务的步骤的索引
-spec set_flag(integer(), integer(), list()|tuple()) -> any().
set_flag(TaskId, Flag, State) when is_record(State, player_data) ->
    Task = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(Task),
    NActiveList = set_flag(TaskId, Flag, ActiveList),
    NTask = player_task_data:set_active_list(Task, NActiveList),
    db_common:write(NTask),
    player_data:set_task(NTask, State);
set_flag(TaskId, Flag, TaskList) when is_list(TaskList) ->
    case lists:keyfind(TaskId, 4, TaskList) of
	false ->
	    0;
	Task ->
	    NTask = Task#player_task{step_index=Flag},
	    lists:keyreplace(TaskId, 4, TaskList, NTask)
    end.

%% 完成任务
complete(TaskId, ItemId, State) ->
    Account = player_data:get_account(State),
    case wallow:in_normal_time(State) of
	true ->
	    Mod = list_to_atom("task_" ++ integer_to_list(TaskId)),
	    case Mod:can_complete(State) of
		true ->
		    do_complete(Account, TaskId, ItemId, Mod, State);
		{false, Reason} ->
		    sys_msg:send(Account, ?msg_task_script, Reason),
		    {false, State}
	    end;
	false ->
	    sys_msg:send(Account, ?err_wallow_complete_task),
	    {false, State}
    end.

-spec do_complete(atom(), integer(), integer(), atom(), tuple()) -> tuple().
do_complete(Account, TaskId, ItemId, Mod, State) ->
    TaskTplt = get_task_tplt(TaskId),
    PlayerTask = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(PlayerTask),
    case lists:keyfind(TaskId, 4, ActiveList) of
	false ->
	    sys_msg:send(Account, ?err_not_exist_task),
	    {false, State};
	Task ->
	    case is_due_date(TaskId, Task) of %% 任务是否到期
		true ->
		    sys_msg:send(Account, ?err_task_has_expried),
		    {false, State};
		false ->
		    do_complete(Account, TaskId, ItemId, TaskTplt, Task, Mod, State)
	    end
    end.
do_complete(Account, TaskId, ItemId, TaskTplt, Task, Mod, State) ->
    %% 判断动作是否完成
    case daily_task:can_finish_actions(TaskTplt, Task) of
	false -> 
	    sys_msg:send(Account, ?err_task_not_contain_item),
	    {false, State};
	true -> 	
	    case hand_props(Account, TaskId, State) of %% 上交任务道具
		{true, NState} ->
		    case player_reward:pay(Account, TaskTplt, ItemId, NState) of %% 奖励玩家物品
			{true, NNState} ->
			    %% 先通知完成
			    do_notify_task(Account, ?msg_task_complete, TaskTplt),
			    do_notify_reward_items(Account, TaskTplt),
			    do_notify_reward_coin(Account, TaskTplt),
			    net_helper:send2client(Account, #notify_task_complete{id=Task#player_task.id}),
			    %% 再做接收新任务之类的处理
			    NNNState = Mod:do_complete(NNState),
			    PropertyId = get_tplt_property_id(TaskTplt),
			    case PropertyId > 0 of
				true ->
				    player_property_by_action:update(?msg_player_property_by_task, 
								     PropertyId, 
								     NNNState);
				false ->
				    ok
			    end,
			    NPlayerTask = player_data:get_task(NNNState),
			    ActiveList = player_task_data:get_active_list(NPlayerTask),
			    CompleteList = player_task_data:get_complete_list(NPlayerTask),
			    
			    NNPlayerTask = 
				case TaskTplt#player_task_tplt.kind of
				    ?tk_daily -> %% 如果是每日任务，就不加到完成列表了
					daily_task:on_complete_task(TaskId, NPlayerTask);
				    _ -> %% 不是每日任务，加到完成列表
					player_task_data:set_complete_list(NPlayerTask, [Task | CompleteList])
				end,
			    
			    NActiveList = lists:delete(Task, ActiveList),
			    NNNPlayerTask = player_task_data:set_active_list(NNPlayerTask, NActiveList),
			    
			    PlayerCoin = player_data:get_coin(NNNState),
			    PlayerPack = player_pack:make_package2(Account, ?pt_bag, NNNState),
			    db_memdisc:write(PlayerCoin),
			    db_common:write([NNNPlayerTask, PlayerPack]),
			    player_coin:send_coin(PlayerCoin),
			    {true, player_data:set_task(NNNPlayerTask, NNNState)};
			{false, not_enough_position} -> %% 包裹没有足够的空格
			    sys_msg:send(Account, ?err_bag_full),
			    {false, State};
			{false, not_contain_item} -> %% 包裹中不存在该任务所需要的物品
			    sys_msg:send(Account, ?err_task_not_contain_item),
			    {false, State}
		    end;
		{false, not_enough_props, PropList} ->
		    sys_msg:send(Account, ?err_task_not_enough_props, [PropList]),
		    {false, State}
	    end
    end.

give_task_id(TaskId, State) ->
    Tasks = player_data:get_task(State),
    ActiveList = player_task_data:get_active_list(Tasks),
    Task = lists:keyfind(TaskId, #player_task.task_id, ActiveList),
    Task#player_task.id.

%%%===================================================================
%%% Internal functions
%%%===================================================================
do_notify_task(Account, Type, TaskTplt) ->
    case TaskTplt#player_task_tplt.is_show == 1 of
	true ->
	    sys_msg:send(Account, Type, [TaskTplt#player_task_tplt.title]);
	false ->
	    ok
    end.

do_notify_reward_items(Account, TaskTplt) ->
    ItemList = TaskTplt#player_task_tplt.fixed_reward_items,
    ItemCount = TaskTplt#player_task_tplt.fixed_reward_items_count,
    do_notify_reward_items(Account, ItemList, ItemCount).

do_notify_reward_items(_Account, [], []) ->
    [];
do_notify_reward_items(Account, [Item|ItemList], [Count|CountList]) ->
    case Item > 0 of
	true ->
	    #item_tplt{name=Name} = tplt:get_data2(item_tplt, Item),
	    sys_msg:send(Account, ?msg_task_reward_item, [Name, Count]),
	    do_notify_reward_items(Account, ItemList, CountList);
	false ->
	    do_notify_reward_items(Account, ItemList, CountList)
    end.

do_notify_reward_coin(Account, TaskTplt) ->
    do_notify_reward_game_coin(Account, TaskTplt),
    do_notify_reward_eq_coin(Account, TaskTplt).

do_notify_reward_game_coin(Account, TaskTplt) ->
    Coin = TaskTplt#player_task_tplt.reward_game_coin,
    case Coin > 0 of
	true ->
	    sys_msg:send(Account, ?msg_task_reward_game_coin, [Coin]);
	false ->
	    ok
    end.    

do_notify_reward_eq_coin(Account, TaskTplt) ->
    Coin = TaskTplt#player_task_tplt.reward_eq_coin,
    case Coin > 0 of
	true ->
	    sys_msg:send(Account, ?msg_task_reward_eq_coin, [Coin]);
	false ->
	    ok
    end.    


get_chain_task_tplt(TaskId) ->
    tplt:get_data(chain_task_tplt, TaskId).

get_chain_task_tplt_is_repeat(#chain_task_tplt{is_repeat=IsRepeat}) ->
    IsRepeat.

get_chain_task_tplt_tasks(#chain_task_tplt{tasks=Tasks}) ->
    Tasks.

%% 获取任务Id
-spec get_task_tplt(integer()) -> tuple().
get_task_tplt(TaskId) ->
    tplt:get_data(player_task_tplt, TaskId).

get_tplt_task_id(TaskTplt) ->
   TaskTplt#player_task_tplt.task_id.

get_tplt_property_id(TaskTplt) ->
    TaskTplt#player_task_tplt.property_id.

get_tplt_is_repeat(TaskTplt) ->
    TaskTplt#player_task_tplt.is_repeat.

get_max_task_count() ->
    ?MAXTASKCOUNT.


%%%===================================================================
%%% Test case
%%%===================================================================
test() ->
    router:cast(ddd, ?msg_req_task_list, #req_task_list{}).
