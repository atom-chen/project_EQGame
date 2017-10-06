%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  农场的农作物
%%% @end
%%% Created : 12 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_crop).

-include("packet_def.hrl").
-include("enum_def.hrl").
-include("tplt_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").

-export([new_crops/2, new_crop/0, new_crop/1, new_crop/2, new_crop/3]).
-export([is_crop/1, refresh_crop_state/1, clear_crop_state/3, get_crop/2, 
	 clear_drought_state/1, has_crop_state/2, get_live_time/1, get_crop_state/3,
	 get_crop_id/1, translate/4, calc_crop_health/1,
	 get_elapse_time/1, get_elapse_time/2, put_damage/5, can_clear_op_damage/3,
	 get_crop_curr_damage/1
	]).

-export([is_crop_ripe/1, is_crop_dead/1, dec_remain/2]).
-export([pick_fruit/3, pick_fruit/4, pick_normalland_fruit/3, can_pick_fruit/5, 
	 refresh_crop_fruit/1, calc_pick_num/2, calc_pick_num/3, add_picker/2]).
-export([get_stage/1, get_crop_stage/2, new_fertilizer/3]).
-export([add_effect_data/2, is_effect_exist/2, is_stage_effect_exist/1]).
-export([set_type/2, get_type/1, set_aberrance_fruit/2, get_aberrance_fruit/1, get_seedid/1,
	 make_crop_fruit/1]).

-define(max_health, 100).   % 健康度最大值

-record(farm_crop,                 % 农场的农作物
	{tempid = 0,               % 庄稼模板ID
	 all_damage = [],          % farm_damage 庄稼生命周期内受到的灾害(虫咬, 杂草, 已被收获)
	 start_time=#stime{},      % 庄稼开始种植的时间
	 recovery=[],              % damage_recovery, 灾害恢复列表
	 remain = 0,               % 剩余的数量
	 output = 0,               % 产量
	 pickers=[],               % account 采摘过的人
	 effect_data=[],           % 使用道具后产生的影响数据(根据道具的不同, 会保存不同的数据结构)
	 land_type=normal,         % 土地类型(普通土地, 红土地)
	 aberrance_fruit=0         % 变异果实的ID
	}               
       ).            

%% 化肥道具产生的数据的结构
-record(fertilizer, {stage,      % 施肥阶段
		     start_time, % 开始影响的时间
		     reduce_time % 缩短的时间, 单位(分钟)
		     }).


new_crops(1, Crops) ->
    [new_crop() | Crops];
new_crops(Count, Crops) when Count > 1->
    new_crops(Count-1, [new_crop() | Crops]).

new_crop()->
    new_crop(0).
new_crop(TempID)->
    #farm_crop{tempid=TempID, start_time=datetime:localtime()}.
new_crop(TempID, DamageList)->
    #farm_crop{tempid=TempID, 
	       all_damage=DamageList, start_time=datetime:localtime()}.  
new_crop(TempID, DamageList, StartTime) when is_record(StartTime, stime) ->
    #farm_crop{tempid=TempID, 
	       all_damage=DamageList, start_time=StartTime};
new_crop(TempID, DamageList, Crop)->
    #farm_crop{tempid=TempID, land_type = Crop#farm_crop.land_type, 
	       all_damage=DamageList, start_time=datetime:localtime()}.  


new_fertilizer(Stage, StartTime, ReduceTime) ->
    #fertilizer{stage=Stage, start_time=StartTime, reduce_time=ReduceTime}.

