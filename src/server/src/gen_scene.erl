%%%-------------------------------------------------------------------
%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%
%%% @end
%%% Created :  5 May 2010 by  <hjx>
%%%-------------------------------------------------------------------
-module(gen_scene).

-behaviour(gen_msgproc).

-include("gen_scene.hrl").
-include("sys_msg.hrl").
-include("router.hrl").
-include("tplt_def.hrl").

-export([start/3]).  
-export([broadcast_all_players/2, broadcast_other_players/2, 
	 get_players/1, get_player_count/1]).

%% API

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3, register/1]).
-export([behaviour_info/1]).


-spec behaviour_info(atom()) -> 'undefined' | [{atom(), arity()}].


behaviour_info(callbacks) ->
    [{init,1},{register, 1},{handle_call,3},{handle_cast,2},
     {can_enter_scene,2}, {after_enter_scene,2}, {before_enter_scene,2}, {resee_scene, 3},
     {before_leave_scene, 2}, {can_stop_scene, 1}, {after_leave_scene, 2}, {get_born_pos, 1}];
behaviour_info(_Other) ->
    undefined.




register([SceneName, Mod, Args]) ->
    Mod:register(Args) ++
    [router:make_event_target(enter_scene, SceneName, {self(), ?MODULE}),
     router:make_event_target(leave_scene, SceneName, {self(), ?MODULE}),
     router:make_event_target(resee_scene, SceneName, {self(), ?MODULE}),
     router:make_event_target(get_players, SceneName, {self(), ?MODULE}),
     router:make_event_target(can_enter_scene, SceneName, {self(), ?MODULE}),
     router:make_event_target(broadcast_other_players, SceneName, {self(), ?MODULE}),
     router:make_event_target(broadcast_all_players, SceneName, {self(), ?MODULE})
    ].

%%%===================================================================
%%% API
%%%===================================================================

%% 向该场景的所有玩家广播消息
broadcast_all_players(Accounts, Packet) when is_list(Accounts) -> 
    [net_helper:send2client(A, Packet) || A <- Accounts];
broadcast_all_players(State, Data) ->
    Accounts = get_players(State),
    broadcast_all_players(Accounts, Data).

%%
%% 向该场景的玩家广播消息,但不包括Account玩家
%%
broadcast_other_players(Accounts, {RequestAccount, Packet}) when is_list(Accounts) ->
    [net_helper:send2client(A, Packet) || A <- Accounts, RequestAccount =/= A];
broadcast_other_players(State, {RequestAccount, Data}) ->
    Accounts = get_players(State),
    broadcast_other_players(Accounts, {RequestAccount, Data}).

