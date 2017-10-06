%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  消息路由器, 用于一个玩家内部各个系统之间的通讯
%%%  比如包裹和玩家的通讯
%%% @end
%%% Created : 30 Mar 2010 by  <>
%%%-------------------------------------------------------------------
-module(router).

-behaviour(gen_server).

-include("router.hrl").

%% API
-export([start_link/0, stop/0]).
-export([send/2, send/3, cast/3, cast/4, broadcast/3]).
-export([send2/2, send2/3]).
-export([register_handler/1, unregister_handler/1]).
-export([make_event/2, make_event_source/3, make_event_target/3, make_event_source_target/4]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-define(SERVER, ?MODULE). 

-type event() :: pos_integer() | atom().
-type object() :: pid() | atom().

%%%===================================================================
%%% API
%%%===================================================================

%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link() ->
    gen_server:start_link({local, router}, ?MODULE, [], []).

stop()->
    gen_server:call(router, stop).

%% EventList是一个消息监听的列表
-spec register_handler(list()) -> ok.
register_handler(MsgList) ->
    gen_server:call(router, {register_handler, MsgList}).

%% 解除处理器监听事件
-spec unregister_handler(list()) -> ok.
unregister_handler(MsgList)->
    gen_server:call(router, {unregister_handler, MsgList}).

%% 以同步的方式发送消息, 有监听该消息的系统都将收到这条消息
-spec send(object(), event(), any()) -> any().
send(Target, Event, MsgData)->
    CanLogError = true,
    do_send(Event, undefined, Target, MsgData, CanLogError).

-spec send(object(), event()) -> any().
send(Target, Event) ->
    CanLogError = true,
    do_send(Event, undefined, Target, null, CanLogError).

%% 功能同send，只是不记录log 
-spec send2(object(), event(), any()) -> any().
send2(Target, Event, MsgData)->
    CanLogError = false,
    do_send(Event, undefined, Target, MsgData, CanLogError).


-spec send2(object(), event()) -> any().
send2(Target, Event) ->
    CanLogError = false,
    do_send(Event, undefined, Target, null, CanLogError).


%% 以异步的方式发送消息给Target
-spec cast(object(), object(), event(), any()) -> any().
cast(Src, Target, Event, MsgData)->
    CanLogError = true,
    do_cast(Event, Src, Target, MsgData, CanLogError).

-spec cast(object(), event(), any()) -> any().
cast(Src, Event, MsgData)->
    CanLogError = true,
    do_cast(Event, Src, undefined, MsgData, CanLogError).

%% 以异步的方式广播事件
-spec broadcast(event(), object(), any()) -> any().
broadcast(Event, Src, MsgData) ->
    do_broadcast(Event, Src, MsgData).

%% 生成只关心事件的消息
-spec make_event(event(), any()) -> tuple().
make_event(Event, Handler) ->
    {#msg{event=Event}, Handler}.

%% 生成关心事件和事件源的消息
-spec make_event_source(event(), object(), any()) -> tuple().
make_event_source(Event, Src, Handler) ->
    {#msg{event=Event, src=Src}, Handler}.

%% 生成关心事件和目标的消息
-spec make_event_target(event(), object(), any()) -> tuple().
make_event_target(Event, Target, Handler) ->
    {#msg{event=Event, target=Target}, Handler}.

%% 生成关心事件, 源 和目标的消息
-spec make_event_source_target(event(), object(), object(), any()) -> tuple().
make_event_source_target(Event, Src, Target, Handler) ->
    {#msg{event=Event, src=Src, target=Target}, Handler}.

%%%===================================================================
%%% gen_server callbacks
%%%===================================================================
init([]) ->
    process_flag(trap_exit, true),
    ets:new(router, [set, protected, named_table]),
    {ok, []}.

handle_call(stop, _From, State) ->
    {stop, normal, State};
handle_call({register_handler, MsgList}, _From, State) ->
    F = fun(Msg, Handler)->
		case ets:lookup(router, Msg)of
		    [_Old] ->
			ets:delete(router, Msg),
			ets:insert(router, {Msg, Handler}),
			logger:log(error, "exist_handler:~p, msg:~p~n", [Handler, Msg]);
		    [] -> 
			ets:insert(router, {Msg, Handler})
		end
	end,
    [F(Message, Handler) || {Message, Handler} <- MsgList],
    {reply, ok, State};

handle_call({unregister_handler, List}, _From, State) ->
    [ets:delete_object(router, Obj) || Obj <- List],
    {reply, ok, State};

handle_call(_Request, _From, State) ->
    {reply, ok, State}.

handle_cast(Request, State) ->
    logger:log(error, "router unknown request:~p~n", [Request]),
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
    data_helper:format("router terminate~n"),
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
%% 获得消息的处理器
get_handler(Message, CanLogError) ->
    % 把4中消息类型构造出来循环查找处理器
    L = make_msg_list(Message),
    case find_handlers(L) of
	[] -> 
	    case CanLogError of
		true -> logger:log(error, "not found handler: ~p~n", [Message]);
		_ -> ok
	    end,

	    undefined;
	Handlers -> Handlers
    end.

make_msg_list(#msg{event=_E, src=undefined, target=undefined}=M) ->
    [M];
make_msg_list(#msg{event=E, src=_S, target=undefined}=M) ->
    [#msg{event=E}, M];
make_msg_list(#msg{event=E, src=undefined, target=_T}=M) ->
    [#msg{event=E}, M];
make_msg_list(#msg{event=E, src=S, target=T} = M) ->
    [#msg{event=E}, #msg{event=E, src=S}, #msg{event=E, target=T}, M].	    

find_handlers(L) ->
    Ret = case lists:member(router, ets:all()) of
	      true ->
		  F = fun(Msg, AccIn) ->
			      case ets:lookup(router, Msg) of
				  []-> AccIn;
				  [{_M, Handler}] -> [Handler | AccIn]
			      end
		      end,
		  lists:foldl(F, [], L);
	      false  -> []
	  end,
    Ret.

do_send(Event, Src, Target, MsgData, CanLogError)->
    Msg = #msg{event=Event, src=Src, target=Target},
    case get_handler(Msg, CanLogError) of
	[{Pid, Mod}] ->
	    case MsgData of 
		null -> gen_msgproc:call_to_mod(Pid, Mod, Msg);
		_  -> gen_msgproc:call_to_mod(Pid, Mod, {Msg, MsgData})
	    end;
	[Pid] ->
	    case MsgData of 
		null -> gen_server:call(Pid, Msg);
		_  -> gen_server:call(Pid, {Msg, MsgData})
	    end;
	undefined -> undefined
    end.

do_cast(Event, Src, Target, MsgData, CanLogError) ->
    Msg = #msg{event=Event, src=Src, target=Target},
    case get_handler(Msg, CanLogError) of
	[{Pid, Mod}] -> gen_msgproc:cast_to_mod(Pid, Mod, {Msg, MsgData});
	[Pid]        -> gen_server:cast(Pid, {Msg, MsgData});
	undefined    -> ok;
	Any -> erlang:error({badret, Any, Msg})
	       
    end.

do_broadcast(Event, Src, MsgData)->
    Msg = #msg{event=Event, src=Src},
    CanLogError = true,
    Handlers = get_handler(Msg, CanLogError),
    [gen_server:cast(Handler, {Msg, MsgData}) || Handler <- Handlers].

%% --------------------------测试代码-----------------------------------
-include_lib("eunit/include/eunit.hrl").

%% test() ->
%%     start_link(),
%%     L = [make_event(req_enter, self()),
%% 	 make_event_source(req_swap, test, self()),
%% 	 make_event_target(req_logout, test, self()),
%% 	 make_event_source_target(req_logout, test, test1, self())
%% 	],
%%     register_handler(L),
%%     ?assertEqual([self()], get_handler(#msg{event=req_enter})),
%%     ?assertEqual([self()], get_handler(#msg{event=req_enter, src=test})),
%%     ?assertEqual([self()], get_handler(#msg{event=req_enter, target=test})),
%%     ?assertEqual([self()], get_handler(#msg{event=req_enter, src=test, target=test1})),

%%     broadcast(req_enter, test, []),
%%     unregister_handler(L).


