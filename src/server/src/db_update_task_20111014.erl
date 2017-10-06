%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  任务数据升级(增加任务动作及数量）
%%% @end
%%% Created : 12 Oct 2011 by hongjx <hongjx@35info.cn>

-module(db_update_task_20111014).

-export([clear_newer_task/0]).

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

%% 有条件地清除新手任务
clear_newer_task() ->
    convert_table(task),
    {atomic, ok}.


convert_table(Tab) ->
    Keys = db_common:all_keys(Tab),
    [begin
	 %%io:format("key:~p~n", [{Tab, K}]),
	 R = db_common:select_one(Tab, K),
	 NewR = convert_record(R),
	 db_common:write(NewR) 
     end || K <- Keys].


convert_record(#task{account=Account, complete_list=CompleteList
		    }=R) -> 
    %% 通过1019来判断是否曾完成过新手任务
    case is_exist(1019, CompleteList) of
	true -> %% 完成过新手任务
	    R;
	_ -> %% 没完成过, 重新初始化新手任务
	    %%io:format("record ~p~n", [R]),
	     
	    case db_common:select_one(player_basic_data, Account) of
		[] -> R;		
		BasicData -> 
		    Sex = basic_data:get_data(sex, BasicData),
		    NActiveList = player_task_init:init(Account, Sex),
		    
		    #task{account=Account, active_list=NActiveList}
	    end
    end.


is_exist(TaskId, TaskList) ->
    is_exist(TaskId, 4, TaskList).


%% 是否存在任务
-spec is_exist(integer(), integer(), list()) -> atom().
is_exist(TaskId, N, TaskList) ->
    case lists:keyfind(TaskId, N, TaskList) of
	false ->
	    false;
	_ ->
	    true
    end.


