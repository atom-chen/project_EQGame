%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010
%%% @doc
%%% 房屋场景处理模块
%%% @end
%%% Created : 17 Mar 2010 by  linyijie
%%%-------------------------------------------------------------------
-module(house).
-behaviour(gen_scene).

-include("gen_scene.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").
-include("tplt_def.hrl").
-include("router.hrl").


%% behaviour
-export([init/1, register/1, handle_call/3, handle_cast/2, terminate/2]).

-export([can_enter_scene/2, after_enter_scene/2, before_enter_scene/2, resee_scene/3,
	 before_leave_scene/2, can_stop_scene/1, after_leave_scene/2, get_born_pos/1]).

-export([get_house_data/1, 
	 get_house_data/2, 
	 set_house_data/2, 
	 get_house_name/1,
	 is_pos_empty/4,
	 is_editing/1,
	 get_creator/1,
	 get_garbage_timer/1,
	 set_garbage_timer/2,
	 db_change_house/3,
	 whether_only_allow_friends_visit/1]).

-export([get_gift_box_vec/1, notify_get_items/1]).

-record(house_scene_data, {
	  creator=undefined, %% 房间创建者(GM房间创建者 与 房间主人是不一样的) 
	  max_players = 100,       %% 最大玩家数
	  house_editing = false,   %% 房屋是否正在编辑
	  house_replacing = false, %% 房屋是否正在替换
	  garbage_timer,           %% 垃圾定时器
	  house_data               %% 场景数据, 现在存储的是家园里的数据, 对应的数据结构是#house_data{}, 目前公共场景对应的值为undefined	  
}).

