%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   ��ҽ���
%%% @end
%%% Created : 27 May 2010 by  <hjx>

-module(player_trade).

-include("mutex_actions.hrl").
-include("packet_def.hrl").
-include("router.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").
-include("enum_def.hrl").
-include("tplt_def.hrl").

-behaviour(player_action).

-record(trade_data, {
	  all_sured=false,                         % ˫����ȷ�Ϲ�
	  trade_player,                            % �������
	  trade_money,                             % ���׽��
	  trade_bag,                               % ������	  
	  trade_sured                              % ����ȷ��	  
	 }).


-export([start/1]).
-export([handle_cast/2, handle_call/3]).
-export([do_start_action/2, do_stop_action/2,
	 can_start_action/2, get_action_register_msgs/2]).


start(_Account) ->
    [
    ].


%%%===================================================================
%%% ������ػص�����
%%%===================================================================

%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
get_action_register_msgs(Account, PID) ->
    [
     router:make_event_source(notify_post_trade, Account, {PID, ?MODULE}),
     router:make_event_target(post_trade, Account, {PID, ?MODULE}),
     router:make_event_target(other_sured, Account, {PID, ?MODULE}),
     router:make_event_target(cancel_sured, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_update_trade_money, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_put_trade_item, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_unput_trade_item, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_swap_trade_item, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_cancel_trade, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_sure_trade, Account, {PID, ?MODULE})
    ].


%% �жϵ�ǰ״̬���ܷ���н���
%% ����true | {false, Error}
can_start_action({_TradePlayer, YourScene, YourPos}, MyState) ->
    MyPos = player_data:get_pos(MyState),
    MyScene = player_data:get_scene_name(MyState),
    CanCreate = can_create_trade_2(YourPos, YourScene, MyPos, MyScene),
    CanCreate.

%% ��ʼ����
do_start_action({TradePlayer, _YourScene, _YourPos}, State) ->
    do_start_action(TradePlayer, State);
