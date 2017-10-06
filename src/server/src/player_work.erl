%%% @author hongjx <hongjx@china-channel.com>
%%% @copyright (C) 2010, hongjx
%%% @doc
%%%    ��Ҵ�
%%% @end
%%% Created : 26 Jul 2010 by hongjx <hongjx@china-channel.com>

-module(player_work).


-include("packet_def.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("mutex_actions.hrl").

-behaviour(player_action).


-export([handle_cast/2, start/1]).
-export([do_start_action/2, do_stop_action/2,
	 can_start_action/2, get_action_register_msgs/2]).


start(Account) ->
    [
     router:make_event_source(?msg_req_start_work, Account, {self(), ?MODULE})
    ].
%%%===================================================================
%%% ������ػص�����
%%%===================================================================
%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
get_action_register_msgs(Account, PID) ->
    [
     router:make_event_source(?msg_req_work, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_end_work, Account, {PID, ?MODULE})
    ].

%% �жϿɷ�ʼ
%% ����true | {false, Error}
can_start_action(_Param, _State) ->
    true.

%% ��ҿ�ʼ��
do_start_action(_Param, State) ->
    Account = player_data:get_account(State),
    net_helper:send2client(Account, #notify_start_work{}),      
    State.


%% ������
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action({auto_stop, _Reason}, State) ->
    ID = 
	case player_data:get_work_info(State) of
	    undefined -> 0;
	    N -> N
	end,    
    do_stop_action(ID, State);
%% ������
do_stop_action(ID, State) ->
    Account = player_data:get_account(State),
    net_helper:send2client(Account, #notify_end_work{id=ID}),    
    player_data:set_work_info(0, State).
	



%%%===================================================================
%%% ����handle_cast
%%%===================================================================

%% �����
handle_cast({_Msg, 
	     #req_start_work{}}, 
	    State) ->    
    NewState = mutex_actions:try_start_action(?action_type_work, undefined, State),
    {noreply, NewState};
%% �����
handle_cast({_Msg, 
	     #req_work{id=ID}}, 
	    State) ->    
    Account = player_data:get_account(State),
    WorkTplt = tplt:get_data(work_tplt, ID),
    case can_work(WorkTplt, State) of	
	true ->
	    NewState = do_start_work(WorkTplt, State),

	    %% ÿ������
	    NNState = daily_task:inc_action_amount(?tat_work, NewState),

	    {noreply, NNState};
	{false, Error} ->
	    sys_msg:send(Account, Error),
	    NewState = mutex_actions:call_stop_action(?action_type_work, ID, State),
	    {noreply, NewState}	    
    end;
    
%% ������
handle_cast({_Msg, 
	     #req_end_work{id=ID}}, 
	    State) -> 
    NewState = mutex_actions:call_stop_action(?action_type_work, ID, State),
    {noreply, NewState}.

%%%===================================================================
%%% �ڲ�����
%%%===================================================================

%% ������Ʒ�ͽ�Ǯ
reward_money_and_item(Events, EventRate, EventRates) when is_list(Events),
							  is_list(EventRates),
							  is_integer(EventRate) ->
    TotalRate = 100,
    %% ������ѡһ��
    EventID = util:rate_select_one(EventRates, Events),
    %% �����¼�
    case (rand:uniform(TotalRate) =< EventRate) and (EventID > 0)of
	true ->
	    EventTplt = tplt:get_data(work_event_tplt, EventID),
	    #work_event_tplt{item_rate=ItemRate, reward_items=RewardItems, 
			     reward_money=RewardMoneyRate} = EventTplt,
	    %% ������Ʒ
	    case rand:uniform(100) =< ItemRate of
		true ->
		    %% ���ѡһ��
		    RewardItem = lists:nth(rand:uniform(length(RewardItems)), RewardItems),
		    {EventID, RewardMoneyRate, RewardItem};
		false ->
		    {EventID, RewardMoneyRate, 0}
	    end;
	false ->
	    {0, 0, 0}
    end.

can_do_work(#player_property{player_clean=PlayerClean, 
    				   player_health=PlayerHealth, 
    				   player_charm=PlayerCharm, 
    				   active_value=PlayerActive}, 
	    PlayerDisease,
	   NeedClean, NeedHealth, NeedCharm, NeedActive, NeedDisease) ->    
    property_enough(PlayerClean, PlayerHealth, PlayerCharm, PlayerActive, PlayerDisease,
		    NeedClean, NeedHealth, NeedCharm, NeedActive, NeedDisease).   


%% -define(dt_normal, 1).
%% -define(dt_white, 2).
%% -define(dt_yellow, 3).
%% -define(dt_purple, 4).
%% -define(dt_red, 5).
%% �ɷ�ʼ��
property_enough(PlayerClean, PlayerHealth, PlayerCharm, PlayerActive, PlayerDisease,
	   NeedClean, NeedHealth, NeedCharm, NeedActive, NeedDisease) ->   
    %%io:format("{PlayerDisease, NeedDisease}=~p~n", [{PlayerDisease, NeedDisease}]),
    case NeedClean > PlayerClean of
	true ->
	    {false, ?err_player_clean_not_enough};
	false ->
	case NeedHealth > PlayerHealth of
	    true -> {false, ?err_player_health_not_enough};
	    false -> 
	    case NeedCharm > PlayerCharm of
		true -> {false, ?err_player_charm_not_enough};
		false ->
		case NeedActive > PlayerActive of
		    true -> {false, ?err_player_active_not_enough};
		    false ->
		    case NeedDisease < PlayerDisease of
			true -> {false, ?err_player_disease_can_not_work};
			_ ->	
			    true
		    end
		end
	    end
	end
    end.					    


%% �жϵ�ǰ״̬�ܷ�ʼ����
%% ����true | {false, Error}
can_work(#work_tplt{need_clean=NeedClean,
		    need_health=NeedHealth,
		    need_charm=NeedCharm,
		    need_active=NeedActive,
		   need_disease=NeedDisease}=_Param, 
	 State) ->
    OPlayerProperty = player_data:get_property(State),
    can_do_work(OPlayerProperty, doctor:get_disease_type(State),
			  NeedClean, NeedHealth, NeedCharm, NeedActive, NeedDisease).    

%% ��ҿ�ʼ��
do_start_work(#work_tplt{id=ID,
			 property_id=PropertyId,
			 money=WorkMoney,
			 event_ids=Events,
			 event_rate=EventRate,
			 event_rates=EventRates}=_Param, 
	      State) ->
    Account = player_data:get_account(State),

    %% ��������ʾ
    case wallow:in_warn_time(State) of
	true ->
	    sys_msg:send(Account, ?err_wallow_work_warn_time);
	false ->
	    case wallow:in_bad_time(State) of
		true ->
		    sys_msg:send(Account, ?err_wallow_work_bad_time);
		false ->
		    ok
	    end
    end,

    %% ���㴥���¼�
    {EventID, RewardMoneyRate, RewardItem} = reward_money_and_item(Events, EventRate, EventRates),
    NRewardItem = wallow:calc_item(RewardItem, State),
    
    %% �����������
    OPlayerProperty = player_data:get_property(State),
    NPlayerProperty = player_property_by_action:calculate(OPlayerProperty, PropertyId, State),
    %% �����Ǯ
    RewardMoney = wallow:calc_income((WorkMoney * RewardMoneyRate div 100), State),
    NWorkMoney = wallow:calc_income(WorkMoney, State),
    GainMoney = NWorkMoney + RewardMoney,
    NewCoin = player_coin:add_coin(?GAMECOIN, GainMoney, player_data:get_coin(State)),
    State1 = player_data:set_coin(NewCoin, State),	    
    
    %% ������
    Bag = player_data:get_pack(?pt_bag, State1),
    {NewItem, NewBag} = 
	case NRewardItem /= 0 of
	    true ->
		TmpItem = item:make_item(NRewardItem),
		{TmpItem, pack:add_item(TmpItem, Bag)};
	    false -> 
		{empty, Bag}
	end,


    State3 = 
    	case NewBag of
    	    full -> 
    		sys_msg:send(Account, ?err_bag_full),
    		State1;
    	    _ ->
    		State2 = player_data:set_pack(?pt_bag, NewBag, State1),		
    		player_pack:save(Account, State2),
    		pack:send_package(Account, ?pt_bag, NewBag),
		State2
    	end,   
    
    %% ���ݿ����
    
    %% ֪ͨ�������Ա仯
    router:send(player_property, player_property_change, {Account, NPlayerProperty, OPlayerProperty}),
    
    %% ֪ͨ�ͻ��˴򹤽��
    Packet = #notify_work_result
      {id=ID, 
       work_money=NWorkMoney,
       event_id=EventID,
       reward_money=RewardMoney,
       reward_item=NRewardItem,
       change_clean=NPlayerProperty#player_property.player_clean - OPlayerProperty#player_property.player_clean,
       change_health=NPlayerProperty#player_property.player_health - OPlayerProperty#player_property.player_health,
       change_charm=NPlayerProperty#player_property.player_charm - OPlayerProperty#player_property.player_charm,
       change_active=NPlayerProperty#player_property.active_value - OPlayerProperty#player_property.active_value
      },
    
    net_helper:send2client(Account, Packet),

    %% ��̨��Ǯ��Ʒ��¼
    ItemInfo = 
	case NewItem of
	    empty ->
		{0, 0, 0};
	    _ ->
		{item:get_tempid(NewItem), 1, item:get_instid(NewItem)}
	end,
    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, GainMoney),
    item_money_log:log(Account, work_gain, ItemInfo, MoneyInfo),

    %% ��ʾ�����Ʒ
    case NewItem of
	empty -> ok;
	_ ->
	    sys_msg:send(Account, ?msg_get_item, [item:get_item_name(NewItem), 1])
    end,
        
    State4 = player_data:set_work_info(ID, State3),
    player_data:set_property(NPlayerProperty, State4).		
    
%%%===================================================================
%%% ���Դ���
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

hit_test()->
    ?assertEqual(a, util:rate_select_one(1, [33, 67], [a, b])),
    ?assertEqual(0, util:rate_select_one(50, [20, 20], [a, b])),
    ?assertEqual(c, util:rate_select_one(77, [20, 30, 50], [a, b, c])),
    ok.
    
-endif.
	





