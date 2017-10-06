%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%%
%%% @end
%%% Created : 19 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm_task_1011).

-export([can_give/1, do_give/1,
	 can_complete/1, do_complete/1]).

%%%===================================================================
%%% API
%%%===================================================================
can_give(_State) ->
    true.

do_give(State) ->
    State.

can_complete(_State) ->
    true.

do_complete(State) ->
    {_Reply, NState} = script:give_farm_task(1012, State),
    NState.
%%%===================================================================
%%% Internal functions
%%%===================================================================
