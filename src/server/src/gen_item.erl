%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  ͨ�õ���Ʒģ��(ģ�巽��)
%%% @end
%%% Created : 31 Mar 2010 by  <>

-module(gen_item).
-include("packet_def.hrl").
-include("tplt_def.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").

-export([start/1, use_item/4]).

-export([behaviour_info/1]).

-define(use_del, 1).      % ʹ����Ʒ��ɾ��
-define(use_not_del, 0).  % ʹ����Ʒ��ɾ��

behaviour_info(callbacks) -> 
    [{can_use, 4},
     {do_use, 5}]; 
behaviour_info(_Other) ->
    undefined.

start(L) ->
    [undefined=put(ItemType, Mod) || {ItemType, Mod} <- L].

%% ʹ����Ʒ�Ķ���ӿ�
use_item(Index, Owner, Grids, PlayerData) when Index >= 0->
    Item = pack:get_item(Index, Grids),
    TempID = item:get_tempid(Item),
    #item_tplt{type=Type} = tplt:get_data(item_tplt, TempID), 
    case can_use_item(Item, Type, Owner, PlayerData) of 
	true ->
	    case do_use_item(Owner, Item, Index, Type, PlayerData) of
		false -> false;
		Ret -> {ok, Ret}
	    end;
	false ->
	    sys_msg:send(Owner, ?err_cannot_use_item),
	    false;
	{false, Reason} -> % ���ݲ���ʹ�õ�ԭ��, ��ʾ�ͻ���
	    sys_msg:send(Owner, Reason),
	    false;
	error -> error
    end;
use_item(Index, Owner, _Grids, _PlayerData) ->
    erlang:error({badarg, Index, Owner}).

%% �жϸ���Ʒ�Ƿ���ʹ��
can_use_item(Item, ItemType, Owner, PlayerData) ->
    can_use_item_1(Item, ItemType, Owner, [], PlayerData).

can_use_item_1(#item{template_id=0}, _ItemType, _Owner, _Param, _PlayerData) -> 
    erlang:error(bagarg);
can_use_item_1(Item, ItemType, Owner, Param, PlayerData)->
    case get(ItemType) of
	undefined ->
	    erlang:error({'not found item type mode', Item, ItemType});
	Mod ->
	    Mod:can_use(Item, Owner, Param, PlayerData)
    end.
	    

%% ʹ����Ʒ
do_use_item(Account, Item, ItemType, Index, PlayerData) when is_record(Item, item), Index >=0 ->
    do_use_item_1(Account, Item, ItemType, Index, [], PlayerData);
do_use_item(Account, Item, Type, Index, PlayerData)->
    erlang:error({bagarg, Account, Item, Type, Index, PlayerData}).

do_use_item_1(_Account, #item{template_id=0}, _ItemType, _Index, _Param, _PlayerData) ->
    erlang:error(bagarg);
do_use_item_1(Account, #item{template_id=TempID, instance_id=InstID}=Item, Index, ItemType, Param, PlayerData) ->
    Mod = get(ItemType),
    case Mod:do_use(Account, Index, Item, Param, PlayerData) of
	{ok, [{?pt_bag, BagGrids} | Rest]} ->
	    % ɾ��ʹ�ú�ģ���ָ��ɾ������Ʒ
	    #item_tplt{use_del=UseDel} = tplt:get_data(item_tplt, TempID),
	    [{?pt_bag, case UseDel of
			   ?use_del -> 
			       NewGrids = pack:del_item(Index, BagGrids),			   
			       %% ��Ǯ��Ʒ��¼
			       ItemInfo = {TempID, 
					   -pack:get_item_count(Index, BagGrids),
					  InstID},
			       item_money_log:log_item(Account, use_and_del_item, ItemInfo),
			       NewGrids;
			   ?use_not_del -> 
			       BagGrids
		       end} | Rest];
	false -> false;
	error -> error
    end.



