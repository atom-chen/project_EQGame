%%% @author  hjx
%%% @copyright (C) 2010, 
%%% @doc
%%%  微博数据库相关
%%% @end
%%% Created : 24 Mar 2010 by  <hjx>

-module(db_weibo).

-export([get/1, write/1]).

-define(table_name, player_weibo).

-spec get(list()) -> tuple() | [].
get(Account) ->
    db_common:select_one(?table_name, Account).

-spec write(tuple()) -> ok.
write(Data) ->
    db_common:write(Data).


