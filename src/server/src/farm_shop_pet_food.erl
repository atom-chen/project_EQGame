%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 农场商店--狗粮
%%% @end
%%% Created : 22 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm_shop_pet_food).

-include("tplt_def.hrl").
-include("sys_msg.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").

-define(min_buy_count, 1).
-define(max_buy_count, 99).

%% API
-export([can_buy_goods/10, do_buy_goods/10]).

%%%===================================================================
%%% API
%%%===================================================================
can_buy_goods(Account, GamePrice, EQPrice, Count, NeedLevel, ItemId, RepeatBuy, BuyType, PayType, Farm) ->
    Depot = farm_depot:read(Account),
    GridIndex = farm_depot:get_index_by_tempid(ItemId, Depot),
    case (RepeatBuy =:= 1) or ((RepeatBuy =:= 0) and (GridIndex =:= not_found)) of
	true ->
	    case farm_exp:get_level(player_farm:get_exp(Farm)) >= NeedLevel of
		true ->
		    case PayType == ?pgt_game_gold of
			true ->
			    case (farm_coin:get_coin(Farm) >= (GamePrice * Count)) and (GamePrice > 0) of
				true ->  
				    validate_buy_type(BuyType, Farm);
				false -> {false, ?err_not_enough_game_coin}
			    end;
			false ->
			    case (player_coin:get_coin(?EQCOIN, Account) >= (EQPrice * Count)) and (EQPrice > 0) of
				true ->
				    validate_buy_type(BuyType, Farm);
				false -> {false, ?err_not_enough_eq_coin}
			    end
		    end;
		false ->
		    {false, ?err_not_enough_level}
	    end;
	_ ->
	    {false, ?err_exist_purchase_item}
    end.

do_buy_goods(Account, ItemID, Count, GamePrice, EQPrice, AddExp, BuyType, PayType, Farm, Rewards) ->
    %% 扣除相应的货币
    NCount = util:middle(Count, ?min_buy_count, ?max_buy_count),
    
    {NFarm, TGamePrice, TEQPrice} = farm_shop:pay_coin(Account, PayType, GamePrice, EQPrice, NCount, Farm),

    NewItem = item:make_item(ItemID),
    Depot = farm_depot:add_item(Account, NewItem, NCount),
    {NDepot, UseList} = farm_shop:do_rewards(Rewards, Depot, []),

    Exp = player_farm:get_exp(NFarm),
    NNFarm = player_farm:add_exp(NFarm, Exp + AddExp),

    db_farm:write(NNFarm, NDepot),
    farm_coin:send_coin(Account, NFarm),
    farm_coin:send_eq_coin(Account),
    player_farm:notify_farm_data(Account, Account, NNFarm),

    %% 物品金钱后台记录
    ItemInfo = {ItemID, Count, item:get_instid(NewItem)},
    MoneyInfo = {-TGamePrice, -TEQPrice},
    item_money_log:log(Account, farm_buy_item, ItemInfo, MoneyInfo),

    %% 货币, 物品的log记录
    farm_log:write(buy, Account, ItemID, NCount, TGamePrice, 
		   farm_coin:get_coin(NFarm)),
    case BuyType == ?bgt_buy of
	true ->
	    farm_depot:send_depot(Account, NDepot),
	    farm_sys_msg:send(Account, ?msg_pet_food_buy_success);
	false ->
	    farm_item:use(Account, ItemID, NDepot, Count)
    end,
    farm_shop:do_use_rewards(Account, UseList, NDepot).

%%%===================================================================
%%% Internal functions
%%%===================================================================
validate_buy_type(BuyType, Farm) ->
    case BuyType == ?bgt_buy of
	true ->
	    true;
	false ->
	    Decoration = player_farm:get_decoration(Farm),
	    case farm_decoration:is_exist_dog(Decoration) of
		true ->
		    true;
		false ->
		    {false, ?msg_farm_does_not_have_dog}
	    end
    end.
