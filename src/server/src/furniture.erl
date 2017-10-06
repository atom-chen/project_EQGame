%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 家具处理模块
%%% @end
%%% Created : 24 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(furniture).

-include("enum_def.hrl").
-include("tplt_def.hrl").
-include("packet_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").
%% API
-export([get_status/1, get_position_index/1]).
-export([get_position_list/1]).
-export([update/2, get_furniture_data/1]).
-export([set_empty_position/4, set_position/3, get_empty_position/2, leave_position/2, exist_player/2]).
-export([is_exist/2, get/2, add/2, add/10, new/9, replace/3, delete/2]).
-export([get_tempid/1, get_instid/1, get_itemid/1]).
-export([can_use_function/2, get_position_index/2]).
-export([replace_room_tex/2, clear_del_time/1]).
-export([get_action_by_permission/0, get_action_by_sex/0, get_action_by_position/0, get_action_by_property/0]).

-define(NormalStatus, 1).
-define(KeepStatus, 2).

%%%===================================================================
%%% API
%%%===================================================================
get_status(#furniture_position{status=Status}) ->
    Status.

get_position_index(#furniture_position{position_index=PositionIndex}) ->
    PositionIndex.

get_position_list(#house_furniture{position_list=PositionList}) ->
    PositionList.

%% 判断指定的家具是否存在
%% 返回值, 存在:true, 不存在:false
-spec is_exist(integer(), tuple()) -> atom().
is_exist(InstanceID, #house_data{furniture_vec=FurVec}) ->
    case lists:keyfind(InstanceID, 2, FurVec) of
	false ->
	    false;
	_ ->
	    true
    end.

%% 获得指定的家具
-spec get(integer(), tuple()) -> atom() | tuple().
get(InstanceID, #house_data{furniture_vec=FurVec}) ->
    case lists:keyfind(InstanceID, 2, FurVec) of
    	false ->
    	    false;
    	Furniture ->
    	    Furniture
    end.

%% 取物品模板id
get_itemid(#house_furniture{item_tempid=ItemTempID}) ->
    ItemTempID.
%% 取家具模板id
get_tempid(#house_furniture{template_id=TemplateID}) ->
    TemplateID.
%% 取家具实例id
get_instid(#house_furniture{instance_id=InstanceID}) ->
    InstanceID.

%% 摆放家具
add(ItemTempID, InstanceID, TemplateID, X, Z, Height, Floor, Face, DelTime, FurnitureVec)->
    add(new(ItemTempID, InstanceID, TemplateID, X, Z, Height, Floor, Face, DelTime), FurnitureVec).

add(FurnitureList, FurnitureVec) when is_list(FurnitureList) ->
    F = fun(X, Acc) ->
		add(X, Acc)
	end,
    lists:foldr(F, FurnitureVec, FurnitureList);
add(#house_furniture{x=X, z=Z, height=Height, floor=Floor}=Furniture, FurnitureVec) ->    
    %% 这里要去掉相同位置的家具(窗户需要这样处理)
    F = fun(#house_furniture{x=X1, z=Z1, height=Height1, floor=Floor1}) -> 
		(X =:= X1) and (Z =:= Z1) 
		    and (Height =:= Height1) and (Floor =:= Floor1)
	end,

    {_Old, List} = lists:partition(F, FurnitureVec),
    [Furniture | List].


new(ItemTempID, InstanceID, TemplateID, X, Z, Height, Floor, Face, DelTime)->
    {PositionList, MaxUsePlayer, UseSex, Permission, FunctionList, UseType, StatusQty}
	= get_furniture_data(TemplateID),
    #house_furniture{item_tempid=ItemTempID, instance_id=InstanceID, 
		     template_id=TemplateID, x=X, z=Z,
		     height=Height, floor=Floor, face=Face,
		     position_list=PositionList,
		     max_use_player=MaxUsePlayer,
		     use_sex=UseSex,
		     permission=Permission,
		     function_list=FunctionList,
		     del_time=DelTime,
		     use_type=UseType,
		     status_qty=StatusQty
		    }.

%% 消除时效，变成永久可用
clear_del_time(HouseFur) ->
    HouseFur#house_furniture{del_time=datetime:empty()}.

%% 替换家具
-spec replace(integer(), tuple(), tuple()) -> any().
replace(InstanceId, #house_data{furniture_vec=FurVec}=HouseData, Furniture) ->
    case is_exist(InstanceId, HouseData) of
	true ->
	    lists:keyreplace(InstanceId, 2, FurVec, Furniture);
	_ ->
	    ok
    end.


%% 删除家具
delete(FurnitureID, FurnitureVec) ->
    lists:keydelete(FurnitureID, 2, FurnitureVec).


%% 获取家具的数据,包括(使用性别，人物清洁度，人物健康值，人物魅力值，活跃值，家具清洁度)
-spec get_furniture_data(integer()) -> tuple().
get_furniture_data(FurnitureId) ->
    case get_furniture(FurnitureId) of
	empty ->
	    {[], 0, 0, 0, [], 0, 0};
	Furniture ->
	    PositionList = create_position_list(Furniture#furni_tplt.max_use_player, []),
	    {PositionList, 
	     Furniture#furni_tplt.max_use_player,
	     Furniture#furni_tplt.use_sex,
	     Furniture#furni_tplt.permission,
	     Furniture#furni_tplt.function_list,
	     Furniture#furni_tplt.use_type,
	     Furniture#furni_tplt.status_qty}
    end.

%% 更新家具数据
update([], NewFurnitureVec) ->
    lists:reverse(NewFurnitureVec);
update([#house_furniture{template_id=FurnitureId}=Furniture|FurnitureVec], NewFurnitureVec) ->
    {_PositionList, MaxUsePlayer, UseSex, Permission, FunctionList, UseType, StatusQty} = 
	get_furniture_data(FurnitureId),
    PositionList = Furniture#house_furniture.position_list,
    NewFurniture = Furniture#house_furniture{
		     position_list = dynamic_set_position(PositionList, MaxUsePlayer),
    		     max_use_player=MaxUsePlayer,
    		     use_sex=UseSex,
    		     permission=Permission,
		     function_list=FunctionList,
		     use_type=UseType,
		     status_qty=StatusQty
    		    },
    update(FurnitureVec, [NewFurniture|NewFurnitureVec]).

dynamic_set_position(PositionList, MaxUsePlayer) ->
    PositionCount = length(PositionList),
    NPositionList = 
	case PositionCount > MaxUsePlayer of
	    true ->
		lists:sublist(PositionList, 1, MaxUsePlayer);
	    false ->
		PositionList1 = create_position_list(MaxUsePlayer - PositionCount, []),
		PositionList ++ PositionList1
	end,
    clear_position(NPositionList, []).

clear_position([], NPositionList) ->
    NPositionList;
clear_position([Position|PositionList], NPositionList) ->
    NPosition = Position#furniture_position{is_used=0, used_account=''},
    clear_position(PositionList, [NPosition|NPositionList]).


%% 获取空位索引

get_empty_position(Account, Furniture) ->
    get_empty_position(Account, Furniture, Furniture#house_furniture.max_use_player).

-spec get_empty_position(atom(), list() | tuple(), integer()) -> tuple().
get_empty_position(Account, #house_furniture{position_list=PositionList}, 1) ->
    [#furniture_position{used_account=UsedAccount, is_used=IsUsed}=Position] = PositionList,
    data_helper:format("Account:~p, UsedAccount:~p~n", [Account, UsedAccount]),
    case Account =:= UsedAccount of
	true ->
	    Position;
	_ ->
	    case IsUsed =:= 0 of
		true ->
		    Position;
		_ ->
		    {false, UsedAccount}
	    end
    end;
get_empty_position(Account, #house_furniture{position_list=PositionList}, MaxPlayer) ->
    get_empty_position(Account, PositionList, MaxPlayer);
get_empty_position(_Account, [], _MaxPlayer) ->
    {false, ''};
get_empty_position(Account, [Position|PositionList], MaxPlayer) ->
    #furniture_position{used_account=UsedAccount, is_used=IsUsed}=Position,
    case Account =:= UsedAccount of %% 如果自己已经占用位置了，返回占用的位置索引
	true ->
	    Position;
	_ ->
	    case IsUsed =:= 0 of 
		true -> %% 如果还没有被使用
		    Position;
		_ ->
		    get_empty_position(Account, PositionList, MaxPlayer)
	    end
    end.

%% 设置空位
-spec set_empty_position(atom(), tuple(), tuple(), integer()) -> tuple().
set_empty_position(Account, Furniture, Position, FuncId) ->
    FurniInteractTplt = tplt:get_data(furni_interact_tplt, FuncId),
    Status = Position#furniture_position.status, 
    #house_furniture{status_qty=StatusQty} = Furniture,
    NStatus = set_status_qty(Status, StatusQty, FurniInteractTplt),
    set_position(Account, Position, NStatus, FuncId).

set_position(Account, Position, Status, FuncId) ->
    Position#furniture_position{is_used=1, used_account=Account, status=Status, func_id=FuncId}.

set_position(Position, Status, FuncId) ->
    Position#furniture_position{is_used=0, used_account='', status=Status, func_id=FuncId}.

set_status_qty(Status, StatusQty, FurniInteractTplt) ->
    case FurniInteractTplt#furni_interact_tplt.status >= 0 of
	true ->
	    FurniInteractTplt#furni_interact_tplt.status;
	false ->
	    NStatus = Status + 1,
	    case NStatus =< StatusQty of
		true ->
		    NStatus;
		false ->
		    0
	    end
    end.

get_position_index([], _Account) ->
    0;
get_position_index([#furniture_position{position_index = PositionIndex, used_account = UsedAccount}| PositionList], Account) ->
    case Account =:= UsedAccount of
	true ->
	    PositionIndex;
	_ ->
	    get_position_index(PositionList, Account)
    end.

%% 玩家离开所在位置
-spec leave_position(tuple(), integer()) -> tuple().
leave_position(Furniture, PositionIndex) when PositionIndex > 0 ->
    PositionList = Furniture#house_furniture.position_list,
    Position = lists:keyfind(PositionIndex, 2, PositionList),
    UseType = Furniture#house_furniture.use_type,
    Timer = datetime:make_time({{0, 0, 0}, {0, 0, 0}}),
    PreStatus = Position#furniture_position.status,
    Status = 
	case UseType of
	    ?NormalStatus -> %% 此情况为正常状态，需要清空状态
		0;
	    ?KeepStatus -> %% 此情况不清除位置的状态
		Position#furniture_position.status
	end,
    NPosition = Position#furniture_position{is_used=0, used_account='', status=Status, use_time=Timer},
    NPositionList = lists:keyreplace(PositionIndex, 2, PositionList, NPosition),
    {NPositionList, PreStatus, NPosition#furniture_position.status};
leave_position(Furniture, PositionIndex) ->
    logger:log(error, "leave position error.\n", [Furniture, PositionIndex]).

%% 位置上是否存在该玩家
-spec exist_player(list(), atom()) -> atom().
exist_player([], _Account) ->
    false;
exist_player([#furniture_position{used_account = UsedAccount} | PositionList], Account) ->
    case UsedAccount =:= Account of
	true ->
	    true;
	_ ->
	    exist_player(PositionList, Account)
    end.

can_use_function(_FunctionId, []) ->
    false;
can_use_function(FunctionId, [FuncId|FunctionList]) ->
    case FunctionId =:= FuncId of
	true ->
	    true;
	_ ->
	    can_use_function(FunctionId, FunctionList)
    end.

%% 权限的验证
-spec get_action_by_permission() -> function().
get_action_by_permission() ->
    fun({Furniture, Owner, Account, FurniturePermission}) ->
	    case Owner =:= Account of
		true -> %% 如果使用者是自己，则不做权限的限制
		    true;
		_ ->
		    if
			FurniturePermission =:= ?fpt_yes -> %% 如果玩家设置为允许好友使用
			    true;
			FurniturePermission =:= ?fpt_no -> %% 如果玩家设置为不允许好友使用
			    if
				Furniture#house_furniture.permission =:= 0 -> %% 只能使用通用类的家具
				    true;
				Furniture#house_furniture.permission =:= 1 ->
				    false
			    end
		    end
	    end
    end.

%% 是否允许使用家具(根据性别)
-spec get_action_by_sex() -> function().
get_action_by_sex() ->
    fun({#house_furniture{use_sex=UseSex}, Sex}) ->
	    case UseSex =:= 0 of
		true ->
		    true;
		_ ->
		    case UseSex =:= Sex of
			true ->
			    true;
			_ ->
			    false
		    end
	    end
    end.

%% 验证是否有位置
-spec get_action_by_position() -> function().
get_action_by_position() ->
    fun({Furniture, Account}) ->
	    data_helper:format("MaxUsePlayer:~p~n", [Furniture#house_furniture.max_use_player]),
	    case get_empty_position(Account, Furniture) of %% 是否还有位置可以使用
		{false, UsedAccount} ->
		    {false, {Furniture, UsedAccount}};
		_ ->
		    true
	    end
    end.

%% 属性是否已满
-spec get_action_by_property() -> function().
get_action_by_property() ->
    fun({PlayerProperty, PropertyId}) ->
	    IsPropertyFull = player_property_by_action:validate_property_is_full(PlayerProperty, PropertyId),
	    case IsPropertyFull of
		true ->
		    false;
		_ ->
		    true
	    end
    end.

%% 替换墙纸
replace_room_tex(#room_tex{room_id=RoomID, type=TexType}=RoomTexInfo, RoomTexVec) 
  when is_list(RoomTexVec) ->
    F = fun(#room_tex{room_id=ID, type=Type}) -> 
		(ID =:= RoomID) and (Type =:= TexType)
	end,
    {_Old, List} = lists:partition(F, RoomTexVec),
    [RoomTexInfo | List].


%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 根据家具Id获取最大使用人数
-spec get_furniture(integer(), list()) -> empty | tuple().
get_furniture(_FurnitureId, []) ->
    empty;
get_furniture(FurnitureId, [#furni_tplt{furniture_id=TpltFurnitureId}=Furniture|FurnitureList]) ->
    case FurnitureId =:= TpltFurnitureId of
	true ->
	    Furniture;
	_ ->
	    get_furniture(FurnitureId, FurnitureList)
    end.


%% 根据家具Id获取最大使用人数
get_furniture(FurnitureId) ->
    FurnitureList = tplt:get_all_data(furni_tplt),
    get_furniture(FurnitureId, FurnitureList).


%% 根据最大数量创建位置
-spec create_position_list(integer(), list()) -> list().
create_position_list(0, PositionList) ->
    PositionList;
create_position_list(Number, PositionList) ->
    NewPositionList = [#furniture_position{position_index=Number, is_used=0, used_account=''} | PositionList],
    create_position_list(Number - 1, NewPositionList).



%%%===================================================================
%%% 测试函数
%%%===================================================================
