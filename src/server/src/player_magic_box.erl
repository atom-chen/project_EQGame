%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   ����ϵͳ(����߼�)
%%% @end
%%% Created : 29 Mar 2011 by hongjx <hongjx@35info.cn>

-module(player_magic_box).


-behaviour(player_action).

-include("packet_def.hrl").
-include("sys_msg.hrl").
-include("tplt_def.hrl").
-include("common_def.hrl").
-include("enum_def.hrl").
-include("mutex_actions.hrl").


-export([handle_cast/2, start/1]).

-export([pick_magic_box/3, get_max_box_per_day/1, 
	 get_player_magic_box_info/1]).
-export([get_today_generate/2, set_today_generate/3]).

-export([do_start_action/2, do_stop_action/2,
	 can_start_action/2, get_action_register_msgs/2]).



-record(player_magic_box, {account, 
			   date = {0, 0, 0},      % ����     
			   pick_box_count = 0,    % ����ʰȡ�������
			   generate_box_count = 0 % �������ɱ������
			  }).


start(Account) ->
    [
     router:make_event_source(?msg_notify_server_empty_box_positions, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_pick_magic_box, Account, {self(), ?MODULE})
    ].


%%%===================================================================
%%% ������ػص�����
%%%===================================================================
%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
get_action_register_msgs(_Account, _PID) ->
    [
    ].

%% �жϵ�ǰ״̬�ܷ�ʼ����
%% ����true | {false, Error}
can_start_action(_Param, _State) ->
    true.

%% ��ʼ
do_start_action({BoxID, PlayerPos},  PlayerData) ->
    case pre_pick_magic_box(BoxID, PlayerPos, PlayerData) of
	{ok, {BoxType, HouseLevel, HouseClean}} ->
	    do_pick_magic_box(BoxType, HouseLevel, HouseClean, PlayerData);	    
	{false, Error} ->
	    Account = player_data:get_account(PlayerData),
	    sys_msg:send(Account, Error),	    
	    PlayerData
    end.



%% ����
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action(_Param, State) ->
    State.


%%%===================================================================
%%% api
%%%===================================================================

pick_magic_box(0, _PlayerPos, PlayerData) ->
    erlang:error(pick_uninstanced_magic_box),
    PlayerData;    
pick_magic_box(BoxID, PlayerPos, State) when is_integer(BoxID) ->  
    mutex_actions:try_start_action(?action_type_pick_magic_box, {BoxID, PlayerPos}, State).

