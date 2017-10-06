%% Author: NoteBook
%% Created: 2009-9-10
%% Description: Add description to database
-module(database).

-behaviour(application).

-behaviour(gen_server).

%% API
-export([start_link/0]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).
%% Application callbacks
-export([start/2, stop/1]).
-export([start/0, stop/0, add_db_node/1, add_table_frag/2, listen/0]).
-export([backup/1, restore/1, show_backup/1, copy_from/1]).

-define(SERVER, ?MODULE). 

-record(state, {}).


%%
%% Include files
%%
%%-include("table_info.hrl").


%%%===================================================================
%%% API
%%%===================================================================


%% 增加新分片
add_table_frag(Tab, DBNode) when is_atom(Tab), is_atom(DBNode) ->
    mnesia:change_table_frag(Tab, {add_frag, [DBNode]}).


%% 增加新节点
add_db_node(NewNode) when is_atom(NewNode) ->
    %% 判断节点是否已存在
    DBNodes = mnesia:system_info(db_nodes),
    case lists:member(NewNode, DBNodes) of
	true -> ok;
	false -> do_add_db_node(NewNode)
    end.

%% 备份
backup(BackupFile) ->
    mnesia:backup(BackupFile).

%% 还原(数据库的表格必须一致, 包括分片表)
restore(BackupFile) ->
    A = mnesia:system_info(tables),
    B = get_backup_tables(BackupFile),
    %% 确定Table都一样
    Diff = (A -- B) ++ (B -- A),
    case Diff of
	[] ->
	    mnesia:restore(BackupFile, []),
	    data_helper:format("restore success, please restart server.~n");	    
	_ ->
	    data_helper:format("restore fail, tables not find: ~p~n",[Diff])	    
    end.

copy_from(SourceDBNode) ->
    case SourceDBNode =:= node() of
	true ->	data_helper:format("copy self.~n");
	_ -> do_copy_from(SourceDBNode)
    end.
	    
do_copy_from(SourceDBNode) when is_atom(SourceDBNode) ->    
    SrcFile = "src_for_copy_db.bup",
    DestFile = "dest_for_copy_db.bup",
    %% 备份
    ok = rpc:call(SourceDBNode, database, backup, [SrcFile]),
    %% 传回备份文件
    {ok, BinStream} = rpc:call(SourceDBNode, file, read_file, [SrcFile]),
    ok = file:write_file(DestFile, BinStream),
    database:restore(DestFile).

show_backup(BackupFile) when is_list(BackupFile) -> 
    FShow =
        fun({schema, _Tab, _Options}=Item, Acc) ->
		data_helper:format("~p~n",[Item]),
		{[Item], Acc};
           (Other, Acc) ->
                {[Other], Acc}
        end,
    Mod = mnesia_backup,
    mnesia:traverse_backup(BackupFile, Mod, dummy, read_only, FShow, 0).

%%%===================================================================
%%% Application callbacks
%%%===================================================================
start() ->
    ok = start_db(),
    case application:start(eq_database) of
	ok ->
	    ok;
	{error, {already_started, _}} ->
	    ok;
	{error, R} ->
	    {error, R}
    end.

stop() ->
    application:stop(eq_database),
    close_db().

listen() ->
    gen_server:cast(?SERVER, listen).

%%--------------------------------------------------------------------
%% @private
%% @doc
%% This function is called whenever an application is started using
%% application:start/[1,2], and should start the processes of the
%% application. If the application is structured according to the OTP
%% design principles as a supervision tree, this means starting the
%% top supervisor of the tree.
%%
%% @spec start(StartType, StartArgs) -> {ok, Pid} |
%%                                      {ok, Pid, State} |
%%                                      {error, Reason}
%%      StartType = normal | {takeover, Node} | {failover, Node}
%%      StartArgs = term()
%% @end
%%--------------------------------------------------------------------
start(_StartType, _StartArgs) ->
    case start_link() of
	{ok, Pid} ->
	    listen(),
	    {ok, Pid};
	Error ->
	    Error
		end.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% This function is called whenever an application has stopped. It
%% is intended to be the opposite of Module:start/2 and should do
%% any necessary cleaning up. The return value is ignored.
%%
%% @spec stop(State) -> void()
%% @end
%%--------------------------------------------------------------------
stop(_State) ->
    ok.


