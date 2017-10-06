%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  ͨ�õİ�������ģ��(�����Ķ���: װ��Ʒ������, ����������Ұ���)
%%% @end
%%% Created : 22 Mar 2010 by  <>
%%%-------------------------------------------------------------------
-module(pack).


-include("packet_def.hrl").
-include("tplt_def.hrl").

%% API
-export([send_package/3, send_farm_package/3, broadcast/3]).
-export([new/1, resize/2, get_size/1,
	 set_item/3, set_item/4, get_item/2, del_item/2, del_item/3, compress/1,
	 swap/3, move/4, index_of/2, count_if/2,
	 to_list/1, is_grid_empty/2]).
-export([find_available_grid/2, find_available_grid/3]).
-export([add_item/2, add_item/3, add_item/4, add_item_from_grid/4]).
-export([lock_grid/2, unlock_grid/2, is_lock/2, lock/2, unlock/2, unlock_all/1]).
-export([get_index/2, get_item_count/2, set_item_count/3, foldl/3, get_index_by_tempid/2]).
-export([is_grid_full/2, find_same_item_grid/3, has_empty_grid/1]).

-define(GRID_LOCK, 1).   %% ��������
-define(GRID_UNLOCK, 0). %% ����û����

%%%===================================================================
%%% API
%%%===================================================================

