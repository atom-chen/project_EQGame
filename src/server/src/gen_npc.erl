%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% npc通用模块
%%% @end
%%% Created : 29 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(gen_npc).

-behaviour(gen_server).

%% API
-export([start_link/3, cast/3]).

-export([test/0]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-export([behaviour_info/1]).

-define(SERVER, ?MODULE). 

-include("packet_def.hrl"). 
-include("tplt_def.hrl").
-include("npc_def.hrl").
-include("sys_msg.hrl").

-record(npc_common, {npc_map_mapping_data = #npc_map_mapping_info{},
		     mod}).

%%%===================================================================
%%% API
%%%===================================================================
behaviour_info(callbacks) -> 
    [{before_command, 3},            %% 执行命令之前
     {command, 3}                    %% 执行命令
    ]; 
behaviour_info(_Other) ->
    undefined.

%% 请求一个事件
-spec cast(atom(), atom(), tuple()) -> any().
cast(Account, NpcKey, RequestData) when is_atom(NpcKey) ->
    gen_server:cast(NpcKey, {Account, RequestData}).

 %%--------------------------------------------------------------------
 %% @doc
 %% Starts the server
 %%
 %% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
 %% @end
 %%--------------------------------------------------------------------
-spec start_link(atom(), tuple(), any()) -> any().
start_link(NpcKey, NpcMapMappingData, Mod) when is_atom(NpcKey) ->
    gen_server:start_link({local, NpcKey}, ?MODULE, [NpcMapMappingData, Mod], []).

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
init([NpcMapMappingData, Mod]) ->
    process_flag(trap_exit, true),
    {ok, #npc_common{npc_map_mapping_data = NpcMapMappingData,
		     mod = Mod}}.

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
handle_cast({Account, ReqNpcCommand}, #npc_common{mod=Mod}=State) ->
    router:send(Account, exec_npc_event, {ReqNpcCommand, Mod}),
    {noreply, State};
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


%%------------------------------测试用例-------------------------------
test() ->
    test_req_command1(),
    test_req_command2(),
    test_req_command3().

test_req_command1() ->
    cast(list_to_atom("test1"), npc_1_seller, #req_npc_command{npc_key=npc_1_seller, page_index=1, command_index=1}).

test_req_command2() ->
    cast(list_to_atom("test1"), npc_1_seller, #req_npc_command{npc_key=npc_1_seller, page_index=1, command_index=2}).

test_req_command3() ->
    cast(list_to_atom("test1"), npc_1_seller, #req_npc_command{npc_key=npc_1_seller, page_index=1, command_index=3}).
