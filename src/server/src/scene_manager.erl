%%% -------------------------------------------------------------------
%%% Author  : NoteBook
%%% Description :
%%%     负责创建和删除场景
%%% Created : 2009-10-27
%%% -------------------------------------------------------------------
-module(scene_manager).
-define(SERVER, ?MODULE).

-behaviour(gen_msgproc).

-export([start_link/0, get_all_scene/0, register/1, get_scene_name/1]).
%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2, terminate/2, code_change/3]).

-include("tplt_def.hrl").
-include("table_info.hrl").
-include("router.hrl").
-include("packet_def.hrl").

-record(state, {}).

register([]) ->
    [
     router:make_event_target(create_house, ?SERVER, {self(), ?MODULE})
    ].

%% ====================================================================
%% API
%% ====================================================================
%% 启动场景管理器
start_link() ->  
    data_helper:format("start scene manager~n"),
    gen_msgproc:start_link({local, ?SERVER}, ?MODULE, [], []).

%% 获取场景名, 如果场景不存在则创建
-spec get_scene_name(atom()) -> atom().
get_scene_name(Account)
  when is_atom(Account) ->
    SceneName = house:get_house_name(Account),
    case util:is_process_alive(SceneName) of
	false ->
	    router:send(scene_manager, create_house, {Account, Account});
	_ ->
	    SceneName
    end.

%% 获取所有公共场景
get_all_scene() ->
    gen_server:call(?SERVER, get_all_scene).
%% ====================================================================
%% Function: init/1
%% Description: Initiates the server
%% Returns: {ok, State}          |
%%          {ok, State, Timeout} |
%%          ignore               |
%%          {stop, Reason}
%% ====================================================================
init([]) ->    
    ets:new(common_scene_copy, [set, public, named_table]),
    AList = extract_common_scene_list(tplt:get_all_data(common_scene)),
    NSceneCopys = [{SID, CID, CName}||
		      {_, _Id, SID, CID, CName} <- tplt:get_all_data(scene_copy)],
    NNSceneCopys = lists:foldl(fun(SceneId, Return) ->
				       NSceneList = lists:filter(fun(Item) -> element(1, Item) == SceneId end, NSceneCopys),
				       case NSceneList of 
					   []->
					       lists:merge(Return,[{SceneId, 1, "SceneId_" ++ integer_to_list(SceneId) ++ "_" ++ integer_to_list(1)}]);
					   _ ->
					       lists:merge(Return, NSceneList)
				       end
			       end, [], AList),
    create_common_scene_list(NNSceneCopys),
    {ok, #state{}}.

%% ====================================================================
%% Function: handle_call/3
%% Description: Handling call messages
%% Returns: {reply, Reply, State}          |
%%          {reply, Reply, State, Timeout} |
%%          {noreply, State}               |
%%          {noreply, State, Timeout}      |
%%          {stop, Reason, Reply, State}   | (terminate/2 is called)
%%          {stop, Reason, State}            (terminate/2 is called)
%% ====================================================================
%% 创建玩家房屋场景(家园)
handle_call({#msg{event=create_house}, {Creator, HouseMaster}}, _From, State) ->
    {NCreator, NHouseOwner} = calc_house_owner(Creator, HouseMaster),
    
    HouseName = house:get_house_name(NHouseOwner),
    case util:is_process_alive(HouseName) of
	false ->
	    %% SupName = list_to_atom("scene_sup_" ++ atom_to_list(HouseName)),
	    PlayerHouseMod = house,
	    M = gen_scene,
	    F = start,
	    A = [HouseName, PlayerHouseMod, [NHouseOwner, HouseName, NCreator]],
	    ChildSpec = {HouseName, {M, F, A},
			 temporary, 2000, worker, [gen_scene]},
	    supervisor:start_child(scene_sup, ChildSpec);
	_ ->
	    ok
    end,
    {reply, HouseName, State};
handle_call(get_all_scene, _From, State) ->
    Scenes = scene_db:select_all(),
    {reply, Scenes, State};
handle_call(_Request, _From, State) ->
    Reply = ok,
    {reply, Reply, State}.

%% ====================================================================
%% Function: handle_cast/2
%% Description: Handling cast messages
%% Returns: {noreply, State}          |
%%          {noreply, State, Timeout} |
%%          {stop, Reason, State}            (terminate/2 is called)
%% ====================================================================

handle_cast(_Msg, State) ->
    {noreply, State}.

%% ====================================================================
%% Function: handle_info/2
%% Description: Handling all non call/cast messages
%% Returns: {noreply, State}          |
%%          {noreply, State, Timeout} |
%%          {stop, Reason, State}            (terminate/2 is called)
%% ====================================================================
handle_info(_Info, State) ->
    {noreply, State}.

%% ====================================================================
%% Function: terminate/2
%% Description: Shutdown the server
%% Returns: any (ignored by gen_server)
%% ====================================================================
terminate(_Reason, _State) ->
    ok.

%% ====================================================================
%% Func: code_change/3
%% Purpose: Convert process state when code is changed
%% Returns: {ok, NewState}
%% ====================================================================
code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

%% ====================================================================
%%% Internal functions
%% ====================================================================

-spec create_common_scene_list(list()) -> any().
create_common_scene_list([]) ->
    ok;
create_common_scene_list([{SceneId, CopyId, CopyName}|SceneCopys]) when is_list(SceneCopys) ->
    create_common_scene(SceneId, CopyId, CopyName),
    create_common_scene_list(SceneCopys).

%% 提取公共场景列表, 每个列表元素为integer
-spec extract_common_scene_list(list()) -> any().
extract_common_scene_list(DataList) when is_list(DataList) ->
    [Data#common_scene.id || Data <- DataList].
    
%% 创建公共场景
-spec create_common_scene(integer(), integer(), list()) -> any().
create_common_scene(SceneID, CopyId, CopyName) when is_integer(SceneID) ->
    case ets:match(common_scene_copy, {{SceneID, CopyId}, '$1', '_'})of
    	[]->
	    common_scene_sup:start(SceneID, CopyId),
	    common_scene_sup:start_children(SceneID, CopyId),
	    ets:insert(common_scene_copy, {{SceneID, CopyId}, 0, 0, CopyName}),
	    npc_sup:start(SceneID, CopyId),
	    {ok};
	[_Data]->
	    {already_start}
    end. 

%% 计算创建者及房主
%% 注：如果是GM好友，则创建者不是自已
calc_house_owner(HouseMaster, HouseMaster) ->
    {HouseMaster, HouseMaster};
calc_house_owner(Creator, HouseMaster) ->     
    %% GM好友房间，做成每人都有独一间GM好友房间
    case gm_friend:get_data(HouseMaster) of
	#gm_friend_tplt{}=Tplt -> %% 是GM好友
    	    HouseOwner = gm_friend:get_house_owner(Creator, HouseMaster),
	    gm_friend:update_house(HouseOwner, Tplt, Creator),
	    {Creator, HouseOwner};
    	_ -> %% 不是GM好友
    	    {HouseMaster, HouseMaster}
    end.
