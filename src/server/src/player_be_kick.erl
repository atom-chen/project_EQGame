%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   ���ߴ���
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
%%% ������ػص�����
%%%===================================================================
%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
get_action_register_msgs(_Account, _PID) ->
    [
    ].

%% �жϵ�ǰ״̬�ܷ�ʼ����
%% ����true | {false, Error} || {false, Error, Params} 
can_start_action({MasterScene, _Reason}, State) 
  when is_atom(MasterScene) -> 
    MyScene = player_data:get_scene_name(State),
    case MasterScene =:= MyScene of
	false -> {false, ?err_no_error}; %% ����ͬһ��������������
	true -> true
    end.


do_start_action({MasterScene, Reason}, State) ->
    SceneName = MasterScene,
    Guest = player_data:get_account(State),

    router:send(SceneName, leave_scene, Guest),

    %% ���˻ؼ�
    NewState = player_change_scene:enter_player_scene(Guest, State),

    %% ֪ͨ����ԭ��
    sys_msg:send(Guest, Reason),

    NewState.

%% ��������
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action(_Param, State) ->
    State.



%%%===================================================================
%%% ����handle_call
%%%===================================================================
%% �������˵ı��ߴ���(�д����뿪����)
handle_call({#msg{event=you_kick_me}, Param}, _From, 
	    MyState) ->

    MyNewState = mutex_actions:try_start_action(?action_type_be_kick, Param, MyState),

    {reply, ok, MyNewState}.


%%%===================================================================
%%% ����handle_cast
%%%===================================================================
%% �������ߴ���
handle_cast({#msg{event=be_kick}, KickReason},  
	    State) ->
    Scene = player_data:get_scene_name(State),
    NewState = mutex_actions:try_start_action(?action_type_be_kick, {Scene, KickReason}, State),
    {noreply, NewState};

%% ���������
handle_cast({_Msg, #req_kick_guest{target_player=TargetAccount}},
	    State) ->
    Account = player_data:get_account(State),
    Guest = list_to_atom(TargetAccount),

    AtHome = player_data:is_at_home(State),
    Scene = player_data:get_scene_name(State),

    case AtHome of
	false -> %% ���ڼ�, ��������
	    sys_msg:send(Account, ?err_only_at_home_can_kick_guest);
	true ->
	    case router:send(Guest, you_kick_me, {Scene, ?err_kick_by_master}) of
		undefined -> %%�Է�������
		    sys_msg:send(Account, ?err_player_offline);
		_ -> 
		    ok
	    end
    end,

    {noreply, State}.


%%%===================================================================
%%% �ڲ�����
%%%===================================================================





