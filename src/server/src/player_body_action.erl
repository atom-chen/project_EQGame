%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   玩家动作表情
%%% @end
%%% Created : 2010-10-09 by  <hjx>

-module(player_body_action).


-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").

-behaviour(player_action).

-export([handle_cast/2, start/1, player_enter_scene/2]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).

start(Account) ->
    [
     router:make_event_source(?msg_req_sync_body_state, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_start_body_action, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_play_animation, Account, {self(), ?MODULE})
    ].


%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(Account, PID) ->
    [
     router:make_event_source(?msg_req_end_body_action, Account, {PID, ?MODULE})
    ].

%% 判断当前状态能否开始动作
%% 返回true | {false, Error}
can_start_action(_Params, _State) ->
    true.

%% 开始
do_start_action({Action, TargetAccount}, State) ->
    Account = player_data:get_account(State),

    %% 通知自已
    Packet = #notify_start_body_action{player_account=atom_to_list(Account), 
				       target_account=atom_to_list(TargetAccount), 
				       action=Action
				      },
    
    net_helper:send2client(Account, Packet),

    ActionInfo = {Action, TargetAccount},
    player_data:set_body_action_info(ActionInfo, State).


%% 结束
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action(_Param, State) ->
    Account = player_data:get_account(State),

    %% 通知自已
    Packet = #notify_end_body_action{player_account=atom_to_list(Account), 
				     target_account=""
				    },
    
    net_helper:send2client(Account, Packet),

    TargetPlayer =
	case player_data:get_body_action_info(State) of
	    {_Action, TargetAccount} -> TargetAccount;
	    {_Action, TargetAccount, _Loop, _Ani} -> TargetAccount
	end,
    
    case TargetPlayer of
	'' -> ok;
	_ ->
	    %% 通知对方停止动作
	    router:cast(TargetPlayer, cast_stop_action, {?action_type_body_action, undefined})
    end,
    
    player_data:set_body_action_info(undefined, State).


%%%===================================================================
%%% 处理api
%%%===================================================================
%% 玩家进入场景的事件
%% Account: 进入场景的玩家
player_enter_scene(HeAccount, State)->
    Account = player_data:get_account(State),
    BodyState = player_data:get_body_state(State),
    NotifyBodyState = #notify_other_body_state{
			     other_account=atom_to_list(Account),
			     body_state=BodyState},

    %% 人物已经在放动画,就不通知身体动作了
    case mutex_actions:contains_action(?action_type_walk, State) 
	or mutex_actions:contains_action(?action_type_use_furniture, State) of
	true ->
	    ok;
	_ ->
	    net_helper:send2client(HeAccount, NotifyBodyState)
    end,

    case player_data:get_body_action_info(State) of
	{Action, TargetAccount, Loop, Animation} ->    
	    Packet = #notify_play_animation{player_account=atom_to_list(Account), 
					    action=Action,
					    target_account=atom_to_list(TargetAccount),
					    ani=Animation,
					    loop=Loop
					   },
	    
	    net_helper:send2client(HeAccount, Packet);
	_ -> 
	    ok
    end.


%%%===================================================================
%%% 处理handle_cast
%%%===================================================================


%% 请求开始动作表情
handle_cast({_Msg, #req_start_body_action{action=Action, target_account=TargetAccount}}, 
	    State) ->
    NewState = mutex_actions:try_start_action(?action_type_body_action, 
					      {Action, list_to_atom(TargetAccount)}, State),
    {noreply, NewState};


%% 请求播放动作(对其它人广播)
handle_cast({_Msg, #req_play_animation{action=Action, target_account=Target,
				       ani=Animation, loop=Loop}}, State) ->
    TargetAccount = list_to_atom(Target),
    NewState =
	case player_data:get_body_action_info(State) of
	    {Action, TargetAccount} ->
		player_data:set_body_action_info({Action, TargetAccount, Loop, Animation}, State);
	    {Action, TargetAccount, _Loop, _Ani} ->
		player_data:set_body_action_info({Action, TargetAccount, Loop, Animation}, State);
	    _ ->
		State
	end,
     
    Account = player_data:get_account(State),
    SceneName = player_data:get_scene_name(State),
    Packet = #notify_play_animation{action=Action, 
				    target_account=Target,
				    ani=Animation, loop=Loop,
				    player_account=atom_to_list(Account)
				   },
    
    router:cast(undefined, SceneName, broadcast_other_players, {Account, Packet}),
    {noreply, NewState};

%% 请求结束动作表情
handle_cast({_Msg, #req_sync_body_state{body_state=BodyState}}, 
	    State) ->
    Account = player_data:get_account(State),
    SceneName = player_data:get_scene_name(State),
    Packet = #notify_other_body_state{other_account=atom_to_list(Account),
				     body_state=BodyState},

    router:cast(undefined, SceneName, broadcast_other_players, {Account, Packet}),
    NewState = player_data:set_body_state(BodyState, State),
    {noreply, NewState};


%% 请求结束动作表情
handle_cast({_Msg, #req_end_body_action{}}, 
	    State) ->
    NewState = mutex_actions:call_stop_action(?action_type_body_action, undefined, State),
    {noreply, NewState}.


%%%===================================================================
%%% 内部函数
%%%===================================================================


