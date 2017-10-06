%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 农场商店道具类
%%% @end
%%% Created : 22 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm_shop_prop).

-include("tplt_def.hrl").

%% API
-export([can_buy_goods/10, do_buy_goods/10]).

%%%===================================================================
%%% API
%%%===================================================================
can_buy_goods(Account, GamePrice, EQPrice, Count, NeedLevel, ItemID, RepeatBuy, BuyType, PayType, Farm) ->
    #farm_prop_tplt{type=Type} = tplt:get_data2(farm_prop_tplt, ItemID),
    data_helper:format("Type:~p~n", [Type]),
    Mod = get_proc_fun(Type),
    Mod:can_buy_goods(Account, GamePrice, EQPrice, Count, NeedLevel, ItemID, RepeatBuy, BuyType, PayType, Farm).

do_buy_goods(Account, ItemID, Count, GamePrice, EQPrice, AddExp, BuyType, PayType, Farm, Rewards) ->
    #farm_prop_tplt{type=Type} = tplt:get_data2(farm_prop_tplt, ItemID),
    Mod = get_proc_fun(Type),
    Mod:do_buy_goods(Account, ItemID, Count, GamePrice, EQPrice, AddExp, BuyType, PayType, Farm, Rewards).


%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 狗粮
get_proc_fun(7) ->
    farm_shop_pet_food;
get_proc_fun(_Type) ->
    farm_shop_prop_other.
