%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  npc商店
%%% @end
%%% Created : 21 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(npcshop).

-include("packet_def.hrl").
-include("router.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").

%% API
-export([start/1]).

%% gen_server callbacks
-export([handle_cast/2]).


start(Account) ->
    [router:make_event_source(?msg_req_buy_npc_shop_goods, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_sell_goods, Account, {self(), ?MODULE})
    ].


%% 买物品
handle_cast({#msg{event=?msg_req_buy_npc_shop_goods, src=Account}, 
	     #req_buy_npc_shop_goods{goods_id=GoodsID}}, PlayerData) ->
    {noreply, do_buy_goods(Account, GoodsID, PlayerData)};

%% 卖物品
handle_cast({#msg{event=?msg_req_sell_goods, src=Account},
	   #req_sell_goods{grid_index=Index}}, PlayerData)->
    Item = player_pack:get_item(Index, PlayerData),
    case do_sell_goods(Account, item:get_tempid(Item), Index, PlayerData) of
	{ok, NPD} ->
	    PlayerCoin = player_data:get_coin(NPD),
	    Bag = player_data:get_pack(?pt_bag, NPD),
	    db_common:write([PlayerCoin, player_pack:make_package(Account, ?pt_bag, Bag)]),
	    player_coin:send_coin(PlayerCoin),
	    player_pack:send(Account, NPD),
	    {noreply, NPD};
	{false, _} ->
	    {noreply, PlayerData}
    end.

%% 购买商品的具体实现 
do_buy_goods(Account, GoodsID, PlayerData) when GoodsID > 0 ->
    #npc_shop_tplt{price=Price, item_id=ItemID} = tplt:get_data2(npc_shop_tplt, GoodsID),
    case shop:buy_goods(Account, ItemID, {?GAMECOIN, Price}, PlayerData) of
	{ok, NPD} -> NPD;
	{error, _Reason} -> PlayerData
    end;
do_buy_goods(Account, GoodsID, PlayerData) ->
    erlang:error({bagarg, Account, GoodsID, PlayerData}).

can_sell_goods(SellPrice, _GameCoin) when SellPrice =:= 0 ->
    {false, ?err_item_cannot_sell};
can_sell_goods(SellPrice, GameCoin) when SellPrice + GameCoin > ?MAX_GAMECOIN ->
    {false, ?msg_game_coin_overflow};
can_sell_goods(_SellPrice, _GameCoin) ->
    true.

%% 卖物品的具体实现
do_sell_goods(Account, ItemID, Index, PlayerData) when ItemID > 0, Index >=0 ->
    #item_tplt{sell_price=SellPrice} = tplt:get_data2(item_tplt, ItemID),
    case can_sell_goods(SellPrice, player_coin:get_coin(?GAMECOIN, PlayerData)) of
	true ->
	    NPD = player_pack:del_item(Index, PlayerData),
	    NPD1 = player_coin:add_coin2(?GAMECOIN, SellPrice, NPD),
	    %% TODO: 物品log日志
	    {ok, NPD1};
	{false, Reason} ->
	    sys_msg:send(Account, Reason),
	    {false, item_cannot_sell}
    end;
do_sell_goods(Account, ItemID, Index, PlayerData) ->
    erlang:error({bagarg, Account, ItemID, Index, PlayerData}).

%% --------------------------test-------------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

new_player_data(GridCount) ->
    PD1 = player_data:new(),
    PD2 = player_data:set_pack(?pt_bag, pack:new(GridCount), PD1),
    player_data:set_coin(player_coin:new("test", 10000, 10000), PD2).

do_sell_goods_test()->
    erlymock:start(),
    erlymock:stub(tplt, get_data2, [item_tplt, 1], [{return, #item_tplt{sell_price=1}}]),
    erlymock:stub(tplt, is_exist, [item_tplt, 1], [{return, true}]),
    erlymock:replay(),

    PD = new_player_data(8),
    PD1 = player_pack:add_item(test, item:new(1,1,#stime{}), PD),
    do_sell_goods(test, 1, 0, PD1),
    erlymock:verify(),
    ok.

-endif.
