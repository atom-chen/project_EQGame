%% ��������
%%
-module(scene).  
-behaviour(gen_scene).

-include("gen_scene.hrl").
-include("sys_msg.hrl").
-include("tplt_def.hrl").
-include("router.hrl").

-export([init/1, register/1, handle_call/3, handle_cast/2, terminate/2,
	 can_enter_scene/2, after_enter_scene/2, before_enter_scene/2, resee_scene/3,
	 before_leave_scene/2, can_stop_scene/1, after_leave_scene/2, get_born_pos/1]).  

-export([id_to_scene/1, id_to_scene/2]).

-record(common_scene_data, {scene_id,            % ����id
			    furniture_list=[],   % �Ҿ��б�   
			    copy_id              % �������
			   }).

%%%===================================================================
%%% API
%%%===================================================================
-spec id_to_scene(integer()) -> atom().
id_to_scene(ID) when is_integer(ID) ->
   id_to_scene(ID, 1).

-spec id_to_scene(integer(), integer()) -> atom().
id_to_scene(ID, CopyId) when is_integer(ID) ->
    list_to_atom("common_scene_" ++ integer_to_list(ID) ++ "_" ++ integer_to_list(CopyId)).
    
register([_SceneID, _CopyId, SceneName]) ->
    [
     router:make_event_target(can_use_furniture, SceneName, {self(), ?MODULE}),
     router:make_event_target(use_furniture, SceneName, {self(), ?MODULE}),
     router:make_event_target(stop_use_furniture, SceneName, {self(), ?MODULE})
    ].

