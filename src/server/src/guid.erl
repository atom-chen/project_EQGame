%%%-------------------------------------------------------------------
%%% File    : guid.erl
%%% Author  :  <>
%%% Description : 用于生成唯一ID
%%%
%%% Created : 27 Oct 2009 by  <>
%%%-------------------------------------------------------------------
-module(guid).

-behaviour(gen_server).

%% API
-export([start_link/0, start_link/1, make/1, fetch_index/0]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-record(state, {server_id,%% 服务器ID
		index = 1,%% 顺序索引
		time%%时间
	       }).

%%====================================================================
%% API
%%====================================================================
%%--------------------------------------------------------------------
%% Function: start_link() -> {ok,Pid} | ignore | {error,Error}
%% Description: Starts the server
%%--------------------------------------------------------------------
start_link()->
    start_link(1).
start_link(ServerId) ->
    gen_server:start_link({local, ?MODULE}, ?MODULE, ServerId, []).

%% 生成唯一的64位整型值
make(Type)->
    gen_server:call(?MODULE, {make, Type}).

fetch_index() ->
    gen_server:call(?MODULE, fetch_index).


%%====================================================================
%% gen_server callbacks
%%====================================================================

%%--------------------------------------------------------------------
%% Function: init(Args) -> {ok, State} |
%%                         {ok, State, Timeout} |
%%                         ignore               |
%%                         {stop, Reason}
%% Description: Initiates the server
%%--------------------------------------------------------------------
init(ServerId) ->
    {ok, #state{server_id=ServerId, time=erlang:localtime()}}.

%%--------------------------------------------------------------------
%% Function: %% handle_call(Request, From, State) -> {reply, Reply, State} |
%%                                      {reply, Reply, State, Timeout} |
%%                                      {noreply, State} |
%%                                      {noreply, State, Timeout} |
%%                                      {stop, Reason, Reply, State} |
%%                                      {stop, Reason, State}
%% Description: Handling call messages
%%--------------------------------------------------------------------
handle_call(fetch_index, _From, State) ->
    Reply = State#state.index,
    NewState = move_next(State),
    {reply, Reply, NewState};
handle_call({make, Type}, _From, State) ->
    Reply = make_impl(State, Type),
    NewState = move_next(State), 
    {reply, Reply, NewState}.


%%--------------------------------------------------------------------
%% Function: handle_cast(Msg, State) -> {noreply, State} |
%%                                      {noreply, State, Timeout} |
%%                                      {stop, Reason, State}
%% Description: Handling cast messages
%%--------------------------------------------------------------------
handle_cast(_Msg, State) ->
    {noreply, State}.

%%--------------------------------------------------------------------
%% Function: handle_info(Info, State) -> {noreply, State} |
%%                                       {noreply, State, Timeout} |
%%                                       {stop, Reason, State}
%% Description: Handling all non call/cast messages
%%--------------------------------------------------------------------
handle_info(_Info, State) ->
    {noreply, State}.

%%--------------------------------------------------------------------
%% Function: terminate(Reason, State) -> void()
%% Description: This function is called by a gen_server when it is about to
%% terminate. It should be the opposite of Module:init/1 and do any necessary
%% cleaning up. When it returns, the gen_server terminates with Reason.
%% The return value is ignored.
%%--------------------------------------------------------------------
terminate(_Reason, _State) ->
    ok.

%%--------------------------------------------------------------------
%% Func: code_change(OldVsn, State, Extra) -> {ok, NewState}
%% Description: Convert process state when code is changed
%%--------------------------------------------------------------------
code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

%%--------------------------------------------------------------------
%%% Internal functions
%%--------------------------------------------------------------------

make_impl(#state{server_id=ServerId, index=Index, time=Time}, Type)when Type < 64 ->
    DiffTime = datetime:diff_time({{2009,10,1}, {0,0,0}}, Time),
    <<ID:64>> = <<ServerId:8, DiffTime:34, Type:6, Index:16>>,
    ID.


move_next(State) ->
    NewIndex = State#state.index + 1,
    if
	NewIndex < 65536 -> %2^16
	    State#state{index=NewIndex};
	true ->
	    State#state{index=1, time=erlang:localtime()}
    end.

    
