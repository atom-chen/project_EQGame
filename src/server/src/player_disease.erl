%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 玩家疾病
%%% @end
%%% Created :  2 Jul 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(player_disease).

-include("enum_def.hrl").
-include("packet_def.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("router.hrl").
-include("player_data.hrl").
-include("records.hrl").
-include("sys_msg.hrl").

-export([start/1, init/1, handle_cast/2, get_pay_money/1, get_disease_data/2]).

%% API
start(Account) ->
    [router:make_event_source(calc_disease_probability, Account, {self(), ?MODULE})
    ].

init(Account) ->
    register(Account).

%%%===================================================================
%%% Handle cast
%%%===================================================================
handle_cast({#msg{event=calc_disease_probability}, {PlayerProperty, OPlayerProperty, _Param}}, #player_data{account=Account, basic_data=BasicData}=State) ->
    PlayerHealth = PlayerProperty#player_property.player_health,
    OPlayerHealth = OPlayerProperty#player_property.player_health,
    HealthStatus = BasicData#player_basic_data.health_status,
    NNState = 
    case ((PlayerHealth /= OPlayerHealth) or ((PlayerHealth == 0) and (OPlayerHealth == 0))) and (HealthStatus /= ?dt_red) of
	true ->
	    DTData = tplt:get_data(player_disease_probability_tplt, PlayerHealth),
	    DiseaseTypeTplt = tplt:get_all_data(player_disease_type_tplt),
	    Probability = get_probability(DTData, DiseaseTypeTplt, HealthStatus),
	    {NHealthStatus, NState} = 
		case can_trigger_disease(Probability) of
		    true ->
			HealthStatus1 = do_trigger_disease(DiseaseTypeTplt, HealthStatus),
			State1 = do_disease_special_event(Account, HealthStatus, HealthStatus1, tplt:get_data(player_disease_type_tplt, HealthStatus1 - 1), State),
			{HealthStatus1, State1};
		    _ ->
			{HealthStatus, State}
		end,
	    NBasicData = BasicData#player_basic_data{health_status=NHealthStatus},
	    case NHealthStatus =/= HealthStatus of 
		true ->%% 健康状况如果发生变化，通知客户端
		    Packet = #notify_player_health_status{account=Account, 
							  status=NHealthStatus},
		    SceneName = player_data:get_scene_name(State),
		    router:cast(undefined, SceneName, broadcast_all_players, Packet);
		_ ->
		    ok
	    end,
	    NState#player_data{basic_data=NBasicData};
	_ ->
	    State
    end,
    {noreply, NNState}.
%%%===================================================================
%%% API
%%%===================================================================


%%%===================================================================
%%% Internal functions
%%%===================================================================
do_disease_special_event(Account, HealthStatus, HealthStatus1, DiseaseTypeTplt, State) ->
    case can_trigger_special_event(HealthStatus, HealthStatus1, DiseaseTypeTplt) of
	true ->
	    SpecialEventIds = DiseaseTypeTplt#player_disease_type_tplt.special_event_id,
	    EventIdProbability = DiseaseTypeTplt#player_disease_type_tplt.event_id_probability,
	    SpecialEventId = get_special_event_id(SpecialEventIds, EventIdProbability, rand:uniform(100)),
	    case SpecialEventId of
		0 -> % 不触发
		    State;
		_ ->
		    NState = do_trigger_special_event(SpecialEventId, State),
		    net_helper:send2client(Account, #notify_disease_special_event{special_event_id=SpecialEventId}),
		    NState
	    end;
	false ->
	    State
    end.

can_trigger_special_event(HealthStatus, HealthStatus1, DiseaseTypeTplt) ->
    case HealthStatus /= HealthStatus1 of
	true ->
	    Probability = DiseaseTypeTplt#player_disease_type_tplt.special_event_probability,
	    rand:uniform(100) =< Probability;
	false ->
	    false
    end.

get_special_event_id([], [], _RandNum) ->
    0;
get_special_event_id([SpecialEventId|SpecialEventIds], [Probability|EventIdProbability], RandNum) ->
    case RandNum =< Probability of
	true ->
	    SpecialEventId;
	false ->
	    get_special_event_id(SpecialEventIds, EventIdProbability, RandNum)
    end.

do_trigger_special_event(SpecialEventId, State) ->
    SpecialEventTplt = tplt:get_data(player_disease_special_event_tplt, SpecialEventId),
    PropertyId = SpecialEventTplt#player_disease_special_event_tplt.property_id,
    player_property_by_action:update(?msg_player_property_by_disease_special_event, PropertyId, State).

%% 获取疾病几率模板表
-spec get_disease_probability_tplt() -> list().
get_disease_probability_tplt() ->
    tplt:get_all_data(player_disease_probability_tplt).

%% 注册需要监听的人物属性的内容
-spec register(atom()) -> list().
register(Account) when is_atom(Account) ->
    ProbabilityListTplt = get_disease_probability_tplt(),
    [router:send(Account, subscribe_player_property, 
		 {#subscribe_property_data{player_health=PlayerHealth}, calc_disease_probability, []}) 
     || #player_disease_probability_tplt{player_health=PlayerHealth} <- ProbabilityListTplt].

%% 获取触发几率
-spec get_probability(tuple(), list(), integer()) -> integer().
get_probability(DTData, DiseaseTypeTplt, HealthStatus) ->
    case HealthStatus =/= ?dt_normal of
	true -> %% 如果已经有得病的情况,增加得病的几率
	    UpgradeProbability = get_upgrade_probability(DiseaseTypeTplt, HealthStatus - 1),
	    DTData#player_disease_probability_tplt.probability + UpgradeProbability;
	_ ->
	    DTData#player_disease_probability_tplt.probability
    end.

%% 根据不同的等级获取提升的几率
-spec get_upgrade_probability(list(), integer()) -> integer().
get_upgrade_probability(DTTplt, Type)
  when is_list(DTTplt), is_integer(Type)->
    case Type > 0 of
	true ->
	    DTData = get_disease_data(DTTplt, Type),
	    DTData#player_disease_type_tplt.upgrate_probability;
	_ ->
	    0
    end.

%% 获取疾病模板数据
-spec get_disease_data(list(), integer()) -> tuple().
get_disease_data([], _Type) ->
    [];
get_disease_data([#player_disease_type_tplt{type=DiseaseType}=DTData|DTTplt], Type) ->
    case DiseaseType =:= Type of
	true ->
	    DTData;
	_ ->
	    get_disease_data(DTTplt, Type)
    end.

%% 能否触发疾病
-spec can_trigger_disease(integer()) -> atom().
can_trigger_disease(Probability) when is_integer(Probability) ->
    case rand:uniform(100) =< Probability of
	true ->
	    true;
	_ ->
	    false
    end.

%% 触发疾病
-spec do_trigger_disease(list(), integer()) -> integer().
do_trigger_disease(DTTplt, HealthStatus) when is_list(DTTplt), is_integer(HealthStatus) ->
    CLevel = get_disease_level(DTTplt, HealthStatus - 1),
    TRate = get_total_rate(DTTplt, 0, CLevel),
    RandNum = rand:uniform(TRate),
    get_disease_type(lists:reverse(DTTplt), RandNum, TRate, CLevel, HealthStatus).

%% 获取疾病
-spec get_disease_type(list(), integer(), integer(), integer(), integer()) -> integer().
get_disease_type([], _RandNum, _TRate, _CLevel, _HealthStatus) ->
    1;
get_disease_type(_DTList, _RandNum, _TRate, _CLevel, HealthStatus) when HealthStatus >= 5 ->
    HealthStatus;
get_disease_type([DTData|DTTplt], RandNum, TRate, CLevel, HealthStatus) when HealthStatus < 5 ->
    Level = DTData#player_disease_type_tplt.level,
    case Level >= CLevel of
	true ->
	    Rate = DTData#player_disease_type_tplt.rate,
	    NRate = (TRate - Rate),
	    case ((RandNum > NRate) and (RandNum =< TRate)) of
		true ->
		    DTData#player_disease_type_tplt.type + 1;
		_ ->
		    get_disease_type(DTTplt, RandNum, NRate, CLevel, HealthStatus)
	    end;
	_ ->
	    get_disease_type(DTTplt, RandNum, TRate, CLevel, HealthStatus)
    end.

%% 获取疾病等级
-spec get_disease_level(list(), integer()) -> integer().
get_disease_level([], _CType) ->
    0;
get_disease_level([DTData|DTTplt], CType) ->
    Type = DTData#player_disease_type_tplt.type,
    case Type =:= CType of
	true ->
	    DTData#player_disease_type_tplt.level;
	_ ->
	    get_disease_level(DTTplt, CType)
    end.


%% 获取总比例
-spec get_total_rate(list(), integer(), integer()) -> integer().
get_total_rate([], TRate, _CLevel) ->
    TRate;
get_total_rate([DTData|DTTplt], TRate, CLevel) ->
    Level = DTData#player_disease_type_tplt.level,
    NRate = 
	case Level >= CLevel of
	    true ->
		DTData#player_disease_type_tplt.rate + TRate;
	    _ ->
		TRate
	end,
    get_total_rate(DTTplt, NRate, CLevel).

%% 获取治疗疾病需要的金额
-spec get_pay_money(tuple()) -> integer().
get_pay_money(DTData) ->
    TreatmentCosts = DTData#player_disease_type_tplt.treatment_costs,
    Min = DTData#player_disease_type_tplt.min,
    Max = DTData#player_disease_type_tplt.max,
    rand:uniform(TreatmentCosts, Min, Max).
%%%===================================================================
%%% Test case
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).


get_pay_money_test() ->
    DTData = #player_disease_type_tplt{treatment_costs=500, min=50, max=50},
    Ret = get_pay_money(DTData),
    ((Ret >= 450) and (Ret =< 550)) =:= true.

get_pay_money1_test() ->
    Ret = get_pay_money(#player_disease_type_tplt{treatment_costs=-500, min=50, max=50}),
    ((Ret >= -550) and (Ret =< -450)) =:= true. 

get_total_rate_test() ->
    DTTplt = get_tplt(),
    ?assertEqual(10, get_total_rate(DTTplt, 0, 0)),
    ?assertEqual(10, get_total_rate(DTTplt, 0, 1)),
    ?assertEqual(9, get_total_rate(DTTplt, 0, 2)),
    ?assertEqual(7, get_total_rate(DTTplt, 0, 3)),
    ?assertEqual(4, get_total_rate(DTTplt, 0, 4)).

get_tplt() ->
    [#player_disease_type_tplt{type=1,  upgrate_probability=10, rate=1, level=1, treatment_costs=500, min=50, max=50},
     #player_disease_type_tplt{type=2,  upgrate_probability=20, rate=2, level=2, treatment_costs=500, min=50, max=50},
     #player_disease_type_tplt{type=3,  upgrate_probability=30, rate=3, level=3, treatment_costs=500, min=50, max=50},
     #player_disease_type_tplt{type=4,  upgrate_probability=40, rate=4, level=4, treatment_costs=500, min=50, max=50}].


get_disease_level_test() ->
    DTTplt = get_tplt(),
    ?assertEqual(1, get_disease_level(DTTplt, 1)),
    ?assertEqual(2, get_disease_level(DTTplt, 2)),
    ?assertEqual(3, get_disease_level(DTTplt, 3)),
    ?assertEqual(4, get_disease_level(DTTplt, 4)).

get_probability_test() ->
    DTData = #player_disease_probability_tplt{player_health=50, probability=10},
    DTTplt = get_tplt(),
    ?assertEqual(20, get_probability(DTData, DTTplt, ?dt_white)),
    ?assertEqual(30, get_probability(DTData, DTTplt, ?dt_yellow)),
    ?assertEqual(40, get_probability(DTData, DTTplt, ?dt_purple)),
    ?assertEqual(50, get_probability(DTData, DTTplt, ?dt_red)).

can_trigger_disease_test() ->
    ?assertEqual(true, can_trigger_disease(100)),
    ?assertEqual(false, can_trigger_disease(0)).

get_disease_data_test() ->
    ?assertEqual(#player_disease_type_tplt{type=1,  upgrate_probability=10, rate=1, level=1, treatment_costs=500, min=50, max=50},
		 get_disease_data(get_tplt(), 1)),
    ?assertEqual(#player_disease_type_tplt{type=4,  upgrate_probability=40, rate=4, level=4, treatment_costs=500, min=50, max=50},
		 get_disease_data(get_tplt(), 4)),
    ?assertEqual(#player_disease_type_tplt{type=3,  upgrate_probability=30, rate=3, level=3, treatment_costs=500, min=50, max=50},
		 get_disease_data(get_tplt(), 3)).

get_upgrade_probability_test() ->
    ?assertEqual(10, get_upgrade_probability(get_tplt(), 1)),
    ?assertEqual(0, get_upgrade_probability(get_tplt(), 0)),
    ?assertEqual(40, get_upgrade_probability(get_tplt(), 4)).

get_disease_type_test() ->
    DTList = lists:reverse(get_tplt()),
    ?assertEqual(4, get_disease_type(DTList, 5, 10, 1, 1)),
    ?assertEqual(2, get_disease_type(DTList, 1, 10, 1, 1)),
    ?assertEqual(5, get_disease_type(DTList, 8, 10, 1, 1)),
    ?assertEqual(5, get_disease_type(DTList, 6, 7, 3, 3)).

-endif.