is_crop(#farm_crop{tempid=0}) ->
    false;
is_crop(Crop) when is_record(Crop, farm_crop)->
    true.

%% 刷新指定农作物的状态 
refresh_crop_state(#farm_crop{tempid=0}=Crop) ->
    Crop;
refresh_crop_state(#farm_crop{all_damage=AllDamage}=Crop) ->
    case get_stage(Crop) of
	die_stage -> 
	    Crop#farm_crop{all_damage=[]};
	_Stage -> 
	    AllDamage1=farm_damage:refresh_all_damage(datetime:localtime(), AllDamage),
	    Crop#farm_crop{all_damage=AllDamage1}
    end.

%% 刷新果实
refresh_crop_fruit(#farm_crop{tempid=0}=Crop) ->
    Crop;
refresh_crop_fruit(#farm_crop{remain=0}=Crop) ->
    case is_crop_ripe(Crop) of
	true ->
	    Output = calc_fruit_output(Crop), 
	    Crop#farm_crop{output=Output, remain=Output};
	false ->
	    Crop
    end;
refresh_crop_fruit(Crop) when is_record(Crop, farm_crop) ->
    Crop.

%% 获得农作物的状态
get_crop_state(Account, Owner, #farm_crop{remain=Remain, output=Output}=Crop) ->
    case is_crop_ripe(Crop) of
	true -> 
	    case is_crop_picked(Account, Crop) of
		false -> 
		    case calc_pick_num(Account=:=Owner, Remain, Output) > 0 of
			true -> ?fms_gain;
			false -> ?fms_normal
		    end;
		true -> ?fms_normal
	    end;
	false ->
	    case get_crop_curr_damage(Crop) of
		[] -> ?fms_normal;
		DmgList -> 
		    NDmgList = lists:sort(DmgList),
		    farm_damage:get_damage_type(hd(NDmgList))
	    end
    end.

%% 计算果实的结果数量
calc_fruit_output(#farm_crop{land_type=normal}=Crop) ->
    do_calc_fruit_output(Crop);
calc_fruit_output(#farm_crop{land_type=red}=Crop) ->
    trunc(do_calc_fruit_output(Crop) * 1.1).

do_calc_fruit_output(#farm_crop{tempid=TempID}=Crop) ->
    #farm_seed_tplt{expect_output=Output} = tplt:get_data2(farm_seed_tplt, TempID),
    trunc(Output * calc_crop_health(Crop) / 100).

%% 清除农作物的当前受到的指定灾害(同类型的灾害只清除一个)
clear_crop_state(Laborer, Damage, #farm_crop{all_damage=AllDamage}=Crop) ->
    Crop#farm_crop{all_damage=farm_damage:clear_curr_one_damage(Laborer, Damage, AllDamage)}.

%% 获得农作物
get_crop(CropList, PlotIndex) when PlotIndex>0, PlotIndex=<length(CropList) ->
    lists:nth(PlotIndex, CropList).

%% 获得农作物的当前状态
get_crop_curr_damage(#farm_crop{all_damage=AllDamage}=Crop) ->
    case get_stage(Crop) of
	picking_stage -> [];
	die_stage -> [];
	_Stage -> farm_damage:get_curr_damage(datetime:localtime(), AllDamage)
    end.

%% 清除干旱状态
clear_drought_state(#farm_crop{all_damage=AllDamage}=Crop) -> 
    AllDamage1 = farm_damage:clear_drought_damage(AllDamage),
    Crop#farm_crop{all_damage=AllDamage1}.

%% 判断农作物是否包含某种自然灾害的状态
has_crop_state(_Damage, #farm_crop{tempid=0}) ->
    {false, ?err_plot_empty};
has_crop_state(Damage, Crop) when is_record(Crop, farm_crop) ->
    CurrDmgList = get_crop_curr_damage(Crop),
    lists:keymember(Damage, 2, CurrDmgList);
has_crop_state(_Damage, Crop) ->
    erlang:error({badarg, Crop}).

%% 获得农作物生长了(存活)多久
%% 返回以秒为单位的时间
get_live_time(#farm_crop{tempid=0})->
    0;
get_live_time(#farm_crop{start_time=StartTime})->
    datetime:diff_time(StartTime, datetime:localtime()).

%% 判断农作物是否成熟了
-spec is_crop_ripe(tuple()) -> boolean().
is_crop_ripe(Crop) when is_record(Crop, farm_crop)->
    get_stage(Crop) =:= picking_stage.

%% 判断农作物是否死亡了
-spec is_crop_dead(tuple()) -> boolean().
is_crop_dead(Crop) when is_record(Crop, farm_crop)->
    get_stage(Crop) =:= die_stage.

get_crop_id(#farm_crop{tempid=TempID}) ->
    TempID.

%% 减少剩余量
dec_remain(#farm_crop{remain=Remain}=Crop, Num) ->
    Crop#farm_crop{remain=erlang:max(0, Remain -  Num)}.

%% 采摘果实
%% Num: 采摘的数量
pick_fruit(Laborer, Owner, Crop) ->
    pick_fruit(Laborer, Owner, [], Crop).

pick_fruit(Laborer, Owner, _AwardedList, #farm_crop{land_type=normal}=Crop) ->
    pick_normalland_fruit(Laborer, Owner, Crop);
pick_fruit(Laborer, Owner, AwardedList, #farm_crop{land_type=red}=Crop) ->
    farm_land:pick_redland(Laborer, Owner, AwardedList, Crop).

pick_normalland_fruit(Account, Owner, #farm_crop{tempid=0}) ->
    erlang:error({not_crop, Account, Owner});
pick_normalland_fruit(Account, Owner, #farm_crop{remain=0}=Crop) ->
    erlang:error({no_fruit, Account, Owner, Crop});
pick_normalland_fruit(Account, Owner, #farm_crop{remain=Remain}=Crop) -> 
    #farm_crop{output=Output}=Crop,
    Count = calc_pick_num(Account =:= Owner, Remain, Output),
    %% 采摘放到仓库中
    ItemID = make_crop_fruit(Crop),
    [{pick_item, {item:make_item(ItemID), Count}}].

make_crop_fruit(#farm_crop{tempid=TempID}=Crop) ->
    FruitID = case get_aberrance_fruit(Crop) of
		  0 ->
		      Tplt = tplt:get_data2(farm_seed_tplt, TempID),
		      Tplt#farm_seed_tplt.fruit_id;
		  ID -> ID
	      end,
    FruitID.

%% 是否能采摘
can_pick_fruit(_Account, _Owner, _PickerCoin, #farm_crop{tempid=0}, _Farm) ->
    {false, ?err_not_crop_pick};
can_pick_fruit(Account, Owner, PickerCoin, Crop, Farm) ->
    case get_stage(Crop) of
	picking_stage -> 
	    %% 采摘过了, 就不能再采摘了
	    case is_crop_picked(Account, Crop) of
		false -> 
		    #farm_crop{output=Output} = Crop,
		    can_pick_fruit_1(Account, Owner, PickerCoin, Output, Crop, Farm);
		true -> 
		    {false, ?err_picked}
	    end;
	die_stage ->
	    {false, ?err_not_crop_pick};
	_Stage ->
	    {false, ?err_not_crop_pick}
    end.

can_pick_fruit_1(Account, Account, _Coin, _Output, #farm_crop{remain=Remain}, _Farm) 
  when Remain =< 0 ->
    {false, ?err_not_remain_fruit};
can_pick_fruit_1(Account, Account, _Coin, _Output, _Crop, _Farm) ->
    true;
can_pick_fruit_1(_Account, _Owner, _Coin, _Output, #farm_crop{remain=Remain}, _Farm) 
  when Remain =< 0 ->
    %% 判断是否还有剩余的果实
    {false, ?err_not_remain_fruit};
can_pick_fruit_1(Account, Owner, Coin, Output, Crop, Farm) ->
    %% 超过40%就不能被其他人采摘
    case calc_pick_num(Account =:= Owner, Crop#farm_crop.remain, Output) > 0 of
	true ->
	    case Crop#farm_crop.land_type of
		red -> 
		    case Coin > 0 of
		    	false -> {false, ?msg_not_money_cannot_steal};
		    	true ->
			    case farm_pet:can_bite(Coin, Farm) of
				true -> {false, bite_dec_money}; %% 被咬, 损失一定的金币, 不能偷
				false -> true
			    end
		    end;
		normal -> 		    
		    case farm_pet:has_pet_and_food(Farm) of
		    	true -> 
		    	    case Coin > 0 of
				false -> {false, bite};
				true -> 
				    case farm_pet:is_bite(Farm) of
					true -> {false, bite_dec_money};
					false -> true
				    end
			    end;
			false -> true
		    end
	    end;
	false -> %% 需要给农场主留一点
	    {false, ?err_cannot_pick}
    end.


%% 判断是否被指定的玩家采摘过
is_crop_picked(Account, #farm_crop{pickers=Pickers}) ->
    lists:member(Account, Pickers).

%% 增加采摘者
add_picker(Picker, Crop) ->
    Crop#farm_crop{pickers = [Picker | Crop#farm_crop.pickers]}.

%% 计算能采摘的量
calc_pick_num(IsOwner, Crop) ->
    calc_pick_num(IsOwner, Crop, 1).

calc_pick_num(IsOwner, #farm_crop{remain=Remain}=Crop, Multiple) ->
    Output = calc_fruit_output(Crop),
    do_calc_pick_num(IsOwner, Remain, Output, Multiple);
calc_pick_num(IsOwner, Remain, Output) ->
    do_calc_pick_num(IsOwner, Remain, Output, 1).

do_calc_pick_num(IsOwner, Remain, Output, Multiple) when Remain < 0 ->
    erlang:error({badarg, IsOwner, Remain, Output, Multiple});
do_calc_pick_num(_IsOwner, 0, _Output, _Multiple) ->
    0;
do_calc_pick_num(false, Remain, Output, Multiple)->
    calc_steal_pick_num(rand:uniform(100), Remain, Output, Multiple);
do_calc_pick_num(true, Remain, _Output, _Multiple) ->
    Remain.

%% 计算采摘别人农场的采摘量
calc_steal_pick_num(Random, Remain, Output, Multiple) ->
    PickNum = get_pick_num(Random) * Multiple,
    MaxStealNum = erlang:max(Output * 0.4 - (Output - Remain), 0),
    Num = case PickNum > MaxStealNum of
	      true -> MaxStealNum;
	      false -> PickNum
	  end,
    trunc(Num).

%% 获得采摘几率
%% 参数: 采摘数量
get_pick_num(Rand) when Rand < 70 ->
    1;
get_pick_num(Rand) when Rand < 82 ->
    2;
get_pick_num(Rand) when Rand < 87 ->
    3;
get_pick_num(Rand) when Rand < 91 ->
    4;
get_pick_num(Rand) when Rand < 94 ->
    5;
get_pick_num(Rand) when Rand < 96 ->
    6;
get_pick_num(Rand) when Rand < 97 ->
    7;
get_pick_num(Rand) when Rand < 98 ->
    8;
get_pick_num(Rand) when Rand < 99 ->
    9;
get_pick_num(Rand) when Rand >= 99 ->
    10.

%% 把数据库中的结构转换成网络传输的结构
%% #farm_crop{} -> #farm_crop_data{}
translate(_Account, _Owner, #farm_crop{tempid=0}=Crop, _PickNum)->
    #farm_crop_data{
	   tempid = 0,
	   health = 0,
	   curr_damage = [],
	   start_time = #stime{},
	   pick_num = 0,
	   remain = 0,
	   output = 0,
	   stage = 0,
	   land_type = case Crop#farm_crop.land_type of
			   normal -> 0;
			   red -> 1
		       end,
	   picked = 0,
	   stage_time = [],
	   aberrance_fruit = 0
	  };
translate(Account, Owner, #farm_crop{output=Output, remain=Remain}=Crop, PickNum)->
    #farm_crop_data{
	   tempid = Crop#farm_crop.tempid,
	   health = calc_crop_health(Crop),
	   curr_damage = farm_damage:get_damage_type(get_crop_curr_damage(Crop)),
	   start_time = Crop#farm_crop.start_time,
	   pick_num = PickNum,
	   remain = Crop#farm_crop.remain,
	   output = Crop#farm_crop.output,
	   stage = translate_stage(get_stage(Crop)),
	   land_type = case Crop#farm_crop.land_type of
			   normal -> 0;
			   red -> 1
		       end,
	   picked = calc_picked_value(Account, Owner, Remain, Output, Crop),
	   stage_time = calc_stage_time(Crop),
	   aberrance_fruit = Crop#farm_crop.aberrance_fruit
	  }.

%% 阶段的转换(把atom改成协议中定义的变量, 方便网络传输和客户端使用)
translate_stage(sprouting_stage) ->
    ?fcs_sprouting;
translate_stage(s_leafing_stage) ->
    ?fcs_s_leafing;
translate_stage(b_leafing_stage) ->
    ?fcs_b_leafing;
translate_stage(blooming_stage) ->
    ?fcs_blooming;
translate_stage(fruiting_stage) ->
    ?fcs_fruiting;
translate_stage(picking_stage) ->
    ?fcs_picking;
translate_stage(die_stage) ->
    ?fcs_die.

calc_picked_value(Account, Owner, Remain, Output, Crop) ->
    case is_crop_ripe(Crop) of
	true -> 
	    case is_crop_picked(Account, Crop) of
		true -> 1;
		false -> 
		    case calc_pick_num(Account=:=Owner, Remain, Output) > 0 of
			true -> 0;
			false -> 2
		    end
	    end;
	false -> 0
    end.
    

calc_stage_time(#farm_crop{effect_data=Effect, tempid=ID}) ->
    Tplt = tplt:get_data2(farm_seed_tplt, ID),
    SproutTime = Tplt#farm_seed_tplt.sprout_time - get_fertilizer_time(sprouting_stage, Effect),
    SLeafTime = Tplt#farm_seed_tplt.s_leaf_time - get_fertilizer_time(s_leafing_stage, Effect),
    BLeafTime = Tplt#farm_seed_tplt.b_leaf_time - get_fertilizer_time(b_leafing_stage, Effect),
    BloomTime = Tplt#farm_seed_tplt.bloom_time - get_fertilizer_time(blooming_stage, Effect),
    FruitTime = Tplt#farm_seed_tplt.fruit_time - get_fertilizer_time(fruiting_stage, Effect),
    PickTime = Tplt#farm_seed_tplt.pick_time - get_fertilizer_time(picking_stage, Effect),
    [SproutTime, SLeafTime, BLeafTime, BloomTime, FruitTime, PickTime].

%% 获得农作物成活的时间
get_elapse_time(#farm_crop{start_time=ST}) ->
    datetime:diff_time(ST, datetime:localtime()).

get_elapse_time(#farm_crop{start_time=ST}, CurrTime) ->
    datetime:diff_time(ST, CurrTime);
get_elapse_time(ST, CurrTime) ->
    datetime:diff_time(ST, CurrTime).

%% 增加一个伤害到农作物中
add_damage(Crop, Damage) ->
    Crop#farm_crop{all_damage=[Damage | Crop#farm_crop.all_damage]}.

%% 计算农作物的当前健康度
calc_crop_health(#farm_crop{all_damage=AllDamage})->
    F = fun(Damage, Acc0)->
		Acc0 + farm_damage:calc_damage_effect(datetime:localtime(), Damage)
	end,
    CurrDmgList = farm_damage:get_curr_damage(datetime:localtime(), AllDamage),
    Result = ?max_health - lists:foldl(F, 0, CurrDmgList),
    erlang:max(0, Result).

%% 放置灾害
put_damage(_Account, #farm_crop{tempid=0}, _Damage, _NotAllowStage, _OpCount)->
    {false, ?err_plot_empty};
put_damage(Account, Crop, Damage, NotAllowStage, OpCount)->
    case can_put_damage(Damage, Crop, NotAllowStage, OpCount) of
	true ->
	    SeedData = tplt:get_data2(farm_seed_tplt, Crop#farm_crop.tempid),
	    #farm_damage_tplt{reduce_health=Reduce}=
		farm_damage:get_damage_data(Damage, SeedData),
	    Damage1 = farm_damage:make_damage(Damage, datetime:localtime(), 
					      Account, Reduce),
	    NCrop = add_damage(Crop, Damage1),
	    {true, NCrop};
	Result -> Result
    end.

%% 能否放置灾害
can_put_damage(_Damage, #farm_crop{tempid=0}, _NotAllowStage, _OpCount) ->
    {false, ?err_plot_empty};
can_put_damage(_Damage, _Crop, _NotAllowStage, OpCount) when OpCount >= 50 ->
    %% 判断每天放置的灾害是否达到数量上限
    {false, ?err_put_damage_too_much}; 
can_put_damage(Damage, #farm_crop{all_damage=AllDmg}=Crop, 
	       NotAllowStage, _OpCount) ->
    %% 判断当前阶段能否放置灾害
    Stage = get_stage(Crop),
    case lists:member(Stage, NotAllowStage) of
	false -> 
	    %% 判断同一个类型的灾害是否达到数量上限
	    case farm_damage:get_damage_count(Damage, datetime:localtime(), AllDmg) < 3 of
		true -> true;
		false -> {false, ?err_damage_full}
	    end;
	true -> {false, ?err_cannot_put_damage}
    end.

%% 能否去除那些可以手工放上去的灾害(杂草, 害虫)
%% can_clear_op_damage(_Account, _Damage, _Crop, OpCount) when OpCount > 150 ->
%%     {false, ?err_farm_operate_too_much};
can_clear_op_damage(_Account, _Damage, #farm_crop{tempid=0}) ->
    {false, ?err_plot_empty};
can_clear_op_damage(Account, Damage, Crop) when is_record(Crop, farm_crop)->
    case has_crop_state(Damage, Crop) of
	true ->
	    CurrDmgList = get_crop_curr_damage(Crop),
	    case farm_damage:can_clear_damage(Account, Damage, CurrDmgList) of
		true -> true;
		false -> {false, ?err_cannot_clear_damage}
	    end;
	false -> 
	    case Damage of
		?fms_grass ->
		    {false, ?err_farm_no_grass};
		?fms_pest -> 
		    {false, ?err_farm_no_pest};
		?fms_drought ->
		    {false, ?err_farm_no_drought}
	    end
    end.


%% 增加效果数据
%% E: 新的效果
add_effect_data(E, #farm_crop{effect_data=Effect}=Crop) ->
    NEffect = [E | Effect],
    Crop#farm_crop{effect_data=NEffect}.

is_effect_exist(E, #farm_crop{effect_data=Effect}) ->
    lists:keymember(element(1, E), 1, Effect).

%% 判断农作物某个阶段是否存在效果
is_stage_effect_exist(#farm_crop{effect_data=Effect}=Crop) ->
    Stage = get_stage(Crop),
    case [Fer || Fer <- Effect, Fer#fertilizer.stage =:= Stage] of
	[] -> false;
	[_] -> true
    end.
	    
%% 确认目前农作物处于哪个阶段
get_stage(#farm_crop{tempid=TempID,effect_data=EL,remain=Remain,output=OP}=Crop) ->
    SeedData = tplt:get_data2(farm_seed_tplt, TempID),
    get_crop_stage(EL, Remain, OP, SeedData, get_live_time(Crop)).

get_crop_stage(SeedData, ElapseTime) ->
    get_crop_stage([], 0, 0, SeedData, ElapseTime).

get_crop_stage(Effect, Remain, Output, SeedData, ElapseTime) ->
    case get_crop_time_stage(Effect, SeedData, ElapseTime) of
	picking_stage ->
	    case (Remain =:= 0) and (Output > 0) of
		true -> die_stage;
		false -> picking_stage
	    end;
	Stage -> Stage
    end.

get_crop_time_stage(Effect, #farm_seed_tplt{sprout_time=SproutTime,
				       s_leaf_time=SLeafTime,
				       b_leaf_time=BLeafTime,
				       bloom_time=BloomTime,
				       fruit_time=FruitTime,
				       pick_time=PickTime
				      }=SeedData, ElapseTime) ->
    NSproutTime = SproutTime - get_fertilizer_time(sprouting_stage, Effect),
    NSLeafTime = SLeafTime - get_fertilizer_time(s_leafing_stage, Effect),
    NBLeafTime = BLeafTime - get_fertilizer_time(b_leafing_stage, Effect),
    NBloomTime = BloomTime - get_fertilizer_time(blooming_stage, Effect),
    NFruitTime = FruitTime - get_fertilizer_time(fruiting_stage, Effect),
    NPickTime = PickTime - get_fertilizer_time(picking_stage, Effect),
    if 
	ElapseTime < NSproutTime ->
	    sprouting_stage;
	ElapseTime < NSLeafTime ->
	    s_leafing_stage;
	ElapseTime < NBLeafTime ->
	    b_leafing_stage;
	ElapseTime < NBloomTime ->
	    blooming_stage;
	ElapseTime < NFruitTime ->
	    fruiting_stage;
	ElapseTime < NPickTime ->
	    picking_stage;
	ElapseTime >= NPickTime ->
	    die_stage;
	true ->
	    erlang:error({badarg, SeedData, ElapseTime})
    end.

%% 根据阶段获得施肥减少的时间
get_fertilizer_time(Stage, Effect)->
    case [RT || #fertilizer{stage=NStage, reduce_time=RT} <- Effect, Stage =:= NStage] of
	[] -> 0;
	[ReduceTime] -> ReduceTime
    end.

set_type(Type, Crop) when (Type =:= normal) or (Type =:= red) ->
    Crop#farm_crop{land_type=Type}.

get_type(#farm_crop{land_type=Type})->
    Type.

set_aberrance_fruit(FruitID, Crop) when FruitID > 0 ->
    Crop#farm_crop{aberrance_fruit = FruitID}.

get_aberrance_fruit(#farm_crop{aberrance_fruit = FruitID})->
    FruitID.

get_seedid(#farm_crop{tempid = SeedID})->
    SeedID.

%%------------------------test------------------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

new_crop_test() ->
    L = new_crops(10, []),
    ?assertEqual(10, length(L)).

%% can_clear_op_damage_test() ->
%%     Damage = ?fms_grass,
%%     STime = datetime:make_time(2010,1,1,0,0,0),
%%     Crop = #farm_crop{start_time=STime, all_damage=[]},
%%     ?assertEqual({false,?err_farm_no_damage}, can_clear_op_damage(test1, Damage, Crop, 0)),
%%     ?assertEqual({false,?err_farm_operate_too_much},can_clear_op_damage(test1, Damage, Crop, 151)),
%%     ok.

get_crop_stage_test() ->
    SeedData = #farm_seed_tplt{sprout_time = 10, s_leaf_time = 20, b_leaf_time = 30,
			   bloom_time = 40, fruit_time = 50, pick_time= 60,
			   drought_id = 1, pest_id = 2, grass_id = 3},
    ?assertEqual(sprouting_stage, get_crop_stage([], 0, 0, SeedData, 5)),
    ?assertEqual(s_leafing_stage, get_crop_stage([], 0, 0, SeedData, 15)),
    ?assertEqual(b_leafing_stage, get_crop_stage([], 0, 0, SeedData, 25)),
    ?assertEqual(blooming_stage, get_crop_stage([], 0, 0, SeedData, 35)),
    ?assertEqual(fruiting_stage, get_crop_stage([], 0, 0, SeedData, 45)),
    ?assertEqual(picking_stage, get_crop_stage([], 0, 0, SeedData, 55)),
    ?assertEqual(picking_stage, get_crop_stage([], 10, 10, SeedData, 55)),
    ?assertEqual(die_stage, get_crop_stage([], 0, 10, SeedData, 55)),
    ?assertEqual(s_leafing_stage, 
		 get_crop_stage([new_fertilizer(sprouting_stage, 0, 1000)], 0, 0, SeedData, 5)),
    ok.

get_fertilizer_time_test() ->
    ?assertEqual(0, get_fertilizer_time(die_stage, [])),
    EL = [{abcd, 2, 3}, new_fertilizer(picking_stage, 0, 1000)],
    ?assertEqual(0, get_fertilizer_time(die_stage, EL)),
    ?assertEqual(1000, get_fertilizer_time(picking_stage, EL)),
    ok.

is_effect_exist_test() ->
    E = new_fertilizer(picking_stage, 0, 0),
    ?assertEqual(false, is_effect_exist(E, #farm_crop{effect_data=[]})),
    EL = [E, {a, b, c}],
    ?assertEqual(true, is_effect_exist(E, #farm_crop{effect_data=EL})),
    ok.

calc_crop_health_test() ->
    T = datetime:make_time(2010, 8, 30, 14, 5, 34),
    Crop = #farm_crop{start_time = T},
    calc_crop_health(Crop),
    ok.

calc__steal_pick_num_test() ->
    ?assertEqual(1, calc_steal_pick_num(30, 20, 20, 1)),
    ?assertEqual(8, calc_steal_pick_num(100, 20, 20, 1)),
    ?assertEqual(0, calc_steal_pick_num(99, 12, 20, 1)),
    ?assertEqual(0, calc_steal_pick_num(80, 5, 20, 1)),
    ?assertEqual(0, calc_steal_pick_num(1, 0, 20, 1)),
    ?assertEqual(3, calc_steal_pick_num(100, 15, 20, 1)),
    ok.
    

-endif. 
