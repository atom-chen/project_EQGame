%%% @author lyj <lyj@35info.cn>
%%% @copyright (C) 2010, lyj
%%% @doc
%%% 土地(红土地)
%%% @end
%%% Created : 22 Nov 2010 by lyj <lyj@35info.cn>

-module(farm_land).

-include("sys_msg.hrl").
-include("packet_def.hrl").
-include("tplt_def.hrl").

-export([can_upgrade/4, upgrade/3, can_sow/2]).
-export([pick_redland/4]).

%% 1为种子，2为化肥道具，3为狗粮，4为金币。
%% rla = redland award
-define(rla_seed, 1).
-define(rla_fertilizer, 2).
-define(rla_petfood, 3).
-define(rla_coin, 4).

%% 能否能升级成红土地
-spec can_upgrade(pos_integer(), pos_integer(), pos_integer(), list()) 
		 -> {true, pos_integer(), list()} | {false, pos_integer(), list()}.
can_upgrade(Level, Money, MaxPlots, Crops) ->
    case length(Crops) =:= MaxPlots of
	false -> {false, ?msg_land_not_hoeing_all, []};
	true ->
	    Index = get_next_upgrade_land_index(MaxPlots, Crops),
	    case Index =:= -1 of
		true -> {false, ?msg_redland_over, []};
		false ->
		    #farmland_upgrade_tplt{level=NeedLevel, money=NeedMoney} = 
			tplt:get_data2(farmland_upgrade_tplt, Index),
		    can_upgrade_impl(Level, Money, NeedLevel, NeedMoney, Crops)
	    end
    end.

can_upgrade_impl(Level, Money, NeedLevel, NeedMoney, _Crops)
  when (Level<NeedLevel) and (Money<NeedMoney) ->
    %% 判断等级是否满足条件
    {false, ?msg_farm_land_level_money_not_enough, [NeedLevel, NeedMoney]};
can_upgrade_impl(Level, _Money, NeedLevel, NeedMoney, _Crops) 
  when Level < NeedLevel ->
    %% 判断等级是否满足条件
    {false, ?msg_farm_land_level_not_enough, [NeedLevel, NeedMoney]};
can_upgrade_impl(_Level, Money, NeedLevel, NeedMoney, _Crops) 
  when Money < NeedMoney ->
    %% 判断金钱是否满足条件
    {false, ?msg_farm_land_money_not_enough, [NeedLevel, NeedMoney]};
can_upgrade_impl(_Level, _Money, NeedLevel, NeedMoney, Crops) when is_list(Crops) ->
    {true, ?msg_upgrade_land, [NeedLevel, NeedMoney]}.

%% 升级成红土地
-spec upgrade(pos_integer(), pos_integer(), list()) -> {list(), pos_integer()}.
upgrade(MaxPlots, FarmMoney, Crops) when is_list(Crops) ->
    Index = get_next_upgrade_land_index(MaxPlots, Crops),
    #farmland_upgrade_tplt{money=NeedMoney} = tplt:get_data2(farmland_upgrade_tplt, Index),
    upgrade_impl(FarmMoney, NeedMoney, Index, Crops).

upgrade_impl(FarmMoney, NeedMoney, Index, Crops) ->
    Crop = farm_crop:get_crop(Crops, Index),
    NCrop = farm_crop:set_type(red, Crop),
    {player_farm:set_crop(Crops, Index, NCrop), FarmMoney - NeedMoney}.

%% 获得需要升级到下个红土地的土地索引
%% 如果都是红土地了, 则返回-1
get_next_upgrade_land_index(MaxPlots, Crops) ->
    Index = lists:foldl(fun(Crop, Index) ->
				case farm_crop:get_type(Crop) =:= normal of
				    true -> Index;
				    false -> Index + 1
				end
			end, 1, Crops),
    case Index > MaxPlots of
	true -> -1;
	false -> Index
    end.

