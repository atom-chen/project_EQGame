%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   玩家交易
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
	  all_sured=false,                         % 双方都确认过
	  trade_player,                            % 交易玩家
	  trade_money,                             % 交易金额
	  trade_bag,                               % 交易栏	  
	  trade_sured                              % 交易确认	  
	 }).


-export([start/1]).
-export([handle_cast/2, handle_call/3]).
-export([do_start_action/2, do_stop_action/2,
	 can_start_action/2, get_action_register_msgs/2]).


start(_Account) ->
    [
    ].


%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================

%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
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


%% 判断当前状态下能否进行交易
%% 返回true | {false, Error}
can_start_action({_TradePlayer, YourScene, YourPos}, MyState) ->
    MyPos = player_data:get_pos(MyState),
    MyScene = player_data:get_scene_name(MyState),
    CanCreate = can_create_trade_2(YourPos, YourScene, MyPos, MyScene),
    CanCreate.

%% 开始交易
do_start_action({TradePlayer, _YourScene, _YourPos}, State) ->
    do_start_action(TradePlayer, State);
do_start_action(TradePlayer, State) ->
    Account = player_data:get_account(State),
    %% 清空交易栏, 清空money
    TradeInfo = #trade_data{trade_player=TradePlayer, trade_money=0, 
			    trade_bag=pack:new(common_def:get_val(trade_grid_count)), 
			    trade_sured=false,
			    all_sured=false},

    NewState = player_data:set_trade_info(TradeInfo, State),

    %% 通知开始交易
    net_helper:send2client(Account, 
			   #notify_start_trade{account=atom_to_list(TradePlayer)}),

    NewState.

%% 交易失败

%% 交易失败, 注: 使用{fail} 而不是fail, 防止不小心与账号同名
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action({fail}, State) ->
    do_trade_fail(State);
%% 交易成功
do_stop_action({succeed}, State) ->
    Account = player_data:get_account(State),
    
    NewState = player_data:set_trade_info(undefined, State),
    %% 通知交易成功    
    net_helper:send2client(Account, #notify_trade_success{}), 
    NewState;

do_stop_action({auto_stop, _Reason}, State) ->
    %%io:format("call auto stop ~p~n~n~n", [player_data:get_actions(State)]),
    Stoper = player_data:get_account(State),
    do_stop_action(Stoper, State);
%% 结束交易(是自已要求取消交易的，需要让对方也取消交易)
%% @参数 Stoper 要求停止交易的人
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
%%% 处理handle_call
%%% ====================================================================
%% 处理两个人交易
handle_call({#msg{event=post_trade}, YourState}, _From, 
	    MyState) ->
    case do_post_trade(YourState, MyState) of
	{YourNewState, MyNewState} ->    
	    {reply, YourNewState, MyNewState}
	%% _ -> 
	%%     {reply, YourState, MyState}
    end;
%% 处理两个人确定交易
handle_call(#msg{event=other_sured}, _From, 
	    State) ->
    Account = player_data:get_account(State),
    TradeInfo = player_data:get_trade_info(State),
    
    AllSured = TradeInfo#trade_data.trade_sured,
    NewTradeInfo = TradeInfo#trade_data{all_sured=AllSured},
    NewState = player_data:set_trade_info(NewTradeInfo, State),

    %% 双方都确认后，就启用倒计时
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
%% 取消确定状态
handle_call(#msg{event=cancel_sured}, _From, 
	    State) ->

    TradeInfo = player_data:get_trade_info(State),
    NewTradeInfo = TradeInfo#trade_data{trade_sured=false},
    NewState = player_data:set_trade_info(NewTradeInfo, State),
    {reply, ok, NewState}.


%%%====================================================================
%%% 处理handle_cast
%%%====================================================================
%% 提交交易(双方确定后3秒)
handle_cast({#msg{event=notify_post_trade}, _TimerRef}, State) ->
    TradeInfo = player_data:get_trade_info(State),
    #trade_data{trade_player=TradePlayer} = TradeInfo,

    %% handle_call另一方
    NewState = router:send(TradePlayer, post_trade, State),
    {noreply, player_data:set_trade_info(undefined, NewState)};
%% 交换交易栏目中的物品
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
	    %% 处理交易栏数据
	    NewBag = pack:swap(Idx1, Idx2, TradeBag),
	    NewTradeInfo = TradeInfo#trade_data{trade_bag=NewBag, trade_sured=false},
	    NewState = player_data:set_trade_info(NewTradeInfo, State),
	    router:send(TradePlayer, cancel_sured),
	    
	    %% 通知自已
	    net_helper:send2client(Account, #notify_swap_trade_item{index1=Idx1, index2=Idx2}),
	    %% 通知对方
	    net_helper:send2client(TradePlayer, 
				   #notify_other_swap_trade_item{index1=Idx1, index2=Idx2}),
	    {noreply, NewState}
    end;
%% 更新交易金额
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
	    %% 如果交易金额大于身上money，将身上的全部money放到交易栏, 如果小于0, 则取0
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

	    %% 通知自已
	    net_helper:send2client(Account, #notify_update_trade_money{money=TradeMoney}),
	    %% 通知对方
	    net_helper:send2client(TradePlayer, #notify_other_update_trade_money{money=TradeMoney}),
	    {noreply, NewState}
    end;	    
%% 将物品放在交易栏
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
	    %% 取得背包物品
	    Bag = get_bag(State),
	    BagItem = pack:get_item(BagIdx, Bag),
	    
	    case can_put_item(BagItem, TradeBag) of
		{false, Reason} ->
		    sys_msg:send(Account, Reason),
		    {noreply, State};		
		true ->
		    %% 取下交易栏对应位置的物品
		    State1 = take_off_trade_item(TradeIdx, State),
		    Bag1 = get_bag(State1),
		    Count = pack:get_item_count(BagIdx, Bag1),
	    
		    %% 算出新的交易栏物品
		    TradeBag1 = pack:set_item(BagItem, TradeIdx, TradeBag),
		    NewTradeBag = pack:set_item_count(TradeIdx, Count, TradeBag1),
		    
		    NewTradeInfo = TradeInfo#trade_data{trade_bag=NewTradeBag, trade_sured=false},    
		    State2 = player_data:set_trade_info(NewTradeInfo, State1),
		    
		    %% 锁定背包物品
		    NewBag = pack:lock_grid(BagIdx, Bag1),
		    NewState = set_bag(NewBag, State2),	    

		    router:send(TradePlayer, cancel_sured),

		    
		    %% 通知自已
		    net_helper:send2client(Account, 
					   #notify_put_trade_item{trade_index=TradeIdx, 
								  item_data=BagItem, 
								  item_count=Count}),
		    %% 通知对方
		    net_helper:send2client(TradePlayer, 
					   #notify_other_put_trade_item{trade_index=TradeIdx, 
									item_data=BagItem, 
									item_count=Count}),
		    {noreply, NewState}
	    end
    end;
%% 将物品从交易栏取下
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
%% 确认交易
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
		    
		    %% 通知自已
		    net_helper:send2client(Account, #notify_sure_trade{}),
		    %% 通知对方
		    net_helper:send2client(TradePlayer, #notify_other_sure_trade{}),
		    
		    %% handle_call另一方, 自已确定交易了
		    OtherSured = router:send(TradePlayer, other_sured),
		    NewState = player_data:set_trade_info(NewTradeInfo#trade_data{all_sured=OtherSured}, 
							  State1),     
		    {noreply, NewState}
	    end;
	_ ->
	    {noreply, State}
    end;
	    
%% 某玩家提出取消交易(交易失败)
handle_cast({_Msg, 
	     #req_cancel_trade{}}, State) ->
    Stoper = player_data:get_account(State),
    NewState = mutex_actions:call_stop_action(?action_type_trade, Stoper, State),
    {noreply, NewState}.




%% ====================================================================
%% 内部函数
%% ====================================================================
%% 解锁交易物品
unlock_trade_items(State) ->
    TradeInfo = player_data:get_trade_info(State),
    MyTradeBag = TradeInfo#trade_data.trade_bag,
    
    MyBag = get_bag(State),
    IndexList = get_item_index_list(MyTradeBag, MyBag),

    %% 解锁物品
    F = fun(ItemIndex, TmpBag) -> pack:unlock_grid(ItemIndex, TmpBag) end,    
    MyNewBag = lists:foldl(F, MyBag, IndexList),

    set_bag(MyNewBag, State).

%% 停止交易相关处理
do_trade_fail(State) ->
    Account = player_data:get_account(State),
    %% 背包物品要解锁
    NewState = unlock_trade_items(State),
    %% 交易失败
    net_helper:send2client(Account, #notify_cancel_trade{}),
    %%io:format("do trade fail: ~p~n~n~n", [Account]),
    player_data:set_trade_info(undefined, NewState).




%% 取该交易物品的索引列表
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


%% 新背包 = 旧背包 - 自已交易栏 + 对方交易栏 
%% 返回新背包 | full | trade_item_not_find
-spec calc_new_bag(array(), array(), array()) -> array() | atom().
calc_new_bag(MyBag, MyTradeBag, YourTradeBag) ->
    %% 取得交易栏物品对应的背包索引
    IndexList = get_item_index_list_1(MyTradeBag, MyBag),
    case lists:member(not_found, IndexList) of
	true ->
	    trade_item_not_find; %% 包果错误，找不到交易物品
	false ->	    
	    %% 删除物品
	    FSub = fun(ItemIndex, TmpBag) -> 
			   Count = pack:get_item_count(ItemIndex, MyBag),
			   pack:del_item(ItemIndex, Count, TmpBag) 
		   end,    
	    Bag = lists:foldl(FSub, MyBag, IndexList),


	    %% 添加物品
	    FAdd = fun(_Idx, {Item, Count}, Acc) ->
			   case is_atom(Acc) of 
			       true -> %% 错误  
				   Acc;
			       false ->
				   case item:is_empty(Item) of
				       true -> Acc;
				       false ->
					   case item:can_trade(Item) of
					       true ->
						   NewItem = item:cut_trade_time(Item),
						   pack:add_item(NewItem, Count, Acc);
					       {false, ?err_trade_cut_time_not_enough} -> %% 物品剩余时间不足，无法交易
						   trade_cut_time_not_enough
					   end
				   end
			   end
		   end,
	    NewBag = pack:foldl(FAdd, Bag, YourTradeBag),
	    NewBag
    end.    

		


%% 判断金钱是否正常
-spec money_valid(integer(), integer(), integer()) -> atom() | tuple().
money_valid(MyMoney, MyTradeMoney, YourTradeMoney) -> 
    case MyMoney < MyTradeMoney of %% 交易的钱比身上的钱还多
	true -> {false, ?err_not_enough_coin};
	false -> 
	    NewMoney = MyMoney - MyTradeMoney + YourTradeMoney,
	    case NewMoney > ?MAX_GAMECOIN of %% 交易结果金钱溢出
		true -> {false, ?err_cannot_trade_money_full};
		false -> 
		    case NewMoney < 0 of %% 交易结果金钱溢出
			true -> {false, ?err_not_enough_coin};
			false -> true
		    end
	    end
    end.


%% 交易成功处理, 返回{YourNewState, MyNewState}
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

    %% 计算新包裹
    MyNewBag = calc_new_bag(MyBag, MyTradeBag, YourTradeBag),
    YourNewBag = calc_new_bag(YourBag, YourTradeBag, MyTradeBag),

    AllSured = MeSured and YouSured,
    %% 判断能否交易
    case can_trade(AllSured, MyNewBag, MyMoney, MyTradeMoney, 
		   YourNewBag, YourMoney, YourTradeMoney,
		   YourState, MyState) of
	true ->
	    %% 计算新money
	    MyNewCoin = player_coin:add_coin2(?GAMECOIN, YourTradeMoney - MyTradeMoney, 
					    player_data:get_coin(MyState)),

	    YourNewCoin = player_coin:add_coin2(?GAMECOIN, MyTradeMoney - YourTradeMoney,
					    player_data:get_coin(YourState)),

	    %% 数据库操作(保存物品, 金钱)	    
	    save_trade_to_db(MyAccount, MyNewBag, MyNewCoin, YourAccount, YourNewBag, YourNewCoin),

	    %% 通知更新金钱
	    player_coin:send_coin(MyNewCoin),
	    player_coin:send_coin(YourNewCoin),

	    MyState1 = set_bag(MyNewBag, player_data:set_coin(MyNewCoin, MyState)),
	    YourState1 = set_bag(YourNewBag, player_data:set_coin(YourNewCoin, YourState)),
 
	    %% 交易成功
	    MyNewState = mutex_actions:call_stop_action(?action_type_trade, {succeed}, MyState1),
	    YourNewState = mutex_actions:call_stop_action(?action_type_trade, {succeed}, YourState1),

	    MyItemInfos = get_bag_items(MyTradeBag),
	    YourItemInfos = get_bag_items(YourTradeBag),
	    %% 物品金钱日志
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
	    
	    %% 提示获得物品
	    [sys_msg:send(MyAccount, ?msg_get_item, [item:get_item_name(ItemTempID), ItemCount])
	     || {ItemTempID, ItemCount, _} <- YourItemInfos],

	    [sys_msg:send(YourAccount, ?msg_get_item, [item:get_item_name(ItemTempID), ItemCount])
	     || {ItemTempID, ItemCount, _} <- MyItemInfos],

	    {YourNewState, MyNewState};    	       
	{false, Reason} ->
	    %% 交易失败

	    %% 通知失败
	    sys_msg:send(MyAccount, Reason),
	    sys_msg:send(YourAccount, get_other_fail_reason(Reason)),
	    
	    %% 停止交易
	    MyNewState = mutex_actions:call_stop_action(?action_type_trade, {fail}, MyState), 
	    YourNewState = mutex_actions:call_stop_action(?action_type_trade, {fail}, YourState),    

	    {YourNewState, MyNewState}
    end.

%% 对方的失败描述
-spec get_other_fail_reason(integer()) -> integer().
get_other_fail_reason(Reason) when is_integer(Reason) ->
    case Reason of
	?err_cannot_trade_money_full -> ?err_cannot_trade_other_money_full;
	?err_cannot_trade_other_money_full -> ?err_cannot_trade_money_full;
	?err_cannot_trade_bag_full -> ?err_cannot_trade_other_bag_full;
	?err_cannot_trade_other_bag_full -> ?err_cannot_trade_bag_full;
	OtherReason -> OtherReason 
    end.

%% 判断可否开始交易
can_trade(AllSured, MyNewBag, MyMoney, MyTradeMoney, 
	  YourNewBag, YourMoney, YourTradeMoney,
	  YourState, MyState) -> 
    case AllSured of 
	false -> {false, ?err_trade_need_all_sured};
	true ->
	case money_valid(MyMoney, MyTradeMoney, YourTradeMoney) of       %% 判断自已钱包
	    {false, Err} -> {false, Err}; 
	    true -> 
	    case money_valid(YourMoney, YourTradeMoney, MyTradeMoney) of  %% 判断对方钱包
		{false, Err} -> {false, get_other_fail_reason(Err)}; 
		true -> 
		case MyNewBag of        %% 判断自已背包
		    full -> {false, ?err_cannot_trade_bag_full};
		    trade_item_not_find -> {false, ?err_trade_item_not_find};
		    trade_cut_time_not_enough -> {false, ?err_trade_cut_time_not_enough};
		    _ -> 
		    case YourNewBag of  %% 判断对方背包
			full -> {false, ?err_cannot_trade_other_bag_full};
			trade_item_not_find -> {false, ?err_trade_item_not_find};
			trade_cut_time_not_enough -> {false, ?err_trade_cut_time_not_enough};
			_ -> 
			case can_create_trade_1(YourState, MyState) of   %% 再次判断是否符合交易的基本条件
			    {false, Reason} -> {false, Reason};
			    true -> true                                 %% 所有条件都ok返回true
			end
		    end
		end
	    end
	end
    end.


%% 距离是否合适
distance_allow_trade(#point{x=X1, y=Y1, z=Z1}, #point{x=X2, y=Y2, z=Z2}) ->
    X = X1 - X2,
    Y = Y1 - Y2,
    Z = Z1 - Z2,
    
    Distance2 = X * X + Y * Y + Z * Z,
    MaxDis = common_def:get_val(max_trade_distance),
    (Distance2 =< (MaxDis * MaxDis)).


%% 能否创建交易
can_create_trade_1(YourState, MyState) ->
    MyPos = player_data:get_pos(MyState),
    MyScene = player_data:get_scene_name(MyState),

    YourPos = player_data:get_pos(YourState),
    YourScene = player_data:get_scene_name(YourState),
    can_create_trade_2(YourPos, YourScene, MyPos, MyScene).


%% 能否创建交易
can_create_trade_2(YourPos, YourScene, MyPos, MyScene) ->
    case (MyScene =:= YourScene) of
	false -> {false, ?err_not_same_scene};
	true -> 
	    case distance_allow_trade(MyPos, YourPos) of
		false -> {false, ?err_cannot_trade_too_far};
		true -> true
	    end
    end.


%% 取下交易栏物品
%% 返回新交易栏物品集合
-spec take_off_trade_item(integer(), any()) -> any().
take_off_trade_item(TradeIdx, State) 
  when is_integer(TradeIdx) ->    
    Account = player_data:get_account(State),
    TradeInfo = player_data:get_trade_info(State),
    #trade_data{trade_player=TradePlayer, trade_bag=TradeBag} = TradeInfo,
    %% 交易栏格子物品
    TradeItem = pack:get_item(TradeIdx, TradeBag),
    %% 根据各种情况做判断
    case TradeItem =:= empty of
	true -> %% 无效操作, 不需要处理
	    State;
	false ->
	    InstanceID = item:get_instid(TradeItem),
	    %% 根据实例id得到原先物品索引
	    Bag = get_bag(State),
	    State1 = 
		case pack:get_index(InstanceID, Bag) of
		    not_found -> %% 有可能物品时效到期，已删除了，所以找不到		    
			State;
		    OldIndex ->	    
			%% 解锁原先背包物品
			NewBag = pack:unlock_grid(OldIndex, Bag),
			set_bag(NewBag, State)		    
		end,

	    %% 改交易栏数据
	    NewTradeBag = pack:set_item(empty, TradeIdx, TradeBag),
	    NewTradeInfo = TradeInfo#trade_data{trade_bag=NewTradeBag, trade_sured=false},
	    NewState = player_data:set_trade_info(NewTradeInfo, State1), 
	    
	    %% 通知自已
	    net_helper:send2client(Account, 
				   #notify_unput_trade_item{trade_index=TradeIdx}),
	    %% 通知对方
	    net_helper:send2client(TradePlayer, 
				   #notify_other_unput_trade_item{trade_index=TradeIdx}),
	    NewState
    end.

%% 自已取消交易
do_cancel_trade(Stoper, Account, State) 
  when Stoper =:= Account ->
    TradeInfo = player_data:get_trade_info(State),
    TradePlayer = TradeInfo#trade_data.trade_player,
    %%io:format("do cancel trade self1 ~p~n~n~n", [Account]),
    %% 通知对方停止交易
    router:cast(TradePlayer, cast_stop_action, {?action_type_trade, Stoper}),
    %%io:format("do cancel trade self2 ~p~n~n~n", [Account]),
    %% 自已停止交易
    do_trade_fail(State);
%% 对方取消交易
do_cancel_trade(_Stoper, Account, State) ->
    %%io:format("do cancel trade other ~p~n~n~n", [Account]),
    %% 通知客户端，对方取消了交易
    sys_msg:send(Account, ?err_other_cancel_trade),
    do_trade_fail(State).


%% 判断可否将物品放至交易栏
can_put_item(BagItem, TradeBag) ->
    case BagItem =:= empty of
	true -> {false, ?err_select_item_is_empty}; %% 选择物品为空
	false ->
	    case item:can_trade(BagItem) of
		{false, Err} -> {false, Err};
		_ ->
		    ItemID = item:get_instid(BagItem),	    
		    %% 取交易栏该物品的索引	    
		    ItemIndex = pack:get_index(ItemID, TradeBag),
		    
		    case ItemIndex of
			not_found -> true;
			_ -> {false, ?err_already_exists_trade_item} %% 交易栏中已存在该物品 
		    end
	    end
    end.


%% 取背包
get_bag(State) ->
    player_data:get_pack(?pt_bag, State).

%% 设置背包
set_bag(NewBag, State) ->    
    Account = player_data:get_account(State),
    NewState = player_data:set_pack(?pt_bag, NewBag, State),
    %% 通知背包改过
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


%% 提取物品
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
%%% 测试函数
%%%====================================================================
