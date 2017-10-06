%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 任务数据
%%% @end
%%% Created : 17 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm_task_data).

-include("packet_def.hrl").
-include("common_def.hrl").

-record(farm_task, {account,                 %% 玩家帐号
		    active_list=[],          %% 当前的任务列表
		    cancel_list=[],          %% 取消的任务列表
		    complete_list=[]         %% 完成的任务列表
		   }).

%% API
-compile(export_all).

%%%===================================================================
%%% API
%%%===================================================================
init(Account) ->
    ActiveList = farm_task_init:init(Account),
    #farm_task{account=Account, active_list=ActiveList}.

create_task(Account, TaskId) ->
    #player_task{id=guid:make(?st_task), account=Account, task_id=TaskId, step_index=0,
		 track = 0, status=?ACTIVE,
		 create_date=datetime:localtime()}.

get_active_list(#farm_task{active_list=ActiveList}) ->
    ActiveList.

set_active_list(Task, ActiveList) ->
    Task#farm_task{active_list=ActiveList}.

get_cancel_list(#farm_task{cancel_list=CancelList}) ->
    CancelList.

set_cancel_list(Task, CancelList) ->
    Task#farm_task{cancel_list=CancelList}.

get_complete_list(#farm_task{complete_list=CompleteList}) ->
    CompleteList.

set_complete_list(Task, CompleteList) ->
    Task#farm_task{complete_list=CompleteList}.
%%%===================================================================
%%% Internal functions
%%%===================================================================
