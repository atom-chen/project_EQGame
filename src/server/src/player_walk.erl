%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   玩家行走
%%% @end
%%% Created : 31 May 2010 by  <hjx>

-module(player_walk).


-include("packet_def.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("player_data.hrl").
-include("mutex_actions.hrl").
-include("sys_msg.hrl").

-behaviour(player_action).


-export([handle_cast/2, start/1]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).



-define(WALK_SPEED, 8). %% 定义一个玩家的速度


start(Account) ->
    [
     router:make_event_source(?msg_req_walk_for_pick_magic_box, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_walk_for_use_furniture, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_walk_for_body_action, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_start_walk, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_stop_walk, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_sync_position, Account, {self(), ?MODULE})
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

%% 玩家开始行走
do_start_action({CurrPos, DestPos, MoveType}, State) ->
    do_start_action({CurrPos, DestPos, MoveType, undefined}, State);

do_start_action({CurrPos, DestPos, MoveType, Plan},
	    #player_data{basic_data=BasicData,scene_name=SceneName}=State) ->
%%    data_helper:format("start_walk ~w\n", [{CurrPos, DestPos}]),
    Account = basic_data:get_data(account, BasicData),
    Packet = #notify_start_walk{account=Account, curr_pos=CurrPos, dest_pos=DestPos, move_type=MoveType},
    router:cast(undefined, SceneName, broadcast_all_players, Packet),
    State#player_data{curr_pos=CurrPos, dest_pos=DestPos, walk_plan=Plan}.


%% 结束行走
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action({auto_stop, ?action_type_walk}, #player_data{curr_pos=Pos}=State) ->
    State#player_data{curr_pos=Pos, dest_pos=Pos, walk_plan=undefined};
do_stop_action({auto_stop, _OtherReason}, #player_data{curr_pos=Pos}=State) ->
    do_stop_action(Pos, State);
do_stop_action(Pos, 
	    #player_data{basic_data=BasicData, 
			 scene_name=SceneName}=State) ->
%%    data_helper:format("stop_walk ~w\n", [Pos]),
    Account = basic_data:get_data(account, BasicData),
    Packet = #notify_stop_walk{account=Account,pos=Pos},
    router:cast(undefined, SceneName, broadcast_all_players, Packet),

    State#player_data{curr_pos=Pos, dest_pos=Pos, walk_plan=undefined}.
	

%%%===================================================================
%%% 处理handle_cast
%%%===================================================================

%% 收到玩家开始行走的请求
handle_cast({_Msg, #req_start_walk{curr_pos=CurrPos, dest_pos=DestPos, move_type=MoveType}}, State) ->
    NewState = mutex_actions:try_start_action(?action_type_walk, {CurrPos, DestPos, MoveType}, State),
    {noreply, NewState};

%% 行走完后使用家具
handle_cast({_Msg, #req_walk_for_use_furniture{curr_pos=CurrPos,dest_pos=DestPos, move_type=MoveType,
					       instance_id=InstID, 
					       function_id=FunID}}, State) ->
    FurPos = DestPos,
    Plan = {use_furniture, InstID, FunID, FurPos},

    NewState = mutex_actions:try_start_action(?action_type_walk, {CurrPos, DestPos, MoveType, Plan}, State),
    {noreply, NewState};

%% 行走完后捡宝箱
handle_cast({_Msg, #req_walk_for_pick_magic_box{curr_pos=CurrPos,dest_pos=DestPos, move_type=MoveType,
					       box_id=InstID}}, State) ->
    Plan = {pick_magic_box, InstID},

    NewState = mutex_actions:try_start_action(?action_type_walk, {CurrPos, DestPos, MoveType, Plan}, State),
    {noreply, NewState};

%% 行走完后做双人动作
handle_cast({_Msg, #req_walk_for_body_action{curr_pos=CurrPos, dest_pos=DestPos, move_type=MoveType,
					       my_action=MyAction, you_action=YouAction,
					    you_account=YouAccount, you_pos=YouPos}}, State) ->
    Plan = {body_action2, MyAction, YouAction, list_to_atom(YouAccount), YouPos},
    NewState = mutex_actions:try_start_action(?action_type_walk, {CurrPos, DestPos, MoveType, Plan}, State),
    {noreply, NewState};

%% 收到客户端同步位置的请求
handle_cast({_Msg,#req_sync_position{pos=Pos}},State) ->
    {noreply, State#player_data{curr_pos=Pos}};

%% 收到玩家结束行走的请求
handle_cast({_Msg, #req_stop_walk{pos=Pos}}, 
	    #player_data{walk_plan=WalkPlan,
			 dest_pos=DestPos}= State) ->
    State1 = mutex_actions:call_stop_action(?action_type_walk, Pos, State),
    
    case (WalkPlan =/= undefined) and position_equal(Pos, DestPos) of
	true ->
	    case WalkPlan of
		{use_furniture, InstID, FunID, FurPos} ->
		    State2 = mutex_actions:try_start_action(?action_type_use_furniture, 
							    {InstID, FunID, FurPos}, State1),
		    {noreply, State2#player_data{curr_pos=Pos, dest_pos=Pos, walk_plan=undefined}};
		{body_action2, MyAction, YouAction, YouAccount, YouPos} ->
		    Account = player_data:get_account(State),
		    NState = 
			case router:send(YouAccount, get_pos) of
			    YouPos ->
				Account = player_data:get_account(State),
				MyParam = {MyAction, YouAccount},
				YourParam = {YouAction, Account},
				mutex_actions:try_start_same_action(?action_type_body_action, MyParam,
								    YourParam, YouAccount, 
								    State);
			    _ -> 
				sys_msg:send(Account, ?err_action2_other_not_at_pos),
				sys_msg:send(YouAccount, ?err_action2_self_not_at_pos),
				State
			end,
		    {noreply, NState#player_data{curr_pos=Pos, dest_pos=Pos, walk_plan=undefined}};
		{pick_magic_box, InstID} ->
		    NewState = player_magic_box:pick_magic_box(InstID, Pos, State1),
		    {noreply, NewState#player_data{curr_pos=Pos, dest_pos=Pos, walk_plan=undefined}}
	    end;
	false ->
	    {noreply, State1}
    end.



%%%===================================================================
%%% 内部函数
%%%===================================================================
%% 判断坐标点是否一样
position_equal(#point{x=X1, y=Y1,z= Z1}, #point{x=X2, y=Y2, z=Z2}) ->
    (trunc(X1) =:= trunc(X2)) and
    (trunc(Y1) =:= trunc(Y2)) and
    (trunc(Z1) =:= trunc(Z2)).


%% %% 发送当前玩家数据给场景中的其他玩家
%% send_to_other_players(OperateType, Data) ->
%%     gen_scene:handle_player_message(OperateType, Data).
