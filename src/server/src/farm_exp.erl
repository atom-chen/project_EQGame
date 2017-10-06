%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  农场经验等级相关
%%% @end
%%% Created : 18 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_exp).

-include("tplt_def.hrl").

-export([get_level/1, get_curr_exp/1, get_work_exp/3]).

%% 获得农场等级
-spec get_level(non_neg_integer()) -> non_neg_integer().
get_level(TotalExp)->
    DL = tplt:get_all_data(farm_level_tplt),
    do_get_level(TotalExp, DL).

do_get_level(_Exp, [#farm_level_tplt{level=Level}]) ->
    Level;
do_get_level(Exp, [#farm_level_tplt{next_exp=NextExp}=L | _Rest]) when Exp < NextExp ->
    L#farm_level_tplt.level;
do_get_level(Exp, [#farm_level_tplt{next_exp=NextExp} | Rest]) ->
    do_get_level(Exp - NextExp, Rest).

%% 获得农场的当前经验
-spec get_curr_exp(non_neg_integer()) -> non_neg_integer().
get_curr_exp(TotalExp) ->
    DL = tplt:get_all_data(farm_level_tplt),
    do_get_curr_exp(TotalExp, DL).

do_get_curr_exp(Exp, [#farm_level_tplt{next_exp=NExp} | Rest]) when Exp >= NExp ->
    do_get_curr_exp(Exp - NExp, Rest);
do_get_curr_exp(Exp, [#farm_level_tplt{next_exp=NExp} | _Rest]) when Exp < NExp ->
    Exp.

%% 获得农场劳动能够得到的经验
get_work_exp(_FarmExp, pick, SeedID) ->
    #farm_seed_tplt{pick_exp=Exp} = tplt:get_data(farm_seed_tplt, SeedID),
    Exp;
get_work_exp(FarmExp, Type, _SeedID) ->
    DL = tplt:get_all_data(farm_level_tplt),
    do_get_work_exp(FarmExp, Type, DL).

do_get_work_exp(_FarmExp, Type, [Tplt]) ->
    get_work_exp_1(Type, Tplt);
do_get_work_exp(FarmExp, Type, [#farm_level_tplt{next_exp=F}=Tplt | _Rest]) when FarmExp < F ->
    get_work_exp_1(Type, Tplt);
do_get_work_exp(FarmExp, Type, [_F | Rest]) ->
    do_get_work_exp(FarmExp, Type, Rest).    

get_work_exp_1(sow, #farm_level_tplt{sow_exp=Exp}) ->
    Exp;
get_work_exp_1(water, #farm_level_tplt{water_exp=Exp}) ->
    Exp;
get_work_exp_1(weed, #farm_level_tplt{weed_exp=Exp}) ->
    Exp;
get_work_exp_1(worm, #farm_level_tplt{worm_exp=Exp}) ->
    Exp;
get_work_exp_1(hoeing, #farm_level_tplt{hoeing_exp=Exp}) ->
    Exp.

%%---------------------------test-----------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).
do_get_level_test() ->
    L = [#farm_level_tplt{level=1, next_exp=10},
	 #farm_level_tplt{level=2, next_exp=20},
	 #farm_level_tplt{level=3, next_exp=30},
	 #farm_level_tplt{level=4, next_exp=40}
	],
    ?assertEqual(1, do_get_level(9, L)),
    ?assertEqual(2, do_get_level(19, L)),
    ?assertEqual(3, do_get_level(39, L)),
    ?assertEqual(3, do_get_level(40, L)),
    ?assertEqual(4, do_get_level(65, L)),
    ok.

do_get_curr_exp_test() ->
    DL = [#farm_level_tplt{level=1, next_exp=100},
	  #farm_level_tplt{level=2, next_exp=200},
	  #farm_level_tplt{level=3, next_exp=300},
	  #farm_level_tplt{level=4, next_exp=400}
	 ],
    ?assertEqual(99, do_get_curr_exp(99, DL)),
    ?assertEqual(99, do_get_curr_exp(199, DL)),
    ?assertEqual(70, do_get_curr_exp(670, DL)),
    ?assertEqual(270, do_get_curr_exp(570, DL)),
    ok.

-endif.

