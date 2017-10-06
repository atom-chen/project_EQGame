%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  系统商城
%%% @end
%%% Created : 21 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(sysshop).

-include("packet_def.hrl").
-include("router.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("player_data.hrl").

-export([start/1]).

-export([handle_cast/2]).

start(Account) ->
    [
     router:make_event_source(?msg_req_buy_sys_shop_goods, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_buy_sys_shop_goods_list, Account, {self(), ?MODULE})
     %% router:make_event_source(?msg_req_replace_room_tex, Account, {self(), ?MODULE})
    ].

handle_cast({#msg{src=Account}, #req_buy_sys_shop_goods{goods_id=GoodsID}}, PlayerData) ->
    {_Result,NPlayerData}= do_buy_goods(Account, GoodsID, PlayerData),   
    {noreply,NPlayerData};

%% 家装模式下购买商品
handle_cast({#msg{src=Account}, #req_buy_sys_shop_goods_list{goods_list=GoodsList}}, 
	    PlayerData) ->
    {noreply, do_buy_goods_list(Account, GoodsList, PlayerData)}.

%%替换墙纸
%% handle_cast({#msg{event=?msg_req_replace_room_tex,src=Account},#req_replace_room_tex{room_tex_info=RoomTexInfo,goods_id=GoodsID}}, PlayerData)->
%%     case do_buy_goods(Account,GoodsID,PlayerData, true) of
%% 	{ok, NPlayerData} ->
%% 	    SceneName = player_data:get_scene_name(NPlayerData),
%% 	    router:send(SceneName,replace_room_tex,RoomTexInfo),
%% 	    {noreply, NPlayerData};
%% 	_->{noreply, PlayerData}
%%     end.

%% 系统商城中购买商品
do_buy_goods(Account, GoodsID, PlayerData) when GoodsID > 0->
    #sys_shop_tplt{item_id=ItemID, price=Price, money_type=MoneyType, type=Kind,
		   broadcast_id=BroadcastID} = tplt:get_data(sys_shop_tplt, GoodsID),
    
    %%case shop:buy_goods(Account, ItemID, {?EQCOIN, Price}, PlayerData) of
    case shop:buy_goods(Account, ItemID, {MoneyType, Price}, PlayerData) of
	{ok, NPD} -> 
	    case BroadcastID > 0 of
		true ->
		    BasicData = player_data:get_player_basic_data(PlayerData),
		    UserName = basic_data:get_data(username, BasicData),

		    ItemName = item:get_item_name(ItemID),
		    %% 发送系统广播
		    sys_broadcast:send(BroadcastID, [UserName, ItemName]);		    
		_ ->
		    ok
	    end,

	   case get_buy_action(Kind) of	       
	       undefined -> {ok, NPD};
	       BuyAction -> %% 每日任务
		   {ok, daily_task:inc_action_amount(BuyAction, NPD)}				
	   end;

	{error, _Reason} -> 
	    {error, PlayerData}
    end;
do_buy_goods(Account, GoodsID, _PlayerData) ->
    erlang:error({bagarg, Account, GoodsID}).

is_room_tex(Good)->
    Good#furniture_goods_data.tex =/= "".

good_to_roomtex(Good)->
    #furniture_goods_data{room_id=RoomId, type=Type, floor=Floor, tex=Tex}=Good,
    #room_tex{room_id=RoomId, type=Type, floor_id=Floor, tex=Tex}.

send_room_tex(SceneName, GoodList) ->
    RoomTexList = [good_to_roomtex(Good) || Good <- GoodList],
    router:send(SceneName, replace_room_tex, RoomTexList).

%% 家装模式下购买商品
do_buy_goods_list(Account, GoodsList, PlayerData) when is_list(GoodsList)->
    %% 判断是否有足够的金钱来购买商品
    {CostGameCoin, CostEQCoin} = calc_goods_price(GoodsList),
    {GC, EC} = player_coin:get_coin(player_coin:read(Account)),

    case (GC >= CostGameCoin) and (EC >= CostEQCoin) of
	true ->
	    %% 通知场景增加家具
	    {RoomTexGoodsList, FurnitureGoodsList} = lists:partition(fun(Good)->is_room_tex(Good)end, GoodsList),
	    FurnitureList = make_furniture_list(FurnitureGoodsList),
	    Scene = player_data:get_scene_name(PlayerData),
	    ok = router:send(Scene, add_furniture_list, {Account, FurnitureList}),

	    NPlayerData = player_coin:dec_coin({CostGameCoin, CostEQCoin}, PlayerData),

	    %% 增加家装值
	    TotalDecoration = calc_goods_decoration(FurnitureGoodsList),
	    player_decoration:add_decoration(Account, TotalDecoration),

	    %% 金钱使用记录
	    FurInfoList = [{furniture:get_itemid(Fur), 1, furniture:get_instid(Fur)}
	     		     || Fur <- FurnitureList],
	    MoneyInfo = {-CostGameCoin, -CostEQCoin},

	    item_money_log:log(Account, buy_furnitures, FurInfoList, MoneyInfo),

	    %% 提示获得物品
	    [sys_msg:send(Account, ?msg_get_item, [item:get_item_name(ItemTempID), ItemCount]) 
	     || {ItemTempID, ItemCount, _} <- FurInfoList],

	    %% 墙纸消息
	    send_room_tex(Scene, RoomTexGoodsList),

	    NPlayerData;
	false ->
	    case GC < CostGameCoin of
		true ->
		    sys_msg:send(Account, ?err_not_enough_game_coin);
		_ ->
		    sys_msg:send(Account, ?err_not_enough_eq_coin)
	    end,
	    PlayerData
    end.

calc_goods_decoration(GoodsList) when is_list(GoodsList)->
    F = fun(#furniture_goods_data{goods_id=GoodsID}, Total) ->
		Tplt = tplt:get_data(sys_shop_tplt, GoodsID),
		ItemID = Tplt#sys_shop_tplt.item_id,
		#item_tplt{decoration=Decoration} = tplt:get_data(item_tplt, ItemID),
		Total + Decoration
	end,
    lists:foldl(F, 0, GoodsList).


calc_goods_price(GoodsList) when is_list(GoodsList)->    
    F = fun(#furniture_goods_data{goods_id=GoodsID}, {GameCoin, EQCoin}) ->
		{G, E} = get_goods_price(GoodsID),
		{GameCoin + G, EQCoin + E}
	end,
    lists:foldl(F, {0, 0}, GoodsList).

get_goods_price(GoodsID) ->
    case tplt:get_data(sys_shop_tplt, GoodsID) of
	#sys_shop_tplt{price=Price, money_type=?GAMECOIN} -> {Price, 0};
	#sys_shop_tplt{price=Price, money_type=?EQCOIN} -> {0, Price};
	empty -> erlang:error({badarg, GoodsID})
    end.

make_furniture_list(GoodsList) when is_list(GoodsList) ->
    [begin Tplt = tplt:get_data(sys_shop_tplt, GoodsID),
	   ItemID = Tplt#sys_shop_tplt.item_id,
	   #item_tplt{sub_id=FurnitureID} = tplt:get_data(item_tplt, ItemID),
	   DelTime = item:get_del_time(ItemID),
	   InstID = guid:make(?st_furniture),
	   furniture:new(ItemID, InstID, FurnitureID, 
		       X, Z, Height, Floor, Face, DelTime)
     end || #furniture_goods_data{goods_id=GoodsID, x=X, z=Z, height=Height,
				  floor=Floor, face=Face} <- GoodsList].

%% tat_buy_shoes,	% 玩家购买一双鞋子 171-175
%% tat_buy_clothes,	% 玩家购买一件衣服 111-153
%% tat_buy_desk,	% 玩家购买一张桌子 211-215
%% tat_buy_chair,	% 玩家购买一把椅子 221-225
%% tat_buy_storage,	% 玩家购买一个柜子 231-236
%% tat_buy_electric,	% 玩家购买一件电器 311-336
%% tat_buy_hat,		% 玩家购买一顶帽子 161-165
%% tat_buy_decoration	% 玩家购买一件装饰品181-184
get_buy_action(K) when (171 =< K) and (K =< 175) ->
    ?tat_buy_shoes;
get_buy_action(K) when (111 =< K) and (K =< 153) ->
    ?tat_buy_clothes;
get_buy_action(K) when (211 =< K) and (K =< 215) ->
    ?tat_buy_desk;
get_buy_action(K) when (221 =< K) and (K =< 225) ->
    ?tat_buy_chair;
get_buy_action(K) when (231 =< K) and (K =< 236) ->
    ?tat_buy_storage;
get_buy_action(K) when (311 =< K) and (K =< 336) ->
    ?tat_buy_electric;
get_buy_action(K) when (161 =< K) and (K =< 165) ->
    ?tat_buy_hat;
get_buy_action(K) when (181 =< K) and (K =< 184) ->
    ?tat_buy_decoration;
get_buy_action(_K) ->
    undefined.
