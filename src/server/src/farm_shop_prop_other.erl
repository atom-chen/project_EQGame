%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 农场商店--狗粮
%%% @end
%%% Created : 22 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm_shop_prop_other).

-include("tplt_def.hrl").
-include("sys_msg.hrl").

-define(min_buy_count, 1).
-define(max_buy_count, 99).

%% API
-export([can_buy_goods/10, do_buy_goods/10]).

%%%===================================================================
%%% API
%%%===================================================================
can_buy_goods(Account, GamePrice, EQPrice, Count, NeedLevel, ItemId, RepeatBuy, _BuyType, PayType, Farm) ->
    Depot = farm_depot:read(Account),
    GridIndex = farm_depot:get_index_by_tempid(ItemId, Depot),
    case (RepeatBuy =:= 1) or ((RepeatBuy =:= 0) and (GridIndex =:= not_found)) of
	true ->
	    case farm_exp:get_level(player_farm:get_exp(Farm)) >= NeedLevel of
		true ->
		    farm_shop:validate_coin(Account, PayType, GamePrice, EQPrice, Count, Farm);
		false ->
		    {false, ?err_not_enough_level}
	    end;
	_ ->
	    {false, ?err_exist_purchase_item}
    end.

do_buy_goods(Account, ItemID, Count, GamePrice, EQPrice, AddExp, _BuyType, PayType, Farm, Rewards) ->
    %% 扣除相应的货币
    NCount = util:middle(Count, ?min_buy_count, ?max_buy_count),
    {NFarm, TGamePrice, TEQPrice} = farm_shop:pay_coin(Account, PayType, GamePrice, EQPrice, NCount, Farm),
    NewItem = item:make_item(ItemID),
    Depot = farm_depot:add_item(Account, NewItem, NCount),
    {NDepot, UseList} = farm_shop:do_rewards(Rewards, Depot, []),

    Exp = player_farm:get_exp(NFarm),
    NNFarm = player_farm:add_exp(NFarm, Exp + AddExp),

    db_farm:write(NNFarm, NDepot),
    farm_depot:send_depot(Account, NDepot),
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
    farm_sys_msg:send(Account, ?msg_farm_shop_buy_success, 
		 [farm_coin:diff_coin(Farm, NFarm), TEQPrice, NCount]),
    farm_shop:do_use_rewards(Account, UseList, NDepot).


%%%===================================================================
%%% Internal functions
%%%===================================================================