%% 种子是否能种(只判断土地的性质)
-spec can_sow(pos_integer(), tuple()) -> boolean().
can_sow(SeedRedLand, Crop) ->
    case farm_crop:get_type(Crop) of
	normal ->
	    case SeedRedLand of
		2 -> false; % 该种子只能在红土地上种植
		1 -> true
	    end;
	red -> true
    end.

%% 采摘红土地上的果实
%% 返回值: [{Op, Data} | ...]
%% Op: item, coin, awarded
%% {item, {Item, Count}}, {coin, Count}, {awarded, AwardedList}
-spec pick_redland(atom(), atom(), list(), tuple()) -> list().
pick_redland(Laborer, Owner, AwardedList, Crop) ->
    %% 有钱才能采摘 
    case Laborer =:= Owner of
	true ->
	    pick_myself(Laborer, Crop, rand:uniform(100), AwardedList);
	false -> 
	    pick_other(Crop, rand:uniform(100), rand:uniform(100))
    end.

%% 采摘自己的农场
pick_myself(Laborer, Crop, Random1, AwardedList) when Random1 =< 5 ->
    case get_pick_self_awards(AwardedList) of
	no_awards ->
	    farm_crop:pick_normalland_fruit(Laborer, Laborer, Crop);
	#red_farmland_harvest_tplt{type=Type,data=Data,count=Count} ->
	    Items = farm_crop:pick_normalland_fruit(Laborer, Laborer, Crop),
	    Awards = do_pick_self_awards(Type, Data, Count),
	    Items ++ [{awarded, [Type | AwardedList]}, Awards]
    end;
pick_myself(Laborer, Crop, _Random1, _AwardedList) ->
    farm_crop:pick_normalland_fruit(Laborer, Laborer, Crop).

do_pick_self_awards(?rla_seed, ItemID, Count) ->
    {award_item, {item:make_item(ItemID), Count}};
do_pick_self_awards(?rla_fertilizer, ItemID, Count) ->
    {award_item, {item:make_item(ItemID), Count}};
do_pick_self_awards(?rla_petfood, ItemID, Count) ->
    {award_item, {item:make_item(ItemID), Count}};
do_pick_self_awards(?rla_coin, _Data, Count) ->
    {coin, Count}.

%% 采摘别人的农场
pick_other(Crop, Random1, Random2) ->
    {Money, Harvest} = calc_pick_other_award(Random1, Random2),
    ItemID = farm_crop:make_crop_fruit(Crop),
    Count = farm_crop:calc_pick_num(false, Crop, Harvest),
    [{coin, Money} || Money =/= 0] ++ 
	[{pick_item, {item:make_item(ItemID), Count}} || Count =/= 0].


%% 计算采摘别人农场的奖励
%% 返回值: {Money, 收成的倍数}, 如果money<0, 则表示扣钱
calc_pick_other_award(Random1, Random2) ->
    case Random1 > 60 of
    	true -> {0, 1};
    	false ->
    	    case Random2 > 50 of
    		true -> {-50, 0};
    		false -> {50, 2}
    	    end
    end.

%% 根据表格获得采摘奖励
get_pick_self_awards(AwardedList) ->
    AwardList = tplt:get_all_data(red_farmland_harvest_tplt),
    do_get_pick_all_awards(AwardedList, AwardList).

