%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  农场灾害相关的模块
%%% @end
%%% Created :  9 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_damage).
 
-include("enum_def.hrl").
-include("tplt_def.hrl").
-include("packet_def.hrl").

-export([make_all_damage/1, get_curr_damage/1, get_curr_damage/2, make_damage/4]).
-export([can_clear_damage/3, clear_drought_damage/1, get_damage_type/1, 
	 calc_damage_effect/2, get_damage_count/3, get_damage_data/2,
	 clear_curr_one_damage/3, refresh_all_damage/2]).

-define(max_same_damage_num, 3). % 允许同样灾害同时存在的个数

-record(farm_damage,           % 农场灾害
	{type,                 % 灾害的类型
	 putter,               % 放置灾害的玩家
	 time,                 % 灾害发生的时间
	 reduce}               % 每次影响健康度的值
       ). 

%% 是否能清除灾害
can_clear_damage(_Account, _Damage, []) ->
    false;
can_clear_damage(Account, Damage, [#farm_damage{type=T, putter=Putter} | _Rest]) 
  when Damage =:= T, Putter =/= Account ->
    true;
can_clear_damage(Account, Damage, [_D | Rest]) ->
    can_clear_damage(Account, Damage, Rest).


%% 清除干旱灾害
clear_drought_damage(DamageList)->
    DroughtList = [D || D <- DamageList, 
			D#farm_damage.type =:= ?fms_drought,
			datetime:diff_time(D#farm_damage.time, datetime:localtime()) < 0],
    RestList = [D || D <- DamageList, 
			D#farm_damage.type =/= ?fms_drought],
    DroughtList ++ RestList.

%% 清除当前的指定的灾害
-spec clear_curr_one_damage(atom(), pos_integer(), list()) -> list().
clear_curr_one_damage(Laborer, Damage, DamageList) ->
    do_clear_curr_one_damage(Laborer, datetime:localtime(), Damage, DamageList).

do_clear_curr_one_damage(Laborer, Now, Damage, DamageList) ->
    %% 从已经生成的灾害中挑出当前阶段的灾害
    F = fun(#farm_damage{time=Time, 
			 type=Type,
			 putter = Putter}=Dmg, {DmgL, Flag}) ->
		case Laborer =:= Putter of
		    true -> {[Dmg | DmgL], false};
		    false -> 
			case datetime:diff_time(Time, Now) >= 0 of
			    true -> 
				case Type =:= Damage of
				    true ->
					case Flag of
					    false -> {DmgL, true};
					    true -> {[Dmg | DmgL], true}
					end;
				    false -> {[Dmg | DmgL], false}
				end;
			    false -> {[Dmg | DmgL], false}
			end
		end
	end,
    {L, _Flag1} = lists:foldl(F, {[], false}, DamageList),
    L.

%% 获得灾害类型的列表
get_damage_type([]) ->
    [?fms_normal];
get_damage_type(DamageList) when is_list(DamageList) ->
    [CD#farm_damage.type || CD <- DamageList];
get_damage_type(#farm_damage{type=Type})->
    Type.

%% 计算灾害的影响
calc_damage_effect(_Now, #farm_damage{reduce=Reduce}) when is_integer(Reduce) ->
    Reduce;
calc_damage_effect(ElapseTime, FarmDmg) ->
    erlang:error({badarg, ElapseTime, FarmDmg}).

%% 获得农作物当前已经存在的灾害(已经得到的灾害)
%% 返回DamageList
%% 其中CurrDamage: [Damage], 当前新生成的灾害列表
%% RestDamage: [{Time, Damage}], 剩余时机未成熟的灾害列表
get_curr_damage(AllDamage) ->
    get_curr_damage(datetime:localtime(), AllDamage).

get_curr_damage(Now, AllDamage) ->
    %% 从已经生成的灾害中挑出当前阶段的灾害
    CurrDamage = [make_damage(Type, Time, Putter, Reduce)
		   || #farm_damage{time=Time, 
				   type=Type,
				   putter=Putter,
				   reduce=Reduce} <- AllDamage, 
		      datetime:diff_time(Time, Now) >= 0],
    DmgList = sort_damage(CurrDamage),
    %% 根据类型来排序, 干旱 > 虫灾 > 草灾
    %% 当前阶段每种灾害最多只能有三个
    F = fun(#farm_damage{type=Type}=Dmg, {DmgType, Count, DmgList1}) ->
		case Count >= ?max_same_damage_num of
		    true -> {DmgType, Count, DmgList1};
		    false -> 
			case Type =:= DmgType of
			    true -> {DmgType, Count + 1, [Dmg | DmgList1]};
			    false -> {DmgType, Count, DmgList1}
			end
		end
	end,
    {_, _, DL1} = lists:foldl(F, {?fms_drought, 0, []}, DmgList),
    {_, _, DL2} = lists:foldl(F, {?fms_grass, 0, []}, DmgList),
    {_, _, DL3} = lists:foldl(F, {?fms_pest, 0, []}, DmgList),
    NDL1 = case DL1 of
	       [] -> [];
	       [D | _Rest] -> [D]
	   end,
    NDL1 ++ DL2 ++ DL3.

%% 刷新所有的灾害
refresh_all_damage(Now, AllDamage)->
    RestDamage = [make_damage(Type, Time, Putter, Reduce)
		   || #farm_damage{time=Time, 
				   type=Type,
				   putter=Putter,
				   reduce=Reduce} <- AllDamage, 
		      datetime:diff_time(Time, Now) < 0],
    get_curr_damage(Now, AllDamage) ++ RestDamage.

%% 根据时间对灾害排序
sort_damage(DamageList) ->
    lists:sort(fun(#farm_damage{time=T1}, #farm_damage{time=T2}) ->
		       datetime:diff_time(T1, T2) > 0
	       end, DamageList).
    

%% 对种子生成所有灾害(所有的灾害在种子被种下去以后一次性生成所有的灾害)
make_all_damage(SeedData) when is_record(SeedData, farm_seed_tplt) ->
    DamageList = [?fms_drought, ?fms_pest, ?fms_grass],
    L = [do_make_damage(DT, SeedData, get_damage_data(DT, SeedData)) || DT <- DamageList],
    L1 = lists:flatten(L),
    lists:sort(L1).

do_make_damage(DT, SeedData, DamageData)->
    do_make_damage(datetime:localtime(), DT, SeedData, DamageData).

do_make_damage(Now, DT, SeedData, DamageData)->
    DamageHappen=lists:seq(0,get_crop_damage_time(SeedData),DamageData#farm_damage_tplt.interval),
    %% 每间隔一定的时间随机生成灾害
    [Dmg || T <- DamageHappen, 
	    begin Dmg = random_make_damage(Now, T, DT, DamageData, SeedData), 
		  Dmg =/= false 
	    end].

%% 随机生成灾害
random_make_damage(Now, T, DmgType, 
		   #farm_damage_tplt{reduce_health=RHealth}=DamageData, 
		   SeedData)->
    Stage = farm_crop:get_crop_stage(SeedData, T),
    DR = get_crop_damage_ratio(Stage, DamageData),
    case rand:uniform(100) =< DR of
	true ->
	    make_damage(DmgType, datetime:add_time(Now, T), RHealth);
	false ->
	    false
    end.

make_damage(Type, Time, Putter, Reduce) when Reduce > 0->
    #farm_damage{time=Time, type=Type, putter=Putter, reduce=Reduce}.
make_damage(Type, Time, Reduce) when  Reduce > 0->
    make_damage(Type, Time, [], Reduce).

%% 获得灾害的数据
get_damage_data(?fms_drought, SeedData)->
    tplt:get_data2(farm_damage_tplt, SeedData#farm_seed_tplt.drought_id);
get_damage_data(?fms_pest, SeedData) ->
    tplt:get_data2(farm_damage_tplt, SeedData#farm_seed_tplt.pest_id);
get_damage_data(?fms_grass, SeedData) ->
    tplt:get_data2(farm_damage_tplt, SeedData#farm_seed_tplt.grass_id).


%% 获得农作物会产生灾害的总时间
get_crop_damage_time(SeedData)->
    SeedData#farm_seed_tplt.fruit_time.

%% 获得每个阶段灾害发生几率
get_crop_damage_ratio(sprouting_stage, DamageData) ->
    DamageData#farm_damage_tplt.sprout_ratio;
get_crop_damage_ratio(s_leafing_stage, DamageData) ->
    DamageData#farm_damage_tplt.s_leaf_ratio;
get_crop_damage_ratio(b_leafing_stage, DamageData) ->
    DamageData#farm_damage_tplt.b_leaf_ratio;
get_crop_damage_ratio(blooming_stage, DamageData) ->
    DamageData#farm_damage_tplt.bloom_ratio;
get_crop_damage_ratio(fruiting_stage, DamageData) ->
    DamageData#farm_damage_tplt.fruit_ratio;
get_crop_damage_ratio(picking_stage, _DamageData) ->
    0;
get_crop_damage_ratio(die_stage, _DamageData) ->
    0.

get_damage_count(Damage, Now, AllDamage) when is_list(AllDamage)->
    CurrDmgList = [make_damage(Type, Time, Reduce)
		   || #farm_damage{time=Time, 
				   type=Type, 
				   reduce=Reduce} <- AllDamage, 
		      Damage =:= Type,
		      datetime:diff_time(Time, Now) >= 0],
    length(CurrDmgList).

%%------------------------test----------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

random_make_damage_test()->
    SeedData = #farm_seed_tplt{sprout_time = 10, s_leaf_time = 20, b_leaf_time = 30,
			   bloom_time = 40, fruit_time = 50, pick_time= 60,
			   drought_id = 1, pest_id = 2, grass_id = 3},
    DamageData = #farm_damage_tplt{sprout_ratio=100, s_leaf_ratio=100, b_leaf_ratio=100,
				   bloom_ratio = 100, fruit_ratio=100, interval=100,
				   reduce_health=10},    
    Now = datetime:make_time(2010,1,1,0,0,0),
    ?assertEqual(make_damage(?fms_drought, datetime:add_time(Now, 5), 10),
		 random_make_damage(Now, 5, ?fms_drought, DamageData, SeedData)).

do_make_damage_test() ->
    SeedData = #farm_seed_tplt{sprout_time = 10, s_leaf_time = 20, b_leaf_time = 30,
			   bloom_time = 40, fruit_time = 50, pick_time= 60,
			   drought_id = 1, pest_id = 2, grass_id = 3},
    DamageData = #farm_damage_tplt{sprout_ratio=100, s_leaf_ratio=100, b_leaf_ratio=100,
				   bloom_ratio = 100, fruit_ratio=100, interval=10, 
				   reduce_health=10},  
    Now = datetime:make_time(2010,1,1,0,0,0),
    Value = do_make_damage(Now, ?fms_drought, SeedData, DamageData),
    ?assertEqual([make_damage(?fms_drought, datetime:add_time(Now, 0), 10), 
		  make_damage(?fms_drought, datetime:add_time(Now, 10), 10), 
		  make_damage(?fms_drought, datetime:add_time(Now, 20), 10), 
		  make_damage(?fms_drought, datetime:add_time(Now, 30), 10), 
		  make_damage(?fms_drought, datetime:add_time(Now, 40), 10)],
		 Value),
    ok.

get_curr_damage_test() ->
    Now = datetime:make_time(2010,8,1,0,0,0),
    T1 = datetime:add_time(Now, 10),
    T2 = datetime:add_time(Now, 12),
    T3 = datetime:add_time(Now, 20),
    AllDamage=[make_damage(?fms_drought, T1, 10), 
	       make_damage(?fms_grass, T2, 10), 
	       make_damage(?fms_pest, T3, 10)],
    Expert = [make_damage(?fms_drought, T1, 10),
	      make_damage(?fms_grass, T2, 10)
	      ],
    ?assertEqual(Expert, get_curr_damage(datetime:add_time(Now, 15), AllDamage)),

    AllDamage1 = [make_damage(?fms_drought, T1, 10), 
		  make_damage(?fms_drought, T1, 10), 
		  make_damage(?fms_drought, T1, 10), 
		  make_damage(?fms_drought, T2, 10), 
		  make_damage(?fms_grass, T2, 10), 
		  make_damage(?fms_pest, T3, 10)],
    Expert1 = [make_damage(?fms_drought, T1, 10),
	       make_damage(?fms_grass, T2, 10)
	      ],
    Value = get_curr_damage(datetime:add_time(Now, 15), AllDamage1),
    ?assertEqual(Expert1, Value),
    ok.

can_clear_damage_test() ->
    Account = test,
    Damage = ?fms_grass,
    DamageList1 = [#farm_damage{type=?fms_pest, putter=ttt}, 
		  #farm_damage{type=?fms_grass, putter=Account}
		 ],
    DamageList2 = [#farm_damage{type=?fms_pest, putter=ttt}, 
		  #farm_damage{type=?fms_grass, putter=Account},
		  #farm_damage{type=?fms_grass, putter=ttt}
		 ],
    ?assertEqual(false, can_clear_damage(Account, Damage, [])),
    ?assertEqual(false, can_clear_damage(Account, Damage, DamageList1)),
    ?assertEqual(true, can_clear_damage(Account, Damage, DamageList2)),
    ok.

get_damage_count_test() ->
    Now = datetime:make_time(2010, 9, 13, 12, 1, 1),
    ?assertEqual(0, get_damage_count(3, Now, [])),

    AllDamage = [make_damage(3,{stime,2010,9,13,11,35,55}, ddd, 2),
		 make_damage(3,{stime,2010,9,13,11,35,6}, ddd,2),
		 make_damage(3,{stime,2010,9,13,11,35,5}, ddd,2),
		 make_damage(3,{stime,2010,9,13,11,35,3}, ddd,2),
		 make_damage(3,{stime,2010,9,13,11,34,58}, ddd,2),
		 make_damage(3,{stime,2010,9,13,11,34,44}, ddd,2),
		 make_damage(3,{stime,2010,9,13,11,34,39}, ddd,2)
		],
    ?assertEqual(7, get_damage_count(3, Now, AllDamage)),
    ok.

do_clear_curr_one_damage_test() ->
    Damage = ?fms_pest,
    DamageList1 = [#farm_damage{type=?fms_pest, time=datetime:make_time(1,1,1,1,1,1)}, 
		   #farm_damage{type=?fms_pest, time=datetime:make_time(1,1,1,1,1,2)}, 
		   #farm_damage{type=?fms_pest, time=datetime:make_time(1,1,1,2,1,1)}, 
		   #farm_damage{type=?fms_grass, time=datetime:make_time(1,1,1,1,1,1)},
		   #farm_damage{type=?fms_drought, time=datetime:make_time(1,1,1,1,1,1)}
		 ],
    Value = [#farm_damage{type=?fms_drought, time=datetime:make_time(1,1,1,1,1,1)},
	     #farm_damage{type=?fms_grass, time=datetime:make_time(1,1,1,1,1,1)},
	     #farm_damage{type=?fms_pest, time=datetime:make_time(1,1,1,2,1,1)}, 
	     #farm_damage{type=?fms_pest, time=datetime:make_time(1,1,1,1,1,2)}
	    ],
    DamageList2 = [#farm_damage{type=?fms_pest, putter=test, time=datetime:make_time(1,1,1,1,1,1)}, 
		   #farm_damage{type=?fms_pest, time=datetime:make_time(1,1,1,1,1,2)}, 
		   #farm_damage{type=?fms_pest, time=datetime:make_time(1,1,1,2,1,1)}, 
		   #farm_damage{type=?fms_grass, time=datetime:make_time(1,1,1,1,1,1)},
		   #farm_damage{type=?fms_drought, time=datetime:make_time(1,1,1,1,1,1)}
		  ],
    Value2 = [#farm_damage{type=?fms_drought, time=datetime:make_time(1,1,1,1,1,1)},
	     #farm_damage{type=?fms_grass, time=datetime:make_time(1,1,1,1,1,1)},
	     #farm_damage{type=?fms_pest, time=datetime:make_time(1,1,1,2,1,1)}, 
	      #farm_damage{type=?fms_pest, putter=test, time=datetime:make_time(1,1,1,1,1,1)}
	    ],
    Now = datetime:make_time(1,1,1,1,1,3),
    ?assertEqual([], do_clear_curr_one_damage(test, Now, Damage, [])),
    ?assertEqual(Value, do_clear_curr_one_damage(test, Now, Damage, DamageList1)),
    ?assertEqual(Value2, do_clear_curr_one_damage(test, Now, Damage, DamageList2)),
    ok.

-endif.