do_start_action(TradePlayer, State) ->
    Account = player_data:get_account(State),
    %% ��ս�����, ���money
    TradeInfo = #trade_data{trade_player=TradePlayer, trade_money=0, 
			    trade_bag=pack:new(common_def:get_val(trade_grid_count)), 
			    trade_sured=false,
			    all_sured=false},

    NewState = player_data:set_trade_info(TradeInfo, State),

    %% ֪ͨ��ʼ����
    net_helper:send2client(Account, 
			   #notify_start_trade{account=atom_to_list(TradePlayer)}),

    NewState.

%% ����ʧ��

%% ����ʧ��, ע: ʹ��{fail} ������fail, ��ֹ��С�����˺�ͬ��
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action({fail}, State) ->
    do_trade_fail(State);
%% ���׳ɹ�
do_stop_action({succeed}, State) ->
    Account = player_data:get_account(State),
    
    NewState = player_data:set_trade_info(undefined, State),
    %% ֪ͨ���׳ɹ�    
    net_helper:send2client(Account, #notify_trade_success{}), 
    NewState;

do_stop_action({auto_stop, _Reason}, State) ->
    %%io:format("call auto stop ~p~n~n~n", [player_data:get_actions(State)]),
    Stoper = player_data:get_account(State),
    do_stop_action(Stoper, State);
%% ��������(������Ҫ��ȡ�����׵ģ���Ҫ�öԷ�Ҳȡ������)
%% @���� Stoper Ҫ��ֹͣ���׵���
do_stop_action(Stoper, State) ->
    case player_data:get_trade_info(State) of
	undefined ->
	    %%io:format("do stop undefined ~p~n~n~n", [player_data:get_account(State)]),
	    State;
	_ ->
	    %%io:format("do stop ok ~p~n~n~n", [player_data:get_account(State)]),
	    Account = player_data:get_account(State),
	    do_cancel_trade(Stoper, Account, State)
    end.



%%%====================================================================
%%% ����handle_call
%%% ====================================================================
%% ���������˽���
handle_call({#msg{event=post_trade}, YourState}, _From, 
	    MyState) ->
    case do_post_trade(YourState, MyState) of
	{YourNewState, MyNewState} ->    
	    {reply, YourNewState, MyNewState}
	%% _ -> 
	%%     {reply, YourState, MyState}
    end;
%% ����������ȷ������
handle_call(#msg{event=other_sured}, _From, 
	    State) ->
    Account = player_data:get_account(State),
    TradeInfo = player_data:get_trade_info(State),
    
    AllSured = TradeInfo#trade_data.trade_sured,
    NewTradeInfo = TradeInfo#trade_data{all_sured=AllSured},
    NewState = player_data:set_trade_info(NewTradeInfo, State),

    %% ˫����ȷ�Ϻ󣬾����õ���ʱ
    case AllSured of
	true ->
	    sys_time:subscribe(?dt_interval, 
			       Account, 
			       common_def:get_val(post_trade_second),
			       notify_post_trade, 
			       1);
	false ->
	    ok
    end,
    {reply, AllSured, NewState};
%% ȡ��ȷ��״̬
handle_call(#msg{event=cancel_sured}, _From, 
	    State) ->

    TradeInfo = player_data:get_trade_info(State),
    NewTradeInfo = TradeInfo#trade_data{trade_sured=false},
    NewState = player_data:set_trade_info(NewTradeInfo, State),
    {reply, ok, NewState}.


%%%====================================================================
%%% ����handle_cast
%%%====================================================================
%% �ύ����(˫��ȷ����3��)
handle_cast({#msg{event=notify_post_trade}, _TimerRef}, State) ->
    TradeInfo = player_data:get_trade_info(State),
    #trade_data{trade_player=TradePlayer} = TradeInfo,

    %% handle_call��һ��
    NewState = router:send(TradePlayer, post_trade, State),
    {noreply, player_data:set_trade_info(undefined, NewState)};
%% ����������Ŀ�е���Ʒ
handle_cast({_Msg, 
	     #req_swap_trade_item{index1=Idx1, index2=Idx2}}, 
	    State) ->
    Account = player_data:get_account(State),
    TradeInfo = player_data:get_trade_info(State),
    #trade_data{trade_player=TradePlayer, trade_bag=TradeBag, all_sured=AllSured} = TradeInfo,
    case AllSured of
	true -> 
	    sys_msg:send(Account, ?err_trade_all_sured),
	    {noreply, State};
	false -> 
	    %% ������������
	    NewBag = pack:swap(Idx1, Idx2, TradeBag),
	    NewTradeInfo = TradeInfo#trade_data{trade_bag=NewBag, trade_sured=false},
	    NewState = player_data:set_trade_info(NewTradeInfo, State),
	    router:send(TradePlayer, cancel_sured),
	    
	    %% ֪ͨ����
	    net_helper:send2client(Account, #notify_swap_trade_item{index1=Idx1, index2=Idx2}),
	    %% ֪ͨ�Է�
	    net_helper:send2client(TradePlayer, 
				   #notify_other_swap_trade_item{index1=Idx1, index2=Idx2}),
	    {noreply, NewState}
    end;
%% ���½��׽��
handle_cast({_Msg, 
	     #req_update_trade_money{money=Money}}, 
	    State) ->
    Account = player_data:get_account(State),
    TradeInfo = player_data:get_trade_info(State),
    #trade_data{trade_player=TradePlayer, all_sured=AllSured} = TradeInfo,

    case AllSured of
	true -> 
	    sys_msg:send(Account, ?err_trade_all_sured),
	    {noreply, State};
	false -> 
	    MyMoney = player_coin:get_coin(?GAMECOIN, player_data:get_coin(State)),
	    %% ������׽���������money�������ϵ�ȫ��money�ŵ�������, ���С��0, ��ȡ0
	    TradeMoney = 
		case Money =< MyMoney of
		    true -> 
			case Money < 0 of
			    true -> 0;
			    false -> Money
			end;
		    false -> MyMoney
		end,
	    
	    NewTradeInfo = TradeInfo#trade_data{trade_money=TradeMoney, trade_sured=false},	    
	    NewState = player_data:set_trade_info(NewTradeInfo, State),
    	    router:send(TradePlayer, cancel_sured),

	    %% ֪ͨ����
	    net_helper:send2client(Account, #notify_update_trade_money{money=TradeMoney}),
	    %% ֪ͨ�Է�
	    net_helper:send2client(TradePlayer, #notify_other_update_trade_money{money=TradeMoney}),
	    {noreply, NewState}
    end;	    
%% ����Ʒ���ڽ�����
handle_cast({_Msg, 
	     #req_put_trade_item{packet_index=BagIdx, trade_index=TradeIdx}}, 
	    State) ->
    Account = player_data:get_account(State),
    TradeInfo = player_data:get_trade_info(State),
    #trade_data{trade_player=TradePlayer, trade_bag=TradeBag, all_sured=AllSured} = TradeInfo,
    case AllSured of
	true -> 
	    sys_msg:send(Account, ?err_trade_all_sured),
	    {noreply, State};
	false -> 
	    %% ȡ�ñ�����Ʒ
	    Bag = get_bag(State),
	    BagItem = pack:get_item(BagIdx, Bag),
	    
	    case can_put_item(BagItem, TradeBag) of
		{false, Reason} ->
		    sys_msg:send(Account, Reason),
		    {noreply, State};		
		true ->
		    %% ȡ�½�������Ӧλ�õ���Ʒ
		    State1 = take_off_trade_item(TradeIdx, State),
		    Bag1 = get_bag(State1),
		    Count = pack:get_item_count(BagIdx, Bag1),
	    
		    %% ����µĽ�������Ʒ
		    TradeBag1 = pack:set_item(BagItem, TradeIdx, TradeBag),
		    NewTradeBag = pack:set_item_count(TradeIdx, Count, TradeBag1),
		    
		    NewTradeInfo = TradeInfo#trade_data{trade_bag=NewTradeBag, trade_sured=false},    
		    State2 = player_data:set_trade_info(NewTradeInfo, State1),
		    
		    %% ����������Ʒ
		    NewBag = pack:lock_grid(BagIdx, Bag1),
		    NewState = set_bag(NewBag, State2),	    

		    router:send(TradePlayer, cancel_sured),

		    
		    %% ֪ͨ����
		    net_helper:send2client(Account, 
					   #notify_put_trade_item{trade_index=TradeIdx, 
								  item_data=BagItem, 
								  item_count=Count}),
		    %% ֪ͨ�Է�
		    net_helper:send2client(TradePlayer, 
					   #notify_other_put_trade_item{trade_index=TradeIdx, 
									item_data=BagItem, 
									item_count=Count}),
		    {noreply, NewState}
	    end
    end;
%% ����Ʒ�ӽ�����ȡ��
handle_cast({_Msg, 
	     #req_unput_trade_item{trade_index=TradeIdx, packet_index=_BagIdx}}, 
	    State) ->
    Account = player_data:get_account(State),
    TradeInfo = player_data:get_trade_info(State),
    #trade_data{all_sured=AllSured, trade_player=TradePlayer} = TradeInfo,
    case AllSured of
	true -> 
	    sys_msg:send(Account, ?err_trade_all_sured),
	    {noreply, State};
	false -> 
	    NewState = take_off_trade_item(TradeIdx, State),
       	    router:send(TradePlayer, cancel_sured),
	    {noreply, NewState}
    end;
%% ȷ�Ͻ���
handle_cast({_Msg, 
	     #req_sure_trade{}}, 
	    State) ->
    Account = player_data:get_account(State),

    case player_data:get_trade_info(State) of
	#trade_data{trade_player=TradePlayer, all_sured=AllSured} = TradeInfo ->
	    case AllSured of
		true -> 
		    {noreply, State};
		false -> 
		    NewTradeInfo = TradeInfo#trade_data{trade_sured=true},
		    State1 = player_data:set_trade_info(NewTradeInfo, State), 
		    
		    %% ֪ͨ����
		    net_helper:send2client(Account, #notify_sure_trade{}),
		    %% ֪ͨ�Է�
		    net_helper:send2client(TradePlayer, #notify_other_sure_trade{}),
		    
		    %% handle_call��һ��, ����ȷ��������
		    OtherSured = router:send(TradePlayer, other_sured),
		    NewState = player_data:set_trade_info(NewTradeInfo#trade_data{all_sured=OtherSured}, 
							  State1),     
		    {noreply, NewState}
	    end;
	_ ->
	    {noreply, State}
    end;
	    
%% ĳ������ȡ������(����ʧ��)
handle_cast({_Msg, 
	     #req_cancel_trade{}}, State) ->
    Stoper = player_data:get_account(State),
    NewState = mutex_actions:call_stop_action(?action_type_trade, Stoper, State),
    {noreply, NewState}.




%% ====================================================================
%% �ڲ�����
%% ====================================================================
%% ����������Ʒ
unlock_trade_items(State) ->
    TradeInfo = player_data:get_trade_info(State),
    MyTradeBag = TradeInfo#trade_data.trade_bag,
    
    MyBag = get_bag(State),
    IndexList = get_item_index_list(MyTradeBag, MyBag),

    %% ������Ʒ
    F = fun(ItemIndex, TmpBag) -> pack:unlock_grid(ItemIndex, TmpBag) end,    
    MyNewBag = lists:foldl(F, MyBag, IndexList),

    set_bag(MyNewBag, State).

%% ֹͣ������ش���
do_trade_fail(State) ->
    Account = player_data:get_account(State),
    %% ������ƷҪ����
    NewState = unlock_trade_items(State),
    %% ����ʧ��
    net_helper:send2client(Account, #notify_cancel_trade{}),
    %%io:format("do trade fail: ~p~n~n~n", [Account]),
    player_data:set_trade_info(undefined, NewState).




%% ȡ�ý�����Ʒ�������б�
get_item_index_list(MyTradeBag, MyBag) ->
    [X || X <- get_item_index_list_1(MyTradeBag, MyBag), X /= not_found].
get_item_index_list_1(MyTradeBag, MyBag) ->
    FGetIndex = fun(_Idx, {TradeItem, _Count}, List) ->
			TradeItemID = item:get_instid(TradeItem),
			case item:is_empty(TradeItem) of
			    false ->
				ItemIndex = pack:get_index(TradeItemID, MyBag),
				[ItemIndex | List];	
			    true -> List	
			end
		end,

    IndexList = pack:foldl(FGetIndex, [], MyTradeBag),
    IndexList.


%% �±��� = �ɱ��� - ���ѽ����� + �Է������� 
%% �����±��� | full | trade_item_not_find
-spec calc_new_bag(array(), array(), array()) -> array() | atom().
calc_new_bag(MyBag, MyTradeBag, YourTradeBag) ->
    %% ȡ�ý�������Ʒ��Ӧ�ı�������
    IndexList = get_item_index_list_1(MyTradeBag, MyBag),
    case lists:member(not_found, IndexList) of
	true ->
	    trade_item_not_find; %% ���������Ҳ���������Ʒ
	false ->	    
	    %% ɾ����Ʒ
	    FSub = fun(ItemIndex, TmpBag) -> 
			   Count = pack:get_item_count(ItemIndex, MyBag),
			   pack:del_item(ItemIndex, Count, TmpBag) 
		   end,    
	    Bag = lists:foldl(FSub, MyBag, IndexList),


	    %% �����Ʒ
	    FAdd = fun(_Idx, {Item, Count}, Acc) ->
			   case is_atom(Acc) of 
			       true -> %% ����  
				   Acc;
			       false ->
				   case item:is_empty(Item) of
				       true -> Acc;
				       false ->
					   case item:can_trade(Item) of
					       true ->
						   NewItem = item:cut_trade_time(Item),
						   pack:add_item(NewItem, Count, Acc);
					       {false, ?err_trade_cut_time_not_enough} -> %% ��Ʒʣ��ʱ�䲻�㣬�޷�����
						   trade_cut_time_not_enough
					   end
				   end
			   end
		   end,
	    NewBag = pack:foldl(FAdd, Bag, YourTradeBag),
	    NewBag
    end.    

		


%% �жϽ�Ǯ�Ƿ�����
-spec money_valid(integer(), integer(), integer()) -> atom() | tuple().
money_valid(MyMoney, MyTradeMoney, YourTradeMoney) -> 
    case MyMoney < MyTradeMoney of %% ���׵�Ǯ�����ϵ�Ǯ����
	true -> {false, ?err_not_enough_coin};
	false -> 
	    NewMoney = MyMoney - MyTradeMoney + YourTradeMoney,
	    case NewMoney > ?MAX_GAMECOIN of %% ���׽����Ǯ���
		true -> {false, ?err_cannot_trade_money_full};
		false -> 
		    case NewMoney < 0 of %% ���׽����Ǯ���
			true -> {false, ?err_not_enough_coin};
			false -> true
		    end
	    end
    end.


%% ���׳ɹ�����, ����{YourNewState, MyNewState}
do_post_trade(YourState, MyState) ->
    MyAccount = player_data:get_account(MyState),
    MyTradeInfo = player_data:get_trade_info(MyState),
    #trade_data{trade_player=TradePlayer, 
		trade_money=MyTradeMoney, 
		trade_bag=MyTradeBag, 
		trade_sured=MeSured} = MyTradeInfo,    
    MyMoney = player_coin:get_coin(?GAMECOIN, player_data:get_coin(MyState)),
    MyBag = get_bag(MyState),

    YourAccount = TradePlayer,
    YourAccount = player_data:get_account(YourState),
    YourTradeInfo = player_data:get_trade_info(YourState),
    #trade_data{trade_money=YourTradeMoney, 
		trade_bag=YourTradeBag, 
		trade_sured=YouSured} = YourTradeInfo,
    YourMoney = player_coin:get_coin(?GAMECOIN, player_data:get_coin(YourState)),
    YourBag = get_bag(YourState),    

    %% �����°���
    MyNewBag = calc_new_bag(MyBag, MyTradeBag, YourTradeBag),
    YourNewBag = calc_new_bag(YourBag, YourTradeBag, MyTradeBag),

    AllSured = MeSured and YouSured,
    %% �ж��ܷ���
    case can_trade(AllSured, MyNewBag, MyMoney, MyTradeMoney, 
		   YourNewBag, YourMoney, YourTradeMoney,
		   YourState, MyState) of
	true ->
	    %% ������money
	    MyNewCoin = player_coin:add_coin2(?GAMECOIN, YourTradeMoney - MyTradeMoney, 
					    player_data:get_coin(MyState)),

	    YourNewCoin = player_coin:add_coin2(?GAMECOIN, MyTradeMoney - YourTradeMoney,
					    player_data:get_coin(YourState)),

	    %% ���ݿ����(������Ʒ, ��Ǯ)	    
	    save_trade_to_db(MyAccount, MyNewBag, MyNewCoin, YourAccount, YourNewBag, YourNewCoin),

	    %% ֪ͨ���½�Ǯ
	    player_coin:send_coin(MyNewCoin),
	    player_coin:send_coin(YourNewCoin),

	    MyState1 = set_bag(MyNewBag, player_data:set_coin(MyNewCoin, MyState)),
	    YourState1 = set_bag(YourNewBag, player_data:set_coin(YourNewCoin, YourState)),
 
	    %% ���׳ɹ�
	    MyNewState = mutex_actions:call_stop_action(?action_type_trade, {succeed}, MyState1),
	    YourNewState = mutex_actions:call_stop_action(?action_type_trade, {succeed}, YourState1),

	    MyItemInfos = get_bag_items(MyTradeBag),
	    YourItemInfos = get_bag_items(YourTradeBag),
	    %% ��Ʒ��Ǯ��־
	    DateTime = datetime:local_time(),
	    Logs = [
		    item_money_log:build_record(MyAccount, player_trade, 
						get_lost_bag_items(MyTradeBag), 
						item_money_log:get_money_info(?GAMECOIN, -MyTradeMoney), 
						YourAccount, DateTime),
		    item_money_log:build_record(YourAccount, player_trade, 
						MyItemInfos, 
						item_money_log:get_money_info(?GAMECOIN, MyTradeMoney), 
						MyAccount, DateTime),
		    item_money_log:build_record(YourAccount, player_trade, 
						get_lost_bag_items(YourTradeBag), 
						item_money_log:get_money_info(?GAMECOIN, -YourTradeMoney), 
						MyAccount, DateTime),
		    item_money_log:build_record(MyAccount, player_trade, 
						YourItemInfos, 
						item_money_log:get_money_info(?GAMECOIN, YourTradeMoney), 
						YourAccount, DateTime)
		   ],
	    db_common:write(Logs),
	    
	    %% ��ʾ�����Ʒ
	    [sys_msg:send(MyAccount, ?msg_get_item, [item:get_item_name(ItemTempID), ItemCount])
	     || {ItemTempID, ItemCount, _} <- YourItemInfos],

	    [sys_msg:send(YourAccount, ?msg_get_item, [item:get_item_name(ItemTempID), ItemCount])
	     || {ItemTempID, ItemCount, _} <- MyItemInfos],

	    {YourNewState, MyNewState};    	       
	{false, Reason} ->
	    %% ����ʧ��

	    %% ֪ͨʧ��
	    sys_msg:send(MyAccount, Reason),
	    sys_msg:send(YourAccount, get_other_fail_reason(Reason)),
	    
	    %% ֹͣ����
	    MyNewState = mutex_actions:call_stop_action(?action_type_trade, {fail}, MyState), 
	    YourNewState = mutex_actions:call_stop_action(?action_type_trade, {fail}, YourState),    

	    {YourNewState, MyNewState}
    end.

%% �Է���ʧ������
-spec get_other_fail_reason(integer()) -> integer().
get_other_fail_reason(Reason) when is_integer(Reason) ->
    case Reason of
	?err_cannot_trade_money_full -> ?err_cannot_trade_other_money_full;
	?err_cannot_trade_other_money_full -> ?err_cannot_trade_money_full;
	?err_cannot_trade_bag_full -> ?err_cannot_trade_other_bag_full;
	?err_cannot_trade_other_bag_full -> ?err_cannot_trade_bag_full;
	OtherReason -> OtherReason 
    end.

%% �жϿɷ�ʼ����
can_trade(AllSured, MyNewBag, MyMoney, MyTradeMoney, 
	  YourNewBag, YourMoney, YourTradeMoney,
	  YourState, MyState) -> 
    case AllSured of 
	false -> {false, ?err_trade_need_all_sured};
	true ->
	case money_valid(MyMoney, MyTradeMoney, YourTradeMoney) of       %% �ж�����Ǯ��
	    {false, Err} -> {false, Err}; 
	    true -> 
	    case money_valid(YourMoney, YourTradeMoney, MyTradeMoney) of  %% �ж϶Է�Ǯ��
		{false, Err} -> {false, get_other_fail_reason(Err)}; 
		true -> 
		case MyNewBag of        %% �ж����ѱ���
		    full -> {false, ?err_cannot_trade_bag_full};
		    trade_item_not_find -> {false, ?err_trade_item_not_find};
		    trade_cut_time_not_enough -> {false, ?err_trade_cut_time_not_enough};
		    _ -> 
		    case YourNewBag of  %% �ж϶Է�����
			full -> {false, ?err_cannot_trade_other_bag_full};
			trade_item_not_find -> {false, ?err_trade_item_not_find};
			trade_cut_time_not_enough -> {false, ?err_trade_cut_time_not_enough};
			_ -> 
			case can_create_trade_1(YourState, MyState) of   %% �ٴ��ж��Ƿ���Ͻ��׵Ļ�������
			    {false, Reason} -> {false, Reason};
			    true -> true                                 %% ����������ok����true
			end
		    end
		end
	    end
	end
    end.


%% �����Ƿ����
distance_allow_trade(#point{x=X1, y=Y1, z=Z1}, #point{x=X2, y=Y2, z=Z2}) ->
    X = X1 - X2,
    Y = Y1 - Y2,
    Z = Z1 - Z2,
    
    Distance2 = X * X + Y * Y + Z * Z,
    MaxDis = common_def:get_val(max_trade_distance),
    (Distance2 =< (MaxDis * MaxDis)).


%% �ܷ񴴽�����
can_create_trade_1(YourState, MyState) ->
    MyPos = player_data:get_pos(MyState),
    MyScene = player_data:get_scene_name(MyState),

    YourPos = player_data:get_pos(YourState),
    YourScene = player_data:get_scene_name(YourState),
    can_create_trade_2(YourPos, YourScene, MyPos, MyScene).


%% �ܷ񴴽�����
can_create_trade_2(YourPos, YourScene, MyPos, MyScene) ->
    case (MyScene =:= YourScene) of
	false -> {false, ?err_not_same_scene};
	true -> 
	    case distance_allow_trade(MyPos, YourPos) of
		false -> {false, ?err_cannot_trade_too_far};
		true -> true
	    end
    end.


%% ȡ�½�������Ʒ
%% �����½�������Ʒ����
-spec take_off_trade_item(integer(), any()) -> any().
take_off_trade_item(TradeIdx, State) 
  when is_integer(TradeIdx) ->    
    Account = player_data:get_account(State),
    TradeInfo = player_data:get_trade_info(State),
    #trade_data{trade_player=TradePlayer, trade_bag=TradeBag} = TradeInfo,
    %% ������������Ʒ
    TradeItem = pack:get_item(TradeIdx, TradeBag),
    %% ���ݸ���������ж�
    case TradeItem =:= empty of
	true -> %% ��Ч����, ����Ҫ����
	    State;
	false ->
	    InstanceID = item:get_instid(TradeItem),
	    %% ����ʵ��id�õ�ԭ����Ʒ����
	    Bag = get_bag(State),
	    State1 = 
		case pack:get_index(InstanceID, Bag) of
		    not_found -> %% �п�����ƷʱЧ���ڣ���ɾ���ˣ������Ҳ���		    
			State;
		    OldIndex ->	    
			%% ����ԭ�ȱ�����Ʒ
			NewBag = pack:unlock_grid(OldIndex, Bag),
			set_bag(NewBag, State)		    
		end,

	    %% �Ľ���������
	    NewTradeBag = pack:set_item(empty, TradeIdx, TradeBag),
	    NewTradeInfo = TradeInfo#trade_data{trade_bag=NewTradeBag, trade_sured=false},
	    NewState = player_data:set_trade_info(NewTradeInfo, State1), 
	    
	    %% ֪ͨ����
	    net_helper:send2client(Account, 
				   #notify_unput_trade_item{trade_index=TradeIdx}),
	    %% ֪ͨ�Է�
	    net_helper:send2client(TradePlayer, 
				   #notify_other_unput_trade_item{trade_index=TradeIdx}),
	    NewState
    end.

%% ����ȡ������
do_cancel_trade(Stoper, Account, State) 
  when Stoper =:= Account ->
    TradeInfo = player_data:get_trade_info(State),
    TradePlayer = TradeInfo#trade_data.trade_player,
    %%io:format("do cancel trade self1 ~p~n~n~n", [Account]),
    %% ֪ͨ�Է�ֹͣ����
    router:cast(TradePlayer, cast_stop_action, {?action_type_trade, Stoper}),
    %%io:format("do cancel trade self2 ~p~n~n~n", [Account]),
    %% ����ֹͣ����
    do_trade_fail(State);
%% �Է�ȡ������
do_cancel_trade(_Stoper, Account, State) ->
    %%io:format("do cancel trade other ~p~n~n~n", [Account]),
    %% ֪ͨ�ͻ��ˣ��Է�ȡ���˽���
    sys_msg:send(Account, ?err_other_cancel_trade),
    do_trade_fail(State).


%% �жϿɷ���Ʒ����������
can_put_item(BagItem, TradeBag) ->
    case BagItem =:= empty of
	true -> {false, ?err_select_item_is_empty}; %% ѡ����ƷΪ��
	false ->
	    case item:can_trade(BagItem) of
		{false, Err} -> {false, Err};
		_ ->
		    ItemID = item:get_instid(BagItem),	    
		    %% ȡ����������Ʒ������	    
		    ItemIndex = pack:get_index(ItemID, TradeBag),
		    
		    case ItemIndex of
			not_found -> true;
			_ -> {false, ?err_already_exists_trade_item} %% ���������Ѵ��ڸ���Ʒ 
		    end
	    end
    end.


%% ȡ����
get_bag(State) ->
    player_data:get_pack(?pt_bag, State).

%% ���ñ���
set_bag(NewBag, State) ->    
    Account = player_data:get_account(State),
    NewState = player_data:set_pack(?pt_bag, NewBag, State),
    %% ֪ͨ�����Ĺ�
    pack:send_package(Account, ?pt_bag, NewBag),
    NewState.

save_trade_to_db(MyAccount, MyNewBag, MyNewCoin, 
		 YourAccount, YourNewBag, YourNewCoin) ->        
    MyRecord = player_pack:make_package(MyAccount, ?pt_bag, MyNewBag),
    YourRecord = player_pack:make_package(YourAccount, ?pt_bag, YourNewBag),
    OpBag = {write, [MyRecord, YourRecord]},
    OpMoney = {write, [MyNewCoin, YourNewCoin]},

    db_common:change([OpBag, OpMoney]),
    ok.


%% ��ȡ��Ʒ
get_lost_bag_items(Bag) ->
    do_get_bag_items(Bag, lost_item).
get_bag_items(Bag) ->
    do_get_bag_items(Bag, get_item).
do_get_bag_items(Bag, Flag) ->
    F = fun(_Idx, {TradeItem, Count}, List) ->
		case item:is_empty(TradeItem) of
		    false ->
			NewCount = case Flag of
				       lost_item -> -Count;
				       _ -> Count
				   end,
			[{item:get_tempid(TradeItem), NewCount, item:get_instid(TradeItem)} | List];	
		    true -> List	
		end
	end,
    pack:foldl(F, [], Bag).

%%%====================================================================
%%% ���Ժ���
%%%====================================================================
