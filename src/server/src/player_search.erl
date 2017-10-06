%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   玩家搜索
%%% @end
%%% Created : 28/ 03/ 2010 by  <hjx>

-module(player_search).


-include("packet_def.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").

-export([handle_cast/2, start/1]).


start(Account) ->
    [
     router:make_event_source(?msg_req_search_player_by_nick, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_search_player_by_mail, Account, {self(), ?MODULE})
    ].


%%%===================================================================
%%% 处理handle_cast
%%%===================================================================
handle_cast({_Msg, #req_search_player_by_nick{nick=NickStr}}, 
	    State) ->
    UserName = NickStr,
    BasicDataList = db_common:index_select(player_basic_data, UserName, #player_basic_data.username),
    Account = player_data:get_account(State),
    do_search(Account, BasicDataList),
    {noreply, State};



handle_cast({_Msg, #req_search_player_by_mail{mail=MailAddrStr}}, 
	    State) ->
    SearchAccount = list_to_atom(string:to_lower(MailAddrStr)),
    BasicDataList = db_common:select_list(player_basic_data, SearchAccount),
    Account = player_data:get_account(State),
    do_search(Account, BasicDataList),
    {noreply, State}.

%%%===================================================================
%%% 内部函数
%%%===================================================================
do_search(Account, BasicDataList) when is_atom(Account), is_list(BasicDataList) ->
    case BasicDataList of
	[] -> 
	    sys_msg:send(Account, ?err_not_find_match_players);
	_ ->
	    AccountList = [atom_to_list(basic_data:get_data(account, X)) || X <- BasicDataList],
	    %% 请求EQ用户信息
	    [router:cast(Account, req_eq_vcard, A) || A <- AccountList],

	    Packet = #notify_search_player{players=AccountList},
	    net_helper:send2client(Account, Packet)
    end.