%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link() ->
    Ret = gen_server:start_link({local, ?SERVER}, ?MODULE, [], []),
    Ret.

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
    {ok, #state{}}.

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
handle_cast(listen, State) ->
    mnesia:subscribe(system),
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
handle_info({mnesia_system_event, {mnesia_down, Node}}, State) ->
    %%data_helper:format("~n mnesia down: ~w~n", [Node]),
    %% 如果有节点停掉了(非无盘节点)，关闭整个数据库
    DiscNodes = mnesia:table_info(schema, disc_copies),    
    case lists:member(Node, DiscNodes) of
	true -> 
	    mnesia:unsubscribe(system),
	    close_db(),
	    {stop, shutdown, State};	
	false -> 
	    {noreply, State}	    
    end;    
handle_info(_Info, State) ->
    %%data_helper:format("~n handle info: ~w~n", [Info]),
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







%% %%
%% %% 鍒濆鍖栧唴瀛樿〃
%% %%
%% init_memory_table() ->
%%     DiscNodes = [node() | nodes()],
%%     init_scene_frag_table(DiscNodes),
%%     init_online_player_frag_table(DiscNodes),
%%     mnesia:wait_for_tables([scene, online_player], 20000).

%% %%
%% %% 鍒濆鍖栧唴瀛樺垎鐗囪〃缁撴瀯
%% %%
%% init_scene_frag_table(Nodes) ->
%% 	FragProps = [{node_pool, Nodes}, {n_fragments, 1}],
%% 	mnesia:create_table(scene, [{frag_properties, FragProps}, {attributes, record_info(fields, scene)}]).

%% init_online_player_frag_table(Nodes) ->
%%     FragProps = [{node_pool, Nodes}, {n_fragments, 1}],
%%     mnesia:create_table(online_player, [{frag_properties, FragProps}, {attributes, record_info(fields, online_player)}]).


%%
%% Local Functions
%%
do_add_db_node(NewNode) ->
    %% 启动远程节点
    ok = rpc:call(NewNode, mnesia, start, []),
    %% 将远程节点加入 当前数据库
    {ok, [NewNode]} = mnesia:change_config(extra_db_nodes, [NewNode]),
    {atomic, ok} = rpc:call(NewNode, mnesia, change_table_copy_type, [schema, NewNode, disc_copies]),

    %% 取得所有表格
    TabList = mnesia:system_info(tables),

    %% 对原有分片表属性的table, 将NewNode 加至 node_pool中, 自动加一个分片上去
    [begin 
	 mnesia:change_table_frag(Tab, {add_node, NewNode}),
	 mnesia:change_table_frag(Tab, {add_frag, [NewNode]})             	 
     end
     ||	Tab <- TabList, mnesia:table_info(Tab, frag_properties) =/= []],
	    
    ok.

start_db() ->
    case mnesia:system_info(is_running) of
	no ->
	    Nodes = [node()],
	    case mnesia:create_schema(Nodes) of
		ok ->
		    start_db_1();
		{error,{_,{already_exists,_}}}->
		    start_db_1();
		{error,{_,{Reason,_}}} ->
		    data_helper:format("init schema error:~p~n", [Reason]),
		    fail
	    end;
	yes ->
	    ok
    end,
    ok.


%% 关闭数据库
close_db() ->
    %% 关闭所有相关节点
    DBNodes = mnesia:system_info(db_nodes),
    {_Ret, _BadNodes} = rpc:multicall(DBNodes, mnesia, stop, []),
    ok.

start_db_1() ->
    %% 启动所有相关节点
    DBNodes = mnesia:system_info(db_nodes),
    {_Ret, BadNodes} = rpc:multicall(DBNodes, mnesia, start, []),
    
    %% 确定节点是否正常启动，(无盘节点除外)
    DiscNodes = mnesia:table_info(schema, disc_copies),
    case lists:any(fun(E) -> lists:member(E, DiscNodes) end, BadNodes) of
	false -> ok;
	_ -> erlang:error({db_nodes_not_start, BadNodes})
    end,

    mnesia:wait_for_tables([schema, db_update_history], 3000), 
    db_update:start(DiscNodes),
    ok.


get_backup_tables(BackupFile) when is_list(BackupFile) -> 
    F =
        fun({schema, Tab, _Options}=Item, Acc) ->
		{[Item], [Tab | Acc]};
           (Other, Acc) ->
                {[Other], Acc}
        end,
    Mod = mnesia_backup,
    {ok, TableList} = mnesia:traverse_backup(BackupFile, Mod, dummy, read_only, F, []),
    TableList.
