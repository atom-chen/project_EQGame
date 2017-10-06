%%%-------------------------------------------------------------------
%%% @author linyijie <linyijie@china-channel.com>
%%% @copyright (C) 2010, linyijie
%%% @doc
%%%  玩家的仓库
%%% @end
%%% Created : 20 Jul 2010 by linyijie <linyijie@china-channel.com>
%%%-------------------------------------------------------------------
-module(player_depot).

-export([register/1, open/2, new/1, change/2, get_pack/1, handle_cast/2]).

-include("packet_def.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").
-include("router.hrl").
-include("sys_msg.hrl").

-define(max_grid_count, 40).

-record(player_depot, {account,
		       money,   %% 游戏币
		       pack     %% 包裹
		      }).

register(Account) ->
    [router:make_event_source(?msg_req_deposit_money_in_depot, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_withdraw_money_in_depot, Account, {self(), ?MODULE})
    ].

new(Account) ->
    new(Account, ?max_grid_count).

new(Account, Count) when is_integer(Count), Count > 0->
    new(Account, Count, 0).

new(Account, Count, Money) ->
    #player_depot{account=Account, money=Money, pack=pack:new(Count)}.

change(Depot, Grids) when is_record(Depot, player_depot)->
    Depot#player_depot{pack=Grids}.

open(Account, PlayerData) ->
    send(Account, player_data:get_depot(PlayerData)), 
    player_pack:send(Account, PlayerData).

