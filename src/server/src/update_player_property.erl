%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%%
%%% @end
%%% Created : 15 Apr 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(update_player_property).

-include("packet_def.hrl").

%% API
-export([update/0]).

%%%===================================================================
%%% API
%%%===================================================================
update() ->
    Keys = get_table_keys(player_basic_data),
    [update_player_property(Key) || Key <- Keys],
    {atomic, ok}.

update_player_property(Key) ->
    PlayerProperty = db_common:select_one(player_property, Key),
    NPlayerProperty = 
	case PlayerProperty of
	    {player_property, Account, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, _HouseClean} ->
		{player_property, Account, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue};
	    {player_property, Account, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue} ->
		{player_property, Account, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue};
	    _ ->
		{player_property, Key, 100, 100, 100, 0}
	end,
    db_common:write(NPlayerProperty).
%%%===================================================================
%%% Internal functions
%%%===================================================================
get_table_keys(Tab) ->
    F = fun() ->
		mnesia:all_keys(Tab)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).
