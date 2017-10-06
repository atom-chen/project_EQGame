%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   ��Ҷ�������
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
%%% ������ػص�����
%%%===================================================================
%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
get_action_register_msgs(Account, PID) ->
    [
     router:make_event_source(?msg_req_end_body_action, Account, {PID, ?MODULE})
    ].

%% �жϵ�ǰ״̬�ܷ�ʼ����
%% ����true | {false, Error}
can_start_action(_Params, _State) ->
    true.

%% ��ʼ
do_start_action({Action, TargetAccount}, State) ->
    Account = player_data:get_account(State),

    %% ֪ͨ����
    Packet = #notify_start_body_action{player_account=atom_to_list(Account), 
				       target_account=atom_to_list(TargetAccount), 
				       action=Action
				      },
    
    net_helper:send2client(Account, Packet),

    ActionInfo = {Action, TargetAccount},
    player_data:set_body_action_info(ActionInfo, State).


%% ����
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action(_Param, State) ->
    Account = player_data:get_account(State),

    %% ֪ͨ����
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
	    %% ֪ͨ�Է�ֹͣ����
	    router:cast(TargetPlayer, cast_stop_action, {?action_type_body_action, undefined})
    end,
    
    player_data:set_body_action_info(undefined, State).


%%%===================================================================
%%% ����api
%%%===================================================================
%% ��ҽ��볡�����¼�
%% Account: ���볡�������
player_enter_scene(HeAccount, State)->
    Account = player_data:get_account(State),
    BodyState = player_data:get_body_state(State),
    NotifyBodyState = #notify_other_body_state{
			     other_account=atom_to_list(Account),
			     body_state=BodyState},

    %% �����Ѿ��ڷŶ���,�Ͳ�֪ͨ���嶯����
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
%%% ����handle_cast
%%%===================================================================


%% ����ʼ��������
handle_cast({_Msg, #req_start_body_action{action=Action, target_account=TargetAccount}}, 
	    State) ->
    NewState = mutex_actions:try_start_action(?action_type_body_action, 
					      {Action, list_to_atom(TargetAccount)}, State),
    {noreply, NewState};


%% ���󲥷Ŷ���(�������˹㲥)
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

%% ���������������
handle_cast({_Msg, #req_sync_body_state{body_state=BodyState}}, 
	    State) ->
    Account = player_data:get_account(State),
    SceneName = player_data:get_scene_name(State),
    Packet = #notify_other_body_state{other_account=atom_to_list(Account),
				     body_state=BodyState},

    router:cast(undefined, SceneName, broadcast_other_players, {Account, Packet}),
    NewState = player_data:set_body_state(BodyState, State),
    {noreply, NewState};


%% ���������������
handle_cast({_Msg, #req_end_body_action{}}, 
	    State) ->
    NewState = mutex_actions:call_stop_action(?action_type_body_action, undefined, State),
    {noreply, NewState}.


%%%===================================================================
%%% �ڲ�����
%%%===================================================================