%% ���Ͱ��������ݸ��ͻ���
send_package(Account, Type, Pack) ->
    net_helper:send2client(Account, #notify_package{account=Account, type=Type, grid_vec=Pack}).

send_farm_package(Account, Type, Pack) ->
    net_helper:farm2client(Account, #notify_package{account=Account, type=Type, grid_vec=Pack}).    

broadcast(Account, Type, Pack) ->
    player:send_to_around(Account, #notify_package{account=Account, type=Type, grid_vec=Pack}).

%% �´���һ������
%% ����:Pack
new(InitCount)->
    array:new(InitCount, {default,#pack_grid{}}).

%% �԰�������
%% ����:NewPack
resize(Size, Pack)->
    array:resize(Size, Pack).

get_size(Pack) ->
    array:size(Pack).

%% �ж�һ�������Ƿ��ǿյ�
is_grid_empty(Index, Pack) ->
    case get_grid(Index, Pack) of	
	#pack_grid{count=0} -> true;
	_ -> false
    end.

%% �ж�һ�������Ƿ�������
is_grid_full(Index, Pack)->
    case get_grid(Index, Pack) of	
	#pack_grid{count=0} ->
	    false;
	#pack_grid{count=C, item_data=#item{template_id=TempID}} -> 
	    #item_tplt{overlap=Overlap} = tplt:get_data2(item_tplt, TempID),
	    Overlap =:= C
    end.

%% ָ���ĸ�������count����Ʒ��, �Ƿ�����
is_grid_overflow(Count, Index, Pack)->
    case get_grid(Index, Pack) of	
	#pack_grid{count=0} ->
	    false;
	#pack_grid{count=C, item_data=#item{template_id=TempID}} -> 
	    #item_tplt{overlap=Overlap} = tplt:get_data2(item_tplt, TempID),
	    C + Count > Overlap;
	_ -> false
    end.

    
%% ��һ��ָ��������������һ����Ʒ
%% ����: NewGrid
add_item(Item, Pack) when is_record(Item, item)->
    add_item(Item, 1, Pack);
add_item(ItemTempID, Pack) when ItemTempID > 0->
    add_item(ItemTempID, 1, Pack);
add_item(Item, _)->
    erlang:error(badarg, Item).

add_item(#item{template_id=TemplateID}=Item, Count, Pack)->
    case find_available_grid(TemplateID, Count, Pack) of
	full ->
	    full;
	Index ->
	    add_item_impl(Item, Count, Index, Pack)
    end;
add_item(ItemTempID, Count, Pack) when is_integer(ItemTempID), ItemTempID > 0->
    Item = item:make_item(ItemTempID),
    add_item(Item, Count, Pack);
add_item(Item, _Count, _)->
    erlang:error({badarg, Item}).

%% ��ָ���ĸ�����������Ʒ
%% Item: ��Ʒģ��ID | #item{}
add_item(Item, Count, Index, Pack) when Index >=0 ->
    case is_grid_overflow(Count, Index, Pack) of
	false ->
	    add_item_impl(Item, Count, Index, Pack);
	true ->
	    false
    end.

add_item_impl(ItemID, Count, Index, Pack) when is_integer(ItemID), ItemID > 0->
    add_item_impl(item:make_item(ItemID), Count, Index, Pack);
add_item_impl(Item, Count, Index, Pack) ->
    case get_grid(Index, Pack) of
	#pack_grid{count = 0} ->
	    array:set(Index, #pack_grid{count=Count,item_data=Item}, Pack);
	#pack_grid{count=Count1}=Grid ->
	    array:set(Index, Grid#pack_grid{count=Count1+Count}, Pack)
    end.

%% ��SrcIndex���ó���Ʒ�ѵ���TargetIndex������
add_item_from_grid(Count, SrcIndex, TargetIndex, Pack) ->
    NPack = del_item(SrcIndex, Count, Pack),
    %% �ж�Ŀ������ܷ���²����������Ʒ
    case is_grid_overflow(Count, TargetIndex, NPack) of
	false ->
	    Item = pack:get_item(SrcIndex, Pack),
	    {ok, add_item(item:get_tempid(Item), Count, TargetIndex, NPack)};
	true ->
	    {error, full}
    end.
    

%% ��һ��ָ������Ʒ�ŵ�������
%% ������������Ƿ��Ѿ�������Ʒ
%% ����NewPack
set_item(Item, Index, Pack) ->
    set_item(Item, 1, Index, Pack).

set_item(empty, _Count, Index, Pack)when Index>=0->
    array:set(Index, #pack_grid{count=0,item_data=#item{},lock=?GRID_UNLOCK}, Pack);
set_item(Item, Count, Index, Pack)when is_record(Item, item), Index >= 0 ->
    array:set(Index, #pack_grid{count=Count, item_data=Item}, Pack).

%% �����Ʒ
%% ����Item
%% ���ָ���ĸ���û����Ʒ, ��ᷢ��ƥ���쳣, ���㶨λbug
-spec get_item(non_neg_integer(), array()) -> empty | tuple().
get_item(Index, Pack) when Index >= 0->
    case get_grid(Index, Pack) of
	#pack_grid{item_data=Item} ->
	    case Item of
		#item{template_id=0} -> empty;
		_ -> Item
	    end;
	undefined -> erlang:error({badarg, Index, Pack})
    end;
get_item(Index, Pack)->
    erlang:error({badarg, Index, Pack}).

get_item_id(Index, Pack) ->
    case get_item(Index, Pack) of
	empty -> 0;
	Item  -> item:get_tempid(Item)
    end.

%% �����Ʒ����
get_item_count(Index, Pack) when Index >= 0 ->
    #pack_grid{count=Count} = get_grid(Index, Pack),
    Count;
get_item_count(Index, Pack) ->
    erlang:error({badarg, Index, Pack}).

%% ������Ʒ����
set_item_count(Index, Count, Pack) when Index >= 0, Count > 0 ->
    case get_grid(Index, Pack) of
	#pack_grid{}=Grid ->
	    array:set(Index, Grid#pack_grid{count=Count}, Pack);
	undefined -> erlang:error({badarg, Index, Pack})
    end.

    
%% ɾ����Ʒ
%% �����µ�Pack
del_item(Index, Pack) ->
    del_item(Index, 1, Pack).

del_item(Index, Count, Pack) when Index >=0, Count > 0 ->
    case is_grid_empty(Index, Pack) of
	true ->
	    erlang:error({grid_empty, Index, Pack});
	false -> 
	    Grid = get_grid(Index, Pack),
	    case Grid#pack_grid.count - Count of
		0 ->
		    array:set(Index, make_none_grid(), Pack);
		C when C > 0 ->
		    array:set(Index, Grid#pack_grid{count=C}, Pack);
		C -> %% ָ����ɾ���������ڸ�������Ʒ������
		    erlang:error({'del item count', Count, C})
	    end
	end;
del_item(_Index, 0, Pack) ->
    %% Ŀǰ��Ʒ���Զ�ɾ���������п��ܿͻ��˺ͷ����ͬʱɾ��ĳ����Ʒ, ���������Ӧ����
    Pack;
del_item(Index, Count, Pack) ->
    erlang:error({badarg, Index, Count, Pack}).

%% ɾ������, �������������Ʒ
compress(Pack) ->
    L = array:to_list(Pack),
    NewL = [Grid || Grid <- L, Grid#pack_grid.count > 0],
    array:from_list(NewL, #pack_grid{}).
    

%% ���ҷ��������ĵ�һ��Ԫ�ص�Index
%% û�ҵ�����-1
%% Fun(X) -> true | false.
-spec index_of(any(), any()) -> integer().
index_of(Fun, Pack) ->
    util:array_index_of(Fun, Pack).


%% ͳ�Ʒ���������Ԫ�ظ���
%% Fun(X) -> true | false.
-spec count_if(any(), any()) -> integer().
count_if(Fun, Pack) ->
    util:array_count_if(Fun, Pack).

%% ��ͬһ�������н���������Ʒ
%% ����������Ӷ�û����Ʒ, ����fail
%% �����ɹ�, ����NewPack
swap(Index1, Index2, Pack) ->
    case is_grid_empty(Index1, Pack) and 
	 is_grid_empty(Index2, Pack) of
	true ->
	    erlang:error({'two grid empty', Index1, Index2, Pack});
	false ->
	    %% ��ͬ���͵���ƷҪ���Զѵ�, ��ͬ���͵���Ʒֱ�ӽ���
	    ItemID1 = get_item_id(Index1, Pack),
	    ItemID2 = get_item_id(Index2, Pack),
	    case ItemID1 =:= ItemID2 of
		true ->
		    #item_tplt{overlap=Overlap} = tplt:get_data2(item_tplt, ItemID1),
		    do_overlap_item(Index1, Index2, Overlap, Pack);
		false ->
		    do_swap(Index1, Index2, Pack)
	    end
    end.

%% �ѵ���Ʒ
do_overlap_item(_SrcIndex, _TargetIndex, 1, Pack) ->
    Pack;
do_overlap_item(SrcIndex, TargetIndex, Overlap, Pack) ->
    SrcCount = get_item_count(SrcIndex, Pack),
    TgtCount = get_item_count(TargetIndex, Pack), 
    SrcRestCount = erlang:max(0, SrcCount - (Overlap - TgtCount)),
    TgtRestCount = erlang:min(Overlap, TgtCount + SrcCount),
    NPack = case SrcRestCount =:= 0 of
		true ->
		    del_item(SrcIndex, SrcCount, Pack);
		false ->
		    set_item_count(SrcIndex, SrcRestCount, Pack)
	    end,
    set_item_count(TargetIndex, TgtRestCount, NPack).

do_swap(Index1, Index2, Pack) ->
    Grid1 = get_grid(Index1, Pack),
    Pack1 = array:set(Index1, get_grid(Index2, Pack), Pack),
    array:set(Index2, Grid1, Pack1).    

%% �ڲ�ͬ���������ƶ���Ʒ
%% ����������Ӷ�û����Ʒ, ����fail
%% ����ɹ�, ����: {NewPack1, NewPack2}
move(Index1, Pack1, Index2, Pack2) ->
    case is_grid_empty(Index1, Pack1) and is_grid_empty(Index2, Pack2) of
	true ->
	    fail;
	false ->
	    Item1 = get_item(Index1, Pack1),
	    Count1 = get_item_count(Index1, Pack1),
	    Item2 = get_item(Index2, Pack2),
	    Count2 = get_item_count(Index2, Pack2),
	    {set_item(Item2, Count2, Index1, Pack1),
	     set_item(Item1, Count1, Index2, Pack2)}
    end.

%% �Ѱ�������ת����list
to_list([])->
    [];
to_list(Pack)->
    array:to_list(Pack).
    
%% �����Ƿ�����
-spec is_lock(pos_integer(), any())-> boolean().
is_lock(Index, Pack) when Index >= 0->
    case get_grid(Index, Pack) of
	#pack_grid{lock=?GRID_LOCK} -> true;
	#pack_grid{lock=?GRID_UNLOCK} -> false;
	_ -> erlang:error({badarg, Index})
    end.  
    

%% ��������
-spec lock_grid(pos_integer(), any()) -> any().
lock_grid(Index, Pack) when is_integer(Index), Index >= 0->
    change_grid_lock_state(Index, Pack, ?GRID_LOCK);
lock_grid(Index, _Pack) ->
    erlang:error({badarg, Index}).

lock(InstID, Pack) ->
    Index = get_index(InstID, Pack),
    lock_grid(Index, Pack).

%% �����������
-spec unlock_grid(non_neg_integer(), any()) -> any().
unlock_grid(Index, Pack) when is_integer(Index), Index >= 0->
    change_grid_lock_state(Index, Pack, ?GRID_UNLOCK);
unlock_grid(Index, _Pack) ->
    erlang:error({badarg, Index}).

unlock(InstID, Pack) ->
    Index = get_index(InstID, Pack),
    unlock_grid(Index, Pack).

%% ȫ������
unlock_all(Pack) ->
    F = fun(_Idx, Grid) ->
		case Grid of
		    #pack_grid{lock=?GRID_LOCK} ->
			Grid#pack_grid{lock=?GRID_UNLOCK};
		    _ ->
			Grid
		end
	end,
    array:map(F, Pack).

change_grid_lock_state(Index, Pack, LockState) ->
    Grid = get_grid(Index, Pack),
    array:set(Index, Grid#pack_grid{lock=LockState}, Pack).

%% ���ҿ��õĸ���, ���ظ��ӵ�����
%% �����������, ����full
find_available_grid(TemplateID, Pack)->
    find_available_grid(TemplateID, 1, Pack).

-spec find_available_grid(pos_integer(), pos_integer(), any()) -> non_neg_integer() | full.
find_available_grid(TemplateID, Count, Pack)->
    case array:is_array(Pack) of
	true ->
	    Size = array:size(Pack),
	    case find_same_item_grid(TemplateID, Count, Pack, Size, 0) of
		-1 -> find_empty_grid(TemplateID, Pack, Size, 0);
		Index -> Index
	    end;
	false ->
	    erlang:error({badarg, Pack})
    end.

%% �Ƿ��пո���
has_empty_grid(Pack) ->
    Size = array:size(Pack), 
    case find_empty_grid(0, Pack, Size, 0) of
	full -> false;
	_ -> true
    end.

%% ��������ͬID����Ʒ�����ҿ��Էŵ���Count��������Ʒ
-spec find_same_item_grid(pos_integer(), pos_integer(), array()) -> non_neg_integer().
find_same_item_grid(ItemID, Count, Pack) ->
    find_same_item_grid(ItemID, Count, Pack, get_size(Pack), 0).

find_same_item_grid(TempID, Count, Pack, Size, StartIdx) ->
    #item_tplt{overlap=Overlap} = tplt:get_data2(item_tplt, TempID),
    do_find_same_item_grid(TempID, Count, Overlap, Pack, Size, StartIdx).

do_find_same_item_grid(_TempID, _Count, _Overlap, _Pack, Size, Idx) when Idx >= Size->
    -1;
do_find_same_item_grid(TempID, Count, Overlap, Pack, Size, Idx) ->
    case get_item(Idx, Pack) of
	#item{template_id=TempID} ->
	    case get_item_count(Idx, Pack) + Count > Overlap of 
		true  -> do_find_same_item_grid(TempID, Count, Overlap, Pack, Size, Idx+1);
		false -> Idx 
	    end;
	_ -> do_find_same_item_grid(TempID, Count, Overlap, Pack, Size, Idx+1)
    end.

%%�����ҿո�����
find_empty_grid(_TemplateID, _Pack, Size, StartIdx) when StartIdx >= Size->
    full;
find_empty_grid(_TemplateID, Pack, Size, StartIdx)->
    case is_grid_empty(StartIdx, Pack) of
	true -> % �ո���
	    StartIdx;
	false -> % ����Ʒ
	    find_empty_grid(_TemplateID, Pack, Size, StartIdx+1)
    end.

%% ������������
%% Fun = fun(Index, {Item, Count}, Acc)
foldl(Fun, Acc0, Pack)->
    F = fun(Index, #pack_grid{item_data=Item, count=Count}, Acc) ->
		Fun(Index, {Item, Count}, Acc) 
	end,
    array:foldl(F, Acc0, Pack).

get_grid(Index, Pack) when Index >= 0->
    case Index < array:size(Pack) of
	true ->
	    array:get(Index, Pack);
	false ->
	    erlang:error({badarg, Index, Pack})
    end;
get_grid(Index, Pack) ->
    erlang:error({badarg, Index, Pack}).

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% ����һ������Ʒ
make_none_grid()->
    #pack_grid{count=0, item_data=#item{template_id=0, instance_id=0}}.

%% ������Ʒ��ʵ��ID�����Ʒ���ڵĸ�������
get_index(InstID, Pack) ->
    do_get_index(InstID, 0, array:size(Pack), Pack). 

do_get_index(_InstID, Index, Size, _Pack) when Index =:= Size->
    not_found;
do_get_index(InstID, Index, Size, Pack) ->
    case get_item(Index, Pack) of
	#item{instance_id=InstID} -> Index;
	_ -> do_get_index(InstID, Index+1, Size, Pack)
    end.

%% ��ȡ��һ������tempid����Ʒ���ڵĸ�������
get_index_by_tempid(TempId, Pack) ->
    do_get_index_by_tempid(TempId, 0, array:size(Pack), Pack).

do_get_index_by_tempid(_TempId, Index, Size, _Pack) when Index =:= Size->
    not_found;
do_get_index_by_tempid(TempId, Index, Size, Pack) ->
    case get_item(Index, Pack) of
	#item{template_id=TempId} -> Index;
	_ -> do_get_index_by_tempid(TempId, Index+1, Size, Pack)
    end.

%%%===================================================================
%%% ���Դ���
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

swap_test()->
    Package = new(10),
    catch swap(0, 1, Package),
    
    Item1 = #item{instance_id=1, template_id=1},
    Pack1 = add_item_impl(Item1, 1, 0, Package),    
    Pack2 = swap(0, 1, Pack1),

    ?assertEqual(empty, get_item(0, Pack2)),
    ?assertEqual(Item1, get_item(1, Pack2)),
    ok.
    
move_test()->
    Pack1 = new(1),
    Pack2 = new(2),
    ?assertEqual(fail, move(0, Pack1, 0, Pack2)),
    Item1 = #item{instance_id=1, template_id=1},
    Item2 = #item{instance_id=2, template_id=2},
    NewPack1 = add_item_impl(Item1, 1, 0, Pack1),
    NewPack2 = add_item_impl(Item2, 1, 0, Pack2),
    {P1, P2} = move(0, NewPack1, 0, NewPack2),
    ?assertEqual(Item2, get_item(0, P1)),
    ?assertEqual(Item1, get_item(0, P2)),
    ok.

lock_grid_test() ->
    Pack = new(1),
    Pack1 = lock_grid(0, Pack),
    ?assertEqual(#pack_grid{lock=?GRID_LOCK}, get_grid(0, Pack1)),    
    Pack2 = unlock_grid(0, Pack1),
    ?assertEqual(#pack_grid{lock=?GRID_UNLOCK}, get_grid(0, Pack2)),
    ok.

add_item_test() ->
    erlymock:start(),
    erlymock:stub(tplt, get_data2, [item_tplt, 1], [{return, #item_tplt{overlap=100}}]),
    erlymock:replay(),

    Pack = new(10),
    Item = #item{template_id=1},
    Pack1 = add_item(Item, Pack),
    ?assertEqual(#pack_grid{count=1,item_data=Item}, get_grid(0, Pack1)),
    Pack2 = add_item(Item, Pack1),
    data_helper:format("Pack1:~p~nPack2:~p~n", [Pack1, Pack2]),
    ?assertEqual(#pack_grid{count=2,item_data=Item}, get_grid(0, Pack2)),
    ?assertEqual(#pack_grid{count=0,item_data=#item{}}, get_grid(1, Pack2)),

    Item1 = item:new(1, 1, #stime{}),
    Pack3 = add_item(Item1, 5, Pack),
    ?assertEqual(#pack_grid{count=5,item_data=Item1}, get_grid(0, Pack3)),

    erlymock:verify(),
    ok.

%% add_item_from_grid_test() ->
%%     erlymock:start(),
%%     erlymock:stub(tplt, get_data2, [item_tplt, 1], [{return, #item_tplt{overlap=10}}]),
%%     erlymock:replay(),

%%     guid:start_link(),

%%     Pack = new(10),
%%     Item = #item{template_id=1},
%%     Pack1 = add_item_impl(Item, 9, 0, Pack),
%%     Pack2 = add_item_impl(Item, 5, 1, Pack1),
%%     {error, full} = add_item_from_grid(5, 1, 0, Pack2),
%%     {ok, Pack3} = add_item_from_grid(1, 1, 0, Pack2),
%%     ?assertEqual(10, get_item_count(0, Pack3)),
%%     ?assertEqual(4, get_item_count(1, Pack3)),

%%     Pack4 = add_item_impl(Item, 9, 0, Pack),
%%     Pack5 = add_item_impl(Item, 1, 1, Pack4),
%%     {ok, Pack6} = add_item_from_grid(1, 1, 0, Pack5),
%%     ?assertEqual(10, get_item_count(0, Pack6)),
%%     ?assertEqual(#pack_grid{count=0,item_data=item:new(0,0)}, get_grid(1, Pack6)),

%%     erlymock:verify(),
%%     ok.

do_find_same_item_grid_test() ->
    ItemID = 1,
    Pack = new(10),
    Item = #item{template_id=ItemID},
    Pack1 = add_item_impl(Item, 9, 0, Pack),
    
    ?assertEqual(0, do_find_same_item_grid(ItemID, 1, 10, Pack1, get_size(Pack1), 0)),
    ?assertEqual(-1, do_find_same_item_grid(ItemID, 2, 10, Pack1, get_size(Pack1), 0)),    
    ok.

-endif.