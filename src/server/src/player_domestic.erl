%%% @author hongjx <hongjx@china-channel.com>
%%% @copyright (C) 2010, hongjx
%%% @doc
%%%    家政服务
%%% @end
%%% Created : 20 Jul 2010 by hongjx <hongjx@china-channel.com>

-module(player_domestic).
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


start(Account) ->
    [
     router:make_event_source(?msg_req_start_domestic, Account, {self(), ?MODULE})
    ].



%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(Account, PID) ->
    [
     router:make_event_source(?msg_req_domestic_service, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_end_domestic, Account, {PID, ?MODULE})
    ].

can_start_action(_Param, _State) ->
    true.

do_start_action(_Param, State) ->
    Account = player_data:get_account(State),
    net_helper:send2client(Account, #notify_start_domestic{}),    
    State.

%% 结束动作
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action({auto_stop, _Reason}, State) ->
    ID = case player_data:get_domestic_info(State) of
	     undefined -> 0;
	     N -> N			    
	 end,
    do_stop_action(ID, State);
%% 结束动作
do_stop_action(ID, State) ->
    Account = player_data:get_account(State),
    net_helper:send2client(Account, #notify_end_domestic{id=ID}),
    player_data:set_domestic_info(0, State).



%%%===================================================================
%%% 处理handle_cast
%%%===================================================================
%% 请求家政
handle_cast({_Msg, 
	     #req_start_domestic{}}, 
	    State) ->    
    NewState = mutex_actions:try_start_action(?action_type_domestic, undefined, State),
    {noreply, NewState};

%% 请求家政
handle_cast({_Msg, 
	     #req_domestic_service{id=ID}}, 
	    State) ->    

    SceneName = player_data:get_scene_name(State),
    %% 取房屋等级 
    HouseLevel = router:send(SceneName, get_house_level),

    %% 取每点清洁数消耗 金钱数
    #domestic_price_tplt{money=OnePrice} = tplt:get_data(domestic_price_tplt, HouseLevel),

    %% 取家政所关联的 属性
    Domestic = tplt:get_data(domestic_service_tplt, ID),
    #domestic_service_tplt{house_clean=HouseClean} = Domestic,

    %% 算可增加清洁点数
    CleanPoints = router:send(SceneName, calc_add_house_clean, HouseClean),

    %% 算需要消耗的金钱
    CostMoney = OnePrice * CleanPoints,      
    Account = player_data:get_account(State),
    case can_domestic({Domestic, CostMoney, SceneName}, State) of
	{false, Error} -> 
	    sys_msg:send(Account, Error),
	    NewState = mutex_actions:call_stop_action(?action_type_domestic, ID, State),
	    {noreply, NewState};
	true ->
	    NewState = do_domestic({Domestic, CostMoney, SceneName}, State),
	    %% 后台金钱物品记录
	    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, -CostMoney),
	    item_money_log:log_money(Account, domestic, MoneyInfo),

	    %% 每日任务
	    NNState = daily_task:inc_action_amount(?tat_domestic, NewState),
	    {noreply, NNState}
    end;

%% 结束家政
handle_cast({_Msg, 
	     #req_end_domestic{id=ID}}, 
	    State) -> 
    NewState = mutex_actions:call_stop_action(?action_type_domestic, ID, State),
    {noreply, NewState}.

%% 判断当前状态能否开始动作
%% 返回true | {false, Error}
can_domestic({_, CostMoney, SceneName}, State) ->
    MyMoney = player_coin:get_coin(?GAMECOIN, player_data:get_coin(State)),
    
    %% 判断可否进行家政
    CanDo = 
	case MyMoney < CostMoney of
	    true -> {false, ?err_domestic_not_enough_coin};
	    false -> 
		IsFull = router:send(SceneName, house_clean_is_full),
		case IsFull of
		    true -> {false, ?err_house_clean_full};
		    false ->
			true
		end
	end,   
    CanDo.

%% 开始动作
do_domestic({#domestic_service_tplt{id=ID,
				    house_clean=HouseClean,
				    discount_rate=DisRate, 
				    money_rate=MoneyRate}, CostMoney, SceneName}, 
	    State) ->    
    Account = player_data:get_account(State),    

    %% 计算打折
    NewMoneyRate = 
	case rand:uniform(100) =< DisRate of
	    true -> MoneyRate;
	    false -> 100
	end,

    %% 计算应扣除金额(四舍五入)
    Cost = round(CostMoney * NewMoneyRate / 100),

    %% 计算金钱
    NewCoin = player_coin:dec_coin2(?GAMECOIN, Cost, player_data:get_coin(State)),

    State1 = player_data:set_domestic_info(ID, State),
    NewState = player_data:set_coin(NewCoin, State1),    

    {AddHouseClean, HouseData} = router:send(SceneName, add_house_clean, HouseClean),

    %% 数据库操作
    OpHouseData = {write, [HouseData]},
    OpMoney = {write, [NewCoin]},
    db_common:change([OpHouseData, OpMoney]),

    %% 通知客户端
    Packet = #notify_domestic_result
      {id=ID, 
       cost_money=Cost, 
       money_rate=NewMoneyRate,
       change_house_clean=AddHouseClean
      },
    net_helper:send2client(Account, Packet),

    player_coin:send_coin(NewCoin),

    NewState.
