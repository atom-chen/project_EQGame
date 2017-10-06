%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%%
%%% @end
%%% Created : 19 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_level_manager).

-behaviour(gen_msgproc).

-include("router.hrl").

%% API
-export([start_link/0]).
-export([register/1]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-export([test/0]).

-define(SERVER, ?MODULE). 

-record(subscribe_data, {level, event}).
-record(state, {subscribe_list}).

%%%===================================================================
%%% API
%%%===================================================================
register([]) ->
    [router:make_event_target(subscribe_player_level, ?MODULE, {self(), ?MODULE}),
     router:make_event_target(player_level_up, ?MODULE, {self(), ?MODULE})
    ].

%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link() ->
    gen_msgproc:start_link({local, ?MODULE}, ?MODULE, [], []).

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
    {ok, #state{subscribe_list=[]}}.

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
handle_call({#msg{event=subscribe_player_level}, {Level, Event}}, _From, 
	    #state{subscribe_list=SList}=State) ->
    {Reply, NList} = 
    case is_exist(SList, Level, Event) of
	false ->
	    List1 = [#subscribe_data{level=Level, event=Event}|SList],
	    {true, List1};
	_ ->
	    {false, SList}
    end,
    NState = State#state{subscribe_list=NList},
    {reply, Reply, NState};
handle_call({#msg{event=player_level_up}, {Account, Level}}, _From, 
	    #state{subscribe_list=SList}=State) ->
    notify_player_level_up(SList, Account, Level),
    {reply, ok, State};
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
%% 通知玩家等级上升
-spec notify_player_level_up(list(), atom(), integer()) -> list().
notify_player_level_up(SList, Account, Level)
  when is_list(SList), is_atom(Account), is_integer(Level)->
    F = fun(SEvent1, SLevel1, Account1, Level1) ->
		case SLevel1 =:= Level1 of
		    true ->
			router:cast(Account1, SEvent1, Level1);
		    _ ->
			ok
		end
        end,

    [F(SEvent, SLevel, Account, Level) || #subscribe_data{event=SEvent, level=SLevel} <- SList].

%% 是否存在该等级与事件
-spec is_exist(list(), integer(), atom()) -> atom().
is_exist([], _Level, _Event) ->
    false;
is_exist([#subscribe_data{level=SLevel, event=SEvent}|SList], Level, Event) ->
    case (SLevel =:= Level) and (SEvent =:= Event) of
	true ->
	    true;
	_ ->
	    is_exist(SList, Level, Event)
    end.

%%%===================================================================
%%% Test case
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").

test() ->
    router:send(player_level_manager, subscribe_player_level, {1, test}).

-ifdef(TEST).

is_exist_test() ->
    SList = [#subscribe_data{level=1, event=1},
	     #subscribe_data{level=2, event=2},
	     #subscribe_data{level=1, event=2},
	     #subscribe_data{level=2, event=1}],
    ?assertEqual(true, is_exist(SList, 1, 1)),
    ?assertEqual(false, is_exist(SList, 1, 3)),
    ?assertEqual(true, is_exist(SList, 1, 2)).

-endif.
