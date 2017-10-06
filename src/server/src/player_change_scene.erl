%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%    切换场景
%%% @end
%%% Created : 31 May 2010 by  <hjx>

-module(player_change_scene).


-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").


-behaviour(player_action).

-export([handle_cast/2, start/1]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).

-export([enter_player_scene/2, immediate_change_scene/2]).
-export([force_enter_player_house/2]).

start(Account) ->
    [
     router:make_event_source(?msg_req_teleport, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_enter_common_scene, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_open_change_scene_guide, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_enter_player_house, Account, {self(), ?MODULE}),
     router:make_event_source(force_enter_player_house, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(Account, PID) ->
    [
     router:make_event_source(?msg_req_close_change_scene_guide, Account, {PID, ?MODULE})
    ].

%% 判断当前状态能否开始动作
%% 返回true | {false, Error}
can_start_action(_Param, _State) ->    
    true.


do_start_action(no_notify, State) ->
    State;
do_start_action(_Param, State) ->
    Account = player_data:get_account(State),
    %% 通知开始
    net_helper:send2client(Account, #notify_open_change_scene_guide{}),   
    State.


%% 结束动作
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action(no_notify, State) ->
    State;
do_stop_action(_Param, State) ->
    Account = player_data:get_account(State),
    %% 通知结束
    net_helper:send2client(Account, #notify_close_change_scene_guide{}),
    State.


%%%===================================================================
%%% 处理handle_cast
%%%===================================================================

%% 开始切换场景
handle_cast({_Msg, 
	     #req_open_change_scene_guide{}}, State) ->
    NewState = mutex_actions:try_start_action(?action_type_change_scene, undefined, State),
    {noreply, NewState};	
%% 结束切换场景
handle_cast({_Msg, 
	     #req_close_change_scene_guide{}}, State) ->
    NewState = mutex_actions:call_stop_action(?action_type_change_scene, undefined, State),
    {noreply, NewState};
%% 处理玩家进入公共场景的请求
handle_cast({_Msg, #req_enter_common_scene{scene_id=TargetSceneID, copy_id=CopyId}}, State) ->    
    Param = {{TargetSceneID, CopyId}, born_pos},
    NewState = smart_change_scene(Param, State),
    {noreply, NewState};	

%% 处理玩家强制进入其它玩家场景的请求
handle_cast({#msg{event=force_enter_player_house}, #req_enter_player_house{account=TargetAccount}}, State) ->    
    Param = list_to_atom(TargetAccount),
    NewState = smart_change_scene(Param, State),
    {noreply, NewState};

%% 处理玩家进入其它玩家场景的请求
handle_cast({_Msg, 
	     #req_enter_player_house{account=TargetAccount}}, State) ->    
    Param = {list_to_atom(TargetAccount), optianal},
    NewState = smart_change_scene(Param, State),
    {noreply, NewState};

%% 处理玩家进入其它玩家场景的请求
handle_cast({_Msg, #req_teleport{id=TeleportID,copy_id=CopyId}}, State) ->
    TeleportTplt = tplt:get_data(teleport_tplt, TeleportID),

    #teleport_tplt{dest_scene_id=TargetSceneID,		   
		   %% dest_x=X,
		   %% dest_y=Y,
		   %% dest_z=Z
		   born_id=BornId
		  } = TeleportTplt,

    %% EnterPos = #point{x=X, y=Y, z=Z}, 
    %% Param = {{TargetSceneID,CopyId}, EnterPos},
    Param = {{TargetSceneID,CopyId}, BornId},
    NewState = smart_change_scene(Param, State),

    {noreply, NewState}.

%%%===================================================================
%%% 内部函数
%%%===================================================================
%% 进入公共场景
enter_common_scene({SceneID,CopyId}, EnterPos, PlayerData) 
  when is_integer(SceneID) ->
    SceneName = scene:id_to_scene(SceneID, CopyId),
    Account = player_data:get_account(PlayerData),
    NPlayerData = do_enter_scene(SceneName, Account, EnterPos, PlayerData),
    {true, NNPlayerData} = 
    case player_task:is_exist(6, NPlayerData) of
	true ->
	    player_task:complete(6, 0, NPlayerData);
	false ->
	    {true, NPlayerData}
    end,
    NNPlayerData.

%% 进入玩家场景
enter_player_scene(TargetAccount, PlayerData) ->
    Account = player_data:get_account(PlayerData),
    SceneName = router:send(scene_manager, create_house, 
			    {Account, TargetAccount}),
    do_enter_scene(SceneName, Account, born_pos, PlayerData).
    

do_enter_scene(SceneName, Account, EnterPos, PlayerData) ->
    BodyPacket = player_pack:get_notify_body_packet(PlayerData),
    SimplePacket = player:get_player_simple_data(PlayerData),
    PlayerLevel = player_level:get_level(PlayerData),
    Pos = router:send(SceneName, enter_scene, {Account, EnterPos, PlayerLevel, 
					       BodyPacket, SimplePacket}),
    player_data:change_scene(SceneName, Pos, PlayerData).


can_enter({SceneID, EnterPos}, State) when is_integer(SceneID) ->
    can_enter({{SceneID, 1}, EnterPos}, State); %%默认进入第一个副本

can_enter({{SceneID, CopyId}, _EnterPos}, State) when is_integer(SceneID) ->
    Account = player_data:get_account(State),
    TargetScene = scene:id_to_scene(SceneID, CopyId),
    data_helper:format("TargetScene:~p~n", [TargetScene]),
    router:send(TargetScene, can_enter_scene, Account);

can_enter({OtherAccount, Priority}, State) when is_atom(OtherAccount) ->   
    Account = player_data:get_account(State),
    case db_player:is_exist(OtherAccount) of
	true ->
	    case atom_to_list(OtherAccount) of
		[] -> {false, ?err_target_is_empty};
		_ ->    
		    SceneName = router:send(scene_manager, create_house, 
					    {Account, OtherAccount}),

		    router:send(SceneName, can_enter_scene, {Account, Priority})
	    end;
	_ -> 
	    {false, ?err_player_no_eq_home}
    end;

can_enter(OtherAccount, State) when is_atom(OtherAccount) -> 
    can_enter({OtherAccount,optional}, State).

%% 处理玩家进入其它玩家场景的请求
do_enter({OtherAccount, _Priority}, State)
  when is_atom(OtherAccount) ->
    do_enter(OtherAccount, State);
do_enter(OtherAccount, State)
  when is_atom(OtherAccount) ->
    Account = player_data:get_account(State),
    SceneName = player_data:get_scene_name(State),
    router:send(SceneName, leave_scene, Account),
    NewPlayerData = enter_player_scene(OtherAccount, State),
    %% 增加活力值
    player_every_day_active:add_active_value(Account, OtherAccount, NewPlayerData);

%% 处理玩家进入公共场景的请求
do_enter({TargetSceneID, EnterPos}, State) 
  when is_integer(TargetSceneID) ->
    do_enter({{TargetSceneID, 1}, EnterPos}, State);

do_enter({{TargetSceneID, CopyId}, EnterPos}, State) 
  when is_integer(TargetSceneID) ->    
    Account = player_data:get_account(State),
    SceneName = player_data:get_scene_name(State),

    router:send(SceneName, leave_scene, Account),
    NewPlayerData = enter_common_scene({TargetSceneID,CopyId}, EnterPos, State),
    NewPlayerData. 	    

enter_scene({{SceneId, CopyId},EnterPos}, State) ->
    %%判断是否是自动选择场景
    Param =  case CopyId of 
		  0 ->
		      {{SceneId, get_copyid(SceneId)}, EnterPos};
		  _->
		      {{SceneId, CopyId}, EnterPos}	      
	      end,
    enter_scene1(Param, State);
enter_scene(Param, State) ->
    enter_scene1(Param,State).

enter_scene1(Param, State) ->
    %%判断是否是自动选择场景
    case can_enter(Param, State) of
	true -> 
	    do_enter(Param, State);
	{false, Reason} ->
	    Account = player_data:get_account(State),	    
	    sys_msg:send(Account, Reason),
	    State		       
    end.

get_copyid(SceneId)->
    {Return, _} = ets:foldl(fun({{SId, CId}, _, CNT, _}, {CopyId, Count})-> 
				    case SId=:=SceneId andalso (CopyId == 0 orelse CNT =< Count) of 
					true->
					    {CId, CNT};
					_->
					    {CopyId, Count}
				    end
			    end, {0, 0},common_scene_copy),
    Return.

%% 瞬时切场景，不通知客户端结束切场景画面
immediate_change_scene(Param, State) ->
    State1 = mutex_actions:try_start_action(?action_type_change_scene, no_notify, State),
    case mutex_actions:contains_action(?action_type_change_scene, State1) of
	true -> %% 说明成功开始
	    State2 = enter_scene(Param, State1),
	    mutex_actions:call_stop_action(?action_type_change_scene, no_notify, State2);
	_ -> 
	    State1
    end.

smart_change_scene(Param, State) ->
    %% 目前存在两种状况: 走路直接踩点 和 开界面选场景，所以需要这样处理
    case mutex_actions:contains_action(?action_type_change_scene, State) of
	true ->
	    enter_scene(Param, State);
	_ ->
	    immediate_change_scene(Param, State)
    end.
  
force_enter_player_house(Account, TargetAccount)->
    router:cast(TargetAccount, ?msg_req_change_house, #req_enter_player_house{account=TargetAccount}),
    router:cast(Account, force_enter_player_house, #req_enter_player_house{account=TargetAccount}).
