%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  任务数据升级(增加任务动作及数量）
%%% @end
%%% Created : 12 Oct 2011 by hongjx <hongjx@35info.cn>

-module(db_update_task_20111012).

-export([add_task_action_field/0]).
-export([add_farm_task_action_field/0]).

%% 旧任务数据
%%-record(player_task, {id=0, account="", task_id=0, step_index=0, 
%% track=0, status=0, create_date}).

%% 新任务数据
%% -record(new_player_task, {id=0, account="", task_id=0, step_index=0, 
%% 		      track=0, status=0, create_date, action_list=[]}).

%% 当时的记录
-record(task, {account,                 %% 玩家帐号
	       active_list=[],          %% 当前的任务列表
	       cancel_list=[],          %% 取消的任务列表
	       complete_list=[],        %% 完成的任务列表
	       active_chain_list=[],    %% 当前的链式任务列表
	       cancel_chain_list=[],    %% 取消的链式任务列表
	       complete_chain_list=[]   %% 完成的链式任务列表
	      }).

%% 当时的记录
-record(farm_task, {account,                 %% 玩家帐号
		    active_list=[],          %% 当前的任务列表
		    cancel_list=[],          %% 取消的任务列表
		    complete_list=[]         %% 完成的任务列表
		   }).


add_farm_task_action_field() ->
    convert_table(farm_task),
    {atomic, ok}.
add_task_action_field() ->
    convert_table(task),
    {atomic, ok}.


convert_table(Tab) ->
    Keys = db_common:all_keys(Tab),
    [begin
	 %%io:format("key:~p~n", [{Tab, K}]),
	 R = db_common:select_one(Tab, K),
	 %%io:format("record:~p~n", [{R}]),
	 NewR = convert_record(R),
	 db_common:write(NewR) 
     end || K <- Keys].

convert_record(#farm_task{active_list=ActiveList,
		     cancel_list=CancelList,
		     complete_list=CompleteList
		    }=R) -> 

    NActiveList = convert_task_list(ActiveList),
    NCancelList = convert_task_list(CancelList),
    NCompleteList = convert_task_list(CompleteList),
    
    R#farm_task{active_list=NActiveList,
		     cancel_list=NCancelList,
		     complete_list=NCompleteList};


convert_record(#task{active_list=ActiveList,
		     cancel_list=CancelList,
		     complete_list=CompleteList,
		     active_chain_list=ChainList,
		     cancel_chain_list=CancelChainList,
		     complete_chain_list=CompleteChainList
		    }=R) -> 

    NActiveList = convert_task_list(ActiveList),
    NCancelList = convert_task_list(CancelList),
    NCompleteList = convert_task_list(CompleteList),
    NChainList = convert_task_list(ChainList),
    NCancelChainList = convert_task_list(CancelChainList),
    NCompleteChainList = convert_task_list(CompleteChainList),
    
    R#task{active_list=NActiveList,
		     cancel_list=NCancelList,
		     complete_list=NCompleteList,
		     active_chain_list=NChainList,
		     cancel_chain_list=NCancelChainList,
		     complete_chain_list=NCompleteChainList}.

convert_task_list(List) ->
    [convert_task(X) || X <- List].

convert_task(Task) when element(1, Task) == player_task,
			size(Task) == 8 -> % 旧数据
    %% 增加动作列表(拜访好友, 捡垃圾之类的动作)
    erlang:append_element(Task, []);
convert_task(Task) when element(1, Task) == player_task,
			size(Task) == 9 -> % 已经转化过了
    Task.



