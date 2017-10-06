%%% @author hongjx <hongjx@china-channel.com>
%%% @copyright (C) 2010, hongjx
%%% @doc
%%%    ��������
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
%%% ������ػص�����
%%%===================================================================
%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
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

%% ��������
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action({auto_stop, _Reason}, State) ->
    ID = case player_data:get_domestic_info(State) of
	     undefined -> 0;
	     N -> N			    
	 end,
    do_stop_action(ID, State);
%% ��������
do_stop_action(ID, State) ->
    Account = player_data:get_account(State),
    net_helper:send2client(Account, #notify_end_domestic{id=ID}),
    player_data:set_domestic_info(0, State).



%%%===================================================================
%%% ����handle_cast
%%%===================================================================
%% �������
handle_cast({_Msg, 
	     #req_start_domestic{}}, 
	    State) ->    
    NewState = mutex_actions:try_start_action(?action_type_domestic, undefined, State),
    {noreply, NewState};

%% �������
handle_cast({_Msg, 
	     #req_domestic_service{id=ID}}, 
	    State) ->    

    SceneName = player_data:get_scene_name(State),
    %% ȡ���ݵȼ� 
    HouseLevel = router:send(SceneName, get_house_level),

    %% ȡÿ����������� ��Ǯ��
    #domestic_price_tplt{money=OnePrice} = tplt:get_data(domestic_price_tplt, HouseLevel),

    %% ȡ������������ ����
    Domestic = tplt:get_data(domestic_service_tplt, ID),
    #domestic_service_tplt{house_clean=HouseClean} = Domestic,

    %% �������������
    CleanPoints = router:send(SceneName, calc_add_house_clean, HouseClean),

    %% ����Ҫ���ĵĽ�Ǯ
    CostMoney = OnePrice * CleanPoints,      
    Account = player_data:get_account(State),
    case can_domestic({Domestic, CostMoney, SceneName}, State) of
	{false, Error} -> 
	    sys_msg:send(Account, Error),
	    NewState = mutex_actions:call_stop_action(?action_type_domestic, ID, State),
	    {noreply, NewState};
	true ->
	    NewState = do_domestic({Domestic, CostMoney, SceneName}, State),
	    %% ��̨��Ǯ��Ʒ��¼
	    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, -CostMoney),
	    item_money_log:log_money(Account, domestic, MoneyInfo),

	    %% ÿ������
	    NNState = daily_task:inc_action_amount(?tat_domestic, NewState),
	    {noreply, NNState}
    end;

%% ��������
handle_cast({_Msg, 
	     #req_end_domestic{id=ID}}, 
	    State) -> 
    NewState = mutex_actions:call_stop_action(?action_type_domestic, ID, State),
    {noreply, NewState}.

%% �жϵ�ǰ״̬�ܷ�ʼ����
%% ����true | {false, Error}
can_domestic({_, CostMoney, SceneName}, State) ->
    MyMoney = player_coin:get_coin(?GAMECOIN, player_data:get_coin(State)),
    
    %% �жϿɷ���м���
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

%% ��ʼ����
do_domestic({#domestic_service_tplt{id=ID,
				    house_clean=HouseClean,
				    discount_rate=DisRate, 
				    money_rate=MoneyRate}, CostMoney, SceneName}, 
	    State) ->    
    Account = player_data:get_account(State),    

    %% �������
    NewMoneyRate = 
	case rand:uniform(100) =< DisRate of
	    true -> MoneyRate;
	    false -> 100
	end,

    %% ����Ӧ�۳����(��������)
    Cost = round(CostMoney * NewMoneyRate / 100),

    %% �����Ǯ
    NewCoin = player_coin:dec_coin2(?GAMECOIN, Cost, player_data:get_coin(State)),

    State1 = player_data:set_domestic_info(ID, State),
    NewState = player_data:set_coin(NewCoin, State1),    

    {AddHouseClean, HouseData} = router:send(SceneName, add_house_clean, HouseClean),

    %% ���ݿ����
    OpHouseData = {write, [HouseData]},
    OpMoney = {write, [NewCoin]},
    db_common:change([OpHouseData, OpMoney]),

    %% ֪ͨ�ͻ���
    Packet = #notify_domestic_result
      {id=ID, 
       cost_money=Cost, 
       money_rate=NewMoneyRate,
       change_house_clean=AddHouseClean
      },
    net_helper:send2client(Account, Packet),

    player_coin:send_coin(NewCoin),

    NewState.
