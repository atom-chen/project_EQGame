%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 玩家
%%% @end 
%%% Created :  6 Aug 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(farm).

%% API
-export([search/3, add_farm_exp/4]).

%%%===================================================================
%%% API
%%%===================================================================
%% 查询玩家
search(Account, Req, HostName) ->
    AccountAtom = list_to_atom(Account),
    SearchResult = rpc:call(HostName, gm_tool, search_farm_player, [AccountAtom]),
    case SearchResult of
	{SAccount, SExp, SMoney} ->
	    Req:respond({200, [{"Content-Type", "text/html; charset=utf-8"}], 
			 [create(HostName, SAccount, SExp, SMoney)]});
	[] ->
	    Req:respond({200, [], ["fail"]})
    end.

%% 增加玩家属性
add_farm_exp(Account, Req, HostName, Exp) ->
    Result = rpc:call(HostName, gm_tool, add_farm_exp, [Account, Exp]),
    case Result of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["doesn't exist node!"]});
	{badrpc, Reason} ->
	    Req:respond({200, [], [Reason]});
	_ ->
	    Req:respond({200, [], ["ok"]})
    end.

%%%===================================================================
%%% Internal functions
%%%===================================================================
create(HostName, Account, Exp, Money) ->
    "<tr id=\"farm_row\" style=\"background-color:#E7E8EB\">" ++ 
	"<td>" ++ atom_to_list(Account) ++ "</td>" ++
	"<td>" ++ integer_to_list(Exp) ++ "</td>" ++
	"<td>" ++ integer_to_list(Money) ++ "</td>" ++
	create_1(HostName, Account) ++
    "</tr>".

create_1(HostName, Account) ->
	    "<td>" ++ 
		"<a href=\"#\" onclick=\"show_add_farm_exp('"++ atom_to_list(HostName) ++"', '"++ atom_to_list(Account) ++"')\">增加农场经验</a></br>" ++
		"</td>".

