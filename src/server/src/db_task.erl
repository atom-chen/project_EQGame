%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 对玩家任务的数据库操作
%%% @end
%%% Created :  1 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(db_task).

-include("common_def.hrl").
-include("packet_def.hrl").
-include_lib("stdlib/include/qlc.hrl").

%% API
-export([get_active_task/1, get_task/2]).
-export([change_step_index/3, complete_step_index/3]).
-export([give/2, give_up/2]).
-export([track/2, cancel_track/2, get_track_list/1]).
-export([is_exist/2, is_exist_complete_task/2]).

%%%===================================================================
%%% API
%%%===================================================================
%% 获取所有激活的任务
-spec get_active_task(atom()) -> list().
get_active_task(Account) when is_atom(Account) ->
    F = fun() -> 

		Q = qlc:q([[PlayerTask] || PlayerTask <- mnesia:table(player_task),
					   PlayerTask#player_task.account =:= Account,
					   PlayerTask#player_task.status =:= ?ACTIVE]), 

		qlc:e(Q)
	end,

    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    [];
	Data ->
	    Data
    end.

%% 获取任务
-spec get_task(atom(), integer()) -> list().
get_task(Account, TaskId) when is_atom(Account), is_integer(TaskId) ->
    F = fun() -> 
		Q = qlc:q([[PlayerTask] || PlayerTask <- mnesia:table(player_task),
					   PlayerTask#player_task.task_id =:= TaskId,
					   PlayerTask#player_task.account =:= Account,
					   PlayerTask#player_task.status =:= ?ACTIVE]),

		qlc:e(Q)
	end,

    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    [];
	[Data] ->
	    Data
    end.

%% 是否已经存在接受的任务
-spec is_exist(atom(), integer()) -> atom().
is_exist(Account, TaskId) when is_atom(Account), is_integer(TaskId) ->
    F = fun() ->
		Q = qlc:q([PlayerTask || PlayerTask <- mnesia:table(player_task),
					 PlayerTask#player_task.account =:= Account,
					 PlayerTask#player_task.task_id =:= TaskId,
					 PlayerTask#player_task.status =:= ?ACTIVE]),
		qlc:e(Q)
	end,
    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    false;
	_ ->
	    true
    end.

is_exist_complete_task(Account, TaskId) when is_atom(Account), is_integer(TaskId) ->
    F = fun() ->
		Q = qlc:q([PlayerTask || PlayerTask <- mnesia:table(player_task),
					 PlayerTask#player_task.account =:= Account,
					 PlayerTask#player_task.task_id =:= TaskId,
					 PlayerTask#player_task.status =:= ?COMPLETE]),
		qlc:e(Q)
	end,
    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    false;
	_ ->
	    true
    end.

%% 接受任务
-spec give(atom(), integer()) -> any().
give(Account, TaskId) when is_atom(Account), is_integer(TaskId) ->
    Id = guid:make(?st_task),
    PlayerTask = #player_task{id=Id, account=Account, task_id=TaskId, step_index=0,
			      track = 0, status=?ACTIVE,
			      create_date=datetime:datetime_to_gregorian_seconds(datetime:local_time())},
    F = fun() -> 
		mnesia:write(PlayerTask)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

%% 放弃任务
-spec give_up(atom(), integer()) -> any().
give_up(Account, Id) when is_atom(Account), is_integer(Id) ->
    F = fun() ->
		Q = qlc:q([PlayerTask || PlayerTask <- mnesia:table(player_task),
					 PlayerTask#player_task.id =:= Id,
					 PlayerTask#player_task.account =:= Account]),
		[OldPlayerTask] = qlc:e(Q),
		NewPlayerTask = OldPlayerTask#player_task{status=?GIVEUP},
		mnesia:write(NewPlayerTask) 
	end,

    mnesia:activity(transaction, F, [], mnesia_frag).

%% 跟踪任务
-spec track(atom(), integer()) -> any().
track(Account, Id) when is_atom(Account), is_integer(Id) ->
    F = fun() ->
		Q = qlc:q([PlayerTask || PlayerTask <- mnesia:table(player_task),
					 PlayerTask#player_task.id =:= Id,
					 PlayerTask#player_task.account =:= Account,
					 PlayerTask#player_task.status =:= ?ACTIVE]),
		[OldPlayerTask] =    qlc:e(Q),
		NewPlayerTask = OldPlayerTask#player_task{track = 1},
		mnesia:write(NewPlayerTask) 
	end,

    mnesia:activity(transaction, F, [], mnesia_frag).

%% 取消跟踪任务
-spec cancel_track(atom(), integer()) -> atom().
cancel_track(Account, Id) when is_atom(Account), is_integer(Id) ->
    F = fun() ->
		Q = qlc:q([PlayerTask || PlayerTask <- mnesia:table(player_task),
					 PlayerTask#player_task.id =:= Id,
					 PlayerTask#player_task.account =:= Account]),
		[OldPlayerTask] =    qlc:e(Q),
		NewPlayerTask = OldPlayerTask#player_task{track = 0},
		mnesia:write(NewPlayerTask) 
	end,

    mnesia:activity(transaction, F, [], mnesia_frag).

%% 获取跟踪任务列表
-spec get_track_list(atom()) -> list().
get_track_list(Account) when is_atom(Account) ->
    F = fun() -> 
		Q = qlc:q([[PlayerTask] || PlayerTask <- mnesia:table(player_task),
					   PlayerTask#player_task.account =:= Account,
					   PlayerTask#player_task.track =:= 1,
					   PlayerTask#player_task.status =:= ?ACTIVE]), 

		qlc:e(Q)
	end,

    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    [];
	Data ->
	    Data
    end.

%% 更改步骤索引
-spec change_step_index(atom(), integer(), integer()) -> atom().
change_step_index(Account, TaskId, StepIndex) when is_atom(Account), is_integer(TaskId), is_integer(StepIndex) ->
    F = fun() ->
		[OldPlayerTask] = select(Account, TaskId),
		NewPlayerTask = OldPlayerTask#player_task{step_index=StepIndex},
		mnesia:write(NewPlayerTask) 
	end,

    mnesia:activity(transaction, F, [], mnesia_frag).   

%% 完成所有步骤
-spec complete_step_index(atom(), integer(), integer()) -> tuple().
complete_step_index(Account, TaskId, StepIndex) when is_atom(Account), is_integer(TaskId), is_integer(StepIndex) ->
    F = fun() ->
		[OldPlayerTask] = select(Account, TaskId),
		NewPlayerTask = OldPlayerTask#player_task{step_index=StepIndex, status=?COMPLETE},
		mnesia:write(NewPlayerTask),
		NewPlayerTask
	end,

    mnesia:activity(transaction, F, [], mnesia_frag). 

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 获取任务
-spec select(atom(), integer()) -> any(). 
select(Account, TaskId) when is_atom(Account), is_integer(TaskId) ->
    Q = qlc:q([PlayerTask || PlayerTask <- mnesia:table(player_task),
			     PlayerTask#player_task.account =:= Account,
			     PlayerTask#player_task.task_id =:= TaskId,
			     PlayerTask#player_task.status =:= ?ACTIVE]),
    qlc:e(Q).
