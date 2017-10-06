%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 玩家属性(系统时间)
%%% @end
%%% Created : 19 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_property_by_stime).

-export([start/1, handle_cast/2]).
-export([subscribe/3]).

-include("tplt_def.hrl").
-include("table_info.hrl").
-include("packet_def.hrl").
-include("player_data.hrl").
-include("router.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").

%%----玩家属性定义-------------------------
-define(MAX_PLAYER_CLEAN, 100).
-define(MAX_PLAYER_HEALTH, 100).
-define(MAX_PLAYER_CHARM, 100).
-define(MAX_ACTIVE_VALUE, 700).

start(Account) ->
    [router:make_event_source_target(update_player_property_by_stime, sys_time, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% Handle cast
%%====================================================================
%% 更新玩家属性
handle_cast({#msg{event=update_player_property_by_stime}, TimerRef}, #player_data{player_property=OPlayerProperty}=State) ->
    Account = player_data:get_account(State),
    STimeTplt = tplt:get_all_data(player_property_by_stime_tplt),
    NPlayerProperty = calculate(OPlayerProperty, STimeTplt, State),
    NState = State#player_data{player_property=NPlayerProperty, property_timer_ref=TimerRef},
    router:send(player_property, player_property_change_with_notify_client, 
		{Account, player_property_by_stime, ?msg_player_property_by_stime, NPlayerProperty, OPlayerProperty}),
    {noreply, NState}.

%%%===================================================================
%%% API
%%%===================================================================
%% 注册监听在线时长
-spec subscribe(atom(), integer(), tuple()) -> any().
subscribe(Account, OnlineTime, LoginTime) 
  when is_atom(Account), is_integer(OnlineTime), is_record(LoginTime, stime) ->
    Timer = common_def:get_val(calc_property_by_systime),
    sys_time:subscribe(?dt_onlinetime, Account, OnlineTime, LoginTime, Timer, update_player_property_by_stime).

%%%===================================================================
%%% Internal functions
%%%===================================================================
calculate(OPlayerProperty, List, State) ->
    PlayerPropertyTplt = get_player_property(OPlayerProperty#player_property.player_clean, List),

    NPlayerClean = add_property(OPlayerProperty#player_property.player_clean,
				PlayerPropertyTplt#player_property_tplt.player_clean, ?MAX_PLAYER_CLEAN),
    NPlayerHealth = add_property(OPlayerProperty#player_property.player_health,
				 PlayerPropertyTplt#player_property_tplt.player_health, ?MAX_PLAYER_HEALTH),
    NPlayerCharm = add_property(OPlayerProperty#player_property.player_charm,
				PlayerPropertyTplt#player_property_tplt.player_charm, ?MAX_PLAYER_CHARM),
    ActiveValue = wallow:calc_income(PlayerPropertyTplt#player_property_tplt.active_value, State),
    NActiveValue = add_property(OPlayerProperty#player_property.active_value, ActiveValue, ?MAX_ACTIVE_VALUE),

    OPlayerProperty#player_property{player_clean = NPlayerClean,
				    player_health = NPlayerHealth,
				    player_charm = NPlayerCharm,
				    active_value = NActiveValue
				   }.

add_property(OldProperty, [], _MaxProperty) ->
    OldProperty;
add_property(OldProperty, NewProperty, MaxProperty) ->
    Result = OldProperty + NewProperty,
    if
	Result =< 0 ->
	    0;
	(Result > 0) and (Result =< MaxProperty) ->
	    Result;
	Result > MaxProperty ->
	    MaxProperty
    end.

%% 根据属性分值获取对应的属性列表(用于不同属性值增加或者减少对因的属性)
-spec get_ref_id(float(), list()) -> tuple().
get_ref_id(_Property, []) ->
    [];
get_ref_id(Property, 
	   [#player_property_by_stime_tplt{min_property=MinProperty, max_property=MaxProperty, ref_id=RefId}|PlayerPropertyList]) ->
    case (Property >= MinProperty) and (Property =< MaxProperty) of 
	true ->
	    RefId;
	_ ->
	    get_ref_id(Property, PlayerPropertyList)
    end.

get_player_property(PropertyValue, List) ->
    RefId = get_ref_id(PropertyValue, List),
    router:send(player_property, get_player_property_by_id, RefId).

%%%===================================================================
%%% Test case
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

add_property_test() ->
    ?assertEqual(2, add_property(1, 1, 255)),
    ?assertEqual(0, add_property(-3, 1, 255)),
    ?assertEqual(255, add_property(255, 1, 255)).

-endif.
