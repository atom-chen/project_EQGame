%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% ��������
%%% @end
%%% Created :  8 Jul 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(house_transaction).

-include("packet_def.hrl").
-include("router.hrl").
-include("tplt_def.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").
-include("enum_def.hrl").
-include("mutex_actions.hrl").

%% API
-export([start/1, handle_cast/2]).
-export([test/0]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).

start(Account) ->
    [
     router:make_event_source(?msg_req_start_buy_house, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_end_buy_house, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_buy_house, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_buy_house_replace, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_buy_house_add, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% ������ػص�����
%%%===================================================================
%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
get_action_register_msgs(_Account, _Pid) ->
    [].

can_start_action(_Param, _State) ->
    true.

do_start_action(replace_house, State) ->
    Account = player_data:get_account(State),
    net_helper:send2client(Account, #notify_buy_house{result=?bhr_replace}),
    State;
do_start_action(_Param, State) ->
    Account = player_data:get_account(State),
    %% ֪ͨ��ʼ
    net_helper:send2client(Account, #notify_start_buy_house{}),

    State.


%% ��������
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action({auto_stop, _Reason}, State) ->
    do_stop_action(stop, State);

%% ��������
do_stop_action(_Param, State) ->
    Account = player_data:get_account(State),
    %% ֪ͨ����
    net_helper:send2client(Account, #notify_end_buy_house{}),
    State.

%%%===================================================================
%%% Handle cast
%%%===================================================================
%% ��ʼ
handle_cast({_Msg, #req_start_buy_house{}}, State) ->
    Account = player_data:get_account(State),

    HousePlot = house_plot:get(Account),
    NewState = 
	case house_plot:get_empty_plot(HousePlot) of
	    0 ->
		mutex_actions:try_start_action(?action_type_buy_house, undefined, State);
	    _ ->
		%% ֪ͨ��ʼ
		net_helper:send2client(Account, #notify_start_buy_house{}),
		State
	end,
    {noreply, NewState};	
%% ����
handle_cast({_Msg, #req_end_buy_house{}}, State) ->
    NewState = 
	case mutex_actions:contains_action(?action_type_buy_house, State) of
	    true->
		mutex_actions:call_stop_action(?action_type_buy_house, undefined, State);
	    false ->
		Account = player_data:get_account(State),
		%% ֪ͨ����
		net_helper:send2client(Account, #notify_end_buy_house{}),
		State
	end,
    {noreply, NewState};
%% �����ȡ���������б�
handle_cast({#msg{event=?msg_req_buy_house}, #req_buy_house{id=Id}}, State) ->
    NState = 
	case can_buy_house(Id, State) of
	    true -> 
		do_buy_house(State),
		State;
	    {false, Reason} ->
		Account = player_data:get_account(State),	    
		sys_msg:send(Account, Reason),
		State		       
	end,
    {noreply, NState};
handle_cast({#msg{event=?msg_req_buy_house_replace}, #req_buy_house_replace{id=Id, type=Type}}, State) ->
    NState = 
	case can_buy_house(Id, State) of
	    true -> 
		do_buy_house_replace(Id, Type, State);
	    {false, Reason} ->
		Account = player_data:get_account(State),	    
		sys_msg:send(Account, Reason),
		State		       
	end,
    {noreply, NState};
handle_cast({#msg{event=?msg_req_buy_house_add}, #req_buy_house_add{id=Id}}, State) ->
    NState = 
	case can_buy_house(Id, State) of
	    true -> 
		do_buy_house_add(Id, State);
	    {false, Reason} ->
		Account = player_data:get_account(State),	    
		sys_msg:send(Account, Reason),
		State		       
	end,
    {noreply, NState}.
%%%===================================================================
%%% API
%%%===================================================================

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% ��֤�Ƿ��������������
can_do(HTData, State) ->
    TpltLevel = HTData#house_transaction_tplt.level,
    PlayerLevel = player_level:get_level(State),
    NeedCoin = HTData#house_transaction_tplt.price,
    NeedDecoration = HTData#house_transaction_tplt.need_decoration,
    CostFlower = HTData#house_transaction_tplt.cost_flower,
    PlayerCoin = player_data:get_coin(State),
    Coin = get_coin(HTData, PlayerCoin),
    PlayerFlower = player_data:get_flower(State),
    Account = player_data:get_account(State),
    PlayerDecoration = player_decoration:get_decoration(Account),

    action:can_do([{get_action_by_level(), {TpltLevel, PlayerLevel}, err_level},
		   {get_action_by_coin(), {NeedCoin, Coin}, err_coin},
		   {get_action_by_coin(), {NeedDecoration, PlayerDecoration}, 
		     ?err_decoration_not_enough_buy_house},
		   {get_action_by_coin(), {CostFlower, PlayerFlower}, 
		     ?err_flower_not_enough_buy_house}
		  ]).

%% �滻����
-spec replace_house(atom(), integer(), tuple(), tuple()) -> tuple().
replace_house(Account, Type, State, HTData) ->
    %% �۳����
    NState = rec_coin(HTData, State),

    SceneName = scene_manager:get_scene_name(Account),
    %% ��ʼ�滻����
    {OldInstID, HouseData} = router:send(SceneName, start_replace_house, {Account, Type, HTData}),
    %% ��ȡ���
    PlayerCoin = player_data:get_coin(NState),
    {NState, OldInstID, HouseData, PlayerCoin}.

%% ��ȡ����������ģ���
-spec get_house_transaction_data(integer()) -> tuple() | empty.
get_house_transaction_data(Id)
  when is_integer(Id) ->
    tplt:get_data(house_transaction_tplt, Id).

%% ��֤���
-spec get_action_by_coin() -> fun(({_,_}) -> boolean()).
get_action_by_coin() ->
    fun({NeedCoin, PlayerCoin}) ->
	    PlayerCoin >= NeedCoin
    end.

%% ��֤�ȼ�
-spec get_action_by_level() -> fun(({_,_}) -> boolean()).
get_action_by_level() ->
    fun({TpltLevel, PlayerLevel}) ->
	    PlayerLevel >= TpltLevel
    end.

%% �۳����
-spec rec_coin(tuple(), tuple()) -> tuple().
rec_coin(HTData, State) ->
    PayType = HTData#house_transaction_tplt.pay_type,
    NeedCoin = HTData#house_transaction_tplt.price,
    player_coin:dec_coin2(PayType, NeedCoin, State).

%% �۳��ʻ�
dec_flower(HTData, State) ->
    Cost = HTData#house_transaction_tplt.cost_flower,
    player_flower_egg:dec_flower(Cost, State).


%% ��ȡ�������
-spec get_coin(tuple(), tuple()) -> integer().
get_coin(HTData, PlayerCoin) ->
    PayType = HTData#house_transaction_tplt.pay_type,
    case PayType =:= ?GAMECOIN of
	true ->
	    PlayerCoin#player_coin.gamecoin;
	_ ->
	    PlayerCoin#player_coin.eqcoin
    end.



can_buy_house(Id, State) ->
    HTData = get_house_transaction_data(Id),
    ActionCanDo = can_do(HTData, State),
    case ActionCanDo of
	true ->
	    true;
	{false, err_level} ->
	    {false, ?err_not_enough_level};
	{false, err_coin} ->
	    {false, ?err_not_enough_game_coin};
	{false, Err} ->
	    {false, Err}
    end.

do_buy_house(State) ->
    Account = player_data:get_account(State),
    HousePlot = house_plot:get(Account),
    case house_plot:get_empty_plot(HousePlot) of
	0 ->
	    case mutex_actions:contains_action(?action_type_buy_house, State) of
		true ->
		    net_helper:send2client(Account, #notify_buy_house{result=?bhr_replace});
		false ->
		    mutex_actions:try_start_action(?action_type_buy_house, replace_house, State)
	    end;
	_ ->
	    net_helper:send2client(Account, #notify_buy_house{result=?bhr_add})
    end.

do_buy_house_replace(Id, Type, State) ->
    Account = player_data:get_account(State),
    HTData = get_house_transaction_data(Id),
    HouseId = HTData#house_transaction_tplt.house_id,
    NState = do_replace_house(Account, Type, HTData, State),
    do_money_log(Account, HouseId, HTData),
    do_broadcast(HTData, NState),
    NState.

do_buy_house_add(Id, State) ->
    Account = player_data:get_account(State),
    HousePlot = house_plot:get(Account),
    HTData = get_house_transaction_data(Id),
    HouseId = HTData#house_transaction_tplt.house_id,
    PlotId = house_plot:get_empty_plot(HousePlot),
    NState = do_add_house(Account, HouseId, PlotId, HTData, HousePlot, State),
    do_money_log(Account, HouseId, HTData),
    do_broadcast(HTData, State),
    NState.

%% ����ʹ�ü�¼
do_money_log(Account, HouseId, HTData) ->
    #house_transaction_tplt{pay_type=PayType, price=Price} = HTData,
    MoneyInfo = item_money_log:get_money_info(PayType, -Price),
    item_money_log:log_money(Account, buy_house, MoneyInfo, HouseId).

%% ϵͳ����
do_broadcast(HTData, State) ->
    BroadcastID = HTData#house_transaction_tplt.broadcast_id,
    case BroadcastID > 0 of
	true ->
	    BasicData = player_data:get_player_basic_data(State),
	    UserName = basic_data:get_data(username, BasicData),
	    HouseName = binary_to_list(HTData#house_transaction_tplt.name),			
	    %% ����ϵͳ�㲥
	    sys_broadcast:send(BroadcastID, [UserName, HouseName]);		    
	_ ->
	    ok
    end.

do_add_house(Account, HouseId, PlotId, HTData, HousePlot, State) ->
    %% �۳����
    NState = rec_coin(HTData, State),
    HouseData = house:get_house_data(Account, HouseId),
    NHousePlot = house_plot:replace(HousePlot, PlotId, HouseData),

    %% ��ȡ���
    PlayerCoin = player_data:get_coin(NState),

    %% ���ʻ�ֵ 
    NewState = dec_flower(HTData, NState),

    %% ������������Ϣ
    HouseRandGarbage = house_magic_box:init_house_rand_garbage(HouseData#house_data.instance_id),

    db_common:write([NHousePlot, PlayerCoin, HouseRandGarbage]),
    net_helper:send2client(Account, #notify_buy_new_house_success{}),
    sys_msg:send(Account, ?msg_house_transaction_success),
    NewState.

do_replace_house(Account, Type, HTData, State) ->
    %% �߳�����Ŀ���
    SceneName = scene_manager:get_scene_name(Account),
    player:kick_guest(Account, SceneName, ?err_player_replacing_house),

    {NState, OldInstID, NHouseData, NPlayerCoin} = 
	replace_house(Account, Type, State, HTData),

    %% ���ʻ�ֵ 
    NewState = dec_flower(HTData, NState),

    %% ������������Ϣ
    HouseRandGarbage = house_magic_box:init_house_rand_garbage(NHouseData#house_data.instance_id),


    %% ��������
    db_common:change([{write, [NPlayerCoin, NHouseData, HouseRandGarbage]},
		     {delete, [{house_magic_box, OldInstID},
			      {house_rand_garbage, OldInstID}]}]),

    net_helper:send2client(Account, #notify_buy_new_house_success{}),
    sys_msg:send(Account, ?msg_house_transaction_success),
    NewState.


%%%===================================================================
%%% Test case
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").

test() ->
    router:cast(test1, ?msg_req_buy_house, #req_buy_house{id=2}).
