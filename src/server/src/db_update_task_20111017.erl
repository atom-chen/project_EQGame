%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%
%%% @end
%%% Created : 27 Oct 2011 by hongjx <hongjx@35info.cn>

-module(db_update_task_20111017).
-export([clear_test_task/0]).

%% 当时的记录
-record(task, {account,                 %% 玩家帐号
	       active_list=[],          %% 当前的任务列表
	       cancel_list=[],          %% 取消的任务列表
	       complete_list=[],        %% 完成的任务列表
	       active_chain_list=[],    %% 当前的链式任务列表
	       cancel_chain_list=[],    %% 取消的链式任务列表
	       complete_chain_list=[],   %% 完成的链式任务列表
	       daily_info=[]            %% 每日任务(记录每天完成过哪些任务) 
	      }).


%% 有条件地清除测试任务
clear_test_task() ->
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


convert_record(#task{account=Account, active_list=ActiveList}=R) -> 
    NActiveList = clear_test_task(ActiveList),
    R#task{account=Account, active_list=NActiveList}.


clear_test_task(TaskList) ->
    [X || X <- TaskList, not is_test_task(X)].


is_test_task(Task) ->
    TaskID = element(4, Task),
    lists:member(TaskID, lists:seq(9900, 9920)).