get_creator(#house_scene_data{creator=Creator}) ->
    Creator.

get_garbage_timer(#house_scene_data{garbage_timer=Timer}) ->
    Timer.

set_garbage_timer(NewTimer, HouseSceneData) ->
    HouseSceneData#house_scene_data{garbage_timer=NewTimer}.

register([_Owner, SceneName, _Creator])->
    house_magic_box:start(SceneName) ++  
	house_clean:start(SceneName) ++
	[router:make_event_target(start_edit_house, SceneName, {self(), ?MODULE}),
	 router:make_event_target(end_edit_house, SceneName, {self(), ?MODULE}),
	 router:make_event_target(can_edit_house, SceneName, {self(), ?MODULE}),
	 router:make_event_target(start_replace_house, SceneName, {self(), ?MODULE}),
	 router:make_event_target(end_replace_house, SceneName, {self(), ?MODULE}),
	 router:make_event_target(replace_house, SceneName, {self(), ?MODULE}),
	 router:make_event_target(modify_house, SceneName, {self(), ?MODULE}),
	 router:make_event_target(recovery_furniture, SceneName, {self(), ?MODULE}),
	 router:make_event_target(get_furniture_item_info, SceneName, {self(), ?MODULE}),
	 router:make_event_target(replace_room_tex, SceneName, {self(), ?MODULE}),
	 router:make_event_target(use_furniture, SceneName, {self(), ?MODULE}),
	 router:make_event_target(control_mutli_furniture, SceneName, {self(), ?MODULE}),
	 router:make_event_target(can_use_furniture, SceneName, {self(), ?MODULE}),
	 router:make_event_target(get_house_data, SceneName, {self(), ?MODULE}),
	 router:make_event_target(update_house_data, SceneName, {self(), ?MODULE}),
	 router:make_event_target(stop_use_furniture, SceneName, {self(), ?MODULE}),
	 router:make_event_target(get_house_level, SceneName, {self(), ?MODULE}),
	 router:make_event_target(add_furniture_list, SceneName, {self(), ?MODULE}),
	 router:make_event_target(whether_only_allow_friends_visit, SceneName, {self(), ?MODULE}),
	 router:make_event_target(get_house_owner, SceneName, {self(), ?MODULE}),
	 router:make_event_target(put_gift_box, SceneName, {self(), ?MODULE}),
	 router:make_event_target(get_gift_box, SceneName, {self(), ?MODULE}),
	 router:make_event_source(notify_update_furniture, SceneName, {self(), ?MODULE}),
	 router:make_event_target(dec_gift_box, SceneName, {self(), ?MODULE}),
	 router:make_event_target(get_custom_data, SceneName, {self(), ?MODULE}),
	 router:make_event_target(change_house, SceneName, {self(), ?MODULE})
	].

%%%===================================================================
%%% API
%%%===================================================================
%% 取出生点
get_born_pos(#house_scene_data{house_data=HouseData}) ->
    get_born_pos(HouseData);
get_born_pos(#house_data{template_id=TpltID}) ->
    HouseTplt = tplt:get_data(house_tplt, TpltID),
    %% X = HouseTplt#house_tplt.born_x,
    %% Y = HouseTplt#house_tplt.born_y,
    %% Z = HouseTplt#house_tplt.born_z,
    BornId=HouseTplt#house_tplt.born_id,
    io:format("BornId:~p",[BornId]),
    BornData = tplt:get_data(born, BornId),
    [X, Y, Z] = BornData#born.pos,
    Pos = #point{x=X, y=Y, z=Z},
    {Pos, BornId}.

get_house_name(Account) ->
    list_to_atom(atom_to_list(Account) ++ "_house_scene").

%% 返回房屋的数据
-spec get_house_data(atom(), integer()) -> tuple().
get_house_data(Account, HouseId) ->
    HTData = tplt:get_data2(house_tplt, HouseId),
    get_house_data(Account, HouseId, HTData).

get_house_data(#house_scene_data{house_data=HouseData}) ->
    HouseData.
set_house_data(CustomData, HouseData) ->
    CustomData#house_scene_data{house_data=HouseData}.

get_house_owner(#house_data{owner=Owner}) ->
    Owner;
get_house_owner(CustomData) ->
    HouseData = get_house_data(CustomData),
    get_house_owner(HouseData).

get_gift_box_vec(#house_data{gift_box_vec=GiftBoxVec}) ->
    GiftBoxVec;
get_gift_box_vec(CustomData) ->
    HouseData = get_house_data(CustomData),
    get_gift_box_vec(HouseData).

set_gift_box_vec(HouseData, GiftBoxVec) when is_record(HouseData, house_data) and is_list(GiftBoxVec) ->
    HouseData#house_data{gift_box_vec = GiftBoxVec};
set_gift_box_vec(HouseData, GiftBox) when is_record(HouseData, house_data) ->
    GiftBoxVec = get_gift_box_vec(HouseData),
    set_gift_box_vec(HouseData, [GiftBox | GiftBoxVec]);
set_gift_box_vec(CustomData, GiftBox) when is_record(CustomData, house_scene_data) ->
    HouseData = get_house_data(CustomData),
    NHouseData = set_gift_box_vec(HouseData, GiftBox),
    CustomData#house_scene_data{house_data=NHouseData}.

dec_gift_box_vec(HouseData, GiftBox) when is_record(HouseData, house_data) ->
    GiftBoxVec = get_gift_box_vec(HouseData),
    NGiftBoxVec = lists:delete(GiftBox, GiftBoxVec),
    set_gift_box_vec(HouseData, NGiftBoxVec);
dec_gift_box_vec(CustomData, GiftBox) when is_record(CustomData, house_scene_data) ->
    HouseData = get_house_data(CustomData),
    NHouseData = dec_gift_box_vec(HouseData, GiftBox),
    CustomData#house_scene_data{house_data=NHouseData}.

%% 是否值允许好友访问
-spec whether_only_allow_friends_visit(atom()) -> atom().
whether_only_allow_friends_visit(Account) ->
    SceneName = get_house_name(Account),
    case util:is_process_alive(SceneName) of
	false ->
	    HouseData = db_common:select_one(house_data, Account),
	    HousePermission = HouseData#house_data.house_permission,
	    HousePermission =:= ?hpt_friend;
	_ ->
	    router:send(SceneName, whether_only_allow_friends_visit)
    end.


%% 判断位置是否为空
is_pos_empty(#grid_pos{x=X, y=Y, z=Z}, HouseData) -> 
    is_pos_empty(X, Y, Z, HouseData).    
is_pos_empty(X, Y, Z, HouseData) when is_integer(X), 
				      is_integer(Y),
				      is_integer(Z) ->
    #house_data{furniture_vec=FurList, 
		gift_box_vec=GiftList,
		magic_box_vec=MagicBoxList}=HouseData,

    B1 = lists:any(fun(#house_furniture{x=X1, z=Z1, height=Y1}) ->  
			  (X =:= X1) and (Y =:= Y1) and (Z =:= Z1) 
		  end, FurList),
    	
    B2 = lists:any(fun(#gift_box{pos=#grid_pos{x=X1, z=Z1, y=Y1}}) ->  
			  (X =:= X1) and (Y =:= Y1) and (Z =:= Z1) 
		  end, GiftList),

    B3 = lists:any(fun(#magic_box{pos=#grid_pos{x=X1, z=Z1, y=Y1}}) ->  
			  (X =:= X1) and (Y =:= Y1) and (Z =:= Z1) 
		  end, MagicBoxList),
    (not B1) and (not B2) and (not B3).

 
%% 提示获得物品
notify_get_items(Owner) when is_atom(Owner) ->
    HouseData = db_house:select(Owner),
    [begin
	 ItemTempID = furniture:get_itemid(Fur),
	 ItemCount = 1,
	 sys_msg:send(Owner, ?msg_get_item, [item:get_item_name(ItemTempID), ItemCount])
     end || Fur <- HouseData#house_data.furniture_vec].

is_editing(#house_scene_data{house_editing=Editing}) ->
    Editing.


%%--------------------------------------------------------------------
%% @private
%% @doc
%% Initializes the server
%%
%% @spec init(Args) -> {ok, State} |
%%                     {ok, State, Timeout} |
%%                     ignore |
%%                     {stop, Reason}
%% @end
%%--------------------------------------------------------------------
init([Owner, SceneName, Creator]) ->
    % 装载房屋场景数据
    HouseData = db_house:select(Owner),
    GiftBoxVec = get_gift_box_vec(HouseData),
    NGiftBoxVec = player_gift:get_name(GiftBoxVec, []),
    NHouseData = set_gift_box_vec(HouseData, NGiftBoxVec),
    NewFurnitureVec = furniture:update(NHouseData#house_data.furniture_vec, []),
    %% 初始化最高人数
    HouseTplt = tplt:get_data(house_tplt, NHouseData#house_data.template_id),
    MaxPlayers = HouseTplt#house_tplt.max_players,
    NNHouseData = NHouseData#house_data{furniture_vec=NewFurnitureVec},

    %% 触发自动更新
    %%ItemUpdateInterval = ?ITEM_UPDATE_INTERVAL,
    ItemUpdateInterval = common_def:get_val(scan_del_item_interval),
    sys_time:subscribe(?dt_interval, SceneName, ItemUpdateInterval, notify_update_furniture),

    %% 算离线产生的垃圾
    NNNHouseData = house_magic_box:add_offline_garbage(NNHouseData, Creator), 

    Players = [],
    NotifyDelete = false, %% 不通知删除
    NewHouseData = update_overtime_furnitures(NNNHouseData, Players, NotifyDelete),
    {ok, #house_scene_data{house_data=NewHouseData, max_players=MaxPlayers, creator=Creator}}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling call messages
%%
%% @spec handle_call(Request, From, State) ->
%%                                   {reply, Reply, State} |
%%                                   {reply, Reply, State, Timeout} |
%%                                   {noreply, State} |
%%                                   {noreply, State, Timeout} |
%%                                   {stop, Reason, Reply, State} |
%%                                   {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
%% 是否允许好友访问
handle_call(#msg{event=whether_only_allow_friends_visit}, _From,
	    #scene_data{custom_data=CustomData} = State) ->
    HouseData = CustomData#house_scene_data.house_data,
    HousePermission = HouseData#house_data.house_permission,
    {reply, HousePermission =:= ?hpt_friend, State};
handle_call(#msg{event=get_house_owner}, _From, #scene_data{custom_data=CustomData} = State) ->
    HouseData = CustomData#house_scene_data.house_data,    
    Owner = HouseData#house_data.owner,

    {reply, Owner, State};
handle_call(#msg{event=get_custom_data}, _From, #scene_data{custom_data=CustomData} = State) ->
    {reply, CustomData, State};
%% 放置礼物
handle_call({#msg{event=put_gift_box}, GiftBox}, _From, #scene_data{custom_data=CustomData} = State) ->
    Owner = get_house_owner(CustomData),
    HouseData = get_house_data(CustomData),
    case is_pos_empty(GiftBox#gift_box.pos, HouseData) of
	true ->
	    NCustomData = set_gift_box_vec(CustomData, GiftBox),
	    NHouseData = get_house_data(NCustomData),
	    NState = State#scene_data{custom_data=NCustomData},
	    GiftBoxVec = NHouseData#house_data.gift_box_vec,
	    %%db_house:write(NHouseData),
	    router:cast(Owner, start_house_gift, GiftBoxVec),
	    gen_scene:broadcast_all_players(NState, #notify_house_gift_box{gift_box_vec=GiftBoxVec}),
	    {reply, NHouseData, NState};
	_ ->
	    {reply, {false, ?err_pos_not_empty}, State}	    
    end;    
handle_call({#msg{event=get_gift_box}, InstId}, _From, #scene_data{custom_data=CustomData} = State) ->
    HouseData = CustomData#house_scene_data.house_data,
    GiftBoxVec = HouseData#house_data.gift_box_vec,
    GiftBox = lists:keyfind(InstId, 2, GiftBoxVec),
    {reply, GiftBox, State};
handle_call({#msg{event=dec_gift_box}, InstId}, _From, #scene_data{custom_data=CustomData} = State) ->
    Owner = get_house_owner(CustomData),
    GiftBoxVec = get_gift_box_vec(CustomData),
    GiftBox = lists:keyfind(InstId, 2, GiftBoxVec),
    NCustomData = dec_gift_box_vec(CustomData, GiftBox),
    NGiftBoxVec = get_gift_box_vec(NCustomData),
    HouseData = get_house_data(NCustomData),
    NState = State#scene_data{custom_data=NCustomData},
    db_house:write(HouseData),
    router:cast(Owner, end_house_gift, HouseData#house_data.gift_box_vec),
    gen_scene:broadcast_all_players(NState, #notify_house_gift_box{gift_box_vec=NGiftBoxVec}),
    {reply, HouseData, NState};
%% 更新房屋数据
handle_call({#msg{event=update_house_data}, NHouseData}, _From,
	    #scene_data{custom_data=CustomData} = State) ->
    NCustomData = CustomData#house_scene_data{house_data=NHouseData},
    NState = State#scene_data{custom_data=NCustomData},
    {reply, ok, NState};
%% 判断能否编辑房屋
handle_call({#msg{event=can_edit_house}, Player}, _From, 
	    #scene_data{custom_data=CustomData} = State) ->
    HouseData = CustomData#house_scene_data.house_data,

    HouseMaster = HouseData#house_data.owner,
    CanEdit = 
	case HouseMaster =:= Player of
	    false -> {false, ?err_only_at_home_can_edit_house};
	    true -> true
	end,

    {reply, CanEdit, State};
%% 主人开始编辑房屋,客人们离开场景
handle_call({#msg{event=start_edit_house}, Master}, _From, 
	    #scene_data{custom_data=CustomData} = State) ->
    Players = gen_scene:get_players(State),
    kick_guests(Master, Players, ?err_player_editing_house),
    
    %% 清除宝箱
    Creator = CustomData#house_scene_data.creator,
    HouseData = CustomData#house_scene_data.house_data,
    NewHouseData = house_magic_box:clear(HouseData, Creator),
    %%NewHouseData = HouseData,

    NewCustomData = CustomData#house_scene_data{house_editing=true, 
						house_data=NewHouseData},
    {Pos, _BornId} = get_born_pos(HouseData),
    {reply, Pos, State#scene_data{custom_data=NewCustomData}};

%% 主人更换房屋，客人们离开场景
handle_call({#msg{event=start_replace_house}, {Account, Type, HTData}}, _From,
	    #scene_data{custom_data=CustomData} = State) ->
    OHouseData = get_house_data(CustomData),
    HouseId = HTData#house_transaction_tplt.house_id,
    HouseData = get_house_data(Account, HouseId),
    TpltLevel = HTData#house_transaction_tplt.level,
    NHouseData = HouseData#house_data{buy_date=datetime:localtime(), level=TpltLevel},
    NNHouseData = set_house_info(Type, OHouseData, NHouseData),
    NCustomData = CustomData#house_scene_data{house_data=NNHouseData},
    OldInstID = OHouseData#house_data.instance_id,
    PlayerCount = gen_scene:get_player_count(State),
    case PlayerCount > 0 of
	true ->
	    {reply, {OldInstID, NNHouseData}, 
	     State#scene_data{custom_data=NCustomData}};
	false ->
	    {stop, normal, {OldInstID, NNHouseData}, 
	     State#scene_data{custom_data=NCustomData}}
    end;
%% 主人结束编辑房屋
handle_call({#msg{event=end_edit_house}, _Master}, _From, 
	    #scene_data{custom_data=CustomData}=State) ->
    NewCustomData = CustomData#house_scene_data{house_editing=false},
    {reply, ok, State#scene_data{custom_data=NewCustomData}};

handle_call({#msg{event=change_house}, {Master, InstId, Type}}, _From,
	    #scene_data{custom_data=CustomData} = State) ->
    Owner = get_house_owner(CustomData),
    OldHouseData = get_house_data(CustomData),
    %% kick_guests(Master, Players, ?msg_change_house),
    HousePlot = house_plot:get(Master),
    PlotId = house_plot:get_house_plot_id(HousePlot, InstId),
    HouseData = house_plot:get_house(HousePlot, PlotId),
    NHouseData = set_house_info(Type, OldHouseData, HouseData),
    NCustomData = set_house_data(CustomData, NHouseData),
    NHousePlot = house_plot:replace(HousePlot, PlotId, OldHouseData),
    db_common:write([NHouseData, NHousePlot]),
    GiftBoxVec = house_data:get_gift_box_vec(NHouseData),
    router:cast(Owner, restart_house_gift, GiftBoxVec),

    AccountList = gen_scene:get_players(State),
    case length(AccountList) > 0 of
	true ->
	    {reply, {AccountList, NHouseData}, State#scene_data{custom_data=NCustomData}};
	false ->
	    {stop, normal, {AccountList, NHouseData}, State#scene_data{custom_data=NCustomData}}
    end;

%% 取房屋等级
handle_call(#msg{event=get_house_level}, _From, 
	    #scene_data{custom_data=CustomData}=State) ->
    #house_scene_data{house_data=HouseData} = CustomData,
    #house_data{level=Level} = HouseData,
    {reply, Level, State};

%% 结束改变房屋
handle_call({#msg{event=end_replace_house}, _Master}, _From,
	    #scene_data{custom_data=CustomData}=State) ->
    NewCustomData = CustomData#house_scene_data{house_replacing=false},
    {reply, ok, State#scene_data{custom_data=NewCustomData}};

%% 替换房屋材质
handle_call({#msg{event=replace_room_tex}, {Account, RoomID, TexType, Tex}}, _From, 
	    #scene_data{custom_data=CustomData}=State)->
    RoomTexInfo = #room_tex{room_id=RoomID, type=TexType, tex=Tex},
    NewCustomData= replace_room_tex(CustomData,Account,RoomTexInfo),
    {reply, ok, State#scene_data{custom_data=NewCustomData}};

%% 替换墙纸
handle_call({#msg{event=replace_room_tex}, RoomTexInfoList}, _From,
	    #scene_data{custom_data=CustomData} = State) when is_list(RoomTexInfoList)->
    HouseData = CustomData#house_scene_data.house_data,
    Account=house_data:get_owner(HouseData),
    NewCustomData= replace_room_tex(CustomData,Account,RoomTexInfoList),
    {reply, ok, State#scene_data{custom_data=NewCustomData}};

handle_call({#msg{event=replace_room_tex},#room_tex{}=RoomTexInfo}, _From,
	    #scene_data{custom_data=CustomData} = State)->
    HouseData = CustomData#house_scene_data.house_data,
    Account=house_data:get_owner(HouseData),
    NewCustomData= replace_room_tex(CustomData,Account,RoomTexInfo),
    {reply, ok, State#scene_data{custom_data=NewCustomData}};    

%% 是否允许使用家具
handle_call({#msg{event=can_use_furniture}, {Account, PlayerProperty, InstanceId, FunctionId, Sex}},
	    _From,
	    #scene_data{custom_data=CustomData}=State) ->
    HouseData = CustomData#house_scene_data.house_data,
    Owner = HouseData#house_data.owner,
    Furniture = furniture:get(InstanceId, HouseData),
    FurnitureFunction = tplt:get_data(furni_interact_tplt, FunctionId),
    Permission = HouseData#house_data.furniture_permission,
    PropertyId = FurnitureFunction#furni_interact_tplt.property_id,

    ActionCanDo = action:can_do([{furniture:get_action_by_permission(), {Furniture, Owner, Account, Permission}, err_permission},
				 {furniture:get_action_by_sex(), {Furniture, Sex}, err_sex},
				 {furniture:get_action_by_position(), {Furniture, Account}, err_position},
				 {furniture:get_action_by_property(), {PlayerProperty, PropertyId}, err_property}
				]),
    {reply, ActionCanDo, State};

handle_call({#msg{event=control_mutli_furniture}, {Account, Status, FurniVec}}, _From, 
	    #scene_data{custom_data=CustomData}=State) ->
    HouseData = CustomData#house_scene_data.house_data,
    {NHouseData, NFurniVec} = do_control_mutli_furniture(FurniVec, Account, Status, HouseData, []),
    gen_scene:broadcast_all_players(State, #notify_use_mutli_furni{furni_vec=NFurniVec}),
    db_house:write(NHouseData),
    NewCustomData = CustomData#house_scene_data{house_data=NHouseData},
    Ret = (length(NFurniVec) > 0),
    {reply, Ret, State#scene_data{custom_data=NewCustomData}};

%% 请求使用家具
handle_call({#msg{event=use_furniture}, {Account, InstanceId, FunctionId}}, _From, 
	    #scene_data{custom_data=CustomData}=State) ->
    HouseData = CustomData#house_scene_data.house_data,
    Furniture = furniture:get(InstanceId, HouseData),
    Position = furniture:get_empty_position(Account, Furniture),
    NPosition = furniture:set_empty_position(Account, Furniture, Position, FunctionId),
    PositionList = furniture:get_position_list(Furniture),
    Index = furniture:get_position_index(NPosition),
    NPositionList = lists:keyreplace(Index, 2, PositionList, NPosition),
    gen_scene:broadcast_all_players(State, #notify_use_furniture_result{account=Account,
									position_index=Index,
									status=furniture:get_status(NPosition),
									instance_id=InstanceId,
									function_id=FunctionId}),
    NewCustomData = update_custom_data(CustomData, NPositionList, Furniture, InstanceId, HouseData),
    NHouseData = get_house_data(NewCustomData),
    db_house:write(NHouseData),
    {reply, {InstanceId, FunctionId, Index}, State#scene_data{custom_data=NewCustomData}};

%% 回收家具
handle_call({#msg{event=recovery_furniture}, {Account, FurInstID}}, _From, 
	    #scene_data{custom_data=CustomData}=State) ->
    HouseData = CustomData#house_scene_data.house_data,
    {Item, NHouseData} = handle_recovery_furniture(Account, FurInstID, HouseData),
    db_house:write(NHouseData), 
    NewCustomData = CustomData#house_scene_data{house_data=NHouseData},
    {reply, Item, State#scene_data{custom_data=NewCustomData}};

%% 获得家具对应的物品数据
handle_call({#msg{event=get_furniture_item_info}, FurInstID}, _From, 
	    #scene_data{custom_data=CustomData}=State) ->
    HouseData = CustomData#house_scene_data.house_data,
    ItemTempID = 
	case furniture:get(FurInstID, HouseData) of
	    #house_furniture{item_tempid=ID} ->
		ID;
	    _ ->
		not_found
	end,
    {reply, ItemTempID, State};
%% 替换房屋
handle_call({#msg{event=replace_house}, {Account, HTData}}, _From, 
	    #scene_data{custom_data=CustomData}=State) ->
    HouseId = HTData#house_transaction_tplt.house_id,
    NHouseData = get_house_data(Account, HouseId),
    NCustomData = CustomData#house_scene_data{house_data=NHouseData},
    {reply, NHouseData, State#scene_data{custom_data=NCustomData}};

%% 停止使用家具(处理断网等情况，此种情况不改变玩家属性)
handle_call({#msg{event=stop_use_furniture}, {Account, InstId, FuncId}}, _From,
	    #scene_data{custom_data=CustomData}=State) ->
    HouseData = CustomData#house_scene_data.house_data,
    Furniture = furniture:get(InstId, HouseData),
    PositionList = Furniture#house_furniture.position_list,
    PositionIndex = furniture:get_position_index(PositionList, Account),
    {NPositionList, PreStatus, Status} = furniture:leave_position(Furniture, PositionIndex),
    NHouseData = stop_use_furniture(Furniture, NPositionList, InstId, HouseData),
    NCustomData = CustomData#house_scene_data{house_data=NHouseData},
    gen_scene:broadcast_all_players(State, 
				    #notify_stop_using_furniture_result{account=Account, 
									position_index=PositionIndex,
									instance_id=InstId,
									pre_status=PreStatus,
									status=Status,
									function_id=FuncId}),
    db_house:write(NHouseData),
    {reply, ok, State#scene_data{custom_data=NCustomData}};

%% 停止使用家具
handle_call({#msg{event=stop_use_furniture}, {Account, InstId, FuncId, Pos}}, _From,
	    #scene_data{custom_data=CustomData}=State) ->
    HouseData = CustomData#house_scene_data.house_data,
    Furniture = furniture:get(InstId, HouseData), 
    PositionList = Furniture#house_furniture.position_list,
    FurnitureFunction = tplt:get_data(furni_interact_tplt, FuncId),
    PositionIndex = furniture:get_position_index(PositionList, Account),
    NHouseData = 
	case PositionIndex > 0 of
	    true ->
		{NPositionList, PreStatus, Status} = furniture:leave_position(Furniture, PositionIndex),
		HouseData1 = stop_use_furniture(Furniture, NPositionList, InstId, HouseData),
		gen_scene:broadcast_all_players(State, 
						#notify_stop_using_furniture_result{account=Account, 
										    position_index=PositionIndex,
										    instance_id=InstId, 
										    function_id=FuncId,
										    pre_status=PreStatus,
										    status=Status,
										    pos=Pos}),

		HouseData1;
	    false ->
		gen_scene:broadcast_all_players(State, 
						#notify_stop_using_furniture_result{account=Account, 
										    position_index=PositionIndex,
										    instance_id=InstId, 
										    function_id=FuncId,
										    pre_status=0,
										    status=0,
										    pos=Pos}),
		HouseData
	end,
    PropertyId = FurnitureFunction#furni_interact_tplt.property_id,
    db_house:write(NHouseData),
    {reply, PropertyId, State#scene_data{custom_data=CustomData#house_scene_data{house_data=NHouseData}}};

%% 处理修改房屋的请求
handle_call({#msg{event=modify_house}, {Account, Data}}, _From, 
	    #scene_data{custom_data=CustomData}=SceneData)->
    HouseData = CustomData#house_scene_data.house_data,
    NewHouseData = 
	case do_modify_house(Account, HouseData, Data) of
	    false ->
		HouseData;
	    HouseData1 ->
		HouseData1
	end,
    NewCustomData = CustomData#house_scene_data{house_data=NewHouseData},
    {reply, ok, SceneData#scene_data{custom_data=NewCustomData}};

%% 批量增加家具
handle_call({#msg{event=add_furniture_list}, {Account, FurnitureList}}, _From, 
	    #scene_data{custom_data=CustomData}=SceneData)->
    HouseData = CustomData#house_scene_data.house_data,
    NewHouseData = 
	case do_modify_house(Account, HouseData, {add_furniture_list, FurnitureList}) of
	    false ->
		HouseData;
	    HouseData1 ->
		HouseData1
	end,
    NewCustomData = CustomData#house_scene_data{house_data=NewHouseData},
    {reply, ok, SceneData#scene_data{custom_data=NewCustomData}};

handle_call(_Request, _From, State) ->  
    Reply = ok,  
    {reply, Reply, State}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling cast messages
%%
%% @spec handle_cast(Msg, State) -> {noreply, State} |
%%                                  {noreply, State, Timeout} |
%%                                  {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
%% 更新物品, 自动删除过期物品
handle_cast({#msg{event=notify_update_furniture}, _TimerRef},
	    #scene_data{custom_data=CustomData} = State) ->    
    HouseData = CustomData#house_scene_data.house_data,
    NewHouseData = update_overtime_furnitures(HouseData, 
					      gen_scene:get_players(State)),

    NewCustomData = CustomData#house_scene_data{house_data=NewHouseData},
    {noreply, State#scene_data{custom_data=NewCustomData}};


handle_cast(stop, State) ->
    {stop, normal, State}.


%%%===================================================================
%%% terminate
%%%===================================================================

terminate(_Reason, #scene_data{custom_data=CustomData} = _State) ->
    %% 保存房间关闭时间
    HouseData = CustomData#house_scene_data.house_data,
    HouseID = HouseData#house_data.instance_id,
    house_magic_box:save_close_time(HouseID),
    ok.

%%%===================================================================
%%% Internal functions
%%%===================================================================
do_control_mutli_furniture([], _Account, _Status, HouseData, NFurniVec) ->
    {HouseData, NFurniVec};
do_control_mutli_furniture([#control_furni_info{instance_id=InstId, function_id=FuncId}|FurniVec], 
			   Account, Status, HouseData, NFurniVec) ->
    Furniture = furniture:get(InstId, HouseData),
    case furniture:get_empty_position(Account, Furniture) of
	{false, _} ->
	    do_control_mutli_furniture(FurniVec, Account, Status, HouseData, NFurniVec);
	Position ->
	    NPosition = 
		case ((Status > 0) and (Position#furniture_position.status > 0)) of
		    true ->
			Position;
		    false ->
			furniture:set_position(Position, Status, FuncId)
		end,
	    PositionList = furniture:get_position_list(Furniture),
	    Index = furniture:get_position_index(NPosition),
	    NPositionList = lists:keyreplace(Index, 2, PositionList, NPosition),
	    MutliFurniInfo = #mutli_furni_info{account=Account, position_index=Index, status=Status,instance_id=InstId,function_id=FuncId},
	    NHouseData = update_house_data(NPositionList, Furniture, InstId, HouseData),
	    do_control_mutli_furniture(FurniVec, Account, Status, NHouseData, [MutliFurniInfo|NFurniVec])
    end.

%% 玩家停止使用家具
stop_use_furniture(Furniture, PositionList, InstId, HouseData) ->
    update_house_data(PositionList, Furniture, InstId, HouseData).

do_modify_house(Player, HouseData, Data)->
    case can_modify(Player, HouseData) of
	true ->
	    HouseData1 =
		case Data of
		    {place_furniture, Account, Furniture} ->
			handle_placed_furniture(Account, Furniture, HouseData);
		    {add_furniture_list, FurnitureList} ->
			handle_placed_furniture_list(Player, FurnitureList, HouseData);
		    {replace_component_mesh, ComponentMesh} ->
			handle_replace_component_mesh(HouseData, ComponentMesh);
		    {replace_component_tex, ComponentTex} ->
			handle_replace_component_tex(HouseData, ComponentTex);
		    {move_furniture, InstanceID, X, Z, Height, Floor, Face} ->
			handle_move_furniture(InstanceID, X, Z, Height, Floor, Face, HouseData)
		end,
	    db_house:write(HouseData1),
	    HouseData1;
	false ->
	    false
    end.

%% 生成房屋家具
make_house_furniture([])->
    [];
make_house_furniture([#house_furniture_tplt{furniture_id=TempID,
					    item_temp_id=ItemTempID,
					    x=X,
					    height=Height,
					    z=Z,
					    flr=Floor,
					   face=Face}|Rest])->
    {PositionList, MaxUsePlayer, UseSex, Permission, FunctionList, UseType, StatusQty} = 
	furniture:get_furniture_data(TempID),
    [#house_furniture{instance_id=guid:make(?st_furniture),
		      template_id = TempID,
		      x=X,
		      height=Height,
		      z=Z,
		      floor=Floor,
		      face=Face,
		      item_tempid=ItemTempID,
		      del_time=item:get_del_time(ItemTempID),
		      position_list=PositionList,
		      max_use_player=MaxUsePlayer,
		      use_sex=UseSex,
		      permission=Permission,
		      function_list=FunctionList,
		      use_type=UseType,
		      status_qty=StatusQty
		     } | make_house_furniture(Rest)].


%% 踢除客人
-spec kick_guests(atom(), list(), integer()) -> any().
kick_guests(Master, AccountList, Reason) ->
    F = fun(Guest) -> 
		%% 处理客人被踢
		router:cast(Guest, be_kick, Reason)			
	end,
    
    [F(A) || A <- AccountList, A =/= Master],
    ok.


%% 获取房屋数据
-spec get_house_data(atom(), integer(), tuple()) -> tuple().
get_house_data(Account, HouseId, HTData) when is_atom(Account), is_integer(HouseId) ->
    File = HTData#house_tplt.house_furniture,
    FileAtom = tplt:transform_filename_atom(File),
    FurniVec = init_furni_vec(FileAtom),
    init_house_data(Account, HouseId, HTData, FurniVec).

%% 初始化房屋家具数据
init_furni_vec([]) ->
    [];
init_furni_vec(FileAtom) ->
    make_house_furniture(tplt:get_all_data(FileAtom)).

%% 初始化房屋
init_house_data(Account, HouseId, HTData, FurniVec) ->
    WelcomeWords = tplt:get_common_def("init_house_welcome_word"),
    Level = HTData#house_tplt.level,
    #house_data{owner=Account, 
		level = Level,
		instance_id = guid:make(?st_house),
		template_id = HouseId,
		furniture_permission = ?fpt_yes,
		house_permission = ?hpt_none,
		furniture_vec = FurniVec,
		house_clean=common_def:get_val(init_house_clean),
		welcome_words = WelcomeWords,
		buy_date=datetime:localtime()
	       }.


%% 处理放置家具的请求
handle_placed_furniture(Account, Furniture,
			#house_data{furniture_vec=FurVec}=HouseData)->
    #house_furniture{instance_id=InstanceID, 
		     template_id=FurnitureTempID,
		     x=X, z=Z, height=Height,
		     floor=Floor,face=Face,
		     del_time=DelTime
		    } = Furniture,
    net_helper:send2client(Account, #notify_add_furniture{instance_id=InstanceID,
							  template_id=FurnitureTempID,
							  x=X, z=Z, height=Height,
							  floor=Floor,face=Face,del_time=DelTime}),
    HouseData#house_data{
      furniture_vec = furniture:add(Furniture, FurVec)}.
%% 批量放置家具列表
handle_placed_furniture_list(Account, FurnitureList, HouseData)->
    net_helper:send2client(Account, #notify_buy_furniture_list{furnitures=FurnitureList}),
    HouseData#house_data{furniture_vec = furniture:add(FurnitureList, 
						       HouseData#house_data.furniture_vec)}.
    
%% 处理回收家具的请求  
handle_recovery_furniture(Account, InstanceID, #house_data{furniture_vec=FurVec}=HouseData)->
    #house_furniture{item_tempid=ItemTempID, del_time=DelTime} = 
	furniture:get(InstanceID, HouseData),
    %% router:send(Account, add_item, ItemTempID),
    net_helper:send2client(Account, #notify_delete_furniture{instance_id=InstanceID}),
    {item:set_decoration_val(item:make_item(ItemTempID, DelTime), 0), 
     HouseData#house_data{furniture_vec = furniture:delete(InstanceID, FurVec)}}.

%% 处理替换组件的mesh的请求
handle_replace_component_mesh(#house_data{component_mesh_vec=ComponentMeshVec}=HouseData,
			      #house_component_mesh{component_name=ComponentName, 
						    new_mesh=NewMesh})->
    HouseData#house_data{component_mesh_vec=
			     replace_component_mesh(ComponentName, NewMesh, ComponentMeshVec)}.

%% 处理替换组件的纹理的请求
handle_replace_component_tex(#house_data{component_tex_vec=ComponentTexVec}=HouseData, 
			     #house_component_tex{component_name=ComponentName, 
						  new_tex=NewTex})->
    HouseData#house_data{component_tex_vec=
			     replace_component_texture(ComponentName, NewTex, ComponentTexVec)}.

%% 处理移动家具的请求
handle_move_furniture(InstanceID, X, Z, Height, Floor, Face, 
		      #house_data{furniture_vec=FurVec}=HouseData)->
    case furniture:is_exist(InstanceID, HouseData) of
	true ->
	    Furniture = lists:keyfind(InstanceID, 2, FurVec),
	    NewFurniture = Furniture#house_furniture{instance_id=InstanceID,x=X,z=Z,
						     height=Height, floor=Floor, face=Face},
	    HouseData#house_data{furniture_vec = 
				     lists:keyreplace(InstanceID, 2, FurVec, NewFurniture)};
	false ->
	    erlang:error({badarg, InstanceID, HouseData})
    end.

% 能否编辑该房屋的数据
% 返回: true 或者 false
can_modify(Player, #house_data{owner=Owner}) when is_atom(Player) ->
    %% TODO: 需要引入授权系统, 目前暂时先做成只有房主才能修改
    if 
	Player =:= Owner ->
	    true;
	true ->
	    false
    end.

%% 替换mesh文件
replace_component_mesh(ComponentName, NewMesh, ComponentMeshVec)->
    case is_component_mesh_exist(ComponentName, ComponentMeshVec) of
	false -> %不存在, 这插入数据
	    [#house_component_mesh{component_name=ComponentName,
				   new_mesh=NewMesh}
	     |ComponentMeshVec];
	true -> %存在, 这替换原来的数据
	    lists:keyreplace(ComponentName, 2, ComponentMeshVec,
			     #house_component_mesh{component_name=ComponentName,new_mesh=NewMesh})
    end.


%% 替换纹理,材质
replace_component_texture(ComponentName, NewTexture, ComponentTexVec)->
    case is_component_texture_exist(ComponentName, ComponentTexVec) of
	false -> %不存在, 这插入数据
	    [#house_component_tex{component_name=ComponentName, new_tex=NewTexture}
	     | ComponentTexVec];
	true -> %存在, 这替换原来的数据
	    lists:keyreplace(ComponentName, 2, ComponentTexVec,
                             #house_component_tex{component_name=ComponentName, new_tex=NewTexture})
    end.

% 判断组件的mesh是否在列表中
is_component_mesh_exist(ComponentName, ComponentMeshVec)->
    case lists:keysearch(ComponentName, 2, ComponentMeshVec) of
	false ->
	    false;
	_ ->
	    true
    end.

% 判断组件纹理是否在列表中
is_component_texture_exist(ComponentName, ComponentTexVec)->
    case lists:keysearch(ComponentName, 2, ComponentTexVec) of
	false ->
	    false;
	_ ->
	    true
    end.

resee_scene(Account, Pos, CustomData) when is_atom(Account) ->
    HouseData = CustomData#house_scene_data.house_data,
    #house_data{owner=Master, template_id=TpltID} = HouseData,
    %% 通知进入玩家房屋
    net_helper:send2client(Account, #notify_enter_player_house{account=atom_to_list(Master), house_tplt_id=TpltID}),
    %% 恢复玩家位置
    net_helper:send2client(Account, #notify_stop_walk{account=Account,pos=Pos}),

    %% 发送家园数据
    net_helper:send2client(Account, #notify_house_data{data=HouseData}).


before_enter_scene({Account, Pos}, CustomData) when is_atom(Account) ->
    HouseData = CustomData#house_scene_data.house_data,
    #house_data{owner=Master, template_id=TpltID} = HouseData,
    %% 通知进入玩家房屋
    net_helper:send2client(Account, 
			   #notify_enter_player_house{account=atom_to_list(Master), 
						      house_tplt_id=TpltID,
						      born_id=Pos}).

after_enter_scene({Account, PlayerLevel}, CustomData) when is_atom(Account) ->
    HouseData = CustomData#house_scene_data.house_data,
    #house_data{owner=Master, instance_id=HouseID} = HouseData,

    NHouseData = 
    case Master == Account of
	true ->
	    HouseData;
	false ->
	    DecHouseClean = common_def:get_val(dec_house_clean_by_visit),
	    house_clean:dec(Master, HouseData, DecHouseClean)
    end,

    %% 发送家园数据
    net_helper:send2client(Account, 
    			   #notify_house_data{data=NHouseData}),

    %% 写房屋日志
    NNHouseData = player_house_log:write(?hvt_visit, Account, Master, datetime:localtime(), NHouseData),
    NNNHouseData = house_magic_box:on_generate_box(Account, PlayerLevel, NNHouseData),    

    OldTimer = CustomData#house_scene_data.garbage_timer,
    {NewTimer, NRandGarbageInfo} = house_magic_box:reset_garbage_timer(HouseID, OldTimer),
    
    db_common:write([NNNHouseData, NRandGarbageInfo]),    
    
    Creator = CustomData#house_scene_data.creator,    
    IsGMHouse = (Creator =/= Master),    
    case IsGMHouse of
	true -> %% GM好友房间，灯要全开全关
	    TurnOn = 1 - gm_friend:get_day_night_now(), %% 白天关，黑夜开
	    net_helper:send2client(Account, #notify_open_all_light{turn_on=TurnOn});
	_ ->
	    %% 通知当天鲜花数    鸡蛋数    
	    player_flower_egg:notify_today_flower_egg_count(Account, Master)
    end,

    CustomData#house_scene_data{house_data=NNNHouseData, garbage_timer=NewTimer}. 

before_leave_scene(_Account, _State) ->
    true.

can_stop_scene(0=_PlayerCount) ->
    true;
can_stop_scene(_PlayerCount) ->
    false.

after_leave_scene(Account, #scene_data{custom_data=CustomData}=State) ->
    HouseData = CustomData#house_scene_data.house_data,
    Master = HouseData#house_data.owner,
    NHouseData = player_house_log:write(?hvt_leave, Account, Master, datetime:localtime(), HouseData),
    db_common:write(NHouseData),
    NCustomData = CustomData#house_scene_data{house_data=NHouseData},
    State#scene_data{custom_data=NCustomData}.


%% 判断可否进入房屋
%% 返回值: true | {false, Reason}
can_enter_scene({Account, Priority},  #scene_data{custom_data=CustomData}=State)->
    Creator = CustomData#house_scene_data.creator,
    #house_data{owner=Owner, house_permission=HousePermission} = CustomData#house_scene_data.house_data,
    
    IsGMHouse = (Creator =/= Owner),
    case IsGMHouse of
	true ->
	    true;
	false -> 
	    MaxPlayerCount = CustomData#house_scene_data.max_players,
	    Players = gen_scene:get_players(State),
	    
	    %% 屋主不计入限制人数
	    PlayerCount = 
		case lists:member(Owner, Players) of
		    true -> length(Players) - 1;
		    _ -> length(Players)
		end,
	    
	    ActionCanDo = action:can_do([{fun({MaxPlayerCount1, PlayerCount1}) -> %% 玩家数超过时不能进入
						  MaxPlayerCount1 > PlayerCount1
					  end, {MaxPlayerCount, PlayerCount}, ?err_player_count_over},
					 {fun(IsEditing) ->                       %% 房间编辑时不能进入
						  not IsEditing
					  end, CustomData#house_scene_data.house_editing, ?err_player_is_editing_house},
					 {fun(IsReplacing) ->                     %% 房屋替换时不能进入
						  not IsReplacing
					  end, CustomData#house_scene_data.house_replacing, ?err_player_replacing_house},
					 {fun(HousePermission1) ->                %% 是否是好友
						  case (Account =/= Owner) and ( HousePermission1 =:= ?hpt_friend) of
						      true ->
							  router:send(Account, is_friend, Owner);
						      _ ->
							  true
						  end
					  end, HousePermission, ?err_not_permission_enter_house}
					]),
	    can_enter_scene1(ActionCanDo, Account, Owner, Priority)
    end;

can_enter_scene(Account, #scene_data{custom_data=CustomData}=State)->
    can_enter_scene({Account, optional}, #scene_data{custom_data=CustomData}=State).


can_enter_scene1(ActionCanDo, Account, Owner, Priority) ->
    case ActionCanDo of
	true ->
	    true;
	{false, ?err_player_count_over} ->  %% 房屋替换过程，主人不能进
	    %%踢人
	    case Priority of
		force->
		    true;
		_->
		    can_enter_scene2(Account, Owner, ?err_player_count_over)
	    end;
	{false, ?err_player_replacing_house} ->  %% 房屋替换过程，主人不能进
	    {false, ?err_player_replacing_house};
	{false, ErrMsg} ->                       %% 其他错误的情况，主人能进
	    can_enter_scene2(Account, Owner, ErrMsg)
    end.

can_enter_scene2(Account, Owner, ErrMsg) ->
    case Account =:= Owner of
	true ->
	    true;
	_ ->
	    {false, ErrMsg}
    end.

-spec update_house_data(list(), tuple(), integer(), tuple()) -> tuple().
update_house_data(PositionList, Furniture, InstId, HouseData) ->
    NFurniture = Furniture#house_furniture{position_list=PositionList},
    NFurnitureVec = furniture:replace(InstId, HouseData, NFurniture),
    HouseData#house_data{furniture_vec=NFurnitureVec}.

-spec update_custom_data(tuple(), list(), tuple(), integer(), tuple()) -> tuple().
update_custom_data(CustomData, PositionList, Furniture, InstId, HouseData) ->
    NHouseData = update_house_data(PositionList, Furniture, InstId, HouseData),
    CustomData#house_scene_data{house_data=NHouseData}.

%% 更新过期家具
-spec update_overtime_furnitures(tuple(), list()) -> tuple().
update_overtime_furnitures(HouseData, Players) when is_list(Players) ->
    update_overtime_furnitures(HouseData, Players, true).
-spec update_overtime_furnitures(tuple(), list(), boolean()) -> tuple().
update_overtime_furnitures(HouseData, Players, NotifyDelete) when is_list(Players) ->
    #house_data{owner=Owner, furniture_vec=FurVec}=HouseData,
    {DelItems, _NextDelItems, NewList} = split_furniture_list(FurVec),
    
    NewHouseData = HouseData#house_data{furniture_vec=NewList},
    case length(DelItems) > 0 of
	true ->
	    %% 数据库操作
	    db_house:write(NewHouseData),
	    [begin
		 InstId = X#house_furniture.instance_id,
		 %% 让正在使用家具的人停止使用
		 PositionList = X#house_furniture.position_list,
		 [begin
		      {FuncId, Pos} = router:send(UsedAccount, furniture_disapear),
		      %% 通知周围的人
		      gen_scene:broadcast_all_players(Players, 
				#notify_stop_using_furniture_result{account=UsedAccount, 
								    position_index=PositionIndex,
								    instance_id=InstId,
								    pos=Pos,
								    function_id=FuncId})
		  end || #furniture_position{used_account=UsedAccount, 
					     position_index=PositionIndex, 
					     is_used=IsUsed} <- PositionList, IsUsed =/= 0],
		 
		 %% 通知场景其它人
		 case NotifyDelete of
		     true ->
			 Packet = #notify_delete_furniture{instance_id=InstId},
			 gen_scene:broadcast_other_players(Players, {Owner, Packet});
		     _ -> %% 房屋刚创建，不需要通知删除
			 ok
		 end
	     end || X <- DelItems],
	    
	    
	    %% 通知物品过期(用邮件方式)
	    Title = binary_to_list(tplt:get_common_def("item_overtime_mail_title")),
	    MsgFormat = binary_to_list(tplt:get_common_def("item_overtime_msg")) ++ [10],	    
	    F = fun(X, Str) -> Str ++ 
				   (item:get_item_name(X#house_furniture.item_tempid) ++ 
					MsgFormat) 
		end,
	    Content = lists:foldl(F, "", DelItems),
	    player_mail:send_sys_mail(Owner, Title, Content, []),

	    %% 后台物品金钱日志
	    Logs = [begin
			ItemInfo = {furniture:get_itemid(X), -1, furniture:get_instid(X)},
			item_money_log:build_record(Owner, auto_del_furniture, ItemInfo)
		    end || X <- DelItems],
	    db_common:write(Logs);
	_ ->
	    ok
    end,


    %% 通知自已
    case util:is_process_alive(Owner) of
	true ->
	    %% 通知物品过期
	    [begin
		 case NotifyDelete of
		     true ->
			 net_helper:send2client(Owner, 
					#notify_delete_furniture{instance_id=
								     X#house_furniture.instance_id});
		     _ -> %% 房屋刚创建，不需要通知删除
			 ok
		 end			     
	     end || X <- DelItems],

	    ok;
	_ ->
	    ok
    end,    
    

    NewHouseData.



%% 遍历家具列表分成 过期，即将过期, 非过期三部分
split_furniture_list(FurList) when is_list(FurList) ->
    Now = datetime:local_time(),

    %%ItemUpdateInterval = ?ITEM_UPDATE_INTERVAL,
    ItemUpdateInterval = common_def:get_val(scan_del_item_interval),
    NextDelTime = datetime:add_time(Now, ItemUpdateInterval),

    F = fun(Fur, {DelItems, NextDelItems, OldList}) ->
			#stime{year=Y, month=M, day=D, 
			       hour=H, minute=N, second=S} = Fur#house_furniture.del_time, 
			DelTime = {{Y, M, D}, {H, N, S}},
			case item:has_del_time(Fur#house_furniture.del_time) of
			    true ->
				case DelTime < Now of
				    true -> %% 过期
					{[Fur | DelItems], NextDelItems, OldList};
				    _ ->
					case DelTime < NextDelTime of
					    true -> %% 即将过期
						{DelItems, [Fur | NextDelItems], [Fur | OldList]};
					    _ ->
						{DelItems, NextDelItems, [Fur | OldList]}	
					end
				end;	
			    _ -> %% 物品无时效属性 
				{DelItems, NextDelItems, [Fur | OldList]}
			end				    
 		end,
    
    lists:foldl(F, {[], [], []}, FurList).

set_house_info(Type, HouseData1, HouseData2) ->
    case Type of
	?bhrt_yes ->
	    WelcomeWord = house_data:get_welcome_words(HouseData1),
	    FurniPermission = house_data:get_furniture_permission(HouseData1),
	    HousePermission = house_data:get_house_permission(HouseData1),
	    NHouseData = house_data:set_welcome_words(HouseData2, WelcomeWord),
	    NNHouseData = house_data:set_furniture_permission(NHouseData, FurniPermission),
	    house_data:set_house_permission(NNHouseData, HousePermission);
	?bhrt_no ->
	    HouseData2
    end.

replace_room_tex(CustomData,Account, RoomTexInfoList)
  when is_list(RoomTexInfoList)->
    HouseData = CustomData#house_scene_data.house_data,
    RoomTexVec = HouseData#house_data.room_tex_vec,

    %% 更换材质
    NewVec = lists:foldl(fun(RoomTexInfo, RoomTexVecOld) -> 
				 furniture:replace_room_tex(RoomTexInfo, RoomTexVecOld) end, 
			 RoomTexVec,
			 RoomTexInfoList),
    NewHouseData = HouseData#house_data{room_tex_vec=NewVec},
    db_house:write(NewHouseData),
    NewCustomData = CustomData#house_scene_data{house_data=NewHouseData},

    %% 通知客房端
    lists:foreach(fun(RoomTexInfo)->
		       net_helper:send2client(Account, #notify_replace_room_tex{room_tex_info=RoomTexInfo}) end, 
	       RoomTexInfoList),
    NewCustomData;

replace_room_tex(CustomData,Account,RoomTexInfo)->
    HouseData = CustomData#house_scene_data.house_data,
    RoomTexVec = HouseData#house_data.room_tex_vec,

    %% 更换材质
    NewVec = furniture:replace_room_tex(RoomTexInfo, RoomTexVec),

    NewHouseData = HouseData#house_data{room_tex_vec=NewVec},
    db_house:write(NewHouseData),
    NewCustomData = CustomData#house_scene_data{house_data=NewHouseData},
    %% 通知客房端
    net_helper:send2client(Account, #notify_replace_room_tex{room_tex_info=RoomTexInfo}),
    NewCustomData.



%% 切换房屋
db_change_house(Master, InstId, Type) ->
    OldHouseData = db_common:select_one(house_data, Master),
    HousePlot = house_plot:get(Master),
    PlotId = house_plot:get_house_plot_id(HousePlot, InstId),
    HouseData = house_plot:get_house(HousePlot, PlotId),
    NHouseData = set_house_info(Type, OldHouseData, HouseData),

    NHousePlot = house_plot:replace(HousePlot, PlotId, OldHouseData),
    db_common:write([NHouseData, NHousePlot]).


%%----------------------------------测试函数-------------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

%% test1() ->
%%     HouseData = #house_data{},

%%     ComponentMesh1 = #house_component_mesh{component_name="comp1", new_mesh="mesh1"},
%%     HouseData1 = handle_replace_component_mesh(HouseData, ComponentMesh1),
%%     #house_data{component_mesh_vec=[ComponentMesh1]} = HouseData1,
    
%%     ComponentMesh2 = #house_component_mesh{component_name="comp1", new_mesh="mesh2"},
%%     HouseData2 = handle_replace_component_mesh(HouseData1, ComponentMesh2),
%%     #house_data{component_mesh_vec=[ComponentMesh2]} = HouseData2,

%%     ComponentMesh3 = #house_component_mesh{component_name="comp3", new_mesh="mesh3"},
%%     HouseData3 = handle_replace_component_mesh(HouseData2, ComponentMesh3),
%%     #house_data{component_mesh_vec=[ComponentMesh3, ComponentMesh2]} = HouseData3.


%% test2() ->
%%     HouseData = #house_data{},

%%     ComponentTex1 = #house_component_tex{component_name="comp1", new_tex="tex1"},
%%     HouseData1 = handle_replace_component_tex(HouseData, ComponentTex1),
%%     #house_data{component_tex_vec=[ComponentTex1]}= HouseData1,
        
%%     ComponentTex2 = #house_component_tex{component_name="comp1", new_tex="tex2"},
%%     HouseData2 = handle_replace_component_tex(HouseData1, ComponentTex2),
%%     #house_data{component_tex_vec=[ComponentTex2]}= HouseData2,
   
%%     ComponentTex3 = #house_component_tex{component_name="comp2", new_tex="tex3"},
%%     HouseData3 = handle_replace_component_tex(HouseData2, ComponentTex3),
%%     #house_data{component_tex_vec=[ComponentTex3, ComponentTex2]}= HouseData3.

%% place_furniture_test()->
%%     erlymock:start(),
%%     erlymock:stub(guid, make, [?st_furniture], [{return, 1}]),
%%     erlymock:stub(guid, make, [?st_furniture], [{return, 2}]),
%%     erlymock:replay(),

%%     HouseData = #house_data{},
%%     HouseData1 = handle_placed_furniture(lyj, 1, 1, 1.0, 1.0, 1.0, 1, 1, HouseData),
%%     ?assertEqual(#house_data{furniture_vec=[#house_furniture{template_id=1}]}, HouseData1),

%%     HouseData2 = handle_placed_furniture(lyj, 1, 2, 2.0, 2.0, 2.0, 1, 1, HouseData1),
%%     ?assertEqual(#house_data{furniture_vec=[#house_furniture{template_id=2},
%%     			       #house_furniture{template_id=1}
%%     			      ]}, HouseData2),
%%     ok.

-endif.	
