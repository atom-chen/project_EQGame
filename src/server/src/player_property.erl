%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 玩家属性
%%% @end
%%% Created :  7 Jul 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(player_property).

-behaviour(gen_msgproc).

%% API
-export([start_link/0]).
-export([register/1, get_player_property/2]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-define(SERVER, ?MODULE). 

-record(subscribe_data, {subscribe_property_data, src, event, param}).
-record(state, {subscribe_list}).

-include("tplt_def.hrl").
-include("table_info.hrl").
-include("packet_def.hrl").
-include("router.hrl").
-include("player_data.hrl").
-include("common_def.hrl").
-include("records.hrl").

%%%===================================================================
%%% API
%%%===================================================================
%% 获取玩家属性
-spec get_player_property(integer(), tuple()) -> integer().
get_player_property(Type, #player_property{player_clean=PlayerClean}) when Type == ?PLAYERCLEAN ->
    PlayerClean;
get_player_property(Type, #player_property{player_health=PlayerHealth}) when Type == ?PLAYERHEALTH ->
    PlayerHealth;
get_player_property(Type, #player_property{player_charm=PlayerCharm}) when Type == ?PLAYERCHARM ->
    PlayerCharm;
get_player_property(Type, #player_property{active_value=ActiveValue}) when Type == ?ACTIVEVALUE ->
    ActiveValue.

register([]) ->
    [router:make_event(subscribe_player_property, self()),
     router:make_event(get_player_property_by_id, self()),
     router:make_event(player_property_change_with_notify_client, self()),
     router:make_event(player_property_change, self())
    ].

%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link() ->
    gen_msgproc:start_link({local, ?SERVER}, ?MODULE, [], []).

%%%===================================================================
%%% gen_server callbacks
%%%===================================================================

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Initializes the server
%%
%% @spec init(Args) -> {ok, State} |
%%                     {ok, State, Timeout} |
%%                     ignore |
%%                     {stop, Reason}
%% @end
%%--------------------------------------------------------------------
init([]) ->
    {ok, #state{subscribe_list=[]}}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling call messages
%%
%% @spec handle_call(Request, From, State) ->
%%                                   {reply, Reply, State} |
%%                                   {reply, Reply, State, Timeout} |
%%                                   {noreply, State} |
%%                                   {noreply, State, Timeout} |
%%                                   {stop, Reason, Reply, State} |
%%                                   {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
%% 更新玩家属性
handle_call({#msg{event=subscribe_player_property}, {SubScribePropertyData, Src, Event, Param}}, _From, 
	    #state{subscribe_list=SList}=State) ->
    NSubScribeList = 
	case is_exist(Src, SubScribePropertyData, Event, SList) of
	    true ->
		SList;
	    _ ->
		[#subscribe_data{subscribe_property_data=SubScribePropertyData, src=Src, event=Event, param=Param}|SList]
	end,
    NState = State#state{subscribe_list=NSubScribeList},
    {reply, ok, NState};
handle_call({#msg{event=subscribe_player_property}, {SubScribePropertyData, Event, Param}}, _From, 
	    #state{subscribe_list=SList}=State) ->
    Src = undefined,
    NSubScribeList = 
	case is_exist(Src, SubScribePropertyData, Event, SList) of
	    true ->
		SList;
	    _ ->
		[#subscribe_data{subscribe_property_data=SubScribePropertyData, src=Src, event=Event, param=Param}|SList]
	end,
    NState = State#state{subscribe_list=NSubScribeList},
    {reply, ok, NState};
handle_call({#msg{event=get_player_property_by_id}, PropertyId}, _From, State) ->
    Property = tplt:get_data(player_property_tplt, PropertyId),
    {reply, Property, State};
handle_call({#msg{event=player_property_change_with_notify_client}, {Account, MsgCode, NPlayerProperty, OPlayerProperty}}, _From, 
	    #state{subscribe_list=SList}=State) ->
    Src = undefined,
    ChangedPlayerProperty = get_changed_player_property(NPlayerProperty, OPlayerProperty),
    notify(NPlayerProperty, OPlayerProperty, Account, Src, SList),
    case has_changed_player_property(ChangedPlayerProperty) of
	true ->
	    send(Account, MsgCode, ChangedPlayerProperty);
	_ ->
	    ok
    end,
    {reply, ok, State};
handle_call({#msg{event=player_property_change_with_notify_client}, {Account, Src, MsgCode, NPlayerProperty, OPlayerProperty}}, _From, 
	    #state{subscribe_list=SList}=State) ->
    ChangedPlayerProperty = get_changed_player_property(NPlayerProperty, OPlayerProperty),
    notify(NPlayerProperty, OPlayerProperty, Account, Src, SList),
    case has_changed_player_property(ChangedPlayerProperty) of
	true ->
	    send(Account, MsgCode, ChangedPlayerProperty);
	_ ->
	    ok
    end,
    {reply, ok, State};
handle_call({#msg{event=player_property_change}, {Account, NPlayerProperty, OPlayerProperty}}, _From, 
	    #state{subscribe_list=SList}=State) ->
    Src = undefined,
    notify(NPlayerProperty, OPlayerProperty, Account, Src, SList),
    {reply, ok, State};
handle_call({#msg{event=player_property_change}, {Account, Src, NPlayerProperty, OPlayerProperty}}, _From, 
	    #state{subscribe_list=SList}=State) ->
    notify(NPlayerProperty, OPlayerProperty, Account, Src, SList),
    {reply, ok, State};
handle_call(_Request, _From, State) ->
    Reply = ok,
    {reply, Reply, State}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling cast messages
%%
%% @spec handle_cast(Msg, State) -> {noreply, State} |
%%                                  {noreply, State, Timeout} |
%%                                  {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
handle_cast(_Msg, State) ->
    {noreply, State}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling all non call/cast messages
%%
%% @spec handle_info(Info, State) -> {noreply, State} |
%%                                   {noreply, State, Timeout} |
%%                                   {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
handle_info(_Info, State) ->
    {noreply, State}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% This function is called by a gen_server when it is about to
%% terminate. It should be the opposite of Module:init/1 and do any
%% necessary cleaning up. When it returns, the gen_server terminates
%% with Reason. The return value is ignored.
%%
%% @spec terminate(Reason, State) -> void()
%% @end
%%--------------------------------------------------------------------
terminate(_Reason, _State) ->
    ok.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Convert process state when code is changed
%%
%% @spec code_change(OldVsn, State, Extra) -> {ok, NewState}
%% @end
%%--------------------------------------------------------------------
code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
-spec send(atom(), integer(), tuple()) -> any().
send(Account, MsgCode, ChangedPlayerProperty) ->
    do_send(player_clean, Account, MsgCode, ChangedPlayerProperty),
    do_send(player_health, Account, MsgCode, ChangedPlayerProperty),
    do_send(player_charm, Account, MsgCode, ChangedPlayerProperty),
    do_send(active_value, Account, MsgCode, ChangedPlayerProperty).

-spec do_send(atom(), atom(), integer(), tuple()) -> any().
do_send(player_clean, Account, MsgCode, 
	#notify_player_property_changed{player_clean=PlayerClean}) when PlayerClean > 0 ->
    Desc = tplt:get_common_def("player_clean"), 
    sys_msg:send(Account, MsgCode, [Desc, PlayerClean]);
do_send(player_clean, Account, MsgCode, 
	#notify_player_property_changed{player_clean=PlayerClean}) when PlayerClean < 0 ->
    Desc = tplt:get_common_def("player_clean_negative"), 
    sys_msg:send(Account, MsgCode, [Desc, PlayerClean]);
do_send(player_clean, _Account, _MsgCode, 
	#notify_player_property_changed{player_clean=PlayerClean}) when PlayerClean == 0 ->
    ok;
do_send(player_health, Account, MsgCode, 
	#notify_player_property_changed{player_health=PlayerHealth}) when PlayerHealth > 0 ->
    Desc = tplt:get_common_def("player_health"),
    sys_msg:send(Account, MsgCode, [Desc, PlayerHealth]);
do_send(player_health, Account, MsgCode, 
	#notify_player_property_changed{player_health=PlayerHealth}) when PlayerHealth < 0 ->
    Desc = tplt:get_common_def("player_health_negative"),
    sys_msg:send(Account, MsgCode, [Desc, PlayerHealth]);
do_send(player_health, _Account, _MsgCode, 
	#notify_player_property_changed{player_health=PlayerHealth}) when PlayerHealth == 0 ->
    ok;
do_send(player_charm, Account, MsgCode, 
	#notify_player_property_changed{player_charm=PlayerCharm}) when PlayerCharm > 0 ->
    Desc = tplt:get_common_def("player_charm"),
    sys_msg:send(Account, MsgCode, [Desc, PlayerCharm]);
do_send(player_charm, Account, MsgCode, 
	#notify_player_property_changed{player_charm=PlayerCharm}) when PlayerCharm < 0 ->
    Desc = tplt:get_common_def("player_charm_negative"),
    sys_msg:send(Account, MsgCode, [Desc, PlayerCharm]);
do_send(player_charm, _Account, _MsgCode, 
	#notify_player_property_changed{player_charm=PlayerCharm}) when PlayerCharm == 0 ->
    ok;
do_send(active_value, Account, MsgCode, 
	#notify_player_property_changed{active_value=ActiveValue}) when ActiveValue > 0 ->
    Desc = tplt:get_common_def("active_value"),
    sys_msg:send(Account, MsgCode, [Desc, ActiveValue]);
do_send(active_value, Account, MsgCode, 
	#notify_player_property_changed{active_value=ActiveValue}) when ActiveValue < 0 ->
    Desc = tplt:get_common_def("active_value_negative"),
    sys_msg:send(Account, MsgCode, [Desc, ActiveValue]);
do_send(active_value, _Account, _MsgCode, 
	#notify_player_property_changed{active_value=ActiveValue}) when ActiveValue == 0 ->
    ok.
    
%% 玩家属性是否有改变
-spec has_changed_player_property(tuple()) -> atom().
has_changed_player_property(PlayerProperty) when is_tuple(PlayerProperty) ->
    (PlayerProperty#notify_player_property_changed.player_clean /= 0) or
    (PlayerProperty#notify_player_property_changed.player_health /= 0) or
    (PlayerProperty#notify_player_property_changed.player_charm /= 0) or
    (PlayerProperty#notify_player_property_changed.active_value /= 0).

%% 获取改变的玩家属性
-spec get_changed_player_property(tuple(), tuple()) -> tuple().
get_changed_player_property(NPlayerProperty, PlayerProperty) ->
    #notify_player_property_changed
	{player_clean=NPlayerProperty#player_property.player_clean - PlayerProperty#player_property.player_clean,
	 player_health=NPlayerProperty#player_property.player_health - PlayerProperty#player_property.player_health,
	 player_charm=NPlayerProperty#player_property.player_charm - PlayerProperty#player_property.player_charm,
	 active_value=NPlayerProperty#player_property.active_value - PlayerProperty#player_property.active_value	}.

%% 判断是否存在注册的时间类型
-spec is_exist(atom(), tuple(),  atom(), list()) -> atom().
is_exist(_Src, _Data, _Event, []) ->
    false;
is_exist(Src, Data, Event, [#subscribe_data{subscribe_property_data=SData, src=SSrc, event=SEvent}|SList]) ->
    case ((Src /= undefined) or (Src == SSrc)) and is_equal(Data, SData) and (Event =:= SEvent) of
	true ->
	    true;
	_ ->
	    is_exist(Src, Data, Event, SList)
    end.

%% 判断注册数据是否相等
-spec is_equal(tuple(), tuple()) -> atom().
is_equal(Data, SData) 
  when is_record(Data, subscribe_property_data), is_record(SData, subscribe_property_data) ->
    #subscribe_property_data{player_clean=PlayerClean, player_health=PlayerHealth, 
			     player_charm=PlayerCharm, active_value=ActiveValue, 
			     house_clean=HouseClean}=Data,
    #subscribe_property_data{player_clean=SPlayerClean, player_health=SPlayerHealth,
			     player_charm=SPlayerCharm, active_value=SActiveValue,
			     house_clean=SHouseClean} = SData,
    ((PlayerClean =:= SPlayerClean) and (PlayerHealth =:= SPlayerHealth) and
	(PlayerCharm =:= SPlayerCharm) and (ActiveValue =:= SActiveValue) and
	(HouseClean =:= SHouseClean));
is_equal(Data, SData)
  when is_record(Data, player_property), is_record(SData, subscribe_property_data) ->
    #player_property{player_clean=PlayerClean, player_health=PlayerHealth, 
		     player_charm=PlayerCharm, active_value=ActiveValue}=Data,
    #subscribe_property_data{player_clean=SPlayerClean, player_health=SPlayerHealth,
			     player_charm=SPlayerCharm, active_value=SActiveValue} = SData,
    (((SPlayerClean =:= undefined) or (PlayerClean =:= SPlayerClean)) and
     ((SPlayerHealth =:= undefined) or (PlayerHealth =:= SPlayerHealth)) and
     ((SPlayerCharm =:= undefined) or (PlayerCharm =:= SPlayerCharm)) and  
     ((SActiveValue =:= undefined) or (ActiveValue =:= SActiveValue))).


%% 通知订阅者
-spec notify(tuple(), tuple(), atom(), atom(), list()) -> atom().
notify(_PlayerProperty, _OPlayerProperty, _Account, _Src, []) ->
    ok;
notify(PlayerProperty, OPlayerProperty, Account, Src, SList) ->
    F = fun(Data, #subscribe_data{subscribe_property_data=SData, src=SSrc, event=SEvent, param=Param}) ->
    		case ((SSrc == undefined) or (Src == SSrc)) and is_equal(Data, SData) of
    		    true ->
    			router:cast(Account, SEvent, {PlayerProperty, OPlayerProperty, Param});
    		    _ ->
    			ok
    		end
    	end,
    [F(PlayerProperty, SPropertyData) || SPropertyData <- SList].

