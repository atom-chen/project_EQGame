%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 系统时间
%%% @end
%%% Created : 30 Jun 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(sys_time).

-behaviour(gen_msgproc).

%% API
%% -export([start_link/0, trigger/2, is_exceed/2]).
%% -export([register/1, get_timer/0, subscribe/3]).
%% -export([get_trigger_time/0]).
-export([start_link/0, register/1]).

-export([subscribe/4, subscribe/5, subscribe/6]).
-export([stop/1]).
%% -export([test/0]).
%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-include("packet_def.hrl").
-include("router.hrl").
-include("common_def.hrl").

-define(SERVER, ?MODULE). 

-record(state, {}).

%% 监听指定的消息, 实现gen_msgproc的注册方法
register([])-> 
    [router:make_event(?msg_req_sys_time, self())
    ].

%%%===================================================================
%%% API
%%%===================================================================
%% 根据在线时长订阅
%% 时间单位为秒
-spec subscribe(integer(), atom(), integer(), tuple(), integer(), atom()) -> reference().
subscribe(?dt_onlinetime, Account, OnlineTime, LoginTime, Timer, Event) 
  when is_integer(Timer), is_atom(Event) ->
    DiffOnlineTime = datetime:diff_time(LoginTime, datetime:localtime()),
    TotalOnlineTime = OnlineTime + DiffOnlineTime,
    RestTriggerTime = calc_rest_trigger_time(TotalOnlineTime, Timer),
    erlang:send_after(RestTriggerTime * 1000, ?SERVER, {calc_onlinetime, Timer, Account, Event}).
%% 根据间隔时间订阅
%% 时间单位为秒
subscribe(?dt_interval, Account, Timer, Event)
  when is_atom(Account), is_integer(Timer), is_atom(Event)->
    erlang:send_after(Timer * 1000, ?SERVER, {calc_interval, Account, Timer, Event}).
%% 根据固定时间订阅
subscribe(?dt_datetime, Account, Timer, Event, Param)
  when is_atom(Account), is_record(Timer, stime), is_atom(Event) ->
    LocalTime = datetime:localtime(),
    DiffTime = datetime:diff_time(LocalTime, Timer),
    subscribe(?dt_datetime, Account, DiffTime, Event, Param);
subscribe(?dt_datetime, Account, Timer, Event, Param)
  when is_atom(Account), is_integer(Timer), is_atom(Event) ->
    case Timer > 0 of
	true ->
	    erlang:send_after(Timer * 1000, ?SERVER, {calc_datetime, Account, Event, Param});
	_ ->
	    ok
    end;
subscribe(?dt_interval, Account, Timer, Event, RepeatCount)
    when is_atom(Account), is_integer(Timer), is_atom(Event), is_integer(RepeatCount) ->
    erlang:send_after(Timer * 1000, ?SERVER, {calc_interval, Account, Timer, Event, RepeatCount}).

stop(TimeRef) ->
    erlang:cancel_timer(TimeRef).

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
%% 请求获取系统时间
handle_cast({#msg{event=?msg_req_sys_time, src=Account}, #req_sys_time{}}, State)->
    net_helper:send2client(Account, #notify_sys_time{sys_time=datetime:localtime()}),
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
%% 每一小时在线时长触发一次
handle_info({calc_onlinetime, Timer, Account, Event}, State) ->
    case util:is_process_alive(Account) of
	false ->
	    ok;
	_ ->
	    TimerRef = erlang:send_after(Timer * 1000, self(), {calc_onlinetime, Timer, Account, Event}),
	    router:cast(sys_time, Account, Event, TimerRef)
    end,
    {noreply, State};
%% 根据固定的时间触发
handle_info({calc_datetime, Account, Event, Param}, State) ->
    case util:is_process_alive(Account) of
	false ->
	    ok;
	_ ->
	    router:cast(Account, Event, Param)
    end,
    {noreply, State};
%% 根据时间间隔触发
handle_info({calc_interval, Account, Timer, Event}, State) ->
    case util:is_process_alive(Account) of
	false ->
	    ok;
	_ ->
	    TimerRef = erlang:send_after(Timer * 1000, self(), {calc_interval, Account, Timer, Event}),
	    router:cast(Account, Event, TimerRef)
    end,
    {noreply, State};
%% 根据时间间隔触发
handle_info({calc_interval, Account, Timer, Event, RepeatCount}, State) ->
    case util:is_process_alive(Account) of
	false ->
	    ok;
	_ ->
	    case RepeatCount > 0 of
		true ->
		    TimerRef = erlang:send_after(Timer * 1000, 
						 self(), 
						 {calc_interval, Account, Timer, Event, RepeatCount - 1}),
		    router:cast(Account, Event, TimerRef);
		_ ->
		    ok
	    end
    end,
    {noreply, State};
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
%% 计算剩余触发的时间
-spec calc_rest_trigger_time(integer(), integer()) -> integer().
calc_rest_trigger_time(OnlineTime, Time) 
  when is_integer(OnlineTime), is_integer(Time), OnlineTime > 0, OnlineTime > Time ->
    OnlineTime rem Time;
calc_rest_trigger_time(OnlineTime, Time)
  when is_integer(OnlineTime), is_integer(Time), OnlineTime > 0, OnlineTime < Time ->
    Time - OnlineTime;
calc_rest_trigger_time(OnlineTime, Time)
  when is_integer(OnlineTime), is_integer(Time), OnlineTime > 0, OnlineTime =:= Time ->
    Time;
calc_rest_trigger_time(OnlineTime, Time)
  when is_integer(OnlineTime), is_integer(Time), OnlineTime =:= 0 ->
    Time.
%%%===================================================================
%%% Test case
%%%===================================================================
%% test() ->
%%     router:send(test1, #req_sys_time{}).
