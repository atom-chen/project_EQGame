%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%%
%%% @end
%%% Created : 28 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(trans_data).

-include("tplt_def.hrl").
-include("packet_def.hrl").
-include("common_def.hrl").

%%%===================================================================
%%% API
%%%===================================================================
-export([npc_list/2, npc_map_mapping_list/2]).

%% 将xml定义的结构转换成协议的结构
%% NewNpcList:协议中定义的结构, NpcList:xml定义的结构
-spec npc_list(list(), list()) -> list().
npc_list(NewNpcList, []) ->
    NewNpcList;
npc_list(NewNpcList, [#npc_tplt{npc_id=NpcId,
				      body=Body,
				      head=Head,
				      hair=Hair,
				      equip1=Equip1,
				      equip2=Equip2,
				      skeleton=Skeleton,
				      npc_name=NpcName}|NpcList]) ->
    NewNpcList1 = #npc_info{npc_id=NpcId,
			    body=Body,
			    head=Head,
			    hair=Hair,
			    equip1=Equip1,
			    equip2=Equip2,
			    skeleton=Skeleton,
			    npc_name=NpcName
			   },
     npc_list([NewNpcList1|NewNpcList], NpcList).

%% 将xml定义的结构转换成协议的结构
%% NewNpcMapMappingList:协议中定义的结构, NpcMapMappingList:xml定义的结构
-spec npc_map_mapping_list(list(), list()) -> list().
npc_map_mapping_list(NewNpcMapMappingList, []) ->
    NewNpcMapMappingList;
npc_map_mapping_list(NewNpcMapMappingList, [#npc_map_mapping_tplt{id=Id, 
								  npc_id=NpcId, 
								  npc_name=NpcName, 
								  born_id=BornId,
								  %% x=X, 
								  %% y=Y, 
								  %% z=Z, 
								  script_id=ScriptId, 
								  action=Action,
								  towards=Towards}|NpcMapMappingList]) ->
    %% Point = #point{x=X, y=Y, z=Z},
    NewNpcMapMappingList1 = #npc_map_mapping_info{id=Id, 
						  npc_id=NpcId, 
						  npc_name=NpcName,
						  born_id=BornId, 
						  script_id=ScriptId, 
						  action=Action,
						  npc_key=list_to_atom(integer_to_list(guid:make(?st_npc))),
						  towards = Towards},
    npc_map_mapping_list([NewNpcMapMappingList1 | NewNpcMapMappingList], NpcMapMappingList).


%% --------------------------测试代码-----------------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

npc_list_test() ->
    [NpcList|_NewNpcList] = npc_list([], [#npc_tplt{npc_id=1,
					    body=2,
					    head=3,
					    hair=4,
					    equip1=5,
					    equip2=6,
					    skeleton=7,
					    npc_name="test"}]),
    #npc_info{npc_id=NpcId,
	      body=Body,
	      head=Head,
	      hair=Hair,
	      equip1=Equip1,
	      equip2=Equip2,
	      skeleton=Skeleton,
	      npc_name=NpcName} = NpcList,
    1 = NpcId,
    2 = Body,
    3 = Head,
    4 = Hair,
    5 = Equip1,
    6 = Equip2,
    7 = Skeleton,
    "test" = NpcName.
    

npc_map_mapping_list_test() ->
    [NpcList | _NewNpcMapMappingList] = npc_map_mapping_list([], [#npc_map_mapping_tplt{id=1, 
											npc_id=2, 
											npc_name="test", 
											born_id=1,
											%% x=11, 
											%% y=12, 
											%% z=13, 
											script_id=14, 
											action=15}]),
    #npc_map_mapping_info{id=Id, 
			  npc_id=NpcId, 
			  npc_name=NpcName, 
			  %% pos=Pos,
			  born_id=1,
			  script_id=ScriptId, 
			  action=Action,
			  npc_key=_NpcKey} = NpcList,
    %% #point{x=X, y=Y, z=Z} = Pos,
    1 = Id,
    2 = NpcId,
    "test" = NpcName,
    %% 11 = X,
    %% 12 = Y,
    %% 13 = Z,
    14 = ScriptId,
    15 = Action.

-endif.
