%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  ������������(������������������
%%% @end
%%% Created : 12 Oct 2011 by hongjx <hongjx@35info.cn>

-module(db_update_task_20111014).

-export([clear_newer_task/0]).

%% ����������
%%-record(player_task, {id=0, account="", task_id=0, step_index=0, 
%% track=0, status=0, create_date}).

%% ����������
%% -record(new_player_task, {id=0, account="", task_id=0, step_index=0, 
%% 		      track=0, status=0, create_date, action_list=[]}).

%% ��ʱ�ļ�¼
-record(task, {account,                 %% ����ʺ�
	       active_list=[],          %% ��ǰ�������б�
	       cancel_list=[],          %% ȡ���������б�
	       complete_list=[],        %% ��ɵ������б�
	       active_chain_list=[],    %% ��ǰ����ʽ�����б�
	       cancel_chain_list=[],    %% ȡ������ʽ�����б�
	       complete_chain_list=[]   %% ��ɵ���ʽ�����б�
	      }).

%% �������������������
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
    %% ͨ��1019���ж��Ƿ�����ɹ���������
    case is_exist(1019, CompleteList) of
	true -> %% ��ɹ���������
	    R;
	_ -> %% û��ɹ�, ���³�ʼ����������
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


%% �Ƿ��������
-spec is_exist(integer(), integer(), list()) -> atom().
is_exist(TaskId, N, TaskList) ->
    case lists:keyfind(TaskId, N, TaskList) of
	false ->
	    false;
	_ ->
	    true
    end.


