%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 属性影响属性
%%% @end
%%% Created :  2 Sep 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(player_property_interaction).

%% API
-export([start/1, subscribe/1]).
-export([handle_cast/2]).

-include("enum_def.hrl").
-include("packet_def.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("router.hrl").
-include("player_data.hrl").
-include("records.hrl").
-include("sys_msg.hrl").

start(Account) ->
    [router:make_event_source(calc_player_property_by_interaction, Account, {self(), ?MODULE})
    ].

%% 注册需要监听的人物属性的内容
-spec subscribe(atom()) -> list().
subscribe(Account) when is_atom(Account) ->
    InteractionTplt = get_player_property_interaction_tplt(),
    [router:send(Account, subscribe_player_property, 
		 {#subscribe_property_data{player_health = subscribe_1(PlayerHealth),
					   player_clean = subscribe_1(PlayerClean),
					   player_charm = subscribe_1(PlayerCharm),
					   active_value = subscribe_1(ActiveValue)
					  }, player_property_by_stime, calc_player_property_by_interaction, RefId}) 
     || #player_property_interaction_tplt{monitor_player_health = PlayerHealth,
					  monitor_player_clean = PlayerClean,
					  monitor_player_charm = PlayerCharm,
					  monitor_active_value = ActiveValue,
					  ref_id = RefId
					 } <- InteractionTplt].

%%%===================================================================
%%% Handle cast
%%%===================================================================
handle_cast({#msg{event=calc_player_property_by_interaction}, {_PlayerProperty, _OPlayerProperty, RefId}}, State) ->
    NState = player_property_by_action:update(?msg_player_property_by_property, RefId, State),
    {noreply, NState}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 获取属性互相作用模板表
-spec get_player_property_interaction_tplt() -> list().
get_player_property_interaction_tplt() ->
    tplt:get_all_data(player_property_interaction_tplt).

subscribe_1(Value) ->
    case Value == -1 of
	true ->
	    undefined;
	_ ->
	    Value
    end.

%% %% 更新玩家属性并通知客户端
%% -spec update(atom(), atom(), integer(), integer(), tuple()) -> tuple().
%% update(Account, Src, MsgCode, PropertyId, OPlayerProperty) ->
%%     NPlayerProperty = player_property_by_action:calculate(OPlayerProperty, PropertyId),
%%     db_common:write(NPlayerProperty),
%%     router:send(player_property, player_property_change_with_notify_client, {Account, Src, MsgCode, NPlayerProperty, OPlayerProperty}),
%%     NPlayerProperty.
