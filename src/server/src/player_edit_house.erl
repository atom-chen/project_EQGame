%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   ��ұ༭����
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
%%% ������ػص�����
%%%===================================================================
%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
get_action_register_msgs(Account, PID) ->
    [
     router:make_event_source(?msg_req_move_furniture, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_placed_furniture, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_recovery_furniture, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_replace_component_mesh, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_replace_component_tex, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_end_edit_house, Account, {PID, ?MODULE})
    ].

%% �жϵ�ǰ״̬�ܷ�ʼ����
%% ����true | {false, Error}
can_start_action(_Param, State) ->
    AtHome = player_data:is_at_home(State),
    case AtHome of
	false -> {false, ?err_only_at_home_can_edit_house};
	_ ->
	    SceneName = player_data:get_scene_name(State),
	    Account = player_data:get_account(State),
	    router:send(SceneName, can_edit_house, Account)
    end.


%% ������ҽ����װģʽ����
do_start_action(_Param, State) ->    
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    %% ��ʼ�༭����
    %% house:start_edit_house(SceneName, Account),
    BornPos = router:send(SceneName, start_edit_house, Account),
    net_helper:send2client(Account, #notify_start_edit_house{}),           
    
    State1 = player_data:set_start_edit_house_time(datetime:local_time(), State),
    NewState = player_data:set_pos(BornPos, State1),
    player_data:set_dest_pos(BornPos, NewState).

%% �����༭����
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action(_Param, State) ->    
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    %% house:end_edit_house(SceneName, Account),
    router:send(SceneName, end_edit_house, Account),
    net_helper:send2client(Account, #notify_end_edit_house{}),

    %% ��¼���α༭����ʱ��
    Now = datetime:local_time(),
    OldTime = player_data:get_start_edit_house_time(State),
    Seconds = datetime:diff_time(OldTime, Now),
    db_game_info_gather:add_edit_house_seconds(Seconds),

    State.


%%%===================================================================
%%% ����handle_cast
%%%===================================================================

%% ������ҽ����װģʽ����
handle_cast({_Msg, #req_start_edit_house{}}, State) ->
    NewState = mutex_actions:try_start_action(?action_type_edit_house, undefined, State),
    {noreply, NewState};

%%-----------------------------���ݱ༭���------------------------------------------
%% �ڷżҾ�
handle_cast({_Msg, #req_placed_furniture{grid_index=GIndex,
					 x=X,z=Z,height=Height,floor=Floor,face=Face}}, State) ->
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),

    case player_pack:get_item(GIndex, State) of
	empty -> erlang:error({badarg, GIndex, State});
	Item -> % TODO: �������������֮��ʹ�����ݿ�����, ��֤���ݵ�һ����
	    ItemTempID = item:get_tempid(Item),
	    DelTime = item:get_del_time(Item),
	    #item_tplt{sub_id=FurnitureTempID} = tplt:get_data(item_tplt, ItemTempID),

	    FurInstID = guid:make(?st_furniture),
	    Furniture = furniture:new(ItemTempID, FurInstID, FurnitureTempID, 
				X, Z, Height, Floor, Face, DelTime),
	    router:send(SceneName, modify_house, {Account, {place_furniture, Account, Furniture}}),
	    NPD = player_pack:del_item2(GIndex, State),

	    %% ���Ӽ�װֵ
	    Decoration = item:get_decoration_val(Item),	    
	    player_decoration:add_decoration(Account, Decoration),

	    %% ��Ʒ��Ǯʹ�ü�¼
	    Target = FurInstID, %% ��target�ֶδ� �Ҿ�ʵ��id
	    ItemCount = 0, %% ������Ʒֻ�Ǳ�ɼҾ߶���
	    ItemInfo = {item:get_tempid(Item), ItemCount, item:get_instid(Item)},
	    item_money_log:log_item(Account, item2furniture, ItemInfo, Target),
	    {noreply, NPD}
    end;


%% �ƶ��Ҿ�
handle_cast({_Msg, #req_move_furniture{instance_id=InstanceID,x=X,z=Z,height=Height,
				       floor=Floor,face=Face}}, State)->
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    router:send(SceneName, modify_house, {Account, {move_furniture, InstanceID,
						    X,Z,Height,Floor,Face}}),
    {noreply, State};

%% ���ռҾ�
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
			     %% ��Ʒ��Ǯʹ�ü�¼
			     Target = FurInstID, %% ��target�ֶδ� �Ҿ�ʵ��id
			     ItemCount = 0, %% ����Ҿ�ֻ�Ǳ����Ʒ����
			     ItemInfo = {item:get_tempid(Item), ItemCount, item:get_instid(Item)},
			     item_money_log:log_item(Account, furniture2item, ItemInfo, Target),
			     PlayerData;
			 false ->
			     sys_msg:send(Account, ?err_bag_full),
			     State
		     end,
	    {noreply, NState}
    end;
%% �滻����
handle_cast({#msg{event=?msg_req_replace_component_mesh}, Packet}, 
	    State) ->
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    router:send(SceneName, modify_house, {Account, Packet}),
    {noreply, State};
 
%% �滻����
handle_cast({#msg{event=?msg_req_replace_component_tex}, Packet}, 
	    State) ->
    SceneName = player_data:get_scene_name(State),
    Account = player_data:get_account(State),
    router:send(SceneName, modify_house, {Account, Packet}),
    {noreply, State};

%% ������ҽ�����װģʽ����
handle_cast({#msg{event=?msg_req_end_edit_house}, #req_end_edit_house{}}, State) ->
    NewState = mutex_actions:call_stop_action(?action_type_edit_house, State),
    {noreply, NewState}.







