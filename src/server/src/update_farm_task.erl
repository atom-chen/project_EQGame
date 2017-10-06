%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%%
%%% @end
%%% Created :  3 Mar 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(update_farm_task).
-export([update/0]).

-include("packet_def.hrl").

%% API


%%%===================================================================
%%% API
%%%===================================================================
update() ->
    Keys = get_table_keys(player_farm),
    [update_farm_task(Key) || Key <- Keys],
    {atomic, ok}.

update_farm_task(Key) ->
    FarmTask = {farm_task, Key,[],[],[]},
    db_common:write(FarmTask).

get_table_keys(Tab) ->
    F = fun() ->
		mnesia:all_keys(Tab)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).
