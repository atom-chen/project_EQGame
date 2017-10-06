%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  ����һ��ҵ����ݿ����
%%% @end
%%% Created : 24 Mar 2010 by  <>

-module(db_coin).

%% Include files

-include("table_info.hrl").
-include("packet_def.hrl").
-include_lib("stdlib/include/qlc.hrl").

%% Exported Functions

-export([write/1, write/2, delete/1, select/1]).
-export([test/0]).

-define(table_name, player_coin).

%% ��ȡָ����ҵ�����
-spec select(atom()) -> tuple().
select(Account) when is_atom(Account) ->
    db_common:select_one(?table_name, Account).

%% ����/����
-spec write(tuple()) -> ok.
write(Data) when is_record(Data, ?table_name) ->
    db_common:write(Data).

%% ����/����
write(Pack1, Pack2)->
    F = fun() -> 
		mnesia:write(Pack1), 
		mnesia:write(Pack2)
	end,
    ok = mnesia:activity(transaction, F, [], mnesia_frag).

%% ɾ��
-spec delete(atom()) -> any().
delete(PlayerName) when is_atom(PlayerName) ->
    db_common:delete(?table_name, PlayerName).

test() ->
    ok.