get_pack(#player_depot{pack=Pack}) ->
    Pack.

send(Account, #player_depot{money=Money, pack=Pack}) ->
    Package = #notify_player_depot{money=Money, grid_vec=Pack}, 
    net_helper:send2client(Account, Package).

%% 存钱到仓库
handle_cast({#msg{src=Account}, #req_deposit_money_in_depot{money=Money}}, PlayerData) ->
    NPlayerData = do_deposit_money_in_depot(Account, Money, PlayerData),
    {noreply, NPlayerData};

%% 从仓库取钱到包裹
handle_cast({#msg{src=Account}, #req_withdraw_money_in_depot{money=Money}}, PlayerData) ->
    NPlayerData = do_withdraw_money_in_depot(Account, Money, PlayerData),
    {noreply, NPlayerData}.

do_deposit_money_in_depot(Account, Money, PlayerData) ->
    PackMoney = player_coin:get_coin(?GAMECOIN, PlayerData),
    DepotMoney = get_depot_money(PlayerData),
    case do_deposit_money(Money, DepotMoney, PackMoney) of
	{true, NewDepotMoney, NewPackMoney} ->
	    %% 保存到数据库
	    Depot = player_data:get_depot(PlayerData),
	    NewDepot = set_depot_money(NewDepotMoney, Depot),
	    NPD = player_coin:set_coin(?GAMECOIN, NewPackMoney, PlayerData),
	    db_common:write([NewDepot, player_data:get_coin(NPD)]),

	    %% 更新到客户端
	    send(Account, NewDepot),
	    player_coin:send_coin(player_data:get_coin(NPD)),
	    player_data:set_depot(NewDepot, NPD);
	{false, Reason} ->
	    sys_msg:send(Account, Reason),
	    PlayerData
    end.

do_withdraw_money_in_depot(Account, Money, PlayerData)->
    PackMoney = player_coin:get_coin(?GAMECOIN, PlayerData),
    DepotMoney = get_depot_money(PlayerData),
    case do_withdraw_money(Money, DepotMoney, PackMoney) of
	{true, NewDepotMoney, NewPackMoney} ->
	    %% 保存到数据库
	    Depot = player_data:get_depot(PlayerData),
	    NewDepot = set_depot_money(NewDepotMoney, Depot),
	    NPD = player_coin:set_coin(?GAMECOIN, NewPackMoney, PlayerData),
	    db_common:write([NewDepot, player_data:get_coin(NPD)]),

	    %% 更新到客户端
	    send(Account, NewDepot),
	    player_coin:send_coin(player_data:get_coin(NPD)),
	    player_data:set_depot(NewDepot, NPD);
	{false, Reason} ->
	    sys_msg:send(Account, Reason),
	    PlayerData
    end.

%% 获得仓库里的金钱
get_depot_money(PlayerData) ->
    #player_depot{money=Money} = player_data:get_depot(PlayerData),
    Money.

%% 设置仓库里的金币
set_depot_money(DepotMoney, Depot) ->
    Depot#player_depot{money=DepotMoney}.

%% 能否存钱到仓库

can_deposit_money(DepositMoney, _DepotMoney, PackMoney) 
  when DepositMoney > PackMoney ->
    {false, ?err_not_enough_coin};
can_deposit_money(DepositMoney, DepotMoney, _PackMoney) 
  when DepotMoney + DepositMoney > ?MAX_GAMECOIN ->
    {false, ?msg_game_coin_overflow};
can_deposit_money(DepositMoney, _DepotMoney, PackMoney) 
  when DepositMoney > 0; is_integer(PackMoney) ->
    true.

%% 存钱到仓库
do_deposit_money(DepositMoney, DepotMoney, PackMoney) ->
    case can_deposit_money(DepositMoney, DepotMoney, PackMoney) of
	true ->
	    %% 扣钱
	    NewPackMoney = PackMoney - DepositMoney,
	    %% 加钱
	    NewDepotMoney = DepotMoney + DepositMoney,
	    {true, NewDepotMoney, NewPackMoney};
	False ->
	    False
    end.

%% 能否从仓库取钱到背包
can_withdraw_money(WithdrawMoney, DepotMoney, _PackMoney) 
  when WithdrawMoney > DepotMoney ->
    {false, ?err_not_enough_coin};
can_withdraw_money(WithdrawMoney, _DepotMoney, PackMoney) 
  when PackMoney + WithdrawMoney > ?MAX_GAMECOIN ->
    {false, ?msg_game_coin_overflow};
can_withdraw_money(WithdrawMoney, DepotMoney, _PackMoney) 
  when WithdrawMoney > 0; is_integer(DepotMoney) ->
    true.

%% 从仓库取钱到背包
do_withdraw_money(WithdrawMoney, DepotMoney, PackMoney) ->
    case can_withdraw_money(WithdrawMoney, DepotMoney, PackMoney) of
	true ->
	    %% 加钱
	    NewPackMoney = PackMoney + WithdrawMoney,
	    %% 扣钱
	    NewDepotMoney = DepotMoney - WithdrawMoney,
	    {true, NewDepotMoney, NewPackMoney};
	False ->
	    False
    end.

%% -------------------------------------test------------------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

can_deposit_money_test() ->
    ?assertEqual({false, ?err_not_enough_coin}, can_deposit_money(10, 0, 1)),
    ?assertEqual(true, can_deposit_money(10, 0, 11)),
    ok. 

do_deposit_money_test() ->
    ?assertEqual({true, 11, 19}, do_deposit_money(1, 10, 20)),
    ok.

can_withdraw_money_test() ->
    ?assertEqual({false, ?err_not_enough_coin}, can_withdraw_money(10, 1, 0)),
    ?assertEqual(true, can_withdraw_money(10, 11, 0)),
    ?assertEqual({false, ?msg_game_coin_overflow}, can_withdraw_money(10, 11, ?MAX_GAMECOIN)),
    ok.

do_withdraw_money_test() ->
    ?assertEqual({true, 9, 21}, do_withdraw_money(1, 10, 20)),
    ok.

new_player_data() ->
    PD1 = player_data:new(),
    PD2 = player_data:set_coin(player_coin:new(test, 100, 100), PD1),
    player_data:set_depot(new(test, 10, 100), PD2).

do_deposit_money_in_depot_test() ->
    erlymock:start(),
    erlymock:stub(db_common, write, [[new(test, 10, 110), 
				     player_coin:new(test, 90, 100)]], 
		  [{return, ok}]),
    erlymock:replay(), 

    PlayerData = new_player_data(),
    do_deposit_money_in_depot(test, 10, PlayerData),

    erlymock:verify(),
    ok.

do_withdraw_money_in_depot_test() ->
    erlymock:start(),
    erlymock:stub(db_common, write, [[new(test, 10, 90), 
				     player_coin:new(test, 110, 100)]], 
		  [{return, ok}]),
    erlymock:replay(), 

    PlayerData = new_player_data(),
    do_withdraw_money_in_depot(test, 10, PlayerData),

    erlymock:verify(),
    ok.

-endif.

