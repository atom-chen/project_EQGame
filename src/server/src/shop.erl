%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  商店的模块, 供系统商城和npc商店使用
%%% @end
%%% Created : 21 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(shop).

-include("common_def.hrl").
-include("sys_msg.hrl"). 
-include("enum_def.hrl").
-include("tplt_def.hrl").

%% API
-export([buy_goods/4]).
%%%===================================================================
%%% API
%%%===================================================================
%% 购买商品
%% buy_goods(Account, ItemID, Coin, PlayerData) ->
%%     buy_goods(Account, ItemID, Coin, PlayerData).

buy_goods(Account, ItemID, {_Type, Price}=Coin, PlayerData) when ItemID > 0, Price > 0 ->
    %% 玩家的当前状态是否能购买商品
    case can_buy_goods(Account, ItemID, Coin, PlayerData) of
	true -> 
	    NPD = do_buy_goods(Account, ItemID, Coin, PlayerData),
	    PlayerCoin = player_data:get_coin(NPD),
	    Bag = player_data:get_pack(?pt_bag, NPD),
	    db_memdisc:write(PlayerCoin),
	    db_common:write([player_pack:make_package(Account, ?pt_bag, Bag)]),
	    player_coin:send_coin(PlayerCoin),
	    player_pack:send(Account, NPD),
	    {ok, NPD};
	{false, Reason} ->
	    sys_msg:send(Account ,Reason),
	    {error, Reason}
    end;

buy_goods(Account, ItemID, Coin, PlayerData) ->
    erlang:error({badarg, Account, ItemID, Coin, PlayerData}).

%% 判断是否能购买无商品
can_buy_goods(_Account, ItemID, {Type, Price}, PlayerData) ->
    %% 判断玩家的游戏币是否足够
    PlayerMoney = player_coin:get_coin(Type, PlayerData),
    case Price > PlayerMoney of
	true -> 
	    case Type of
		?GAMECOIN -> {false, ?err_not_enough_game_coin};
		?EQCOIN ->  {false, ?err_not_enough_eq_coin}
	    end;
	false ->
	    %% 判断玩家的背包是否有空位
	    case player_pack:can_add_item(ItemID, 1, PlayerData) of
		true -> true;
		false -> {false, ?err_bag_full}
	    end
    end.

do_buy_goods(Account, ItemID, {Type, Price}, PlayerData) ->
    %% 扣除相应的货币
    NPD = player_coin:dec_coin2(Type, Price, PlayerData),
    NewItem = item:make_item(ItemID),
    %% 增加新的物品到背包
    NPD1 = player_pack:add_item(Account, NewItem, NPD),
    %% 货币, 物品的log记录
    Count = 1,
    ItemInfo = {ItemID, Count, item:get_instid(NewItem)},
    MoneyInfo = item_money_log:get_money_info(Type, -Price),
    item_money_log:log(Account, buy_item, ItemInfo, MoneyInfo),
    %% 提示获得物品
    sys_msg:send(Account, ?msg_get_item, [item:get_item_name(NewItem), Count]),
    NPD1.

%%------------------------test--------------------------------------
-include("packet_def.hrl").
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

%% do_buy_goods_test() ->
%%     erlymock:start(),
%%     erlymock:stub(tplt, is_exist, [item_tplt, 1], [{return, true}]),
%%     erlymock:stub(tplt, get_data2, [item_tplt, 1], [{return, #item_tplt{overlap=100}}]),
%%     erlymock:replay(), 
%%     guid:start_link(),

%%     PD = player_data:new(),
%%     PD1 = player_data:set_coin(player_coin:new("test", 10000, 10000), PD),
%%     PD2 = player_data:set_pack(?pt_bag, pack:new(8), PD1),
%%     PD3 = do_buy_goods(test1, 1, {?GAMECOIN, 1}, PD2),
%%     ?assertEqual(10000-1, player_coin:get_coin(?GAMECOIN, PD3)),
%%     ?assertEqual(1, item:get_tempid(player_pack:get_item(0, PD3))),
%%     erlymock:verify(),
%%     ok.

can_buy_goods_test() ->
    erlymock:start(),
    erlymock:stub(tplt, is_exist, [item_tplt, 1], [{return, true}]),
    erlymock:stub(tplt, get_data2, [item_tplt, 1], [{return, #item_tplt{overlap=100}}]),
    erlymock:replay(), 
    guid:start_link(),

    PD = player_data:new(),
    PD1 = player_data:set_coin(player_coin:new("test", 0, 0), PD),
    PD2 = player_data:set_pack(?pt_bag, pack:new(8), PD1),
    ?assertEqual({false, ?err_not_enough_coin}, can_buy_goods(test1, 1, {?GAMECOIN, 1}, PD2)),

    PD3 = player_data:set_coin(player_coin:new("test", 1000, 0), PD),
    PD4 = player_data:set_pack(?pt_bag, pack:new(0), PD3),
    ?assertEqual({false, ?err_bag_full}, can_buy_goods(test1, 1, {?GAMECOIN, 1}, PD4)),
    erlymock:verify(),
    ok.

-endif.
