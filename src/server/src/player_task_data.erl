%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% ��������
%%% @end
%%% Created : 17 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(player_task_data).

-include("packet_def.hrl").
-include("common_def.hrl").

-record(task, {account,                 %% ����ʺ�
	       active_list=[],          %% ��ǰ�������б�
	       cancel_list=[],          %% ȡ���������б�
	       complete_list=[],        %% ��ɵ������б�
	       active_chain_list=[],    %% ��ǰ����ʽ�����б�
	       cancel_chain_list=[],    %% ȡ������ʽ�����б�
	       complete_chain_list=[],  %% ��ɵ���ʽ�����б�
	       daily_info=[]            %% ÿ������(��¼ÿ����ɹ���Щ����)                       
	      }).

%% API
-compile(export_all).

%%%===================================================================
%%% API
%%%===================================================================

init(Account, Sex) ->
    ActiveList = player_task_init:init(Account, Sex),
    NActiveList = player_task:get_all_once_task(Account) ++ ActiveList,
    
    #task{account=Account, active_list=NActiveList}.

create_task(Account, TaskId) ->
    #player_task{id=guid:make(?st_task), account=Account, task_id=TaskId, step_index=0,
		 track = 0, status=?ACTIVE,
		 create_date=datetime:localtime()}.

get_active_list(#task{active_list=ActiveList}) ->
    ActiveList.

set_active_list(Task, ActiveList) ->
    Task#task{active_list=ActiveList}.

get_cancel_list(#task{cancel_list=CancelList}) ->
    CancelList.

set_cancel_list(Task, CancelList) ->
    Task#task{cancel_list=CancelList}.

get_complete_list(#task{complete_list=CompleteList}) ->
    CompleteList.

set_complete_list(Task, CompleteList) ->
    Task#task{complete_list=CompleteList}.

get_active_chain_list(#task{active_chain_list=ActiveChainList}) ->
    ActiveChainList.

set_active_chain_list(Task, ActiveChainList) ->
    Task#task{active_chain_list=ActiveChainList}.

get_cancel_chain_list(#task{cancel_chain_list=CancelChainList}) ->
    CancelChainList.

set_cancel_chain_list(Task, CancelChainList) ->
    Task#task{cancel_chain_list=CancelChainList}.

get_complete_chain_list(#task{complete_chain_list=CompleteChainList}) ->
    CompleteChainList.

set_complete_chain_list(Task, CompleteChainList) ->
    Task#task{complete_chain_list=CompleteChainList}.


get_daily_info(#task{daily_info=DailyInfo}) ->
    DailyInfo.

set_daily_info(Task, DailyInfo) ->
    Task#task{daily_info=DailyInfo}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
