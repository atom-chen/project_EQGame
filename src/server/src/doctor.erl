%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 私人医生
%%% @end
%%% Created :  8 Jul 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(doctor).

-export([start/1, handle_call/3]).
-export([whether_have_disease/1, get_disease_type/1, treat_disease/1, get_treat_money/1]).
-export([test/0]).

-include("enum_def.hrl").
-include("packet_def.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("router.hrl").
-include("player_data.hrl").
-include("records.hrl").

%% API
start(Account) ->
    [router:make_event_target(whether_have_disease, Account, {self(), ?MODULE}),
     router:make_event_target(treat_disease, Account, {self(), ?MODULE}),
     router:make_event_target(get_treat_money, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% API
%%%===================================================================
%% 是否有疾病
handle_call(#msg{event=whether_have_disease}, _From, State) ->
    Reply = whether_have_disease(State),
    {reply, Reply, State};
%% 治疗疾病
handle_call(#msg{event=treat_disease}, _From, State) ->
    NState = treat_disease(State),
    {reply, ok, NState};
handle_call(#msg{event=get_treat_money}, _From, State) ->
    Reply = get_treat_money(State),
    {reply, Reply, State}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
whether_have_disease(State) ->
    BasicData = player_data:get_basic_data(State),
    HealthStatus = basic_data:get_data(health_status, BasicData),
    HealthStatus =/= ?dt_normal.

%% 获取疾病类型
get_disease_type(State) ->
    BasicData = player_data:get_basic_data(State),
    basic_data:get_data(health_status, BasicData).

treat_disease(State) ->
    Account = player_data:get_account(State),
    BasicData = player_data:get_basic_data(State),
    %% 改变玩家健康状况
    NHealthStatus = ?dt_normal,
    NBasicData = BasicData#player_basic_data{health_status=NHealthStatus},
    %% 通知客户端，治疗结果
    Packet = #notify_player_health_status{account=Account, status=NHealthStatus},
    SceneName = player_data:get_scene_name(State),
    router:cast(undefined, SceneName, broadcast_all_players, Packet),

    player_data:set_basic_data(NBasicData, State).

get_treat_money(State) ->
    BasicData = player_data:get_basic_data(State),
    HealthStatus = basic_data:get_data(health_status, BasicData),
    DiseaseTypeTplt = tplt:get_all_data(player_disease_type_tplt),
    DTData = player_disease:get_disease_data(DiseaseTypeTplt, HealthStatus - 1),
    player_disease:get_pay_money(DTData).


%%%===================================================================
%%% Test case
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").

test() ->
    ?assertEqual(true, router:send(test1, whether_have_disease)),
    PayMoney = router:send(test1, get_treat_money),
    data_helper:format("PayMoney:~p~n", [PayMoney]),
    router:send(test1, treat_disease).
