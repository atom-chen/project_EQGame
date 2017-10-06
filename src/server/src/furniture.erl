%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% �Ҿߴ���ģ��
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

%% �ж�ָ���ļҾ��Ƿ����
%% ����ֵ, ����:true, ������:false
-spec is_exist(integer(), tuple()) -> atom().
is_exist(InstanceID, #house_data{furniture_vec=FurVec}) ->
    case lists:keyfind(InstanceID, 2, FurVec) of
	false ->
	    false;
	_ ->
	    true
    end.

%% ���ָ���ļҾ�
-spec get(integer(), tuple()) -> atom() | tuple().
get(InstanceID, #house_data{furniture_vec=FurVec}) ->
    case lists:keyfind(InstanceID, 2, FurVec) of
    	false ->
    	    false;
    	Furniture ->
    	    Furniture
    end.

%% ȡ��Ʒģ��id
get_itemid(#house_furniture{item_tempid=ItemTempID}) ->
    ItemTempID.
%% ȡ�Ҿ�ģ��id
get_tempid(#house_furniture{template_id=TemplateID}) ->
    TemplateID.
%% ȡ�Ҿ�ʵ��id
get_instid(#house_furniture{instance_id=InstanceID}) ->
    InstanceID.

%% �ڷżҾ�
add(ItemTempID, InstanceID, TemplateID, X, Z, Height, Floor, Face, DelTime, FurnitureVec)->
    add(new(ItemTempID, InstanceID, TemplateID, X, Z, Height, Floor, Face, DelTime), FurnitureVec).

add(FurnitureList, FurnitureVec) when is_list(FurnitureList) ->
    F = fun(X, Acc) ->
		add(X, Acc)
	end,
    lists:foldr(F, FurnitureVec, FurnitureList);
add(#house_furniture{x=X, z=Z, height=Height, floor=Floor}=Furniture, FurnitureVec) ->    
    %% ����Ҫȥ����ͬλ�õļҾ�(������Ҫ��������)
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

%% ����ʱЧ��������ÿ���
clear_del_time(HouseFur) ->
    HouseFur#house_furniture{del_time=datetime:empty()}.

%% �滻�Ҿ�
-spec replace(integer(), tuple(), tuple()) -> any().
replace(InstanceId, #house_data{furniture_vec=FurVec}=HouseData, Furniture) ->
    case is_exist(InstanceId, HouseData) of
	true ->
	    lists:keyreplace(InstanceId, 2, FurVec, Furniture);
	_ ->
	    ok
    end.


%% ɾ���Ҿ�
delete(FurnitureID, FurnitureVec) ->
    lists:keydelete(FurnitureID, 2, FurnitureVec).


%% ��ȡ�Ҿߵ�����,����(ʹ���Ա��������ȣ����｡��ֵ����������ֵ����Ծֵ���Ҿ�����)
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

%% ���¼Ҿ�����
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


%% ��ȡ��λ����

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
    case Account =:= UsedAccount of %% ����Լ��Ѿ�ռ��λ���ˣ�����ռ�õ�λ������
	true ->
	    Position;
	_ ->
	    case IsUsed =:= 0 of 
		true -> %% �����û�б�ʹ��
		    Position;
		_ ->
		    get_empty_position(Account, PositionList, MaxPlayer)
	    end
    end.

%% ���ÿ�λ
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

%% ����뿪����λ��
-spec leave_position(tuple(), integer()) -> tuple().
leave_position(Furniture, PositionIndex) when PositionIndex > 0 ->
    PositionList = Furniture#house_furniture.position_list,
    Position = lists:keyfind(PositionIndex, 2, PositionList),
    UseType = Furniture#house_furniture.use_type,
    Timer = datetime:make_time({{0, 0, 0}, {0, 0, 0}}),
    PreStatus = Position#furniture_position.status,
    Status = 
	case UseType of
	    ?NormalStatus -> %% �����Ϊ����״̬����Ҫ���״̬
		0;
	    ?KeepStatus -> %% ����������λ�õ�״̬
		Position#furniture_position.status
	end,
    NPosition = Position#furniture_position{is_used=0, used_account='', status=Status, use_time=Timer},
    NPositionList = lists:keyreplace(PositionIndex, 2, PositionList, NPosition),
    {NPositionList, PreStatus, NPosition#furniture_position.status};
leave_position(Furniture, PositionIndex) ->
    logger:log(error, "leave position error.\n", [Furniture, PositionIndex]).

%% λ�����Ƿ���ڸ����
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

%% Ȩ�޵���֤
-spec get_action_by_permission() -> function().
get_action_by_permission() ->
    fun({Furniture, Owner, Account, FurniturePermission}) ->
	    case Owner =:= Account of
		true -> %% ���ʹ�������Լ�������Ȩ�޵�����
		    true;
		_ ->
		    if
			FurniturePermission =:= ?fpt_yes -> %% ����������Ϊ�������ʹ��
			    true;
			FurniturePermission =:= ?fpt_no -> %% ����������Ϊ���������ʹ��
			    if
				Furniture#house_furniture.permission =:= 0 -> %% ֻ��ʹ��ͨ����ļҾ�
				    true;
				Furniture#house_furniture.permission =:= 1 ->
				    false
			    end
		    end
	    end
    end.

%% �Ƿ�����ʹ�üҾ�(�����Ա�)
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

%% ��֤�Ƿ���λ��
-spec get_action_by_position() -> function().
get_action_by_position() ->
    fun({Furniture, Account}) ->
	    data_helper:format("MaxUsePlayer:~p~n", [Furniture#house_furniture.max_use_player]),
	    case get_empty_position(Account, Furniture) of %% �Ƿ���λ�ÿ���ʹ��
		{false, UsedAccount} ->
		    {false, {Furniture, UsedAccount}};
		_ ->
		    true
	    end
    end.

%% �����Ƿ�����
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

%% �滻ǽֽ
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
%% ���ݼҾ�Id��ȡ���ʹ������
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


%% ���ݼҾ�Id��ȡ���ʹ������
get_furniture(FurnitureId) ->
    FurnitureList = tplt:get_all_data(furni_tplt),
    get_furniture(FurnitureId, FurnitureList).


%% ���������������λ��
-spec create_position_list(integer(), list()) -> list().
create_position_list(0, PositionList) ->
    PositionList;
create_position_list(Number, PositionList) ->
    NewPositionList = [#furniture_position{position_index=Number, is_used=0, used_account=''} | PositionList],
    create_position_list(Number - 1, NewPositionList).



%%%===================================================================
%%% ���Ժ���
%%%===================================================================
