%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  农场仓库
%%% @end
%%% Created : 13 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_depot).

-include("enum_def.hrl").
-include("router.hrl").
-include("packet_def.hrl").
-include("tplt_def.hrl").

-export([start/1, handle_cast/2]).
-export([open_depot/1, send_depot/2, save/1]).
-export([new/1, make/2, read/1, add_item/2, add_item/3, add_items/2, get_item/2, 
	 del_item/3, get_item_count/2, get_item_count_by_index/2, get_index/2, get_index_by_tempid/2]).
-export([is_exist/2, foldl/3, get_grids/1, compress/1]).

-record(farm_depot, {account, grids}).

start(Account) ->
    [router:make_event_source(?msg_req_lock_depot_item, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_unlock_depot_item, Account, {self(), ?MODULE})
    ].

handle_cast({#msg{src=Account}, #req_lock_depot_item{item_inst_id=InstID}}, State) ->
    #farm_depot{grids=Grids} = read(Account),
    NewGrids = pack:lock(InstID, Grids),
    NewDepot = make(Account, NewGrids),
    send_depot(Account, NewDepot),
    save(NewDepot),
    {noreply, State};
handle_cast({#msg{src=Account}, #req_unlock_depot_item{item_inst_id=InstID}}, State) ->
    #farm_depot{grids=Grids} = read(Account),
    NewGrids = pack:unlock(InstID, Grids),
    NewDepot = make(Account, NewGrids),
    send_depot(Account, NewDepot),
    save(NewDepot),
    {noreply, State}.


new(Account)->
    make(Account, pack:new(0)).

make(Account, Grids) ->
    #farm_depot{account=Account, grids=Grids}.

%% 打开农场仓库的界面
open_depot(Account)->
    Depot = read(Account),
    send_depot(Account, Depot).

%% 读取数据
read(Account)->
    db_common:select_one(farm_depot, Account).

%% 增加物品
add_item(Account, Item) when is_atom(Account) ->
    Depot = read(Account),
    add_item(Depot, Item);
add_item(Depot, Item) when is_record(Depot, farm_depot), is_tuple(Item) ->
    add_item(Depot, Item, 1);
add_item(Depot, ItemList) when is_record(Depot, farm_depot), is_list(ItemList) ->
    do_add_item(Depot, ItemList);
add_item(Depot, Items) ->
    erlang:error({badarg, Depot, Items}).

add_item(Account, Item, Count) when is_atom(Account), Count > 0 ->
    Depot = read(Account),
    add_item(Depot, Item, Count);
add_item(Depot, ItemID, Count) when is_record(Depot, farm_depot), 
				    is_integer(ItemID), ItemID > 0 ->
    do_add_item(Depot, item:make_item(ItemID), Count);
add_item(Depot, Item, Count) when is_record(Depot, farm_depot), Count > 0 ->
    do_add_item(Depot, Item, Count);
add_item(Depot, _Item, Count) when Count == 0 ->
    Depot.

do_add_item(Depot, []) ->
    Depot;
do_add_item(Depot, [Item|ItemList]) ->
    NDepot = do_add_item(Depot, Item, 1),
    do_add_item(NDepot, ItemList). 

do_add_item(#farm_depot{grids=GridVec}=Depot, Item, Count) ->
    NGrids = 
	case pack:add_item(Item, Count, GridVec) of
	    full ->
		Grids1 = pack:resize(pack:get_size(GridVec)+1, GridVec),
		pack:add_item(Item, Count, Grids1);
	    Grids2 -> 
		Grids2
	end,
    Depot#farm_depot{grids=NGrids}.

add_items([], Depot) ->
    Depot;
add_items([{Item, Count} | ItemList], Depot) ->
    NDepot = do_add_item(Depot, Item, Count),
    do_add_item(NDepot, ItemList).

save(Depot)->
    db_common:write(Depot).    

%% 获得物品
%% 返回Item
get_item(Index, #farm_depot{grids=GridVec}) when Index >= 0->
    pack:get_item(Index, GridVec).

get_item_count(InstID, #farm_depot{grids=GridVec}) when InstID > 0->
    Index = pack:get_index(InstID, GridVec),
    pack:get_item_count(Index, GridVec).

get_item_count_by_index(Index, #farm_depot{grids=GridVec}) when Index >= 0->
    pack:get_item_count(Index, GridVec).

get_index(InstID, #farm_depot{grids=GridVec}) when InstID > 0 ->
    pack:get_index(InstID, GridVec).

get_index_by_tempid(TempId,  #farm_depot{grids=GridVec}) when TempId > 0 ->
    pack:get_index_by_tempid(TempId, GridVec);
get_index_by_tempid(TempID, Depot) ->
    erlang:error({badarg, TempID, Depot}).

%% 删除物品
del_item(Index, Count, #farm_depot{grids=GridVec} = Depot) when Index >= 0, Count > 0 ->
    Pack1 = pack:del_item(Index, Count, GridVec),
    Pack2 = pack:compress(Pack1),
    Depot#farm_depot{grids=Pack2};
del_item(Index, Count, Depot) ->
    erlang:error({badarg, Index, Count, Depot}).

foldl(Fun, Acc0, #farm_depot{grids = GridVec})->
    pack:foldl(Fun, Acc0, GridVec).

get_grids(#farm_depot{grids=Grids}) ->
    Grids.

%%　发送农场仓库数据
send_depot(Account, #farm_depot{grids=Grids}) ->
    pack:send_farm_package(Account, ?pt_farmdepot, Grids).

%% 判断指定的物品是否存在
is_exist(Account, InstID) ->
    #farm_depot{grids=Grids} = read(Account),
    case pack:get_index(InstID, Grids) of
	not_found -> false;
	_ -> true
    end.

compress(Depot) ->
    NGrids = pack:compress(Depot#farm_depot.grids),
    Depot#farm_depot{grids = NGrids}.

%% ----------------------------test--------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

add_item_test()->
    Item = item:new(1, 1, #stime{}),
    Item1 = item:new(2, 2, #stime{}),
    Depot = new(test),
    erlymock:start(),
    erlymock:stub(db_common, select_one, [farm_depot, test], [{return, Depot}]),
    erlymock:stub(tplt, get_data2, [item_tplt, 1], [{return, #item_tplt{overlap=100}}]),
    erlymock:stub(tplt, get_data2, [item_tplt, 2], [{return, #item_tplt{overlap=100}}]),
    erlymock:replay(),
    ?assertEqual(Item, get_item(0, add_item(test, Item))),
    ?assertEqual(Item, get_item(0, add_item(test, [Item, Item1]))),

    Depot1 = add_item(Depot, [Item, Item1]),
    ?assertEqual(Item, get_item(0, Depot1)),
    ?assertEqual(Item1, get_item(1, Depot1)),
    ?assertEqual(Item, get_item(0, add_item(Depot, Item, 1))),

    erlymock:verify(),

    ok.

-endif.
