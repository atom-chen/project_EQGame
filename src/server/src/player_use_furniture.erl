%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   使用家具
%%% @end
%%% Created :  1 Jun 2010 by  <hjx>

-module(player_use_furniture).

-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("player_data.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").

-behaviour(player_action).


-export([handle_cast/2, handle_call/3, start/1]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).




start(Account) ->
    [
     router:make_event_target(furniture_disapear, Account, {self(), ?MODULE}),
     router:make_event_target(player_leave_furniture, Account, {self(), ?MODULE})
    ].



%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(Account, PID) ->
    [
     router:make_event_source(?msg_req_use_mutli_furni, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_stop_using_furniture, Account, {PID, ?MODULE})
    ].

%% 判断当前状态能否开始动作
%% 返回 true | {false, Reason}
can_start_action({_FurniVec, _UsedFuncId}=_Param, _State) ->
    true;
can_start_action({InstId, FuncId, _Pos}=_Param,
		  #player_data{account=Account, 
			       player_property=PlayerProperty,
			       scene_name=SceneName, 
			       basic_data=BasicData} = _State) ->
    %%data_helper:format("\n call can create action use furniture.\n"),
    Sex = BasicData#player_basic_data.sex,
    ActionCanDo = router:send(SceneName, can_use_furniture, {Account, PlayerProperty, 
							     InstId, FuncId, Sex}),
    can_create_action_1(ActionCanDo).

can_create_action_1(ActionCanDo) ->
    case ActionCanDo of
	true ->
	    true;
	{false, err_permission} -> 
	    {false, ?err_furniture_not_permission};
	{false, err_sex} ->
	    {false, ?err_furniture_sex};
	{false, err_position, {Furniture, UsedAccount}} ->
	    can_create_action_2(Furniture, UsedAccount);
	{false, err_property} ->
	    {false, ?err_furniture_property_is_full}
    end.

can_create_action_2(Furniture, UsedAccount) ->
    MaxUsePlayer = Furniture#house_furniture.max_use_player,
    case MaxUsePlayer of
	1 ->
	    UsedUserName = player:get_user_name(UsedAccount),
	    {false, ?err_furniture_has_used, [UsedUserName]};
	_ ->
	    {false, ?err_furniture_not_position}
    end.


%% 开始
do_start_action({FurniVec, UsedFurniFuncId} = _Param, 
		#player_data{account=Account, 
			     scene_name=SceneName} = State) when is_list(FurniVec) ->

    FurniInteractTplt = tplt:get_data(furni_interact_tplt, UsedFurniFuncId),
    Status = FurniInteractTplt#furni_interact_tplt.status,
    router:send(SceneName, control_mutli_furniture, {Account, Status, FurniVec}), 
    ActionType = FurniInteractTplt#furni_interact_tplt.action_type,
    %%io:format("action type: ~p~n~n~n", [ActionType]),
    %% 每日任务
    NState = daily_task:inc_action_amount(ActionType, State),		
    NState;
do_start_action({InstId, FuncId, Pos}=_Param, #player_data{account=Account, scene_name=SceneName} = State) ->
    %%data_helper:format("\n start_use furniture \n"),
    {UsedFurniInstId, UsedFurniFuncId, UsedFurniIndex} = 
	router:send(SceneName, use_furniture, {Account, InstId, FuncId}),

    FurniInteractTplt = tplt:get_data(furni_interact_tplt, UsedFurniFuncId),
    ActionType = FurniInteractTplt#furni_interact_tplt.action_type,
    %%io:format("action type: ~p~n~n~n", [ActionType]),
    %% 每日任务
    NState = daily_task:inc_action_amount(ActionType, State),		


    NState#player_data{used_furni_inst_id = UsedFurniInstId, 
		      used_furni_func_id = UsedFurniFuncId, 
		      used_furni_index   = UsedFurniIndex,
		      used_furni_pos     = Pos}.

