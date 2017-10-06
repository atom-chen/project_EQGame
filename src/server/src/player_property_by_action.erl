%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 玩家属性(人物行为)
%%% @end
%%% Created : 19 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_property_by_action).

-export([start/1, handle_call/3]).
-export([calculate/3, calculate/6, validate_property_is_full/2]).
-export([update/3, update/6, get_max_value/1]).

-include("tplt_def.hrl").
-include("table_info.hrl").
-include("packet_def.hrl").
-include("player_data.hrl").
-include("router.hrl").
-include("sys_msg.hrl").

-define(MAX_PLAYER_CLEAN, 100).
-define(MAX_PLAYER_HEALTH, 100).
-define(MAX_PLAYER_CHARM, 100).
-define(MAX_ACTIVE_VALUE, 700).

start(Account) ->
    [router:make_event_target(update_player_property_by_action, Account, {self(), player_property_by_action})
    ].

%%%===================================================================
%%% Handle call
%%====================================================================
%% 更新玩家属性(人物行为影响)
handle_call({#msg{event=update_player_property_by_action}, {MsgCode, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue}}, _From, State) ->
    NState = update(MsgCode, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, State),
    {reply, player_data:get_property(State), NState};
handle_call({#msg{event=update_player_property_by_action}, {MsgCode, RefId}}, _From, State) ->
    NState = update(MsgCode, RefId, State),
    {reply, player_data:get_property(State), NState}.

%%%===================================================================
%%% API
%%%===================================================================
get_max_value(Type) when Type == player_clean->
    ?MAX_PLAYER_CLEAN;
get_max_value(Type) when Type == player_health ->
    ?MAX_PLAYER_HEALTH;
get_max_value(Type) when Type == player_charm ->
    ?MAX_PLAYER_CHARM;
get_max_value(Type) when Type == active_value ->
    ?MAX_ACTIVE_VALUE.
    
%% 更新玩家属性并通知客户端
-spec update(integer(), integer(), tuple()) -> tuple().
update(MsgCode, PropertyId, State) ->
    Account = player_data:get_account(State),
    OPlayerProperty = player_data:get_property(State),
    NPlayerProperty = calculate(OPlayerProperty, PropertyId, State),
    db_common:write(NPlayerProperty),
    router:send(player_property, player_property_change_with_notify_client, {Account, MsgCode, NPlayerProperty, OPlayerProperty}),
    player_data:set_property(NPlayerProperty, State).

%% 更新玩家属性并通知客户端
-spec update(integer(), integer(), integer(), integer(), integer(), tuple()) -> tuple().
update(MsgCode, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, State) ->
    Account = player_data:get_account(State),
    OPlayerProperty = player_data:get_property(State),
    NPlayerProperty = calculate(OPlayerProperty, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, State),
    db_common:write(NPlayerProperty),
    router:send(player_property, player_property_change_with_notify_client, {Account, MsgCode, NPlayerProperty, OPlayerProperty}),
    player_data:set_property(NPlayerProperty, State).

%% 计算玩家属性
-spec calculate(tuple(), integer(), tuple()) -> tuple().
calculate(OPlayerProperty, PropertyId, State) ->
    PlayerPropertyTplt = router:send(player_property, get_player_property_by_id, PropertyId),
    calculate(OPlayerProperty, 
	      PlayerPropertyTplt#player_property_tplt.player_clean,
	      PlayerPropertyTplt#player_property_tplt.player_health,
	      PlayerPropertyTplt#player_property_tplt.player_charm,
	      PlayerPropertyTplt#player_property_tplt.active_value, 
	      State).

%% 验证属性是否已满
-spec validate_property_is_full(tuple(), integer()) -> atom().
validate_property_is_full(PlayerProperty, PropertyId) ->
    PlayerPropertyTplt = router:send(player_property, get_player_property_by_id, PropertyId),
    validate_is_full(PlayerProperty#player_property.player_clean, 
		     PlayerPropertyTplt#player_property_tplt.player_clean, ?MAX_PLAYER_CLEAN) and
	validate_is_full(PlayerProperty#player_property.player_health, 
			 PlayerPropertyTplt#player_property_tplt.player_health, ?MAX_PLAYER_HEALTH) and
	validate_is_full(PlayerProperty#player_property.player_charm, 
			 PlayerPropertyTplt#player_property_tplt.player_charm, ?MAX_PLAYER_CHARM) and
	validate_is_full(PlayerProperty#player_property.active_value, 
			 PlayerPropertyTplt#player_property_tplt.active_value, ?MAX_ACTIVE_VALUE).

%%%===================================================================
%%% Internal functions
%%%===================================================================
calculate(PlayerProperty, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, State) ->
    NPlayerClean = add_property(PlayerProperty#player_property.player_clean, PlayerClean, ?MAX_PLAYER_CLEAN),
    NPlayerHealth = add_property(PlayerProperty#player_property.player_health, PlayerHealth, ?MAX_PLAYER_HEALTH),
    NPlayerCharm = add_property(PlayerProperty#player_property.player_charm, PlayerCharm, ?MAX_PLAYER_CHARM),
    NActiveValue = 
	case ActiveValue < 0 of
	    true ->
		ActiveValue;
	    false ->
		wallow:calc_income(ActiveValue, State)
	end,
    NNActiveValue = add_property(PlayerProperty#player_property.active_value, NActiveValue, ?MAX_ACTIVE_VALUE),
    PlayerProperty#player_property{player_clean=NPlayerClean, 
    				   player_health=NPlayerHealth, 
    				   player_charm=NPlayerCharm, 
    				   active_value=NNActiveValue}.

add_property(OldProperty, [], _MaxProperty) ->
    OldProperty;
add_property(OldProperty, NewProperty, MaxProperty) ->
    Result = OldProperty + NewProperty,
    if
	Result =< 0 ->
	    0;
	(Result > 0) and (Result < MaxProperty) ->
	    Result;
	Result >= MaxProperty ->
	    MaxProperty
    end.

validate_is_full(PlayerProperty, AddProperty, MaxProperty) ->
    case (AddProperty =< 0) or ((AddProperty > 0) and (PlayerProperty < MaxProperty)) of
	true ->
	    false;
	_ ->
	    true
    end.