do_get_pick_all_awards(AwardedList, AwardList) ->
    AwardList1 = [Tplt || Tplt <- AwardList,
			  lists:member(Tplt#red_farmland_harvest_tplt.type, AwardedList)=:=false],
    Range = lists:sum([Tplt1#red_farmland_harvest_tplt.ratio || Tplt1 <- AwardList1]),
    case Range =:= 0 of
	true -> no_awards;
	false ->
	    Random = rand:uniform(Range),
	    calc_pick_award(Random, AwardList1)
    end.

%% 根据随机值计算具体的奖项的列
calc_pick_award(Random, [#red_farmland_harvest_tplt{ratio=Ratio}=Award | AwardList]) ->
    case Random > Ratio of
	true ->
	    calc_pick_award(Random - Ratio, AwardList);
	false ->
	    Award
    end.

%%------------------------test------------------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

can_upgrade_impl_test() ->
    Crops = farm_crop:new_crops(1, []),
    ?assertEqual({false, ?msg_farm_land_level_money_not_enough, [24, 200000]}, 
		 can_upgrade_impl(10, 10, 24, 200000, Crops)),
    ?assertEqual({false, ?msg_farm_land_level_not_enough, [24, 200000]}, 
		 can_upgrade_impl(10, 200000, 24, 200000, Crops)),
    ?assertEqual({false, ?msg_farm_land_money_not_enough, [24, 200000]}, 
		 can_upgrade_impl(30, 100000, 24, 200000, Crops)),
    ?assertEqual({true, ?msg_upgrade_land, [24, 200000]}, 
		 can_upgrade_impl(30, 200000, 24, 200000, Crops)),
    ok.

get_next_upgrade_land_index_test() ->
    Crops = farm_crop:new_crops(1, []),
    ?assertEqual(1, get_next_upgrade_land_index(1, Crops)),
    ?assertEqual(-1, get_next_upgrade_land_index(1, [farm_crop:set_type(red, Crop) 
						     || Crop <- Crops])),
    ok.

upgrade_impl_test() ->
    Crops = farm_crop:new_crops(1, []),
    ?assertEqual({[farm_crop:set_type(red, Crop) || Crop <- Crops], 90},
		 upgrade_impl(100, 10, 1, Crops)),
    ok.

can_sow_test() ->
    Crop = farm_crop:new_crop(1, []),
    ?assertEqual(true, can_sow(1, Crop)),
    ?assertEqual(false, can_sow(2, Crop)),
    ?assertEqual(true, can_sow(2, farm_crop:set_type(red, Crop))),
    ok.

calc_pick_award_test() ->
    D1 = #red_farmland_harvest_tplt{ratio=20},
    D2 = #red_farmland_harvest_tplt{ratio=40},
    D3 = #red_farmland_harvest_tplt{ratio=40},
    List = [D1, D2, D3],
    ?assertEqual(D1, calc_pick_award(10, List)),
    ?assertEqual(D2, calc_pick_award(50, List)),
    ?assertEqual(D2, calc_pick_award(60, List)),
    ?assertEqual(D3, calc_pick_award(61, List)),
    ok.

do_get_pick_all_awards_test() ->
    erlymock:start(),
    erlymock:stub(rand, uniform, [90], [{return, 15}]),
    erlymock:stub(rand, uniform, [60], [{return, 21}]),
    erlymock:stub(rand, uniform, [40], [{return, 25}]),
    erlymock:stub(rand, uniform, [50], [{return, 25}]),
    erlymock:replay(),

    D1 = #red_farmland_harvest_tplt{type=1, ratio=20},
    D2 = #red_farmland_harvest_tplt{type=2, ratio=30},
    D3 = #red_farmland_harvest_tplt{type=3, ratio=40},
    D4 = #red_farmland_harvest_tplt{type=3, ratio=10},

    ?assertEqual(D1, do_get_pick_all_awards([], [D1, D2, D3])),
    ?assertEqual(D3, do_get_pick_all_awards([], [D1, D3])),
    ?assertEqual(D3, do_get_pick_all_awards([], [D3])),
    ?assertEqual(D2, do_get_pick_all_awards([3], [D1, D2, D3, D4])),
    ?assertEqual(no_awards, do_get_pick_all_awards([], [])),
    ?assertEqual(no_awards, do_get_pick_all_awards([1, 2, 3], [D1, D2, D3, D4])),

    erlymock:verify(),
    ok.

-endif.