init([SceneID, CopyId, SceneName]) when is_integer(SceneID), is_atom(SceneName) -> 
    scene_db:insert(SceneName),
    %% ���ؼҾ߱�
    FurnitureList = get_scene_furniture_list(SceneID),
    %%FurnitureList = [],

    {ok, #common_scene_data{scene_id=SceneID, copy_id=CopyId, furniture_list=FurnitureList}}.


%% �жϿɷ���볡��
can_enter_scene(Account, #scene_data{custom_data=CustomData}=State) 
  when is_atom(Account) ->
    SceneId=CustomData#common_scene_data.scene_id,
    SceneData = tplt:get_data(common_scene, SceneId),
    MaxPlayerCount = SceneData#common_scene.max_players,
    PlayerCount = gen_scene:get_player_count(State),
    %% ���������ʱ���ܽ���
    case MaxPlayerCount > PlayerCount of
	true ->
	    true;
	false ->
	    {false, ?err_player_count_over}
     end.

%% ���½��ܳ��������Ϣ
resee_scene(Account, Pos, #common_scene_data{scene_id=SceneID, copy_id=CopyId}) ->
    %% ֪ͨ�ͻ��˽��빫������
    net_helper:send2client(Account, #notify_enter_common_scene{scene_id=SceneID}),
    %% �ָ����λ��
    net_helper:send2client(Account, #notify_stop_walk{account=Account,pos=Pos}),

    %% ���빫��������npc��Ϣ���͸��ͻ���
    npc_manager:notify(SceneID, CopyId, Account).

    
before_enter_scene({Account, Pos}, #common_scene_data{scene_id=SceneID}) 
  when is_atom(Account), is_integer(SceneID) ->
    %% ֪ͨ�ͻ��˽��빫������
    %% net_helper:send2client(Account, #notify_enter_common_scene{scene_id=SceneID, enter_pos=Pos}).
    net_helper:send2client(Account, #notify_enter_common_scene{scene_id=SceneID, born_id=Pos}).

after_enter_scene({Account, _PlayerLevel}, #common_scene_data{scene_id=SceneID, copy_id=CopyId}=CustomData) 
  when is_atom(Account), is_integer(SceneID) ->
    update_player_count(SceneID, CopyId, 1),    
    %% ���빫��������npc��Ϣ���͸��ͻ���
    npc_manager:notify(SceneID, CopyId, Account),

    CustomData.

%% ͬ����������
update_player_count(SceneID, CopyId, AdjustCount)->
    [[PInfo, Count, CopyName]|_Tail] = ets:match(common_scene_copy, {{SceneID, CopyId}, '$1', '$2', '$3'}),
    Count1=Count + AdjustCount,
    ets:insert(common_scene_copy, {{SceneID, CopyId}, PInfo, Count1, CopyName}).

before_leave_scene(_Account, _State) ->
    true.

can_stop_scene(_PlayerCount) ->
    false.

after_leave_scene(_Account, #scene_data{custom_data=CustomData}=State) ->
    #common_scene_data{scene_id=SceneID, copy_id=CopyId}=CustomData,
    update_player_count(SceneID, CopyId, -1),    
    State.

%% ȡ������
get_born_pos(#common_scene_data{scene_id=SceneID}) ->
    SceneData = tplt:get_data(common_scene, SceneID),
    %% X = SceneData#common_scene.x,
    %% Y = SceneData#common_scene.y,
    %% Z = SceneData#common_scene.z,
    BornId = SceneData#common_scene.born_id,
    BornData = tplt:get_data(born, BornId),
    [X, Y, Z] = BornData#born.pos,
    Pos = #point{x=X, y=Y, z=Z},
    {Pos, BornId}.
    
%%%===================================================================
%%% Handle call
%%%===================================================================
%% �Ƿ�����ʹ�üҾ�
handle_call({#msg{event=can_use_furniture}, {Account, PlayerProperty, InstanceId, FunctionId, Sex}},
	    _From,
	    #scene_data{custom_data=CustomData}=State) ->
    %%io:format("call can use furniture ~p~n", [{Account, InstanceId, FunctionId}]),
    FurList = CustomData#common_scene_data.furniture_list,
    case lists:keyfind(InstanceId, #house_furniture.instance_id, FurList) of
    	false ->
	    {reply, furniture_not_find, State};
    	Furniture ->
	    FurnitureFunction = tplt:get_data(furni_interact_tplt, FunctionId),
	    PropertyId = FurnitureFunction#furni_interact_tplt.property_id,
	    
	    ActionCanDo = action:can_do([{furniture:get_action_by_sex(), {Furniture, Sex}, err_sex},
					 {furniture:get_action_by_position(), {Furniture, Account}, err_position},
					 {furniture:get_action_by_property(), {PlayerProperty, PropertyId}, err_property}
					]),
	    {reply, ActionCanDo, State}
    end;

%% ����ʹ�üҾ�
handle_call({#msg{event=use_furniture}, {Account, InstanceId, FunctionId}}, _From, 
	    #scene_data{custom_data=CustomData}=State) ->
    %%io:format("call use furniture ~p~n", [{Account, InstanceId, FunctionId}]),
    FurList = CustomData#common_scene_data.furniture_list,
    case lists:keyfind(InstanceId, #house_furniture.instance_id, FurList) of
    	false ->
	    {reply, furniture_not_find, State};
    	Furniture ->
	    Position = furniture:get_empty_position(Account, Furniture),
	    NPosition = furniture:set_empty_position(Account, Furniture, Position, FunctionId),
	    PositionList = furniture:get_position_list(Furniture),
	    Index = furniture:get_position_index(NPosition),
	    NPositionList = lists:keyreplace(Index, #furniture_position.position_index, 
					     PositionList, NPosition),
	    gen_scene:broadcast_all_players(State, #notify_use_furniture_result{account=Account,
										position_index=Index,
										status=furniture:get_status(NPosition),
										instance_id=InstanceId,
										function_id=FunctionId}),
	    NFurniture = Furniture#house_furniture{position_list=NPositionList},

	    NFurList = lists:keyreplace(InstanceId, #house_furniture.instance_id, FurList, NFurniture),

	    NewCustomData = CustomData#common_scene_data{furniture_list=NFurList},
	    {reply, {InstanceId, FunctionId, Index}, State#scene_data{custom_data=NewCustomData}}	
    end;

%% ֹͣʹ�üҾ�(������������������������ı��������)
handle_call({#msg{event=stop_use_furniture}, {Account, InstId, FuncId}}, _From,
	    #scene_data{custom_data=CustomData}=State) ->
    %%io:format("call stop use furniture ~p~n", [{Account, InstId, FuncId}]),
    FurList = CustomData#common_scene_data.furniture_list,
    case lists:keyfind(InstId, #house_furniture.instance_id, FurList) of
    	false ->
	    {reply, furniture_not_find, State};
    	Furniture ->
	    PositionList = Furniture#house_furniture.position_list,
	    PositionIndex = furniture:get_position_index(PositionList, Account),
	    {NPositionList, PreStatus, Status} = furniture:leave_position(Furniture, PositionIndex),
	    NFurniture = Furniture#house_furniture{position_list=NPositionList},
	    NFurList = lists:keyreplace(InstId, #house_furniture.instance_id, FurList, NFurniture),
	    gen_scene:broadcast_all_players(State, 
					    #notify_stop_using_furniture_result{account=Account, 
									position_index=PositionIndex,
										instance_id=InstId,
										pre_status=PreStatus,
										status=Status,
										function_id=FuncId}),
	    
	    NCustomData = CustomData#common_scene_data{furniture_list=NFurList},
	    {reply, ok, State#scene_data{custom_data=NCustomData}}
    end;

%% ֹͣʹ�üҾ�
handle_call({#msg{event=stop_use_furniture}, {Account, InstId, FuncId, Pos}}, _From,
	    #scene_data{custom_data=CustomData}=State) ->
    FurList = CustomData#common_scene_data.furniture_list,
    case lists:keyfind(InstId, #house_furniture.instance_id, FurList) of
    	false ->
	    {reply, furniture_not_find, State};
    	Furniture ->
	    PositionList = Furniture#house_furniture.position_list,
	    FurnitureFunction = tplt:get_data(furni_interact_tplt, FuncId),
	    PropertyId = FurnitureFunction#furni_interact_tplt.property_id,
	    PositionIndex = furniture:get_position_index(PositionList, Account),
	    NFurList = 
		case PositionIndex > 0 of
		    true ->
			{NPositionList, PreStatus, Status} = furniture:leave_position(Furniture, PositionIndex),
			NFurniture = Furniture#house_furniture{position_list=NPositionList},
			NewFurList = lists:keyreplace(InstId, #house_furniture.instance_id, FurList, NFurniture),

			gen_scene:broadcast_all_players(State, 
							#notify_stop_using_furniture_result{account=Account, 
											    position_index=PositionIndex,
											    instance_id=InstId, 
											    function_id=FuncId,
											    pre_status=PreStatus,
											    status=Status,
											    pos=Pos}),
			NewFurList;
		    false ->
			gen_scene:broadcast_all_players(State, 
							#notify_stop_using_furniture_result{account=Account, 
											    position_index=PositionIndex,
											    instance_id=InstId, 
											    function_id=FuncId,
											    pre_status=0,
											    status=0,
											    pos=Pos}),
			FurList
		end,

	    NewCustomData = CustomData#common_scene_data{furniture_list=NFurList},
	    {reply, PropertyId, State#scene_data{custom_data=NewCustomData}}
    end;

handle_call(get_data, _from, #scene_data{custom_data=CustomData}=State) ->
    SceneId = CustomData#common_scene_data.scene_id,
    CommonSceneTplt = tplt:get_data(common_scene, SceneId),
    PlayerCount = gen_scene:get_player_count(State),
    {reply, {PlayerCount, CommonSceneTplt#common_scene.name}, State};
handle_call(_Request, _From, State) ->  
    Reply = ok,  
    {reply, Reply, State}.

%%%===================================================================
%%% Handle cast
%%%===================================================================
handle_cast(stop, State) ->
    {stop, normal, State}.


%%%===================================================================
%%% terminate
%%%===================================================================
terminate(_Reason, _State) ->
    ok.

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% ���ɹ����Ҿ�
make_furniture(#common_scene_furniture{id=InstID, object_id=FurID})->
    {PositionList, MaxUsePlayer, UseSex, Permission, FunctionList, UseType, StatusQty} = 
	furniture:get_furniture_data(FurID),

    #house_furniture{instance_id=InstID, %%guid:make(?st_furniture),
		     template_id=FurID,
		     del_time=datetime:empty(),
		     position_list=PositionList,
		     max_use_player=MaxUsePlayer,
		     use_sex=UseSex,
		     permission=Permission,
		     function_list=FunctionList,
		     use_type=UseType,
		     status_qty=StatusQty
		    }.



get_scene_furniture_list(SceneID) ->
    SceneList = tplt:get_all_data(common_scene),
    case lists:keyfind(SceneID, #common_scene.id, SceneList) of
    	#common_scene{file_res=FileRes} ->
	    [IDStr, _] = string:tokens(binary_to_list(FileRes), "/"),
	    FileAtom = list_to_atom("common_scene" ++ IDStr),
	    FurList = tplt:get_all_data(FileAtom),
	    [make_furniture(X) || X <- FurList]
    end.




