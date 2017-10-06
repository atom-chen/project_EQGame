%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 农场商店道具类
%%% @end
%%% Created : 22 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm_shop_decoration).

-include("tplt_def.hrl").
-include("sys_msg.hrl").
-include("enum_def.hrl").

-define(min_buy_count, 1).
-define(max_buy_count, 99).

%% API
-export([can_buy_goods/10, do_buy_goods/10]).

%%%===================================================================
%%% API
%%%===================================================================
can_buy_goods(Account, GamePrice, EQPrice, Count, NeedLevel, ItemId, RepeatBuy, BuyType, PayType, Farm) ->
    farm_shop:can_buy_goods(Account, GamePrice, EQPrice, Count, NeedLevel, ItemId, RepeatBuy, BuyType, PayType, Farm).

do_buy_goods(Account, ItemId, Count, GamePrice, EQPrice, AddExp, _BuyType, PayType, Farm, Rewards) ->
    #farm_decoration_tplt{type=Type} = tplt:get_data2(farm_decoration_tplt, ItemId),
    %% 扣除相应的货币
    NCount = util:middle(Count, ?min_buy_count, ?max_buy_count),
    {NFarm, TGamePrice, TEQPrice} = farm_shop:pay_coin(Account, PayType, GamePrice, EQPrice, NCount, Farm),
    NewItem = item:make_item(ItemId),
    Depot = farm_depot:add_item(Account, NewItem, NCount),
    {NDepot, UseList} = farm_shop:do_rewards(Rewards, Depot, []),

    Exp = player_farm:get_exp(NFarm),
    NNFarm = player_farm:add_exp(NFarm, Exp + AddExp),
    NNNFarm = do_use_decoration(Type, ItemId, NDepot, NNFarm),
    db_farm:write(NNNFarm, NDepot),
    farm_depot:send_depot(Account, NDepot),
    farm_coin:send_coin(Account, NNNFarm),
    farm_coin:send_eq_coin(Account),
    player_farm:notify_farm_data(Account, Account, NNNFarm),

    %% 物品金钱后台记录
    ItemInfo = {ItemId, Count, item:get_instid(NewItem)},
    MoneyInfo = {-TGamePrice, -TEQPrice},
    item_money_log:log(Account, farm_buy_item, ItemInfo, MoneyInfo),


    %% 货币, 物品的log记录
    farm_log:write(buy, Account, ItemId, NCount, TGamePrice, 
		   farm_coin:get_coin(NNNFarm)),
    do_notify(Type, Account, Farm, NNNFarm, NCount, TEQPrice),
    farm_shop:do_use_rewards(Account, UseList, NDepot).

do_notify(Type, Account, Farm, NNNFarm, NCount, TEQPrice) ->
    case Type == 4 of
	true ->
	    farm_sys_msg:send(Account, ?msg_farm_shop_buy_dog_success);
	false ->
	    farm_sys_msg:send(Account, ?msg_farm_shop_buy_success, 
			      [farm_coin:diff_coin(Farm, NNNFarm), TEQPrice, NCount])
    end.    

do_use_decoration(Type, ItemId, Depot, Farm) ->
    case Type == 4 of
	true ->
	    Decoration = player_farm:get_decoration(Farm),
	    case farm_decoration:is_exist_dog(Decoration) of
		true ->
		    Farm;
		false ->
		    Index = farm_depot:get_index_by_tempid(ItemId, Depot),
		    Item = farm_depot:get_item(Index, Depot),
		    farm_decoration:set(4, Farm, Decoration, item:get_instid(Item), item:get_tempid(Item))
	    end;
	false ->
	    Farm
    end.    

