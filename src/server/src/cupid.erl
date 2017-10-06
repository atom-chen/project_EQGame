%%% -------------------------------------------------------------------
%%% Author  : NoteBook
%%% Description :
%%%
%%% Created : 2009-9-24
%%% -------------------------------------------------------------------
-module(cupid).

-behaviour(gen_msgproc).

-include("router.hrl").
-include("packet_def.hrl").

-record(cupid_state,{match_state, matched_list}). %%��Ҫ����ƥ����̵�״̬
-export([start_link/0]).
-export([init/1, handle_call/3, handle_cast/2, handle_info/2, terminate/2, code_change/3, register/1]).
-export([match_finished/0, matched_successed/1, cancel_participate/1]).

start_link()->
    gen_msgproc:start_link({local, ?MODULE}, ?MODULE, [], []).  

init(_Args)->
    db_cupid:create(),
    cupid_match:start_link(),
    {ok, #cupid_state{match_state=upon, matched_list=[]}}.

register(_Args)->
    [
     router:make_event(match_successed, {self(), ?MODULE}), 
     router:make_event(match_finished, {self(), ?MODULE}),
     router:make_event(?msg_req_cupid_participate, {self(), ?MODULE}),
     router:make_event(?msg_req_cupid_cancel_participate, {self(), ?MODULE}),
     router:make_event(?msg_req_cupid_girl_confirm, {self(), ?MODULE}),
     router:make_event(?msg_req_cupid_boy_confirm, {self(), ?MODULE})
     %%router:make_event_target(add_coin, Account, {self(), player_coin}),
     %%router:make_event_target(dec_coin, Account, {self(), player_coin})
    ].

handle_call(MSG, _From, State) ->
    {reply, MSG, State}.

%%ƥ��ɹ�
handle_cast({#msg{event=match_successed}, {_BoyAccount, GirlAccount, Tags}=Data}, State)->
    io:format("Data:~p~n",[Data]),
    net_helper:send2client(list_to_atom(GirlAccount), #notify_cupid_girl_match_result{tags=Tags}),
    MatchedList = State#cupid_state.matched_list ++ [Data],
    {noreply, State#cupid_state{matched_list=MatchedList}};

%%ƥ�����
handle_cast(#msg{event=match_finished}, State)->
    {noreply, State};

%%����
handle_cast({#msg{event=?msg_req_cupid_participate}, #req_cupid_participate{account=Account,sex=Sex, tags=Tags}}, State)->
    Result = db_cupid:add(Account, Sex, Tags),
	case Result of 
	    true ->
		net_helper:send2client(list_to_atom(Account),#notify_cupid_participate{result=1}),
		cupid_match:start_match();		
	    _->
		net_helper:send2client(list_to_atom(Account),#notify_cupid_participate{result=0})
	end,
    {noreply, State};

%%ȡ������
handle_cast({#msg{event=?msg_req_cupid_cancel_participate}, #req_cupid_cancel_participate{account=Account}}, State)->
    Item = db_cupid:select(Account),
    case Item of 
	undefined->ok;
	_->
	    {Account, _Sex, _Tags, Status} = Item,
	    case Status of 
		successed->
		    net_helper:send2client(list_to_atom(Account),#notify_cupid_cancel_participate{result=0});
		_->
		    %% db_cupid:update_status(Account, cancel),
		    db_cupid:remove(Account),
		    net_helper:send2client(list_to_atom(Account),#notify_cupid_cancel_participate{result=1})
	    end
    end,
    {noreply, State};

%%Ů��ȷ��
handle_cast({#msg{event=?msg_req_cupid_girl_confirm}, #req_cupid_girl_confirm{account=Girl, result=Result}}, 
#cupid_state{matched_list=MatchedList}=State)->
    {Boy, Girl, Tags} = get_matched_girl(Girl, MatchedList),
    case Result of 
	1->
	    %%ȥ������
	    net_helper:send2client(list_to_atom(Boy), #notify_cupid_boy_match_result{tags=Tags, girl_result=1}),
	    {noreply, State};
	2->
	    %%ȥŮ����
	    net_helper:send2client(list_to_atom(Boy), #notify_cupid_boy_match_result{tags=Tags, girl_result=2}),
	    {noreply, State};
	_->
	    %%ȡ�����������ӳɹ�������ɾ��, ״̬����Ϊmatched
	    db_cupid:update_status(Boy, matched),
	    db_cupid:update_status(Girl, matched),
	    NewMatchList = MatchedList -- [{Boy, Girl, Tags}],
	    {noreply, State#cupid_state{matched_list=NewMatchList}}
    end;

%%����ȷ��
handle_cast({#msg{event=?msg_req_cupid_boy_confirm},
 #req_cupid_boy_confirm{account=Boy, result=Result, girl_result=GirlResult}}, 
	    #cupid_state{matched_list=MatchedList}=State)->
    {Boy, Girl, Tags} = get_matched_boy(Boy, MatchedList),
    case Result of 
	1->
	    %%ƥ��ɹ�
	    %%����״̬
	    db_cupid:remove(Boy),
	    db_cupid:remove(Girl),
	    %% ���뷿��
	    case GirlResult of 
		1->
		    player_change_scene:force_enter_player_house(Girl, Boy);
		_->
		    player_change_scene:force_enter_player_house(Boy, Girl)
	    end;
	_->
	    %%ȡ������Ů���ӳɹ�������ɾ��, ״̬����Ϊmatched
	    db_cupid:update_status(Boy, matched),
	    db_cupid:update_status(Girl, matched)
    end,
    NewMatchedList=MatchedList--[{Boy, Girl, Tags}],
    {noreply, State#cupid_state{matched_list=NewMatchedList}};

handle_cast(_MSG, State)->
    {noreply, State}.

handle_info(_Info, State) ->
    {noreply, State}.

terminate(_Reason, _State) ->
    ok.

code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

matched_successed(Data)->
    gen_server:cast(cupid, {#msg{event=match_successed}, Data}).

match_finished()->
    gen_server:cast(cupid, #msg{event=match_finished}).

get_matched_boy(_BoyAccount, [])->
    undefined;
    
get_matched_boy(BoyAccount, [{Boy, _Girl, _Tag}=Item|MatchedList])->
    case Boy==BoyAccount of 
	true->
	    Item;
	_ ->
	    get_matched_boy(BoyAccount, MatchedList)
end.

get_matched_girl(_GirlAccount, [])->
    undefined;
    
get_matched_girl(GirlAccount, [{_Boy, Girl, _Tag}=Item|MatchedList])->
    io:format("Girl ~p is GirlAccount ~p~n", [Girl, GirlAccount]),
    case Girl==GirlAccount of 
	true->
	    Item;
	_ ->
	    get_matched_girl(GirlAccount, MatchedList)
end.

cancel_participate(Account)->
    gen_server:cast(cupid, {#msg{event=?msg_req_cupid_cancel_participate}, #req_cupid_cancel_participate{account=Account}}).
