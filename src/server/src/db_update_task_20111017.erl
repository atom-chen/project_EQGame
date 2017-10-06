%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%
%%% @end
%%% Created : 27 Oct 2011 by hongjx <hongjx@35info.cn>

-module(db_update_task_20111017).
-export([clear_test_task/0]).

%% ��ʱ�ļ�¼
-record(task, {account,                 %% ����ʺ�
	       active_list=[],          %% ��ǰ�������б�
	       cancel_list=[],          %% ȡ���������б�
	       complete_list=[],        %% ��ɵ������б�
	       active_chain_list=[],    %% ��ǰ����ʽ�����б�
	       cancel_chain_list=[],    %% ȡ������ʽ�����б�
	       complete_chain_list=[],   %% ��ɵ���ʽ�����б�
	       daily_info=[]            %% ÿ������(��¼ÿ����ɹ���Щ����) 
	      }).


%% �������������������
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




