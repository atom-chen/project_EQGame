%% Author: linyijie
%% Created: 2010-03-16
%% Description: 该模块是对house_data表的操作
-module(db_house).

%%
%% Include files
%%
-include("table_info.hrl").
-include("packet_def.hrl").
-include_lib("stdlib/include/qlc.hrl").
%%
%% Exported Functions
%%
-export([write/1, delete/1, select/1]).

%% 获取指定玩家的数据
-spec select(atom()) -> tuple().
select(PlayerName) when is_atom(PlayerName) ->
    db_common:select_one(house_data, PlayerName).

%% 增加/更新 房屋的数据
-spec write(list()|tuple()) -> ok.
write(HouseData) when is_list(HouseData) ->
    db_common:write(HouseData);

write(HouseData) when is_record(HouseData, house_data) ->
    db_common:write(HouseData).



%% 删除在线场景 
-spec delete(atom()) -> any().
delete(PlayerName) when is_atom(PlayerName) ->
    db_common:delete(house_data, PlayerName).

%% test() ->
%%     ok = write(#house_data{owner=lyj}),
%%     HouseData = #house_data{owner=lyj, instance_id = 1},
%%     ok = write(HouseData),
%%     HouseData = select(lyj),
%%     delete(lyj),
%%     [] = select(lyj).
