%%% @author lyj <lyj@35info.cn>
%%% @copyright (C) 2010, lyj
%%% @doc
%%%  农场果实变异
%%% @end
%%% Created : 25 Nov 2010 by lyj <lyj@35info.cn>

-module(farm_aberrance).

-include("tplt_def.hrl").
-include("sys_msg.hrl").

-export([can_use/3, do_use/5, after_use/2]).

%% 能否在指定的农作物上使用变异物品
can_use(Farm, Index, _Item) ->
    Crop = player_farm:get_crop(Index, Farm),
    SeedID = farm_crop:get_seedid(Crop),
    %% 只能在模板表中规定的农作物才能使用
    case farm_crop:is_crop(Crop) of
	true ->
	    case tplt:get_data(farmfruit_aberrance_tplt, SeedID) of
		empty -> {false, ?msg_crop_cannot_aberrance};
		_Tplt ->
		    %% 农作物在采摘阶段和枯萎阶段是不能使用该类型的物品的
		    case farm_crop:is_crop_ripe(Crop) or farm_crop:is_crop_dead(Crop) of
			true -> {false, ?msg_aberrance_item_not_stage};
			false ->
			    %% 已经使用过物品的就不能再使用
			    case is_used_item(Crop) of
				true -> {false, ?msg_used_aberrance_item};
				false -> true
			    end
		    end
	    end;
	false -> {false, ?err_plot_empty}
    end.


%% 是否已经使用过变异物品了
is_used_item(Crop) ->
    0 =/= farm_crop:get_aberrance_fruit(Crop).

%% 在指定的农作物上使用变异物品
do_use(_Account, Farm, PlotIndex, _Tplt, _Item)->
    Crop = player_farm:get_crop(PlotIndex, Farm),
    SeedID = farm_crop:get_seedid(Crop),
    Tplt = tplt:get_data2(farmfruit_aberrance_tplt, SeedID),
    FruitID = get_aberrance_fruit_id(rand:uniform(100), Tplt),
    NCrop = farm_crop:set_aberrance_fruit(FruitID, Crop),
    NFarm = player_farm:set_crop(Farm, PlotIndex, NCrop),
    {NFarm, ok}.

after_use(_Account, _Reason) ->
    ok.

%% 根据随机值获得变异果实的id
get_aberrance_fruit_id(Random, Tplt) ->
    L = change_data(Tplt),
    L1 = [{Ratio, FruitID} || {Ratio, FruitID} <- L, Ratio =/= 0],
    do_get_aberrance_fruit_id(Random, L1).

do_get_aberrance_fruit_id(Random, [{Ratio, FruitID} | Rest]) ->
    case Random - Ratio > 0 of
	true ->
	    do_get_aberrance_fruit_id(Random - Ratio, Rest);
	false ->
	    FruitID
    end.

change_data(Tplt) ->
    [{Tplt#farmfruit_aberrance_tplt.ratio1, Tplt#farmfruit_aberrance_tplt.fruit_id1},
     {Tplt#farmfruit_aberrance_tplt.ratio2, Tplt#farmfruit_aberrance_tplt.fruit_id2},
     {Tplt#farmfruit_aberrance_tplt.ratio3, Tplt#farmfruit_aberrance_tplt.fruit_id3},
     {Tplt#farmfruit_aberrance_tplt.ratio4, Tplt#farmfruit_aberrance_tplt.fruit_id4},
     {Tplt#farmfruit_aberrance_tplt.ratio5, Tplt#farmfruit_aberrance_tplt.fruit_id5},
     {Tplt#farmfruit_aberrance_tplt.ratio6, Tplt#farmfruit_aberrance_tplt.fruit_id6},
     {Tplt#farmfruit_aberrance_tplt.ratio7, Tplt#farmfruit_aberrance_tplt.fruit_id7},
     {Tplt#farmfruit_aberrance_tplt.ratio8, Tplt#farmfruit_aberrance_tplt.fruit_id8},
     {Tplt#farmfruit_aberrance_tplt.ratio9, Tplt#farmfruit_aberrance_tplt.fruit_id9},
     {Tplt#farmfruit_aberrance_tplt.ratio10,Tplt#farmfruit_aberrance_tplt.fruit_id10},
     {Tplt#farmfruit_aberrance_tplt.ratio11,Tplt#farmfruit_aberrance_tplt.fruit_id11},
     {Tplt#farmfruit_aberrance_tplt.ratio12,Tplt#farmfruit_aberrance_tplt.fruit_id12}
    ].

%%------------------------test------------------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

get_aberrance_fruit_id_test() ->
    Tplt = #farmfruit_aberrance_tplt{ratio1=20, fruit_id1 = 1,
				     ratio2=30, fruit_id2 = 2,
				     ratio3=50, fruit_id3 = 3,
				     ratio4=0, fruit_id4 = 0,
				     ratio5=0, fruit_id5 = 0,
				     ratio6=0, fruit_id6 = 0,
				     ratio7=0, fruit_id7 = 0,
				     ratio8=0, fruit_id8 = 0,
				     ratio9=0, fruit_id9 = 0,
				     ratio10=0, fruit_id10 = 0,
				     ratio11=0, fruit_id11 = 0,
				     ratio12=0, fruit_id12 = 0
				    },
    ?assertEqual(1, get_aberrance_fruit_id(10, Tplt)),
    ?assertEqual(2, get_aberrance_fruit_id(30, Tplt)),
    ?assertEqual(3, get_aberrance_fruit_id(90, Tplt)),
    ok.

-endif.


