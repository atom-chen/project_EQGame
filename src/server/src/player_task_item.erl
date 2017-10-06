%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% ���������Ʒ
%%% @end
%%% Created :  8 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_task_item).

%% API
-export([get_fixed_items/3, get_selected_items/3]).
-export([get_item_list/3, get_item_list_1/3]).
-export([contains/2]).

%%%===================================================================
%%% API
%%%===================================================================
%% ��ȡ�̶���Ʒ
%% ���ظ�ʽ:[��ƷId, ��ƷId]
-spec get_fixed_items(list(), list(), list()) -> list().
get_fixed_items([], [], Items) ->
    Items;
get_fixed_items([Item|ItemList], [Count|ItemCount], Items) ->
    get_fixed_items(ItemList, ItemCount, get_items(Item, Count, Items)).

%% ��ȡ��Ʒ�б�
%% ���ظ�ʽ[{��ƷId, ��Ʒ����}]
-spec get_item_list(list(), list(), list()) -> list().
get_item_list([], [], Items) ->
    Items;
get_item_list([Item|ItemList], [Count|ItemCount], Items) ->
    get_item_list(ItemList, ItemCount, [{Item, Count}|Items]).

get_item_list_1([], [], Items) ->
    Items;
get_item_list_1([Item|ItemList], [Count|ItemCount], Items) ->
    get_item_list_1(ItemList, ItemCount, get_items(Item, Count, Items)).
				     

%% ��ȡѡ�����Ʒ
%% ���ظ�ʽ:[��ƷId, ��ƷId]
-spec get_selected_items(list(), list(), list()) -> list().
get_selected_items([], [], _ItemId) ->
    [];
get_selected_items([Item|ItemList], [Count|ItemCount], ItemId) ->
    case Item =:= ItemId of
	true ->
	    get_items(Item, Count, []);
	_ ->
	    get_selected_items(ItemList, ItemCount, ItemId)
    end.

%% �Ƿ��������Ʒ
-spec contains(list(), integer()) -> atom().
contains([], _ItemId) ->
    ok;
contains([Item|ItemList], ItemId) ->
    case Item =:= ItemId of
	true ->
	    true;
	_ ->
	    contains(ItemList, ItemId)
    end.

%%%===================================================================
%%% Internal functions
%%%===================================================================
get_items(_, 0, Items) ->
    Items;
get_items(Item, Count, Items) ->
    get_items(Item, Count - 1, [Item|Items]).
