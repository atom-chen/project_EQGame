%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% GM工具数据库操作
%%% @end
%%% Created :  6 Aug 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(db_gm_tool).

-include_lib("stdlib/include/qlc.hrl").
-include("packet_def.hrl").

%% API
-export([search_player/1, search_farm_player/1, search_sys_broadcast/1]).

%%%===================================================================
%%% API
%%%===================================================================
search_player(Account) ->
    F = fun() ->
		Q = qlc:q([{PlayerBasicData#player_basic_data.account, 
			    PlayerBasicData#player_basic_data.username,
			    PlayerCoin#player_coin.gamecoin,
			    PlayerCoin#player_coin.eqcoin,
			    PlayerProperty#player_property.player_clean,
			    PlayerProperty#player_property.player_health,
			    PlayerProperty#player_property.player_charm,
			    PlayerProperty#player_property.active_value,
			    House#house_data.house_clean
			   } || PlayerBasicData <- mnesia:table(player_basic_data),
				PlayerBasicData#player_basic_data.account=:=Account,
				PlayerCoin <- mnesia:table(player_coin),
				PlayerCoin#player_coin.account =:= PlayerBasicData#player_basic_data.account,
				PlayerProperty <- mnesia:table(player_property),
				PlayerProperty#player_property.account=:=Account,
				House <- mnesia:table(house_data),
				House#house_data.owner =:=Account
			  ]),
		qlc:e(Q)
	end,
    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    [];
	[Data] ->
	    Data
    end.

search_farm_player(Account) ->
    F = fun() ->
		Q = qlc:q([{player_farm:get_account(PlayerFarm),
			    player_farm:get_exp(PlayerFarm),
			    player_farm:get_coin(PlayerFarm)
			   } || PlayerFarm <- mnesia:table(player_farm),
				player_farm:get_account(PlayerFarm)=:=Account
			  ]),
		qlc:e(Q)
	end,
    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    [];
	[Data] ->
	    Data
    end.

search_sys_broadcast(Type) ->
    F = fun() ->
		TH = qlc:keysort(2, mnesia:table(sys_broadcast), [{order, ascending}]),
		Q = qlc:q([SysBroadcast || SysBroadcast <- TH,
					   sys_broadcast:get_type(SysBroadcast)=:=Type
			  ]),
		qlc:e(Q) 
	end,
    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    [];
	Data ->
	    Data
    end.

%%%===================================================================
%%% Internal functions
%%%===================================================================
