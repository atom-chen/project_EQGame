-module(cupid_match).

-behaviour(gen_server).

-include("enum_def.hrl").
-define(null, undefined).

-record(cupid_match_state, {}).

-export([init/1, handle_call/3, handle_cast/2, handle_info/2, terminate/2, code_change/3]).
-export([start_link/0]).
-export([start_match/0]).

start_link() ->
    gen_server:start_link({local, ?MODULE}, ?MODULE, [], []).

init([]) ->
    process_flag(trap_exit, true),
    {ok, #cupid_match_state{}}.

start_match()->
    gen_server:cast(?MODULE, start_match),
    ok.

match()->
    Item = db_cupid:first_registered(),
    match_loop(Item).

match_loop(undefined)->
    ok;

match_loop({Account, Sex, _Tags, _State}=Item)->
    db_cupid:update_status(Account, matching),
    case Sex of 
	?st_boy->
	    io:format("Account ~p is a boy~n", [Account]),
	    Girl = db_cupid:first_matched_girl(),
	    boy_match_girl(Item, Girl);
	?st_girl->
	    io:format("Account ~p is a girl~n", [Account]),
	    Boy = db_cupid:first_matched_boy(),
	    girl_match_boy(Item, Boy)
    end.
    %%match_loop(db_cupid:next_registered(Account)).

match_faild(Account)->
    Item = db_cupid:select(Account),
    case Item of
	?null->
	    ok;
	_->
	    {Account, _Sex, _Tags, Status} = Item,
	    case Status of 
		canceled->
		    ok;
		_->
		    db_cupid:update_status(Account, matched)
	    end
    end.

boy_match_girl(Boy, Girl)->
    case Girl of 
	?null ->
	    io:format("there is no matched girl~n"),
	    {Account, _Sex, _Tags, _State} = Boy,
	    match_faild(Account),
	    false;
	_->
	    Result = match(Boy, Girl),
	    case Result of
		false->
		    boy_match_girl(Boy, db_cupid:next_matched_girl(Girl));
		_->
		    {Account, _Sex, _Tags, _State} = Girl,
		    case check_status(Account) of 
			true->
			    match_success(Result);
			_->
			    match_faild(Account)
		    end
	    end
    end.

girl_match_boy(Girl, Boy)->
    case Boy of 
	?null ->
	    io:format("there is no matched boy~n"),
	    {Account, _Sex, _Tags, _State} = Girl,
	    match_faild(Account),
	    false;
	_->
	    Result = match(Boy, Girl),
	    case Result of
		false->
		    girl_match_boy(Girl, db_cupid:next_matched_boy(Boy));
		_->
		    {Account, _Sex, _Tags, _State} = Boy,
		    case check_status(Account) of 
			true->
			    match_success(Result);
			_->
			    match_faild(Account)
		    end
	    end
    end.

check_status(Account)->
    Item = db_cupid:select(Account),
    case Item of 
	?null->
	    false;
	_ ->
	    {Account, _Sex, _Tags, Status}=Item,
	    case Status of 
		matching->
		    true;
		matched->
		    true;
		_->
		    false
	    end
    end.

match_success({ok, {BoyAccount, GirlAccount, _Tags}=Data})->
    db_cupid:update_status(BoyAccount, successed),
    db_cupid:update_status(GirlAccount, successed),
    cupid:matched_successed(Data),
    ok.

match({BoyAccount, ?st_boy, BoyTags, _BoyState}, {GirlAccount, ?st_girl, GirlTags, _GirlState})->
    io:format("boy ~p match girl ~p~n",[BoyAccount, GirlAccount]),
    Divide = BoyTags -- GirlTags,
    case length(Divide)=:=length(BoyTags) of
	true->
	    false;
	_ ->
	    {ok, {BoyAccount, GirlAccount, BoyTags -- Divide}}
    end.

handle_call(_Request, _From, State) ->
    Reply = ok,
    {reply, Reply, State}.

handle_cast(start_match, State) ->
    match(),
    {noreply, State};

handle_cast(_Request, State) ->
    {noreply, State}.

handle_info(_Info, State) ->
    {noreply, State}.

terminate(_Reason, _State) ->
    ok.

code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

%%%===================================================================
%%% ²âÊÔ´úÂë
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

hit_test()->
    ?assertEqual(1, 1),
    Boy={boy, ?st_boy, [a,b], matching},
    Girl={girl, ?st_girl, [a,c], matching},
    Result=match(Boy, Girl),
    %% io:format("~p~n",[Result]),
    ?assertEqual({ok, [a]}, Result),
    ok.
    
-endif.


