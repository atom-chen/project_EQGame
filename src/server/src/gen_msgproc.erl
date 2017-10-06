%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  通用的网络消息监听器, 只要是想处理网络消息的进程, 都被定义为消息的监听器,
%%%  该监听器是基于gen_server, 对gen_server的一个简单的扩展, 绝大部分的使用
%%%  和gen_server的使用是一致的, 唯一的不同是需要多一个注册消息的函数register()
%%%  该函数返回一个注册的消息列表, 例如:register() -> [?msg_req_create].
%%% @end
%%% Created :  7 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(gen_msgproc).
 
%% API
-export([start_link/3,start_link/4]).

-export([behaviour_info/1]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-export([call_to_mod/3, cast_to_mod/3]).

%%%===================================================================
%%% API
%%%===================================================================
-spec behaviour_info(atom()) -> 'undefined' | [{atom(), arity()}].

behaviour_info(callbacks) ->
    [{init,1},{register,1},{handle_call,3},{handle_cast,2},{handle_info,2},
     {terminate,2},{code_change,3}];
behaviour_info(_Other) ->
    undefined. 
%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
%% Target: 进程名字 | 进程Pid
start_link(Mod, Args, Options) ->
    gen_server:start_link(?MODULE, {Mod, Args}, Options).

start_link(Name, Mod, Args, Options) ->
    gen_server:start_link(Name, ?MODULE, {Mod, Args}, Options).

%% 同步发送给回调模块处理
call_to_mod(Pid, Mod, Msg) when is_pid(Pid) ->
    gen_server:call(Pid, {cb, Mod, Msg}).

%% 异步发送给回调模块处理
cast_to_mod(Pid, Mod, Msg) ->
    gen_server:cast(Pid, {cb, Mod, Msg}).

%%%===================================================================
%%% gen_server callbacks
%%%===================================================================
init({Mod, Args}) ->
    process_flag(trap_exit, true),

    {A1, A2, A3} = now(),
    random:seed(A1, A2, A3),

    undefined = put(mod, Mod),
    L = Mod:register(Args),
    router:register_handler(L),
    undefined = put('$msgs', L),
    try 
     	Mod:init(Args)
    catch
    	A: B ->
    	    router:unregister_handler(L),
	    %% 处理错误，让讯息更明确
	    logger:log(error, "~p~n ~p~n stack:~p~n", 
			     [A, B, erlang:get_stacktrace()]),

    	    throw({A, B})
    end.

handle_call({cb, Mod, Data}, From, State) ->
    Mod:handle_call(Data, From, State);
handle_call(Request, From, State) ->
    Mod = get(mod),
    Mod:handle_call(Request, From, State).

handle_cast({cb, Mod, Msg}, State) ->
    Mod:handle_cast(Msg, State);
handle_cast(Msg, State) ->
    Mod = get(mod),
    Mod:handle_cast(Msg, State).

handle_info(Info, State) ->
    Mod = get(mod),
    Mod:handle_info(Info, State).

terminate(Reason, State) ->
    Mod = get(mod),
    L = get('$msgs'),
    try
	Mod:terminate(Reason, State)
    catch ErrType: ErrMsg ->
        error_logger:error_msg("Error in ~p:terminate: ~p.\n", [Mod, {ErrType, ErrMsg}])    	
    end,
    router:unregister_handler(L).

code_change(OldVsn, State, Extra) ->
    Mod = get(mod),
    Mod:code_change(OldVsn, State, Extra).
