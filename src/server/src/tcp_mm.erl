-module(tcp_mm).
-author('saleyn@gmail.com').

-behaviour(gen_fsm).
-include("packet_def.hrl"). 
-include("enum_def.hrl").
-include("tcp_mm_data.hrl").

-export([start_link/0, start_link/1, set_socket/2, stop/1]).

%% gen_fsm callbacks
-export([init/1, handle_event/3, send/2,
         handle_sync_event/4, handle_info/3, terminate/3, code_change/4]).

%% FSM States
-export([
	 'WAIT_FOR_SOCKET'/2,
	 'WAIT_FOR_DATA'/2,
	 'WAIT_FOR_AUTH'/2
]).

%% -record(state, {
%%                 socket,    	% client socket
%%                 addr,       % client address
%% 				account
%%                }).

-define(TIMEOUT, infinity).

%%%------------------------------------------------------------------------
%%% API
%%%------------------------------------------------------------------------

%%-------------------------------------------------------------------------
%% @spec (Socket) -> {ok,Pid} | ignore | {error,Error}
%% @doc To be called by the supervisor in order to start the server.
%%      If init/1 fails with Reason, the function returns {error,Reason}.
%%      If init/1 returns {stop,Reason} or ignore, the process is
%%      terminated and the function returns {error,Reason} or ignore,
%%      respectively.
%% @end
%%-------------------------------------------------------------------------
start_link() ->
    gen_fsm:start_link(?MODULE, [], []).
start_link([]) ->
    start_link().

stop(Pid) ->
    gen_fsm:send_event(Pid, stop).

set_socket(Pid, Socket) when is_pid(Pid), is_port(Socket) ->
    gen_fsm:send_event(Pid, {socket_ready, Socket}).

%% 发送消息到客户端
send(MMPid, Packet) ->
    net_helper:send2mm(MMPid, Packet).

%%%------------------------------------------------------------------------
%%% Callback functions from gen_server
%%%------------------------------------------------------------------------

