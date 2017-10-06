%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   被踢处理
%%$      
%%% @end
%%% Created : 17 Jun 2010 by  <hjx>

-module(player_be_kick).
-behaviour(player_action).


-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").


-export([handle_call/3, handle_cast/2, start/1]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).



start(Account) ->
    [
     router:make_event_target(you_kick_me, Account, {self(), ?MODULE}),
     router:make_event_source(be_kick, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_kick_guest, Account, {self(), ?MODULE})
    ].


%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(_Account, _PID) ->
    [
    ].

%% 判断当前状态能否开始动作
%% 返回true | {false, Error} || {false, Error, Params} 
can_start_action({MasterScene, _Reason}, State) 
  when is_atom(MasterScene) -> 
    MyScene = player_data:get_scene_name(State),
    case MasterScene =:= MyScene of
	false -> {false, ?err_no_error}; %% 不在同一场景，不能踢人
	true -> true
    end.


do_start_action({MasterScene, Reason}, State) ->
    SceneName = MasterScene,
    Guest = player_data:get_account(State),

    router:send(SceneName, leave_scene, Guest),

    %% 客人回家
    NewState = player_change_scene:enter_player_scene(Guest, State),

    %% 通知被踢原因
    sys_msg:send(Guest, Reason),

    NewState.

%% 结束动作
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action(_Param, State) ->
    State.



%%%===================================================================
%%% 处理handle_call
%%%===================================================================
%% 正常踢人的被踢处理(有处理离开场景)
handle_call({#msg{event=you_kick_me}, Param}, _From, 
	    MyState) ->

    MyNewState = mutex_actions:try_start_action(?action_type_be_kick, Param, MyState),

    {reply, ok, MyNewState}.


%%%===================================================================
%%% 处理handle_cast
%%%===================================================================
%% 其它被踢处理
handle_cast({#msg{event=be_kick}, KickReason},  
	    State) ->
    Scene = player_data:get_scene_name(State),
    NewState = mutex_actions:try_start_action(?action_type_be_kick, {Scene, KickReason}, State),
    {noreply, NewState};

%% 处理踢玩家
handle_cast({_Msg, #req_kick_guest{target_player=TargetAccount}},
	    State) ->
    Account = player_data:get_account(State),
    Guest = list_to_atom(TargetAccount),

    AtHome = player_data:is_at_home(State),
    Scene = player_data:get_scene_name(State),

    case AtHome of
	false -> %% 不在家, 不能踢人
	    sys_msg:send(Account, ?err_only_at_home_can_kick_guest);
	true ->
	    case router:send(Guest, you_kick_me, {Scene, ?err_kick_by_master}) of
		undefined -> %%对方不在线
		    sys_msg:send(Account, ?err_player_offline);
		_ -> 
		    ok
	    end
    end,

    {noreply, State}.


%%%===================================================================
%%% 内部函数
%%%===================================================================





