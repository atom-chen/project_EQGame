%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  对玩家包裹的数据库操作
%%% @end
%%% Created : 24 Mar 2010 by  <>

-module(db_pack).

%% Include files

-include("table_info.hrl").
-include("packet_def.hrl").
-include_lib("stdlib/include/qlc.hrl").

%% Exported Functions

-export([write/1, write/2, delete/1, select/1, get_item_id_list/1]).
-export([test/0]).

-define(table_name, player_pack).

%% 获取指定玩家的数据
-spec select(atom()) -> tuple().
select(PlayerName) when is_atom(PlayerName) ->
    db_common:select_one(?table_name, PlayerName).

%% 增加/更新 包裹的数据
-spec write(tuple()) -> ok.
write(Data) when is_record(Data, ?table_name) ->
    db_common:write(Data);
write(Data) ->
    erlang:error({badarg, Data}).

%% 增加/更新 两个包裹的数据
write(Pack1, Pack2)->
    F = fun() -> 
		mnesia:write(Pack1), 
		mnesia:write(Pack2)
	end,
    ok = mnesia:activity(transaction, F, [], mnesia_frag).

%% 删除包裹
-spec delete(atom()) -> any().
delete(PlayerName) when is_atom(PlayerName) ->
    db_common:delete(?table_name, PlayerName).


get_item_id_list(Account) ->
    case db_common:select_one(?table_name, Account) of
	#player_pack{grid_vec=Bag} -> 
	    do_get_item_id_list(Bag);
	_ -> []
    end.


do_get_item_id_list(Bag) ->
    F = fun(_Idx, {TradeItem, _Count}, List) ->
		case item:is_empty(TradeItem) of
		    false ->
			[item:get_tempid(TradeItem) | List];	
		    true -> List	
		end
	end,
    pack:foldl(F, [], Bag).

test() ->
    ok.
