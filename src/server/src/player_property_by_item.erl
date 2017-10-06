%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 玩家属性(道具)
%%% @end
%%% Created : 19 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_property_by_item).


-export([start/1, handle_call/3]).

-include("tplt_def.hrl").
-include("table_info.hrl").
-include("packet_def.hrl").
-include("player_data.hrl").
-include("router.hrl").
-include("common_def.hrl").

start(Account) ->
    [router:make_event_target(update_player_property_by_item, Account, {self(), player_property_by_item})
    ].

%%%===================================================================
%%% Handle call
%%====================================================================
%% 更新玩家属性(人物行为影响)
handle_call({#msg{event=update_player_property_by_item}, {MsgCode, RefId}},
	    _From, #player_data{account=Account, player_property=OPlayerProperty}=State) ->
    NPlayerProperty = calculate(OPlayerProperty, RefId, State),
    db_common:write(NPlayerProperty),
    router:send(player_property, player_property_change_with_notify_client, {Account, MsgCode, NPlayerProperty, OPlayerProperty}),
    {reply, NPlayerProperty, State#player_data{player_property=NPlayerProperty}}.

%%%===================================================================
%%% API
%%%===================================================================
calculate(PlayerProperty, RefId, State) ->
    PlayerPropertyTplt = router:send(player_property, get_player_property_by_id, RefId),
    NewPlayerClean = add_property(PlayerProperty#player_property.player_clean,
				  PlayerPropertyTplt#player_property_tplt.player_clean),
    NewPlayerHealth = add_property(PlayerProperty#player_property.player_health,
				   PlayerPropertyTplt#player_property_tplt.player_health),
    NewPlayerCharm = add_property(PlayerProperty#player_property.player_charm, 
				  PlayerPropertyTplt#player_property_tplt.player_charm),
    ActiveValue = wallow:calc_income(PlayerPropertyTplt#player_property_tplt.active_value, State),
    NewActiveValue = add_property(PlayerProperty#player_property.active_value, ActiveValue),

    PlayerProperty#player_property{player_clean=NewPlayerClean, 
    				   player_health=NewPlayerHealth, 
    				   player_charm=NewPlayerCharm, 
    				   active_value=NewActiveValue}.


%%%===================================================================
%%% Internal functions
%%%===================================================================
add_property(OldProperty, []) ->
    OldProperty;
add_property(OldProperty, NewProperty) ->
    Result = OldProperty + NewProperty,
    if
	Result =< 0 ->
	    0;
	Result >= 255 ->
	    255
    end.
