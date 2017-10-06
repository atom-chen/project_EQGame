%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   玩家被邀请
%%% @end
%%% Created : 27 May 2010 by  <hjx>

-module(player_be_invite).

-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").

-behaviour(player_action).

-record(invite_data, {
	  invitor,                                 % 邀请者
	  invite_type,                             % 邀请类型
	  invite_params                            % 邀请参数
	 }).

-export([handle_cast/2, start/1]).
-export([do_start_action/2, do_stop_action/2,
	 can_start_action/2, get_action_register_msgs/2]).



start(Account) ->
    [
     router:make_event_source(?msg_req_agree_invitation, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_disagree_invitation, Account, {self(), ?MODULE})
    ].


%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(_Account, _PID) ->
    [
    ].

%% 判断当前状态能否开始动作
%% 返回true | {false, Error}
can_start_action(_Param, _State) ->
    true.

%% 开始被邀请状态
do_start_action({Invitor, _InvitorUserName, _Invitee, _InviteeUsername, Type, Params},
	    State) ->
    Account = player_data:get_account(State),
    [UserName] = Params,
    
    InviteInfo = #invite_data{invitor=Invitor, invite_type=Type, invite_params=Params},

    Packet = #notify_invitation{invitor=atom_to_list(Invitor), 
				invitor_name=UserName, type=Type},
    %% 对方收到邀请
    net_helper:send2client(Account, Packet),
    
    %% 保存邀请者和邀请类型
    player_data:set_invite_info(InviteInfo, State).


%% 结束被邀请状态
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action({auto_stop, Reason}, State) ->
    InviteInfo = player_data:get_invite_info(State),
    case InviteInfo of
	#invite_data{invitor=Invitor} ->
    	    BasicData = player_data:get_player_basic_data(State),
	    UserName = basic_data:get_data(username, BasicData),
	    case Reason of
		logout ->		
		    sys_msg:send(Invitor, ?err_other_logout_invite_fail, [UserName]);
		_ ->
		    Account = player_data:get_account(State),
		    %% 通知客户端
		    net_helper:send2client(Account, #notify_cancel_invitation{}),
		    %% 通知邀请方
		    sys_msg:send(Invitor, ?err_other_busy_stop_invite, [UserName])		    
	    end;
	_ ->
	    ok
    end,
    %% 清空(邀请者，邀请类型)
    player_data:set_invite_info(undefined, State);
do_stop_action(_Param, State) ->
    Account = player_data:get_account(State),
    %% 通知客户端
    net_helper:send2client(Account, #notify_cancel_invitation{}),
    %% 清空(邀请者，邀请类型)
    player_data:set_invite_info(undefined, State).




%%%===================================================================
%%% 处理handle_cast
%%%===================================================================


%% 处理同意邀请
handle_cast({_Msg, 
	     #req_agree_invitation{invitor=InvitorStr, type=Type}}, 
	    State) ->
    Invitor = list_to_atom(InvitorStr),
    Invitee = player_data:get_account(State),    

    InviteInfo = player_data:get_invite_info(State),
    case InviteInfo of
	#invite_data{invitor=Invitor, 
		     invite_type=Type, 
		     invite_params=Params} ->
	    %% 结束被邀请状态
	    State1 = mutex_actions:call_stop_action(?action_type_be_invite, State),
	    NewState = do_agree_invitation(Invitor, Invitee, Type, Params, State1),
	    
	    %% 通知对方同意了
	    net_helper:send2client(Invitor, 
				   #notify_agree_invitation{agreer=atom_to_list(Invitee), type=Type}),
 	    {noreply, NewState};
	_ -> % 其它情况:离线邀请
	    NewState = do_agree_invitation(Invitor, Invitee, Type, [], State),
	    %% 删除离线邀请
	    db_friends:del_offline_invite(Invitee, Invitor, Type),
	    {noreply, NewState}
    end;

%% 处理拒绝邀请
handle_cast({_Msg, 
	     #req_disagree_invitation{invitor=InvitorStr, type=Type}}, 
	    State) ->
    Invitor = list_to_atom(InvitorStr),
    InviteInfo = player_data:get_invite_info(State),
    NewState = 
	case InviteInfo of
	    #invite_data{invitor=Invitor, 
			 invite_type=Type
			} ->
		mutex_actions:call_stop_action(?action_type_be_invite, State);
	    _ -> % 其它情况:离线邀请
		Invitee = player_data:get_account(State),    
		%% 删除重复的离线邀请
		db_friends:del_offline_invite(Invitee, Invitor, Type),
		State
	end,

    %% 消息提示
    case util:is_process_alive(Invitor) of
	true ->
	    sys_msg:send(Invitor, ?err_reject_invitation);
	_ ->
	    case Type of
		?ivt_make_friend ->
		    BasicData = player_data:get_basic_data(State),
		    UserName = basic_data:get_data(username, BasicData),    
		    
		    OfflineMsg = {?err_reject_add_friend, [UserName]},
		    sys_msg:save_offline_msgs(Invitor, [OfflineMsg]);
		_ ->
		    ok
	    end
    end,

    {noreply, NewState}.

%%%===================================================================
%%% Api
%%%===================================================================




%%%===================================================================
%%% 内部函数
%%%===================================================================
do_agree_invitation('', _Invitee, _Type, _Params, State) ->
    erlang:error({'Invitor can not be empty.'}),
    State;

%% 处理同意邀请(加好友邀请)
do_agree_invitation(Invitor, Invitee, ?ivt_make_friend, _Params, State) ->
    router:send(Invitee, make_friend, Invitor),
    State;

%% 处理同意邀请(进房屋邀请)
do_agree_invitation(Invitor, _Invitee, ?ivt_enter_house, _Params, State) ->
    player_change_scene:immediate_change_scene(Invitor, State);

%% 处理同意邀请(交易邀请)
do_agree_invitation(Invitor, Invitee, ?ivt_trade, _Params, 
		    MyState) ->
    MyPos = player_data:get_pos(MyState),
    MyScene = player_data:get_scene_name(MyState),

    Invitee = player_data:get_account(MyState),
    MyParam = Invitor, 
    YourParam = {Invitee, MyScene, MyPos},
    YourAccount = Invitor,
    mutex_actions:try_start_same_action(?action_type_trade, MyParam,
    					YourParam, YourAccount, 
    					MyState);

%% 处理同意邀请(进亲吻邀请)
do_agree_invitation(_Invitor, _Invitee, ?ivt_kiss, _Params, State) ->
    %%player_change_scene:immediate_change_scene(Invitor, State);
    %%io:format("~n~nkiss~n~n"),

    State;

do_agree_invitation(_Invitor, _Invitee, Type, _Params, State) ->
    erlang:error({'unhandle do agree invitation type' , Type}),
    State.
