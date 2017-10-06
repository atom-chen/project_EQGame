%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  ������������(������������������
%%% @end
%%% Created : 12 Oct 2011 by hongjx <hongjx@35info.cn>

-module(db_update_task_20111012).

-export([add_task_action_field/0]).
-export([add_farm_task_action_field/0]).

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

%% ��ʱ�ļ�¼
-record(farm_task, {account,                 %% ����ʺ�
		    active_list=[],          %% ��ǰ�������б�
		    cancel_list=[],          %% ȡ���������б�
		    complete_list=[]         %% ��ɵ������б�
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
			size(Task) == 8 -> % ������
    %% ���Ӷ����б�(�ݷú���, ������֮��Ķ���)
    erlang:append_element(Task, []);
convert_task(Task) when element(1, Task) == player_task,
			size(Task) == 9 -> % �Ѿ�ת������
    Task.



