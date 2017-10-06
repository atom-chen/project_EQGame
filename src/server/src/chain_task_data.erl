%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 链式任务数据
%%% @end
%%% Created : 19 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(chain_task_data).

-record(chain_task, {inst_id,           %% 任务实例Id
		     task_id,           %% 链式任务Id
		     task_list,         %% 链式任务列表
		     curr_task_id,      %% 当前所在的任务
		     create_date        %% 任务创建日期
		    }).

%% API
-compile(export_all).
%%%===================================================================
%%% API
%%%===================================================================
new(InstId, TaskId, TaskList, CurrTaskId, CreateDate) ->
    #chain_task{inst_id=InstId,
		task_id=TaskId,
		task_list=TaskList,
		curr_task_id=CurrTaskId,
		create_date=CreateDate
	       }.

get_inst_id(#chain_task{inst_id=InstId}) ->
    InstId.

set_inst_id(ChainTask, InstId) ->
    ChainTask#chain_task{inst_id=InstId}.

get_task_id(#chain_task{task_id=TaskId}) ->
    TaskId.

set_task_id(ChainTask, TaskId) ->
    ChainTask#chain_task{task_id=TaskId}.

get_task_list(#chain_task{task_list=TaskList}) ->
    TaskList.

set_task_list(ChainTask, TaskList) ->
    ChainTask#chain_task{task_list=TaskList}.

get_curr_task_id(#chain_task{curr_task_id=CurrTaskId}) ->
    CurrTaskId.

set_curr_task_id(ChainTask, CurrTaskId) ->
    ChainTask#chain_task{curr_task_id=CurrTaskId}.

get_create_date(#chain_task{create_date=CreateDate}) ->
    CreateDate.

set_create_date(ChainTask, CreateDate) ->
    ChainTask#chain_task{create_date=CreateDate}.
%%%===================================================================
%%% Internal functions
%%%===================================================================