get_players(#scene_data{players=Players}) ->
    dict:fetch_keys(Players).

get_player_count(#scene_data{players=Players}) ->
    dict:size(Players).

remove_player(Account, #scene_data{players=PlayerDict}=State) when is_atom(Account) ->
    NPlayerDict = dict:erase(Account, PlayerDict),
    State#scene_data{players=NPlayerDict}.

%% 启动场景
%% SceneName:场景名
-spec start(atom(), atom(), list()) -> any().
start(SceneName, Mod, Args) when is_atom(SceneName), 
				 is_atom(Mod), is_list(Args) ->
    Ret = gen_msgproc:start_link({local, SceneName}, ?MODULE, 
			   [SceneName, Mod, Args], []),
    data_helper:format("start scene: ~w~n", [{SceneName, Ret}]).

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


init([SceneName, Mod, Args]) when is_atom(SceneName), is_atom(Mod), is_list(Args) ->
    process_flag(trap_exit, true),
    {ok, CustomData} = Mod:init(Args),
    {ok, #scene_data{scene_name=SceneName, module=Mod, custom_data=CustomData}}.

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
%% 添加玩家到场景
handle_call({#msg{event=enter_scene}, {Account, EnterPos, PlayerLevel, BodyPacket, SimplePacket}}, 
	    _From, 
	    #scene_data{module=Mod,  
			custom_data=CustomData, scene_name=SceneName} = State) ->

    {Pos, BornId} = 
	case EnterPos of
	    born_pos ->
		Mod:get_born_pos(CustomData);
	    _ ->
		BornData = tplt:get_data(born, EnterPos),
		[X, Y, Z] = BornData#born.pos,
		{#point{x=X, y=Y, z=Z},	EnterPos}
	end,
    Mod:before_enter_scene({Account, BornId}, CustomData),
    NCustomData = Mod:after_enter_scene({Account, PlayerLevel}, CustomData),
    {OtherPlayerList, State1} = do_add_player(Account, Pos, State),
    notify_other_players_info(SceneName, Account, OtherPlayerList),

    %% 通知其它玩家
    NewSimplePacket = SimplePacket#notify_other_player_data{curr_pos=Pos, dest_pos=Pos},
    broadcast_all_players(OtherPlayerList, NewSimplePacket), 
    broadcast_all_players(OtherPlayerList, BodyPacket), 
    net_helper:send2client(Account, #notify_finish_enter_scene{}),    
    {reply, Pos, State1#scene_data{custom_data=NCustomData}};   
%% 玩家重新看到场景
handle_call({#msg{event=resee_scene}, {Account, Pos}}, _From, 
	    #scene_data{module=Mod,  
			custom_data=CustomData, scene_name=SceneName} = State) ->
    Mod:resee_scene(Account, Pos, CustomData),
    PlayerList = get_players(State),
    notify_other_players_info(SceneName, Account, PlayerList),
    net_helper:send2client(Account, #notify_finish_enter_scene{}),    
    {reply, ok, State};   

%% 判断可否进入场景
handle_call({#msg{event=can_enter_scene}, {Account, Priority}}, _From, 
	    State)->
    Mod = State#scene_data.module,
    PlayerList = get_players(State),
    CanEnter = 
	case lists:member(Account, PlayerList) of
	    true -> %% 自已已经在场景中了
		{false, ?err_you_already_in_scene};
	    _ ->
		Mod:can_enter_scene({Account, Priority}, State)
	end,
    {reply, CanEnter, State};

handle_call({#msg{event=can_enter_scene}, Account}, _From, 
	    State)->
    Mod = State#scene_data.module,
    PlayerList = get_players(State),
    CanEnter = 
	case lists:member(Account, PlayerList) of
	    true -> %% 自已已经在场景中了
		{false, ?err_you_already_in_scene};
	    _ ->
		Mod:can_enter_scene(Account, State)
	end,
    {reply, CanEnter, State};

%% 取场景中的所有玩家
handle_call(#msg{event=get_players}, _From, 
	    State)->
    Players = get_players(State),
    {reply, Players, State};

%% 删除场景中的玩家
handle_call({#msg{event=leave_scene}, Account}, _From, State) ->
    Mod = State#scene_data.module,
    case Mod:before_leave_scene(Account, State) of
	true ->
	    NState = remove_player(Account, State),
	    broadcast_all_players(NState,
				  #notify_player_leave_scene{account=atom_to_list(Account)}),
	    NState1 = Mod:after_leave_scene(Account, NState),

	    case Mod:can_stop_scene(get_player_count(NState1)) of
		true ->
		    data_helper:format("stop scene~n"),
		    {stop, normal, stopped, NState1};
		_ ->
		    {reply, ok, NState1}
	    end;
	_ ->
	    {reply, ok, State}
    end;

handle_call(Request, From, State) ->
    %% 没处理的消息，传到对应模块
    Mod = State#scene_data.module,
    Mod:handle_call(Request, From, State).

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
%% 广播所有玩家(包括自己)
handle_cast({#msg{event=broadcast_all_players}, Data},
	    State) ->
    broadcast_all_players(State, Data),
    {noreply, State};

%% 广播部分玩家(不包括自己)
handle_cast({#msg{event=broadcast_other_players}, {RequestAccount, Packet}},
	    State) ->
    broadcast_other_players(State, {RequestAccount, Packet}),
    {noreply, State};

handle_cast(Msg, State) ->
    %% 没处理的消息，传到对应模块
    Mod = State#scene_data.module,
    Mod:handle_cast(Msg, State).

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
handle_info({timer, Mod, Fun, Arg}, State) ->
    NewState = Mod:Fun(Arg, State),
    {noreply, NewState};
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
terminate(Reason, State) ->
    Mod = State#scene_data.module,
    Mod:terminate(Reason, State),
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


%% 增加玩家到场景    
do_add_player(Account, Pos, #scene_data{players=PlayerDict}=State)->
    NPlayerDict = dict:store(Account, 
		#move_data{pos=Pos,speed=0,path=[],pos_index=1},
		PlayerDict),
    OtherPlayers = dict:fetch_keys(dict:erase(Account, PlayerDict)),
    {OtherPlayers, State#scene_data{players=NPlayerDict}}.


notify_other_players_info(SceneName, Account, PlayerList) ->
    %% 让其它玩家把信息通知 进入场景玩家
    %% 用cast调用，如果改用call, 可能会导致 死锁timeout
    [gen_server:cast(A, {player_enter_scene, {SceneName, Account}}) || 
	A <- PlayerList, A =/= Account],
    ok.

