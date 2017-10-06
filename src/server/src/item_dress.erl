%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  服装类的物品
%%% @end
%%% Created :  6 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(item_dress).

-include("tplt_def.hrl").
-include("enum_def.hrl").
-include("packet_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").

-behaviour(gen_item). 

%% API 
-export([can_use/4, do_use/5, putoff_dress/6]).

can_use(#item{template_id=TempID}, _Owner, _Param, PlayerData) -> 
    #item_dress_tplt{sex=Sex, equip_pos=_EPos} = tplt:get_dress_data(TempID),
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    Body = player_data:get_pack(?pt_body, PlayerData),
    case player_pack:putoff_old_dress(TempID, Bag, Body) of
	{false, Err} -> 
	    {false, Err};
	_ ->
	    PSex = player_data:get_sex(PlayerData),
	    case Sex of 
		0 -> true;
		PSex -> true;
		_ -> {false, ?err_sex_not_same}
	    end
    end.


%% 穿上衣服
do_use(Account, Index, #item{template_id=TempID}, _Param, PlayerData)->
    % 需要判断目标格子是否已经被锁定了
    #item_dress_tplt{equip_pos=EPos} = tplt:get_dress_data(TempID),
    do_use_impl(Account, Index, EPos, TempID, PlayerData).


do_use_impl(Account, BagIndex, EPos, NewDressTempID, PlayerData)->
    % 脱下旧装备
    PlayerData1 = putoff_old_equip(EPos, NewDressTempID, PlayerData),


    {BagGrids3, BodyGrids3} = player_pack:do_move_item(?pt_bag, BagIndex, 
						     ?pt_body, EPos, PlayerData1),
    % 通知身边的所有玩家改变外观
    player:send_to_around(Account,
			  #notify_around_change_dress{account=Account,
						     pos=EPos,
						     template_id=NewDressTempID}),
    pack:broadcast(Account, ?pt_body, BodyGrids3),
    {ok, [{?pt_bag, BagGrids3}, {?pt_body, BodyGrids3}]}.

%% 脱下衣服的处理过程
putoff_dress(?pt_body, BodyIndex, ?pt_bag, BagIndex, Owner, PlayerData)->
    case player_pack:get_body_item(BodyIndex, PlayerData) of
	empty ->
	    sys_msg:send(Owner, ?err_grid_empty),
	    false;
	_Item ->
	    do_use_impl(Owner, BagIndex, BodyIndex, 0, PlayerData)
    end.

%% 脱下旧装备
putoff_old_equip(_BodyIndex, NewDressTempID, PlayerData) ->
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    Body = player_data:get_pack(?pt_body, PlayerData),
    [{?pt_bag, NBag}, {?pt_body, NBody}]=player_pack:putoff_old_dress(NewDressTempID, Bag, Body),
    PD1 = player_data:set_pack(?pt_bag, NBag, PlayerData),
    player_data:set_pack(?pt_body, NBody, PD1).
    
    
	    
