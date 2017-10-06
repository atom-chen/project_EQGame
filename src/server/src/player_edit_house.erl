%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   玩家编辑房屋
%%% @end
%%% Created : 31 May 2010 by  <hjx>

-module(player_edit_house).

-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").
-include("tplt_def.hrl").



-behaviour(player_action).


-export([handle_cast/2, start/1]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).


start(Account) ->
    [
     router:make_event_source(?msg_req_start_edit_house, Account, {self(), ?MODULE})
    ].


%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(Account, PID) ->
    [
     router:make_event_source(?msg_req_move_furniture, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_placed_furniture, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_recovery_furniture, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_replace_component_mesh, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_replace_component_tex, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_end_edit_house, Account, {PID, ?MODULE})
    ].

%% 判断当前状态能否开始动作
%% 返回true | {false, Error}
can_start_action(_Param, State) ->
    AtHome = player_data:is_at_home(State),
    case AtHome of
	false -> {false, ?err_only_at_home_can_edit_house};
	_ ->
	    SceneName = player_data:get_scene_name(State),
	    Account = player_data:get_account(State),
	    router:send(SceneName, can_edit_house, Account)
    end.


%% 处理玩家进入家装模式请求
do_start_action(_Param, State) ->    
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    %% 开始编辑房屋
    %% house:start_edit_house(SceneName, Account),
    BornPos = router:send(SceneName, start_edit_house, Account),
    net_helper:send2client(Account, #notify_start_edit_house{}),           
    
    State1 = player_data:set_start_edit_house_time(datetime:local_time(), State),
    NewState = player_data:set_pos(BornPos, State1),
    player_data:set_dest_pos(BornPos, NewState).

%% 结束编辑房屋
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action(_Param, State) ->    
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    %% house:end_edit_house(SceneName, Account),
    router:send(SceneName, end_edit_house, Account),
    net_helper:send2client(Account, #notify_end_edit_house{}),

    %% 记录本次编辑房屋时间
    Now = datetime:local_time(),
    OldTime = player_data:get_start_edit_house_time(State),
    Seconds = datetime:diff_time(OldTime, Now),
    db_game_info_gather:add_edit_house_seconds(Seconds),

    State.


%%%===================================================================
%%% 处理handle_cast
%%%===================================================================

%% 处理玩家进入家装模式请求
handle_cast({_Msg, #req_start_edit_house{}}, State) ->
    NewState = mutex_actions:try_start_action(?action_type_edit_house, undefined, State),
    {noreply, NewState};

%%-----------------------------房屋编辑相关------------------------------------------
%% 摆放家具
handle_cast({_Msg, #req_placed_furniture{grid_index=GIndex,
					 x=X,z=Z,height=Height,floor=Floor,face=Face}}, State) ->
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),

    case player_pack:get_item(GIndex, State) of
	empty -> erlang:error({badarg, GIndex, State});
	Item -> % TODO: 如何在两个进程之间使用数据库事务, 保证数据的一致性
	    ItemTempID = item:get_tempid(Item),
	    DelTime = item:get_del_time(Item),
	    #item_tplt{sub_id=FurnitureTempID} = tplt:get_data(item_tplt, ItemTempID),

	    FurInstID = guid:make(?st_furniture),
	    Furniture = furniture:new(ItemTempID, FurInstID, FurnitureTempID, 
				X, Z, Height, Floor, Face, DelTime),
	    router:send(SceneName, modify_house, {Account, {place_furniture, Account, Furniture}}),
	    NPD = player_pack:del_item2(GIndex, State),

	    %% 增加家装值
	    Decoration = item:get_decoration_val(Item),	    
	    player_decoration:add_decoration(Account, Decoration),

	    %% 物品金钱使用记录
	    Target = FurInstID, %% 用target字段存 家具实例id
	    ItemCount = 0, %% 这里物品只是变成家具而已
	    ItemInfo = {item:get_tempid(Item), ItemCount, item:get_instid(Item)},
	    item_money_log:log_item(Account, item2furniture, ItemInfo, Target),
	    {noreply, NPD}
    end;


%% 移动家具
handle_cast({_Msg, #req_move_furniture{instance_id=InstanceID,x=X,z=Z,height=Height,
				       floor=Floor,face=Face}}, State)->
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    router:send(SceneName, modify_house, {Account, {move_furniture, InstanceID,
						    X,Z,Height,Floor,Face}}),
    {noreply, State};

%% 回收家具
handle_cast({_Msg, #req_recovery_furniture{furniture_id=FurInstID}}, State) ->
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    case router:send(SceneName, get_furniture_item_info, FurInstID) of
	not_found ->
	    sys_msg:send(Account, ?err_furniture_not_exist),
	    {noreply, State};
	ItemID ->
	    NState = case player_pack:can_add_item(ItemID, 1, State) of
			 true ->
			     Item = router:send(SceneName, recovery_furniture, {Account, FurInstID}),	     
			     {ok, PlayerData} = player_pack:add_item2(Account, Item, State),
			     %% 物品金钱使用记录
			     Target = FurInstID, %% 用target字段存 家具实例id
			     ItemCount = 0, %% 这里家具只是变成物品而已
			     ItemInfo = {item:get_tempid(Item), ItemCount, item:get_instid(Item)},
			     item_money_log:log_item(Account, furniture2item, ItemInfo, Target),
			     PlayerData;
			 false ->
			     sys_msg:send(Account, ?err_bag_full),
			     State
		     end,
	    {noreply, NState}
    end;
%% 替换纹理
handle_cast({#msg{event=?msg_req_replace_component_mesh}, Packet}, 
	    State) ->
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    router:send(SceneName, modify_house, {Account, Packet}),
    {noreply, State};
 
%% 替换材质
handle_cast({#msg{event=?msg_req_replace_component_tex}, Packet}, 
	    State) ->
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    router:send(SceneName, modify_house, {Account, Packet}),
    {noreply, State};

%% 处理玩家结束家装模式请求
handle_cast({#msg{event=?msg_req_end_edit_house}, #req_end_edit_house{}}, State) ->
    NewState = mutex_actions:call_stop_action(?action_type_edit_house, State),
    {noreply, NewState}.







