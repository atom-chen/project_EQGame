%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 管理npc的创建，销毁等
%%% @end
%%% Created : 27 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(npc_manager).

-behaviour(gen_msgproc).

%% API
-export([start_link/0, notify/3, request/1]).
-export([register/1]).
-export([test/0]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
 	terminate/2, code_change/3]).

-define(SERVER, ?MODULE).

-include("tplt_def.hrl").
-include("packet_def.hrl").
-include("npc_def.hrl").
-include("router.hrl").
-include("common_def.hrl").

-record(npc_data,{}).

%%%===================================================================
%%% API
%%%===================================================================
-spec notify(integer(), integer(), atom()) -> any().
notify(SceneId, CopyId, Account) when is_integer(SceneId) and is_atom(Account) ->
    gen_server:call(npc_manager, {notify_npc_list, SceneId, CopyId, Account}).

-spec request(tuple()) -> any().
request(RequestData) when is_tuple(RequestData) ->
    gen_server:call(npc_manager, RequestData).

%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link() ->
    gen_msgproc:start_link({local, npc_manager}, ?MODULE, [], []).

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
    process_flag(trap_exit, true),
    {ok, #npc_data{}}.

%% 监听指定的消息, 实现gen_msgproc的注册方法
register([])-> 
    [router:make_event(?msg_req_npc_command, self())
    ].

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
handle_call({notify_npc_list, SceneId, CopyId, Account}, _From, State) ->
    NpcMapMappingName = get_npc_map_mapping_name(SceneId),
    case tplt:info(NpcMapMappingName) of
	undefined ->
	    ok;
	_ ->
	    NewNpcMapMappingList = 
		case get(SceneId) of
		    undefined ->
			NpcMapMappingList = get_npc_map_mapping_data(SceneId),   
			NpcMapMappingListTransedData = trans_data:npc_map_mapping_list([], NpcMapMappingList),
			%% 将转换后的数据放在进程字典里面
			put(SceneId, NpcMapMappingListTransedData),
			NpcMapMappingListTransedData;
		    NpcData ->
			NpcData
		end,

	    %%创建NPC
	    create_npc(NewNpcMapMappingList, State, SceneId, CopyId),

	    NotifyNpcList = #notify_npc_list{npc_map_mapping_list=NewNpcMapMappingList},
	    %% 将npc列表发送给玩家
	    net_helper:send2client(Account, NotifyNpcList)
    end,
    {reply, ok, State}.

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
handle_cast({#msg{event=?msg_req_npc_command, src=Account}, 
	     #req_npc_command{npc_key=NpcKey}=RequestData}, State)->
    gen_npc:cast(Account, list_to_atom(NpcKey), RequestData),
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
%% 创建NPC
%% NpcName:Npc进程名称, _NpcList:所有Npc列表
-spec create_npc(list(), tuple(), integer(), integer()) -> atom().
create_npc([], _State, _SceneId, _CopyId) ->
    ok;
create_npc([#npc_map_mapping_info{id=Id, npc_key=NpcKey}=NpcMapMappingData|NpcMapMappingList], 
	   State,
	   SceneId,
	   CopyId) ->
    Mod = list_to_atom("npc_" ++ integer_to_list(SceneId) ++ "_" ++ integer_to_list(Id)),
    npc_sup:start_children(SceneId, CopyId, NpcKey, NpcMapMappingData, Mod),
    create_npc(NpcMapMappingList, State, SceneId, CopyId).  

%% 返回NPC与地图的对应数据
-spec get_npc_map_mapping_data(integer()) -> list().				      
get_npc_map_mapping_data(SceneId) when is_integer(SceneId) ->
    NpcMapMappingName = get_npc_map_mapping_name(SceneId),
    tplt:get_all_data(NpcMapMappingName).

get_npc_map_mapping_name(SceneId) ->
    list_to_atom("npc_map_mapping_" ++ integer_to_list(SceneId) ++ "_tplt").

%% --------------------------测试代码-----------------------------------
test() ->
    tplt:start_link(),
    test_get_npc_map_mapping_name(),
    test_req_npc_command().

test_get_npc_map_mapping_name() ->
    npc_map_mapping_1_tplt =:= get_npc_map_mapping_name(1).

test_req_npc_command() ->
    gen_server:cast(npc_manager, {#msg{event=?msg_req_npc_command, src=list_to_atom("test1")}, #req_npc_command{npc_key="npc_1_seller", page_index=1, command_index=1}}).

