%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   �������
%%% @end
%%% Created : 27 May 2010 by  <hjx>

-module(player_invite).


-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").

-behaviour(player_action).

-export([handle_cast/2, start/1, notify_offline_invite_list/1]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).

start(Account) ->
    [
     router:make_event_source(req_invite_someone, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_invite_someone, Account, {self(), ?MODULE})
    ].


%%%===================================================================
%%% Api
%%%===================================================================
notify_offline_invite_list(Account) ->
    %% ֪ͨ��������    
    InviteList = db_friends:get_offline_invite_list(Account),

    case length(InviteList) > 0 of
	true ->
	    List = [#offline_invite_record{invite_type=?ivt_make_friend, 
					   invitor=atom_to_list(X),
					   invitor_name=db_player:get_user_name(X),
					   date=DateTime
				   } 
	     || {X, _Type, DateTime} <- InviteList, is_atom(X)],
	    
	    NotifyPacket = #notify_offline_invites{inv_list=List},
	    net_helper:send2client(Account, NotifyPacket);
	_ ->
	    ok  
    end.


%%%===================================================================
%%% ������ػص�����
%%%===================================================================
%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
get_action_register_msgs(_Account, _PID) ->
    [
    ].

%% �жϵ�ǰ״̬�ܷ�ʼ����
%% ����true | {false, Error}
can_start_action({Account, _MyUserName, OtherAccount, YourUserName, 
		  ?ivt_enter_house, _Params}, _State) ->
    FriendVisit = house:whether_only_allow_friends_visit(Account),
    IsFriend = router:send(Account, is_friend, OtherAccount),
    case FriendVisit of
	true ->
	    case IsFriend of 
		true -> %% ����Ϊ����
		    true;
		_ ->
		    {false, ?err_house_permission_not_friend, [YourUserName]}
	    end;
	_ ->
	    true
    end;
can_start_action({_Account, _MyUserName, _OtherAccount, _YourUserName, 
		  _Type, _Params}, _State) ->
    true.


%% ��ʼ����
do_start_action({_Account, _MyUserName, _OtherAccount, _YourUserName, 
		  ?ivt_make_friend, _Params}, State) ->
    Account = player_data:get_account(State),
    %% ��ʾ�����ͳɹ�
    sys_msg:send(Account, ?msg_send_make_friend_request),

    State; 	    
do_start_action(_Param, State) ->
    State. 	    

%% ����״̬
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action(_Param, State) ->
    State.



%%%===================================================================
%%% ����handle_cast
%%%===================================================================

%% ��������
handle_cast({#msg{event=req_invite_someone}, {Type, YourAccount}}, 
	    MyState) -> 
    NewState = do_invite_someone(Type, YourAccount, MyState),    
    {noreply, NewState};
%% ��������
handle_cast({_Msg, 
	     #req_invite_someone{type=Type, invitee=Invitee}}, 
	    MyState) ->    
    YourAccount = list_to_atom(Invitee),    
    NewState = do_invite_someone(Type, YourAccount, MyState),    
    {noreply, NewState}.


%%%===================================================================
%%% �ڲ�����
%%%===================================================================
do_invite_someone(Type, YourAccount, MyState) when is_atom(YourAccount) ->
    Account = player_data:get_account(MyState),

    MyUserName = basic_data:get_data(username, player_data:get_basic_data(MyState)),
    YourUserName = player:get_user_name(YourAccount), 
    Params = [MyUserName],

    MyParam = {Account, MyUserName, YourAccount, YourUserName, Type, Params},
    YourParam = MyParam,
    MyActionID = ?action_type_invite,
    YourActionID = ?action_type_be_invite,
    NewState = mutex_actions:try_start_two_action(MyActionID, MyParam, YourActionID, YourParam, YourAccount, MyState),
    NewState.

