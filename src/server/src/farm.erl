%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%%
%%% @end
%%% Created : 11 Nov 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm).

-include("router.hrl").
-include("packet_def.hrl").

-behaviour(gen_server).

%% API
-export([start_link/4]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2, terminate/2, 
	 code_change/3]).

-export([cast/4, logout/1, get_account/1, 
	 get_name/1, get_eq_pid/1, get_task/1, set_task/2,
	 get_wallow_info/1]).

-define(SERVER, ?MODULE). 

-record(state, {account, 
		mm_pid, 
		eq_pid, 
		task, 
		wallow_info  %% 防沉迷信息
	       }).

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
start_link(Account, MMPid, EQPid, IsWallowAccount) ->
    gen_server:start_link({local, get_name(Account)}, ?MODULE, [Account, MMPid, EQPid, IsWallowAccount], []).

cast(Pid, Src, Event, MsgData)->
    gen_server:cast(Pid, {msg, Src, Event, MsgData}).

logout(FarmName) ->
    gen_server:call(FarmName, req_logout).

get_account(#state{account=Account}) ->
    Account.

get_eq_pid(#state{eq_pid=EQPid}) ->
    EQPid.

get_task(State) ->
    State#state.task.

set_task(Task, State) ->
    State#state{task=Task}.

get_name(Account) when is_atom(Account) ->
    list_to_atom(atom_to_list(Account) ++ "_farm").

%% 防沉迷相关信息
get_wallow_info(#state{wallow_info=WallowInfo}) ->
    WallowInfo.

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
register_handler(Account)->
    L = farm_shop:start(Account) ++ 
	farm_log:start(Account) ++ 
	farm_comment:start(Account) ++
	farm_decoration:start(Account) ++
	farm_depot:start(Account)++
	farm_setting:start(Account) ++
	player_farm:start(Account) ++
	farm_item:start(Account) ++ 
	farm_task:start(Account),
    [undefined = put(Msg, Handler) || {Msg, Handler} <- L],
    ok.
    
init([Account, MMPid, EQPid, IsWallowAccount]) ->    
    process_flag(trap_exit, true),

    register_handler(Account),

    %% 防沉迷信息
    WallowInfo = wallow:on_login(Account, farm, IsWallowAccount),

    {ok, #state{account=Account, 
		mm_pid=MMPid, 
		eq_pid=EQPid, 
		wallow_info=WallowInfo
		}}.

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
handle_call(req_logout, _From, #state{account=Account, mm_pid=MMPid}=State) ->
    Packet = protocal:encode(#notify_repeat_login{account=Account}),
    tcp_mm:send(MMPid, Packet), 
    {stop , repeat_login, true, State};
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
handle_cast({msg, Src, Event, MsgData}, State) ->
    do_cast(Event, Src, undefined, MsgData, State);
    %% {noreply, State};
handle_cast({send2client, Packet}, #state{mm_pid=MMPid}=State) ->
    tcp_mm:send(MMPid, Packet),
    {noreply, State};
handle_cast(_Msg, State) ->
    data_helper:format("unknown handle cast Msg:~p~n", [_Msg]),
    {noreply, State}.

do_cast(Event, Src, Target, MsgData, State) ->
    Msg = #msg{event=Event, src=Src, target=Target},
    case get_handler(Msg) of
	[{_Pid, Mod}] -> Mod:handle_cast({Msg, MsgData}, State);
	[Pid]        -> gen_server:cast(Pid, {Msg, MsgData});
	undefined    -> ok;
	Any -> erlang:error({badret, Any, Msg})       
    end.

%% 获得消息的处理器
get_handler(Message) ->
    % 把4中消息类型构造出来循环查找处理器
    L = make_msg_list(Message),
    case find_handlers(L) of
	[] -> 
	    logger:log(error, "not found handler: ~p~n~p~n", [Message, get()]),
	    undefined;
	Handlers -> Handlers
    end.

make_msg_list(#msg{src=undefined, target=undefined}=M) ->
    [M];
make_msg_list(#msg{event=E, src=undefined}=M) ->
    [#msg{event=E}, M];
make_msg_list(#msg{event=E, target=undefined}=M) ->
    [#msg{event=E}, M].%% ;
%% make_msg_list(#msg{event=E, src=S, target=T} = M) ->
%%     [#msg{event=E}, #msg{event=E, src=S}, #msg{event=E, target=T}, M].	    

find_handlers(L) ->
    F = fun(Msg, AccIn) ->
		case get(Msg) of
		    undefined -> AccIn;
		    Handler -> [Handler | AccIn]
		end
	end,
    lists:foldl(F, [], L).

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
terminate(_Reason, #state{mm_pid=MMPid, 
			  wallow_info=WallowInfo,
			  account=Account}) ->
    tcp_mm:stop(MMPid),

    %% 防沉迷, 保存上线时间
    wallow:on_logout(Account, farm, WallowInfo),
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


