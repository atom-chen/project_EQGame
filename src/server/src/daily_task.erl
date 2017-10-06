%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  每日任务
%%% @end
%%% Created :  8 Oct 2011 by hongjx <hongjx@35info.cn>

-module(daily_task).

-export([update_today/1, init/2, on_complete_task/2, inc_action_amount/2, can_finish_actions/2]).


-include("tplt_def.hrl").
-include("enum_def.hrl").
-include("packet_def.hrl").

init(Task, #player_task_tplt{kind=Kind, require_actions=Actions}) ->
    case Kind of
	?tk_daily -> %% 如果是每日任务，初始化每日任务内容
	    case Actions of
		[0] -> 
		    Task#player_task{action_list=[]};
		_ -> 
		    ActionList = [#player_task_action{type=Type, finish_amount=0} || Type <- Actions],
		    Task#player_task{action_list=ActionList}
	    end;
	_ ->
	    Task#player_task{action_list=[]}
    end.


%% 完成任务时的处理
on_complete_task(TaskId, Task) ->
    {TheDay, CompleteList} = player_task_data:get_daily_info(Task),
    case lists:member(TaskId, CompleteList) of
	true -> Task;	    
	_ ->
	    player_task_data:set_daily_info(Task, {TheDay, [TaskId |  CompleteList]})
    end.


%% 更新今天任务
update_today(PlayerData) ->
    %% 完成过新手任务，才能接每日任务
    ControlTaskID = common_def:get_val(control_daily_task_id),
    case player_task:is_exist_complete_task(ControlTaskID, PlayerData) of
	true ->
	    Task = player_data:get_task(PlayerData),
	    {Today, _Time} = datetime:local_time(),
	    case player_task_data:get_daily_info(Task) of
		{Today, _CompleteList} -> %% 当天的任务已经产生过了
		    PlayerData;
		_ -> %% 需要产生今天任务	    
		    ActiveList = player_task_data:get_active_list(Task),
		    AllDailyTasks = get_all_daily_task(),
		    DailyIDList = get_daily_tasks(ActiveList, AllDailyTasks),
		    
		    %% 每日任务最大数
		    TodayMax = common_def:get_val(every_day_max_task_count),
		    
		    %% 任务总数限制
		    LimitN = player_task:get_max_task_count() - length(ActiveList),
		    
		    NeedN = TodayMax - length(DailyIDList),
		    
		    N = case NeedN > LimitN of
			    true -> LimitN;
			    _ -> NeedN
			end,
		    
		    %% 随机取得多个任务ID
		    NTaskList = random_pick(AllDailyTasks -- DailyIDList, N),
		    
		    %% 分发任务
		    F = fun(TaskId, State) ->
				{_Reply, NewState} = player_task:give(TaskId, State),
				NewState
			end,
		    
		    %% 清空完成列表
		    NTask = player_task_data:set_daily_info(Task, {Today, []}),
		    NPlayerData = player_data:set_task(NTask, PlayerData),
		    
		    NewPlayerData = lists:foldl(F, NPlayerData, NTaskList),
		    
		    NewPlayerData
	    end;
	_ ->
	    PlayerData
    end.    


%% 随机选取
random_pick(List, NSelect) when NSelect >= 0, 
				  length(List) >= NSelect ->
    F = fun(_X, {Acc, Rest}) ->
		I = random:uniform(length(Rest)),
		E = lists:nth(I, Rest),
		{[E | Acc], lists:delete(E, Rest)}
	end,
    
    {Result, _Rest} = lists:foldl(F, {[], List}, lists:seq(1, NSelect)),
    Result;
random_pick(_List, _NSelect) ->
    [].



%% 取得当前进行中的每日任务
get_daily_tasks(ActiveTaskList, AllDailyTasks) when is_list(ActiveTaskList) ->
    [TaskId || #player_task{task_id=TaskId} <- ActiveTaskList, 
	       lists:member(TaskId, AllDailyTasks)].

%% 取得所有每日任务
get_all_daily_task() ->
    List = tplt:get_all_data(player_task_tplt),
    [TaskId || #player_task_tplt{task_id=TaskId, kind=Kind} <- List, Kind =:= ?tk_daily].


%% 增加动作次数
inc_action_amount(ActionType, PlayerData) ->
    Task = player_data:get_task(PlayerData),
    ActiveList = player_task_data:get_active_list(Task),
    F = fun(#player_task{id=InstId, task_id=TaskId, action_list=ActionList}=X, {Tag, L}) ->
		case lists:keyfind(ActionType, #player_task_action.type, ActionList) of
		    #player_task_action{finish_amount=N}=R -> 	     
			#player_task_tplt{require_actions=RequireActions, require_actions_times=Times} = 
			    tplt:get_data(player_task_tplt, TaskId),
			%% 取上限
			Dict = lists:zip(RequireActions, Times),
			{_ActionType, Max} = lists:keyfind(ActionType, 1, Dict),

			Account = player_data:get_account(PlayerData),
			case N < Max of
			    true -> %% 小于上限				
				NewActions = lists:keyreplace(ActionType, #player_task_action.type, 
							      ActionList, R#player_task_action{finish_amount=N + 1}),
				net_helper:send2client(Account, 
						       #notify_update_task_progress{id=InstId,
										   action_type=ActionType,
										   action_amount=N + 1}),
				{changed, [X#player_task{action_list=NewActions} | L]};
			    _ ->
				net_helper:send2client(Account, 
						       #notify_update_task_progress{id=InstId,
										   action_type=ActionType,
										   action_amount=N}),
				{Tag, [X | L]}
			end;
		    _ -> 
			{Tag, [X | L]}
		end
	end,    

    case lists:foldr(F, {unchanged, []}, ActiveList) of
	{changed, NActiveList} ->
	    %% 有改变才存到数据库
	    NPlayerTask = player_task_data:set_active_list(Task, NActiveList),    
	    db_common:write(NPlayerTask),
	    player_data:set_task(NPlayerTask, PlayerData);
	{unchanged, _List} ->
	    PlayerData
    end.


%% 是否完成所有动作
can_finish_actions(#player_task_tplt{require_actions=RequireActions, require_actions_times=Times}, 
		   #player_task{action_list=ActionList}) ->
    case ActionList of
	[] -> true;
	_ ->
	    %% 取上限
	    Dict = lists:zip(RequireActions, Times),
	    F = fun(#player_task_action{type=ActionType, finish_amount=FinishN}, Acc) ->
			{_ActionType, Max} = lists:keyfind(ActionType, 1, Dict),
			case FinishN < Max of
			    true -> false; %% 如果数量没完成
			    _ -> Acc
			end
		end,
	    lists:foldl(F, true, ActionList)	    
    end.




