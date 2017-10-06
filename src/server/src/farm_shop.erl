%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  农场商店
%%% @end
%%% Created : 17 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_shop).

-include("packet_def.hrl").
-include("router.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("sys_msg.hrl").
-include("enum_def.hrl").

-define(min_buy_count, 1).
-define(max_buy_count, 99).

-export([start/1, can_buy_goods/10, do_buy_goods/10, handle_cast/2]).
-export([do_rewards/3, do_use_rewards/3, validate_coin/6, pay_coin/6]).

start(Account) -> 
    [router:make_event_source(?msg_req_buy_farm_shop_goods, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_sell_farm_shop_goods, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_sell_farm_shop_all_goods, Account, {self(), ?MODULE})
    ].

%% 买物品
handle_cast({#msg{src=Account}, #req_buy_farm_shop_goods{item_id=ItemID, count=Count, type=BuyType, pay_type=PayType}}, State) ->
    io:format("BuyType:~p, PayType:~p~n", [BuyType, PayType]),
    #farm_shop_tplt{gameprice=GamePrice, eqprice=EQPrice, 
		    level=NeedLevel, repeat_buy=RepeatBuy, add_exp=AddExp, rewards=Rewards}
	= tplt:get_data2(farm_shop_tplt, ItemID),
    Farm = player_farm:get_farm_data(Account),
    NFarm = player_farm:refresh_farm_state(Farm),
    NAddExp = 
	case AddExp > 0 of
	    true ->
		case wallow:in_warn_time(State) of
		    true ->
			farm_sys_msg:send(Account, ?err_wallow_buy_good_warn_time);
		    false ->
			case wallow:in_bad_time(State) of
			    true ->
				farm_sys_msg:send(Account, ?err_wallow_buy_good_bad_time);
			    false ->
				ok
			end
		end,
		wallow:calc_income(AddExp, State); %% 加入防沉迷控制经验
	    false ->
		AddExp
	end,
    buy_goods(Account, ItemID, Count, GamePrice, EQPrice, NeedLevel, RepeatBuy, NAddExp, BuyType, PayType, NFarm, Rewards),
    {noreply, State};

%% 卖物品
handle_cast({#msg{src=Account}, 
	     #req_sell_farm_shop_goods{instance_id=InstID, count=Count}}, State) ->
    case wallow:in_normal_time(State) of
	true ->
	    Depot = farm_depot:read(Account),
	    Farm = player_farm:get_farm_data(Account),
	    GridIndex = farm_depot:get_index(InstID, Depot),
	    IsLocked = pack:is_lock(GridIndex, farm_depot:get_grids(Depot)), 
	    Item = farm_depot:get_item(GridIndex, Depot),
	    #item_tplt{sell_price=SellPrice} = tplt:get_data2(item_tplt, item:get_tempid(Item)),
	    NCount = util:middle(Count, 1, farm_depot:get_item_count(InstID, Depot)),
	    Coin = farm_coin:get_coin(Farm),
	    case can_sell_item(SellPrice, NCount, Coin, IsLocked) of
		true ->
		    {Depot1, NCoin}=
			sell_goods(GridIndex, NCount, SellPrice, Depot, Coin),
		    %% TODO: 货币, 物品的log记录
		    ItemInfo = {item:get_tempid(Item), -Count, InstID},
		    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, calc_sell_price(SellPrice, Count)),
		    item_money_log:log(Account, farm_sell_item, ItemInfo, MoneyInfo),

		    %%farm_log:write(sell,Account,TempID,Count,player_coin:get_coin(?GAMECOIN, PlayerCoin), 
		    %% player_coin:get_coin(?GAMECOIN, PlayerCoin)),
		    %% db_common:write([farm_coin:set_coin(NCoin, Farm), NDepot]),
		    NDepot = farm_depot:compress(Depot1),
		    db_farm:write(farm_coin:set_coin(NCoin, Farm), NDepot),
		    farm_depot:send_depot(Account, NDepot),
		    farm_coin:send_coin(Account, NCoin),
		    send_sell_success_msg(Account, NCount, 
					  GridIndex, Depot, calc_sell_price(SellPrice, Count)),
		    {noreply, State};
		{false, Reason} ->
		    farm_sys_msg:send(Account, Reason),
		    {noreply, State}
	    end;
	false ->
	    farm_sys_msg:send(Account, ?err_wallow_sell_good),
	    {noreply, State}
    end; 

%% 卖出指定类型的所有物品
handle_cast({#msg{src=Account}, #req_sell_farm_shop_all_goods{type=Type}}, State) ->
    case wallow:in_normal_time(State) of
	true ->
	    Depot = farm_depot:read(Account),
	    Farm = player_farm:get_farm_data(Account),
	    Coin = farm_coin:get_coin(Farm),
	    case can_sell_all_goods(Depot, Type, Coin) of
		{true, Income, ItemInfoList} ->
		    {Depot1, NCoin} = sell_all_goods(Depot, Type, Coin), 

		    %% TODO: 货币, 物品的log记录
		    ItemInfo = ItemInfoList,
		    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, Income),
		    item_money_log:log(Account, farm_sell_items, ItemInfo, MoneyInfo),
		    %% farm_log:write(sell,Account,TempID,Count,player_coin:get_coin(?GAMECOIN,PlayerCoin), 
		    %% 		   player_coin:get_coin(?GAMECOIN, PlayerCoin)),
		    %% db_common:write([farm_coin:set_coin(NCoin, Farm), NDepot]),
		    NDepot = farm_depot:compress(Depot1),
		    db_farm:write(farm_coin:set_coin(NCoin, Farm), NDepot),
		    farm_coin:send_coin(Account, NCoin),
		    farm_depot:send_depot(Account, NDepot),
		    send_sell_all_success_msg(Account, NCoin, Coin);
		{false, Reason} ->
		    farm_sys_msg:send(Account, Reason)
	    end;
	false ->
	    farm_sys_msg:send(Account, ?err_wallow_sell_good)
    end,
    {noreply, State}.

send_sell_success_msg(Account, NCount, GridIndex, Depot, SellPrice)->
    Item = farm_depot:get_item(GridIndex, Depot),
    #item_tplt{name=Name} = tplt:get_data2(item_tplt, item:get_tempid(Item)),
    farm_sys_msg:send(Account, ?msg_farm_shop_sell_success, 
		 [NCount, binary_to_list(Name), SellPrice]).

send_sell_all_success_msg(Account, NCoin, Coin) ->
    SellPrice = NCoin - Coin,
    farm_sys_msg:send(Account, ?msg_farm_shop_sell_all_success, [SellPrice]).
    
 
%% 购买商品
buy_goods(Account, ItemID, Count, GamePrice, EQPrice, NeedLevel, RepeatBuy, AddExp, BuyType, PayType, Farm, Rewards) 
  when ItemID > 0, ((GamePrice > 0) or (EQPrice > 0)) ->
    %% 玩家的当前状态是否能购买商品
    #item_tplt{type=Type}=tplt:get_data2(item_tplt, ItemID),
    Mod = get_proc_fun(Type),
    case Mod:can_buy_goods(Account, GamePrice, EQPrice, Count, NeedLevel, ItemID, RepeatBuy, BuyType, PayType, Farm) of
	true -> 
	    Result = Mod:do_buy_goods(Account, ItemID, Count, GamePrice, EQPrice, AddExp, BuyType, PayType, Farm, Rewards),

	    Result;
	{false, Reason} ->
	    farm_sys_msg:send(Account, Reason)
    end.

get_proc_fun(6) ->
    farm_shop_prop;
get_proc_fun(7) ->
    farm_shop_decoration;
get_proc_fun(_Type) ->
    farm_shop.

validate_coin(Account, PayType, GamePrice, EQPrice, Count, Farm) ->
    case PayType == ?pgt_game_gold of
	true ->
	    case (farm_coin:get_coin(Farm) >= (GamePrice * Count)) and (GamePrice > 0) of
		true ->  true;
		false -> {false, ?err_not_enough_game_coin}
	    end;
	false ->
	    case (player_coin:get_coin(?EQCOIN, Account) >= (EQPrice * Count)) and (EQPrice > 0) of
		true -> true;
		false -> {false, ?err_not_enough_eq_coin}
	    end
    end.

pay_coin(Account, PayType, GamePrice, EQPrice, Count, Farm) ->
    case PayType == ?pgt_game_gold of
	true ->
	    GamePrice1 = GamePrice*Count,
	    Farm1 = farm_coin:dec_coin(GamePrice1, Farm),
	    {Farm1, GamePrice1, 0};
	false ->
	    EQPrice1 = EQPrice * Count,
	    player_coin:dec_coin(?EQCOIN, EQPrice1, Account),
	    {Farm, 0, EQPrice1}
    end.

%% 判断是否能购买商品
can_buy_goods(Account, GamePrice, EQPrice, Count, NeedLevel, ItemId, RepeatBuy, _BuyType, PayType, Farm) ->
    Depot = farm_depot:read(Account),
    GridIndex = farm_depot:get_index_by_tempid(ItemId, Depot),
    case (RepeatBuy =:= 1) or ((RepeatBuy =:= 0) and (GridIndex =:= not_found)) of
	true ->
	    case farm_exp:get_level(player_farm:get_exp(Farm)) >= NeedLevel of
		true ->
		    validate_coin(Account, PayType, GamePrice, EQPrice, Count, Farm);
		false ->
		    {false, ?err_not_enough_level}
	    end;
	_ ->
	    {false, ?err_exist_purchase_item}
    end.

do_buy_goods(Account, ItemID, Count, GamePrice, EQPrice, AddExp, _BuyType, PayType, Farm, Rewards) ->
    %% 扣除相应的货币
    NCount = util:middle(Count, ?min_buy_count, ?max_buy_count),
    {NFarm, TGamePrice, TEQPrice} = pay_coin(Account, PayType, GamePrice, EQPrice, NCount, Farm),
    NewItem = item:make_item(ItemID),
    Depot = farm_depot:add_item(Account, NewItem, NCount),
    {NDepot, UseList} = do_rewards(Rewards, Depot, []),

    Exp = player_farm:get_exp(NFarm),
    NNFarm = player_farm:add_exp(NFarm, Exp + AddExp),
    db_farm:write(NNFarm, NDepot),

    farm_depot:send_depot(Account, NDepot),
    farm_coin:send_coin(Account, NFarm),
    farm_coin:send_eq_coin(Account),
    player_farm:notify_farm_data(Account, Account, NNFarm),
    %% 物品金钱后台记录
    ItemInfo = {ItemID, Count, item:get_instid(NewItem)},
    MoneyInfo = {-TGamePrice, TEQPrice},
    item_money_log:log(Account, farm_buy_item, ItemInfo, MoneyInfo),

    %% 货币, 物品的log记录(玩家用的)
    farm_log:write(buy, Account, ItemID, NCount, TGamePrice, 
		   farm_coin:get_coin(NFarm)),
    farm_sys_msg:send(Account, ?msg_farm_shop_buy_success, 
		      [farm_coin:diff_coin(Farm, NFarm), 0, NCount]),
    do_use_rewards(Account, UseList, NDepot),
    NNFarm.

%% 赋予玩家奖励的物品
do_rewards([], Depot, UseList) ->
    {Depot, UseList};
do_rewards([Reward|Rewards], Depot, UseList) ->
    NReward = binary_to_list(Reward),
    case NReward == "0" of
	true ->
	    {Depot, UseList};
	false ->
	    List = string:tokens(NReward, "-"),
	    ItemId = list_to_integer(lists:nth(1, List)),
	    Count = list_to_integer(lists:nth(2, List)),
	    IsUse = list_to_integer(lists:nth(3, List)),
	    {NDepot, NUseList} = do_create_reward(ItemId, Count, IsUse, Depot, UseList),
	    do_rewards(Rewards, NDepot, NUseList)
    end.

%% 使用奖励的物品
do_use_rewards(_Account, [], _Depot) ->
    ok;
do_use_rewards(Account, UseList, Depot) ->
    [farm_item:use(Account, item:get_tempid(Item), Depot, 1) || Item <- UseList].

%% 创建奖励的物品
do_create_reward(_ItemId, 0, _IsUse, Depot, UseList) ->
    {Depot, UseList};
do_create_reward(ItemId, Count, IsUse, Depot, UseList) ->
    Item = item:make_item(ItemId),
    NDepot = farm_depot:add_item(Depot, Item, 1),
    case IsUse == 1 of
	true ->  %% 赋予奖励并立即使用该物品
	    NUseList = [Item|UseList],
	    do_create_reward(ItemId, Count - 1, IsUse, NDepot, NUseList);
	false ->
	    do_create_reward(ItemId, Count - 1, IsUse, NDepot, UseList)
    end.

can_sell_item(SellPrice, _Count, _GameCoin, _IsLocked) when SellPrice =:= 0 ->
    {false, ?err_item_cannot_sell};
can_sell_item(SellPrice, Count, GameCoin, IsLocked) ->
    Income = calc_sell_price(SellPrice, Count),
    case GameCoin + Income > ?MAX_GAMECOIN of
	true ->
	    {false, ?msg_game_coin_overflow};
	false ->
	    case IsLocked of
		true ->
		    {false, ?err_grid_lock};
		_ ->
		    true
	    end
    end.

%% 卖出物品
sell_goods(Index, Count, SellPrice, Depot, Coin) ->
    NDepot = farm_depot:del_item(Index, Count, Depot),
    Income = calc_sell_price(SellPrice, Count),
    NCoin = farm_coin:add_coin(Income, Coin),
    {NDepot, NCoin}.

can_sell_all_goods(Depot, Type, GameCoin) ->
    F = fun(Index, _Value, {TotalPrice, List}=Acc) ->
		case farm_depot:get_item(Index, Depot) of
		    empty -> 
			Acc;
		    Item ->
			ItemID = item:get_tempid(Item),
			#item_tplt{type=ItemType, sell_price=SellPrice} = 
			    tplt:get_data2(item_tplt, ItemID),
			IsLocked = pack:is_lock(Index, farm_depot:get_grids(Depot)),
			case (Type =:= ItemType) and (not IsLocked) of
			    true ->
				InstID = item:get_instid(Item),
				Count = farm_depot:get_item_count(InstID, Depot),
				{TotalPrice + SellPrice * Count, [{ItemID, -Count, InstID} | List]};
			    false ->
				Acc
			end
		end
	end,
    {Income, ItemInfoList} = farm_depot:foldl(F, {0, []}, Depot),
    case Income =:= 0 of
	true ->
	    {false, ?msg_not_item_to_sell};
	false ->
	    case GameCoin + Income > ?MAX_GAMECOIN of
		true ->
		    {false, ?msg_game_coin_overflow};
		false ->
		    {true, Income, ItemInfoList}
	    end
    end.

%% 卖出所有的物品
sell_all_goods(Depot, Type, Coin)->
    F = fun(_, {Item, Count}, {Depot1, Coin1}) ->
		case item:is_empty(Item) of
		    true -> 
			{Depot1, Coin1};
		    false ->
			ItemID = item:get_tempid(Item),
			#item_tplt{type=ItemType, sell_price=SellPrice} = 
			    tplt:get_data2(item_tplt, ItemID),
			case Type =:= ItemType of
			    true ->
				Index = farm_depot:get_index_by_tempid(ItemID, Depot1),
				NDepot = farm_depot:del_item(Index, Count, Depot1),
				Income = calc_sell_price(SellPrice, Count),
				NCoin = farm_coin:add_coin(Income, Coin1),
				{NDepot, NCoin};
			    false ->
				{Depot1, Coin1}
			end
		end
	end,
    farm_depot:foldl(F, {Depot, Coin}, Depot).

%% 计算卖出价格
calc_sell_price(Price, Count) ->
    Price * Count.


%% ----------------test------------------------------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

sell_all_goods_test()->
    erlymock:start(),
    erlymock:stub(tplt, get_data2, [item_tplt, 1], [{return, #item_tplt{type=1, sell_price=1}}]),
    erlymock:replay(), 

    Depot = farm_depot:new(test),
    NDepot = farm_depot:add_item(Depot, item:new(1, 1, #stime{})),
    {NDepot1, _NPlayerCoin} = sell_all_goods(NDepot, 1, 100),
    ?assertEqual(empty, farm_depot:get_item(0, NDepot1)),

    erlymock:verify(),
    ok.

-endif.