%%-------------------------------------------------------------------------
%% Func: init/1
%% Returns: {ok, StateName, StateData}          |
%%          {ok, StateName, StateData, Timeout} |
%%          ignore                              |
%%          {stop, StopReason}
%% @private
%%-------------------------------------------------------------------------
init([]) ->
    process_flag(trap_exit, true),
    data_helper:start(),
    {ok, EQPid} = eq_manager:start_link(),
    {ok, 'WAIT_FOR_SOCKET', #tcp_mm_data{eq_pid=EQPid}}.

%%-------------------------------------------------------------------------
%% Func: StateName/2
%% Returns: {next_state, NextStateName, NextStateData}          |
%%          {next_state, NextStateName, NextStateData, Timeout} |
%%          {stop, Reason, NewStateData}
%% @private
%%-------------------------------------------------------------------------
'WAIT_FOR_SOCKET'({socket_ready, Socket}, State) when is_port(Socket) ->
    % Now we own the socket
    inet:setopts(Socket, [{active, once}, {packet, 4}, binary]),
    {ok, {IP, _Port}} = inet:peername(Socket),
    {next_state, 'WAIT_FOR_AUTH', State#tcp_mm_data{socket=Socket, addr=IP}, ?TIMEOUT};
'WAIT_FOR_SOCKET'(Other, State) ->
    error_logger:error_msg("State: 'WAIT_FOR_SOCKET'. Unexpected message: ~p\n", [Other]),
    %% Allow to receive async messages
    {next_state, 'WAIT_FOR_SOCKET', State}.


%% 等待权限的验证
%% 包括登录跟注册
'WAIT_FOR_AUTH'({data, Binary}, #tcp_mm_data{socket=Socket}=State) ->
    DecodeData = net_helper:get_data(Binary),
    data_helper:format("~p~n", [DecodeData]),
    player_auth:call(Socket, State, ?TIMEOUT, DecodeData);
'WAIT_FOR_AUTH'(timeout, State) ->
    error_logger:error_msg("~p Client connection timeout - closing.\n", [self()]),
    {stop, normal, State};
'WAIT_FOR_AUTH'(Data, State) ->
    data_helper:format("~p Ignoring data: ~p\n", [self(), Data]),
    {next_state, 'WAIT_FOR_DATA', State, ?TIMEOUT}.

%% Notification event coming from client
'WAIT_FOR_DATA'({data, Binary}, #tcp_mm_data{account=Account, socket=Socket}=State) ->
    D = net_helper:get_data(Binary),
    {Type, Data} = D,
    data_helper:format("Data:~p~n", [D]),
    %% TODO: 可能存在性能热点, 可以改成发给gamesvr中的player进程
    %% 然后那边再router转发, 这样可以提高效率, 避免mnesia操作
    case Type =:= ?msg_req_fixed_broadcast of
	true -> %% 请求系统固定公告
	    sys_broadcast:req_fixed_broadcast(Socket, Data);
	_ ->
	    router:cast(Account, Type, Data)
    end,
    {next_state, 'WAIT_FOR_DATA', State, ?TIMEOUT};

'WAIT_FOR_DATA'(timeout, #tcp_mm_data{account=Account}=State) ->
    error_logger:error_msg("Account:~p Client connection timeout - closing.\n", [Account]),
    {stop, normal, State};

'WAIT_FOR_DATA'(stop, State) ->
    {stop, normal, State};

'WAIT_FOR_DATA'(Data, State) ->
    data_helper:format("~p Ignoring data: ~p\n", [self(), Data]),
    {next_state, 'WAIT_FOR_DATA', State, ?TIMEOUT}.

%%-------------------------------------------------------------------------
%% Func: handle_event/3
%% Returns: {next_state, NextStateName, NextStateData}          |
%%          {next_state, NextStateName, NextStateData, Timeout} |
%%          {stop, Reason, NewStateData}
%% @private
%%-------------------------------------------------------------------------
handle_event(_Event, StateName, StateData) ->
    {next_state, StateName, StateData}.

%%-------------------------------------------------------------------------
%% Func: handle_sync_event/4
%% Returns: {next_state, NextStateName, NextStateData}            |
%%          {next_state, NextStateName, NextStateData, Timeout}   |
%%          {reply, Reply, NextStateName, NextStateData}          |
%%          {reply, Reply, NextStateName, NextStateData, Timeout} |
%%          {stop, Reason, NewStateData}                          |
%%          {stop, Reason, Reply, NewStateData}
%% @private
%%-------------------------------------------------------------------------
handle_sync_event(Event, _From, StateName, StateData) ->
    {stop, {StateName, undefined_event, Event}, StateData}.

%%-------------------------------------------------------------------------
%% Func: handle_info/3
%% Returns: {next_state, NextStateName, NextStateData}          |
%%          {next_state, NextStateName, NextStateData, Timeout} |
%%          {stop, Reason, NewStateData}
%% @private
%%-------------------------------------------------------------------------
handle_info({send2client, Bin}, StateName, #tcp_mm_data{socket=Socket} = StateData) ->
    gen_tcp:send(Socket, Bin),
    {next_state, StateName, StateData, ?TIMEOUT};
handle_info({tcp, Socket, Bin}, StateName, #tcp_mm_data{socket=Socket} = StateData) ->
    % Flow control: enable forwarding of next TCP message
    inet:setopts(Socket, [{active, once}]),
    ?MODULE:StateName({data, Bin}, StateData);

handle_info({tcp_closed, _Socket}, _StateName, StateData) ->
    {stop, normal, StateData};
handle_info(Packet, StateName, #tcp_mm_data{socket=Socket, eq_pid=EQPid, account=Account}=State) ->
    PacketType = eq_manager:get_packet_type(Packet),
    RawPacket = eq_manager:get_raw_packet(Packet),
    RawPacketType = eq_manager:get_raw_packet_type(RawPacket),
  
    case PacketType of
	iq -> 
	    case (RawPacketType =:= "result") of 
		true ->
		    RawPacketId = eq_manager:get_raw_packet_id(RawPacket),
		    case RawPacketId of
			"get_vcard" ->
			    player_auth:call(Socket, State, ?TIMEOUT, {get_vcard, Packet});
			"list_friend" ->
			    eq_manager:received_friend_list(EQPid, Packet),
			    router:cast(Account, received_eq_packet, Packet);
			_ ->
			    router:cast(Account, received_eq_packet, Packet)
		    end;
		false ->
		    ok
	    end;
	presence -> %% 上线状态，个性签名相关
	    eq_manager:received_eq_presence(EQPid, Packet);
	_ ->
	    ok
    end,
    {next_state, StateName, State, ?TIMEOUT};
handle_info(_Info, StateName, StateData) ->
    {noreply, StateName, StateData}.

%%-------------------------------------------------------------------------
%% Func: terminate/3
%% Purpose: Shutdown the fsm
%% Returns: any
%% @private
%%-------------------------------------------------------------------------
terminate(_Reason, _StateName, #tcp_mm_data{account=Account, player_sup=Pid, socket=Socket, eq_pid=EQPid}) ->
    shutdown(Pid, Account),
    eq_manager:stop(EQPid),
    (catch gen_tcp:close(Socket)),
    ok.

%%-------------------------------------------------------------------------
%% Func: code_change/4
%% Purpose: Convert process state when code is changed
%% Returns: {ok, NewState, NewStateData}
%% @private
%%-------------------------------------------------------------------------
code_change(_OldVsn, StateName, StateData, _Extra) ->
    {ok, StateName, StateData}. 

shutdown(Pid, Account) ->
    db_player:delete_online_player(Account),
    case Pid =/= undefined of
	true ->
	    Node = node(Pid),
	    player_sup:stop(Node, Account);
	false -> ok
    end.



    