%%%===================================================================
%%% ����handle_cast
%%%===================================================================
%% �ͻ��˷��ؿ�λ���б�
handle_cast({_Msg, #notify_server_empty_box_positions{pos_list=PosList}}, 
	    State) ->
    SceneName = player_data:get_scene_name(State),
    router:cast(SceneName, fetch_empty_box_positions, PosList),
    {noreply, State};


%% ����ʰȡ����
handle_cast({_Msg, #req_pick_magic_box{box_id=BoxID}}, 
	    State) ->
    PlayerPos = player_data:get_pos(State),
    NewState = mutex_actions:try_start_action(?action_type_pick_magic_box, 
					      {BoxID, PlayerPos}, State),
    
    {noreply, NewState}.


%%%===================================================================
%%% �ڲ�����
%%%===================================================================

%% �����Ƿ����
distance_allow_pick(#point{x=X1, y=Y1, z=Z1}, #grid_pos{x=X2, y=Y2, z=Z2}) ->
    X = X1 - X2 * ?GRID_SIZE,
    Y = Y1 - Y2 * ?GRID_SIZE,
    Z = Z1 - Z2 * ?GRID_SIZE,
    
    Distance2 = X * X + Y * Y + Z * Z,
    MaxDis = common_def:get_val(pick_magic_box_distance),
    (Distance2 =< (MaxDis * MaxDis)).


%% ������ҵȼ� �� ʰȡ��������
get_max_pick_times(PlayerLevel) when is_integer(PlayerLevel) ->
    #player_level_magic_box_tplt{reward_times=MaxPickTimes} 
	= tplt:get_data2(player_level_magic_box_tplt, PlayerLevel),
    MaxPickTimes.

%% ������ҵȼ� �� �ܲ������ٸ�����(ÿ��)
get_max_box_per_day(PlayerLevel) when is_integer(PlayerLevel) ->
    #player_level_magic_box_tplt{max_box_per_day=MaxBox} 
	= tplt:get_data2(player_level_magic_box_tplt, PlayerLevel),
    MaxBox.

%% ���㱦���ջ�
calc_magic_box_gain(HouseLevel, TodayPick, PlayerData) ->
    PlayerLevel = player_level:get_level(PlayerData),
    PickLimit = get_max_pick_times(PlayerLevel),
    %% �ж�ʰȡ����, �����˾͸���������
    case TodayPick >= PickLimit of
	true ->
	    ItemID = 0,
	    {ItemID, common_def:get_val(magic_box_base_award)};
	_ ->
	    calc_magic_box_gain_1(HouseLevel)
    end.

%% ���ݷ��ݵȼ��������Ľ�Ǯ��Ʒ
calc_magic_box_gain_1(HouseLevel) ->
    %% ���ݷ��ݵȼ��������Ľ�Ǯ��Ʒ
    #house_magic_box_tplt{box_ids=BoxIDs, box_rates=BoxRates} 
	= tplt:get_data2(house_magic_box_tplt, HouseLevel),
    
    %% ������ѡһ��
    BoxID = util:rate_select_one(BoxRates, BoxIDs),
    case BoxID > 0 of
	true ->
	    #magic_box_tplt{money_range={MinMoney, MaxMoney}, 
			    item_ids=ItemIDs, 
			    item_rates=ItemRates} 
		= tplt:get_data2(magic_box_tplt, BoxID),
	    

	    %% ������ѡһ��
	    GainItemID = util:rate_select_one(ItemRates, ItemIDs),
	    
	    GainMoney = rand:uniform(MinMoney, MaxMoney),

	    {GainItemID, GainMoney};
	 _ -> 
	    {0, 0}
    end.


%% ���ݷ��ݵȼ���������Ľ�Ǯ��Ʒ
calc_garbage_gain(HouseLevel) ->
    %% ���ݷ��ݵȼ���������Ľ�Ǯ��Ʒ
    #house_garbage_tplt{garbage_ids=BoxIDs, garbage_rates=BoxRates} 
	= tplt:get_data2(house_garbage_tplt, HouseLevel),
    
    %% ������ѡһ��
    BoxID = util:rate_select_one(BoxRates, BoxIDs),
    case BoxID > 0 of
	true ->
	    #garbage_tplt{money_range={MinMoney, MaxMoney}, 
			    item_ids=ItemIDs, 
			    item_rates=ItemRates} 
		= tplt:get_data2(garbage_tplt, BoxID),
	    
	    %% ������ѡһ��
	    GainItemID = util:rate_select_one(ItemRates, ItemIDs),
	    
	    GainMoney = rand:uniform(MinMoney, MaxMoney),
	    {GainItemID, GainMoney};
	 _ -> 
	    {0, 0}
    end.
	    
%% ȡ�����ʰȡ��
get_today_pick(Today, 
	       #player_magic_box{date=Date, pick_box_count=PickCount}) ->    
    case Date =:= Today of
	true -> PickCount;
	_ -> 0
    end.

%% ���ý����ʰȡ��
set_today_pick(Today, PickCount, 
	       #player_magic_box{account=Account, date=Date} = MagicBoxInfo) ->
    %% ������ڲ�һ��������
    case Date =:= Today of
	true -> 
	    MagicBoxInfo#player_magic_box{pick_box_count=PickCount};
	_ -> 
	    #player_magic_box{account=Account, date=Today, pick_box_count=PickCount}
    end.


%% ���ý�������ı������
set_today_generate(Today, GenCount, 
		   #player_magic_box{account=Account, date=Date} = MagicBoxInfo) ->
    %% ������ڲ�һ��������
    case Date =:= Today of
	true -> 
	    MagicBoxInfo#player_magic_box{generate_box_count=GenCount};
	_ -> 
	    #player_magic_box{account=Account, date=Today, generate_box_count=GenCount}
    end.


%% ȡ����Ĳ����ı������
get_today_generate(Today, 
	       #player_magic_box{date=Date, generate_box_count=PickCount}) ->    
    case Date =:= Today of
	true -> PickCount;
	_ -> 0
    end.



%% �жϿɷ�ʰȡ������һЩ����
pre_pick_magic_box(BoxID, PlayerPos, PlayerData) ->
    SceneName = player_data:get_scene_name(PlayerData),
    
    %% ȡ����
    case router:send(SceneName, get_magic_box, BoxID) of
	{false, Err} -> %% �п��ܱ��䲻����
	    {false, Err};
	#magic_box{pos=BoxPos, type=BoxType} ->	    
	    %% �жϾ���
	    case distance_allow_pick(PlayerPos, BoxPos) of
		false ->
		    {false, ?err_distance_not_allow_pick};
		_ ->
		    case can_pick_box_type(BoxType, PlayerData) of
			{false, Err} ->
			    {false, Err};
			_ ->
			    Account = player_data:get_account(PlayerData),
			    case router:send(SceneName, fetch_magic_box, {Account, BoxID}) of
				{false, Err} -> %% �п��ܱ��䲻����, Ҳ���ܲ��Ǻ���
				    {false, Err};
				Ret ->
				    {ok, Ret}
			    end			    
		    end
	    end
    end.

%% ���������жϿɷ�ʰȡ
can_pick_box_type(?mbt_garbage, PlayerData) -> 
    #player_property{active_value=PlayerActive} = player_data:get_property(PlayerData),
    %% �жϻ�Ծֵ
    case PlayerActive >= common_def:get_val(garbage_cost_player_active) of
	false ->
	    {false, ?err_less_active_can_not_pick};
	_ ->
	    true
    end;    
can_pick_box_type(_BoxType, _PlayerData) -> 
    true.    


%% ʰ����
do_pick(?mbt_magic_box, HouseLevel, _HouseClean, _SceneName, PlayerData) ->
    Account = player_data:get_account(PlayerData),
    {Today, _Time} = datetime:local_time(),
    
    MagicBoxInfo = get_player_magic_box_info(Account),

    TodayPick = get_today_pick(Today, MagicBoxInfo),
    
    {ItemID, GainMoney} = calc_magic_box_gain(HouseLevel, TodayPick, PlayerData),
    %% ������¼
    NewMagicBoxInfo = set_today_pick(Today, TodayPick + 1, MagicBoxInfo),
    
    {ItemID, GainMoney, [NewMagicBoxInfo], PlayerData};

%% ʰ����
do_pick(?mbt_garbage, HouseLevel, HouseClean, SceneName, PlayerData) ->

    %% ��������Ϊ0ʱ���̶���ý�Ǯ
    {ItemID, GainMoney} = 
	case HouseClean > 0 of 
	    true ->
		calc_garbage_gain(HouseLevel);
	    _ ->    
		{0, common_def:get_val(garbage_min_gain)}
	end,
    %% �ۻ�Ծֵ
    OPlayerProperty = player_data:get_property(PlayerData),
    #player_property{active_value=PlayerActive} = OPlayerProperty, 
    Cost = common_def:get_val(garbage_cost_player_active),
    NPlayerProperty = OPlayerProperty#player_property{active_value=PlayerActive - Cost},    
    
    Account = player_data:get_account(PlayerData),
    router:send(player_property, player_property_change_with_notify_client, 
		{Account, ?msg_player_property_by_furniture, NPlayerProperty, OPlayerProperty}),
    
    NPlayerData = player_data:set_property(NPlayerProperty, PlayerData),
    
    %% ������־
    router:cast(SceneName, log_pick_garbage, {Account, ItemID}),
    {ItemID, GainMoney, [NPlayerProperty], NPlayerData}.



do_pick_item(ItemID, PlayerData) ->
    Account = player_data:get_account(PlayerData),
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    ItemCount = 1,
    case ItemID > 0 of
	true ->
	    %% �жϱ����Ƿ��п�λ
	    case pack:add_item(ItemID, ItemCount, Bag) of
		full ->
		    sys_msg:send(Account, ?err_bag_full_can_not_pick),
		    NewItemID = 0,
		    {NewItemID, [], PlayerData};
		NBag -> 
		    %% �����Ʒ��ʾ
		    sys_msg:send(Account, ?msg_get_item, [item:get_item_name(ItemID), ItemCount]),
		    %% ֪ͨ��������    
		    pack:send_package(Account, ?pt_bag, NBag),
		    NewBagRecord = player_pack:make_package(Account, ?pt_bag, NBag),
		    NewPlayerData = player_data:set_pack(?pt_bag, NBag, PlayerData),
		    {ItemID, [NewBagRecord], NewPlayerData}
	    end;
	_ ->
	    {ItemID, [], PlayerData}			    
    end.

do_pick_money(GainMoney, PlayerData) ->
    case GainMoney > 0 of
	true ->
	    Coin = player_data:get_coin(PlayerData),
	    %% ������money
	    NCoin = player_coin:add_coin2(?GAMECOIN, GainMoney, 
					  Coin),
	    
	    NPlayerData = player_data:set_coin(NCoin, PlayerData),
	    NewGainMoney = player_coin:diff_coin(?GAMECOIN, Coin, NCoin),

	    %% �ж�Ǯ�Ƿ���
	    case NewGainMoney =:= 0 of
		true ->  
		    Account = player_data:get_account(PlayerData),
		    sys_msg:send(Account, ?err_money_full_can_not_pick);
		_ ->
		    %% ֪ͨ���½�Ǯ
		    player_coin:send_coin(NCoin)
	    end,	
	    
	    {NewGainMoney, [NCoin], NPlayerData};
	_ ->
	    {GainMoney, [], PlayerData}
    end.



	    

do_pick_magic_box(BoxType, HouseLevel, HouseClean, PlayerData) ->
    Account = player_data:get_account(PlayerData),

    %% ��������ʾ
    case wallow:in_warn_time(PlayerData) of
	true ->
	    sys_msg:send(Account, ?err_wallow_magic_box_warn_time);
	false ->
	    case wallow:in_bad_time(PlayerData) of
		true ->
		    sys_msg:send(Account, ?err_wallow_magic_box_bad_time);
		false -> 
		    ok
	    end
    end,

    SceneName = player_data:get_scene_name(PlayerData),

    {GainItem, GainMoney, WriteRecords, PlayerData1} = do_pick(BoxType, HouseLevel, 
							       HouseClean, SceneName, PlayerData),    

    NGainMoney = wallow:calc_income(GainMoney, PlayerData),
    NGainItem = wallow:calc_item(GainItem, PlayerData),

    {NewGainMoney, CoinRecords, PlayerData2} = do_pick_money(NGainMoney, PlayerData1),

    {NewGainItem, BagRecords, PlayerData3} = do_pick_item(NGainItem, PlayerData2),

    %% ���ݿ����
    db_common:write(WriteRecords ++ CoinRecords ++ BagRecords),

    %% ��Ʒ��Ǯ��¼
    ItemCount = 1,
    ItemInfo = {NGainItem, ItemCount, 0},
    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, NGainMoney),
    item_money_log:log(Account, pick_magic_box, ItemInfo, MoneyInfo),

    %% ֪ͨ�ͻ���
    net_helper:send2client(Account, 
			   #notify_pick_magic_box_gain{type=BoxType,
						       item_id=NewGainItem, 
						       money=NewGainMoney}),
    case (NewGainMoney > 0) and (BoxType =:= ?mbt_garbage) of 
	true ->
	    MsgID = case player_data:is_at_home(PlayerData) of
			true -> ?msg_self_home_gain_garbage_coin;
			_ -> ?msg_friend_home_gain_garbage_coin
		    end,
	    sys_msg:send(Account, MsgID, [integer_to_list(NewGainMoney)]);
	_ -> ok
    end,
    

    case (BoxType =:= ?mbt_garbage) of 
	true ->
	    %% ÿ������
	    daily_task:inc_action_amount(?tat_pick_garbage, PlayerData3);
	_ ->
	    PlayerData3
    end.



get_player_magic_box_info(Account) when is_atom(Account) ->
    case db_common:select_one(player_magic_box, Account) of
	#player_magic_box{}=MagicBoxInfo ->
	    MagicBoxInfo;
	_ ->
	    #player_magic_box{account=Account}
    end.