%% 结束
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action({auto_stop, logout}, #player_data{account=Account,
				       scene_name=SceneName,
				       used_furni_inst_id=UsedFurniInstId,
				       used_furni_func_id=UsedFurniFuncId}=State) ->
    %%data_helper:format("stop use furniture undefined~n"),
    IsExistScene = util:is_process_alive(SceneName),
    case (UsedFurniInstId > 0) and IsExistScene of
	true ->
	    router:send(SceneName, stop_use_furniture, {Account, UsedFurniInstId, UsedFurniFuncId});
	_ ->
	    ok
    end,
    State#player_data{used_furni_inst_id=0, 
		      used_furni_func_id=0, 
		      used_furni_index=0,
		      used_furni_pos=#point{x=0.0,y=0.0,z=0.0}};
do_stop_action({auto_stop, _OtherReason}, State) ->
    do_stop_action(stop_use_furniture, State);    
do_stop_action(stop_use_furniture, #player_data{account=Account,
						scene_name=SceneName,
						used_furni_inst_id=UsedFurniInstId,
						used_furni_func_id=UsedFurniFuncId,
						used_furni_pos=Pos}=State) ->
    %%data_helper:format("\n stop_use furniture \n"),
    NState = 
	case UsedFurniInstId > 0 of
	    true ->
		PropertyId = router:send(SceneName, stop_use_furniture, 
					 {Account, UsedFurniInstId, UsedFurniFuncId, Pos}),
		case PropertyId > 0 of
		    true ->
			%% 保存玩家属性
			player_property_by_action:update(?msg_player_property_by_furniture, 
							 PropertyId,
							 State);
		    false ->
			ok
		end;
	    _ ->
		State
	end,
    NState#player_data{used_furni_inst_id=0, 
		       used_furni_func_id=0, 
		       used_furni_index=0,
		       used_furni_pos=#point{x=0.0,y=0.0,z=0.0}};
do_stop_action(furniture_disapear, State) ->
    State#player_data{used_furni_inst_id=0, 
		       used_furni_func_id=0, 
		       used_furni_index=0,
		       used_furni_pos=#point{x=0.0,y=0.0,z=0.0}}.

%%%===================================================================
%%% 处理handle_cast
%%%===================================================================
handle_cast({#msg{event=?msg_req_use_mutli_furni}, #req_use_mutli_furni{furni_vec=FurniVec}}, 
	    #player_data{used_furni_func_id=UsedFurniFuncId, account=Account, scene_name=SceneName}=State) ->
    FurniInteractTplt = tplt:get_data(furni_interact_tplt, UsedFurniFuncId),
    Status = FurniInteractTplt#furni_interact_tplt.status,
    router:send(SceneName, control_mutli_furniture, {Account, Status, FurniVec}),
    {noreply, State};

%% 停止使用家具
handle_cast({#msg{event=?msg_req_stop_using_furniture}, 
	     #req_stop_using_furniture{account=_Account}}, State) ->
    NewState = mutex_actions:call_stop_action(?action_type_use_furniture, stop_use_furniture,  State),
    {noreply, NewState}.
%% 家具消失
handle_call(#msg{event=furniture_disapear}, _From, 
	    #player_data{used_furni_func_id=UsedFurniFuncId,
			used_furni_pos=Pos}=State) ->
    NewState = mutex_actions:call_stop_action(?action_type_use_furniture, furniture_disapear,  State),
    {reply, {UsedFurniFuncId, Pos}, NewState};

handle_call({#msg{event=player_leave_furniture}, NewPlayerProperty}, _From, State) ->
    {reply, ok, State#player_data{player_property=NewPlayerProperty, 
				  used_furni_inst_id=0, 
				  used_furni_func_id=0, 
				  used_furni_index=0,
				  used_furni_pos=#point{x=0.0,y=0.0,z=0.0}}}.


%%%===================================================================
%%% 内部函数
%%%===================================================================
