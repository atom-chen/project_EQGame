%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  对农场的操作
%%% @end
%%% Created : 24 Mar 2010 by  <>

-module(db_farm).

%% Include files

-include("table_info.hrl").
-include("packet_def.hrl").
-include_lib("stdlib/include/qlc.hrl").

%% Exported Functions

-export([write/1, write/2, select/1]).

-define(table_name, player_farm).

%% 获取指定玩家的数据
-spec select(list() | atom()) -> list() | tuple().
select([]) ->
    [];
select(Keys) ->
    db_memdisc:read(?table_name, Keys).

%% 增加/更新
write(Farm, Depot) ->
    db_memdisc:write(Farm, Depot).

write(Farm) ->
    db_memdisc:write(Farm).



