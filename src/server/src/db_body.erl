%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  对玩家身体的数据库操作
%%% @end
%%% Created : 24 Mar 2010 by  <>

-module(db_body).

%% Include files

-include("table_info.hrl").
-include("packet_def.hrl").
-include_lib("stdlib/include/qlc.hrl").

%% Exported Functions

-export([write/1, delete/1, select/1, get_body/1]).

-define(table_name, player_body).

%% 获取装备数据
-spec get_body(atom()) -> any().
get_body(Account) when is_atom(Account) ->
    case  db_common:select_one(?table_name, Account) of
	{?table_name, Account, Body} ->
	    Body;
	_ ->
	    undefined
    end.


%% 获取指定数据
-spec select(atom()) -> tuple().
select(Account) when is_atom(Account) ->
    db_common:select_one(?table_name, Account).

%% 增加/更新 数据
-spec write(tuple()) -> ok.
write(Data) when is_record(Data, ?table_name)->
    db_common:write(Data);
write(Data) ->
    F = fun() -> mnesia:write(player_body, Data, write) end,
    ok = mnesia:activity(transaction, F, [], mnesia_frag).

%% 删除
-spec delete(atom()) -> any().
delete(PlayerName) when is_atom(PlayerName) ->
    db_common:delete(?table_name, PlayerName).
