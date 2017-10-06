%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   宝箱系统(玩家逻辑)
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
			   date = {0, 0, 0},      % 日期     
			   pick_box_count = 0,    % 当天拾取宝箱个数
			   generate_box_count = 0 % 当天生成宝箱次数
			  }).


start(Account) ->
    [
     router:make_event_source(?msg_notify_server_empty_box_positions, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_pick_magic_box, Account, {self(), ?MODULE})
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

%% 开始
do_start_action({BoxID, PlayerPos},  PlayerData) ->
    case pre_pick_magic_box(BoxID, PlayerPos, PlayerData) of
	{ok, {BoxType, HouseLevel, HouseClean}} ->
	    do_pick_magic_box(BoxType, HouseLevel, HouseClean, PlayerData);	    
	{false, Error} ->
	    Account = player_data:get_account(PlayerData),
	    sys_msg:send(Account, Error),	    
	    PlayerData
    end.



%% 结束
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
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
%%% 处理handle_cast
%%%===================================================================
%% 客户端返回空位置列表
handle_cast({_Msg, #notify_server_empty_box_positions{pos_list=PosList}}, 
	    State) ->
    SceneName = player_data:get_scene_name(State),
    router:cast(SceneName, fetch_empty_box_positions, PosList),
    {noreply, State};


%% 请求拾取宝箱
handle_cast({_Msg, #req_pick_magic_box{box_id=BoxID}}, 
	    State) ->
    PlayerPos = player_data:get_pos(State),
    NewState = mutex_actions:try_start_action(?action_type_pick_magic_box, 
					      {BoxID, PlayerPos}, State),
    
    {noreply, NewState}.


%%%===================================================================
%%% 内部函数
%%%===================================================================

%% 距离是否合适
distance_allow_pick(#point{x=X1, y=Y1, z=Z1}, #grid_pos{x=X2, y=Y2, z=Z2}) ->
    X = X1 - X2 * ?GRID_SIZE,
    Y = Y1 - Y2 * ?GRID_SIZE,
    Z = Z1 - Z2 * ?GRID_SIZE,
    
    Distance2 = X * X + Y * Y + Z * Z,
    MaxDis = common_def:get_val(pick_magic_box_distance),
    (Distance2 =< (MaxDis * MaxDis)).


%% 根据玩家等级 算 拾取次数上限
get_max_pick_times(PlayerLevel) when is_integer(PlayerLevel) ->
    #player_level_magic_box_tplt{reward_times=MaxPickTimes} 
	= tplt:get_data2(player_level_magic_box_tplt, PlayerLevel),
    MaxPickTimes.

%% 根据玩家等级 算 能产生多少个宝箱(每天)
get_max_box_per_day(PlayerLevel) when is_integer(PlayerLevel) ->
    #player_level_magic_box_tplt{max_box_per_day=MaxBox} 
	= tplt:get_data2(player_level_magic_box_tplt, PlayerLevel),
    MaxBox.

%% 计算宝箱收获
calc_magic_box_gain(HouseLevel, TodayPick, PlayerData) ->
    PlayerLevel = player_level:get_level(PlayerData),
    PickLimit = get_max_pick_times(PlayerLevel),
    %% 判断拾取上限, 超过了就给基础奖励
    case TodayPick >= PickLimit of
	true ->
	    ItemID = 0,
	    {ItemID, common_def:get_val(magic_box_base_award)};
	_ ->
	    calc_magic_box_gain_1(HouseLevel)
    end.

%% 根据房屋等级算出宝箱的金钱物品
calc_magic_box_gain_1(HouseLevel) ->
    %% 根据房屋等级算出宝箱的金钱物品
    #house_magic_box_tplt{box_ids=BoxIDs, box_rates=BoxRates} 
	= tplt:get_data2(house_magic_box_tplt, HouseLevel),
    
    %% 按机率选一个
    BoxID = util:rate_select_one(BoxRates, BoxIDs),
    case BoxID > 0 of
	true ->
	    #magic_box_tplt{money_range={MinMoney, MaxMoney}, 
			    item_ids=ItemIDs, 
			    item_rates=ItemRates} 
		= tplt:get_data2(magic_box_tplt, BoxID),
	    

	    %% 按机率选一个
	    GainItemID = util:rate_select_one(ItemRates, ItemIDs),
	    
	    GainMoney = rand:uniform(MinMoney, MaxMoney),

	    {GainItemID, GainMoney};
	 _ -> 
	    {0, 0}
    end.


%% 根据房屋等级算出垃圾的金钱物品
calc_garbage_gain(HouseLevel) ->
    %% 根据房屋等级算出垃圾的金钱物品
    #house_garbage_tplt{garbage_ids=BoxIDs, garbage_rates=BoxRates} 
	= tplt:get_data2(house_garbage_tplt, HouseLevel),
    
    %% 按机率选一个
    BoxID = util:rate_select_one(BoxRates, BoxIDs),
    case BoxID > 0 of
	true ->
	    #garbage_tplt{money_range={MinMoney, MaxMoney}, 
			    item_ids=ItemIDs, 
			    item_rates=ItemRates} 
		= tplt:get_data2(garbage_tplt, BoxID),
	    
	    %% 按机率选一个
	    GainItemID = util:rate_select_one(ItemRates, ItemIDs),
	    
	    GainMoney = rand:uniform(MinMoney, MaxMoney),
	    {GainItemID, GainMoney};
	 _ -> 
	    {0, 0}
    end.
	    
%% 取今天的拾取数
get_today_pick(Today, 
	       #player_magic_box{date=Date, pick_box_count=PickCount}) ->    
    case Date =:= Today of
	true -> PickCount;
	_ -> 0
    end.

%% 设置今天的拾取数
set_today_pick(Today, PickCount, 
	       #player_magic_box{account=Account, date=Date} = MagicBoxInfo) ->
    %% 如果日期不一样就重置
    case Date =:= Today of
	true -> 
	    MagicBoxInfo#player_magic_box{pick_box_count=PickCount};
	_ -> 
	    #player_magic_box{account=Account, date=Today, pick_box_count=PickCount}
    end.


%% 设置今天产生的宝箱次数
set_today_generate(Today, GenCount, 
		   #player_magic_box{account=Account, date=Date} = MagicBoxInfo) ->
    %% 如果日期不一样就重置
    case Date =:= Today of
	true -> 
	    MagicBoxInfo#player_magic_box{generate_box_count=GenCount};
	_ -> 
	    #player_magic_box{account=Account, date=Today, generate_box_count=GenCount}
    end.


%% 取今天的产生的宝箱次数
get_today_generate(Today, 
	       #player_magic_box{date=Date, generate_box_count=PickCount}) ->    
    case Date =:= Today of
	true -> PickCount;
	_ -> 0
    end.



%% 判断可否拾取，并做一些操作
pre_pick_magic_box(BoxID, PlayerPos, PlayerData) ->
    SceneName = player_data:get_scene_name(PlayerData),
    
    %% 取宝箱
    case router:send(SceneName, get_magic_box, BoxID) of
	{false, Err} -> %% 有可能宝箱不存在
	    {false, Err};
	#magic_box{pos=BoxPos, type=BoxType} ->	    
	    %% 判断距离
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
				{false, Err} -> %% 有可能宝箱不存在, 也可能不是好友
				    {false, Err};
				Ret ->
				    {ok, Ret}
			    end			    
		    end
	    end
    end.

%% 根据类型判断可否拾取
can_pick_box_type(?mbt_garbage, PlayerData) -> 
    #player_property{active_value=PlayerActive} = player_data:get_property(PlayerData),
    %% 判断活跃值
    case PlayerActive >= common_def:get_val(garbage_cost_player_active) of
	false ->
	    {false, ?err_less_active_can_not_pick};
	_ ->
	    true
    end;    
can_pick_box_type(_BoxType, _PlayerData) -> 
    true.    


%% 拾宝箱
do_pick(?mbt_magic_box, HouseLevel, _HouseClean, _SceneName, PlayerData) ->
    Account = player_data:get_account(PlayerData),
    {Today, _Time} = datetime:local_time(),
    
    MagicBoxInfo = get_player_magic_box_info(Account),

    TodayPick = get_today_pick(Today, MagicBoxInfo),
    
    {ItemID, GainMoney} = calc_magic_box_gain(HouseLevel, TodayPick, PlayerData),
    %% 次数记录
    NewMagicBoxInfo = set_today_pick(Today, TodayPick + 1, MagicBoxInfo),
    
    {ItemID, GainMoney, [NewMagicBoxInfo], PlayerData};

%% 拾垃圾
do_pick(?mbt_garbage, HouseLevel, HouseClean, SceneName, PlayerData) ->

    %% 房屋清洁度为0时，固定获得金钱
    {ItemID, GainMoney} = 
	case HouseClean > 0 of 
	    true ->
		calc_garbage_gain(HouseLevel);
	    _ ->    
		{0, common_def:get_val(garbage_min_gain)}
	end,
    %% 扣活跃值
    OPlayerProperty = player_data:get_property(PlayerData),
    #player_property{active_value=PlayerActive} = OPlayerProperty, 
    Cost = common_def:get_val(garbage_cost_player_active),
    NPlayerProperty = OPlayerProperty#player_property{active_value=PlayerActive - Cost},    
    
    Account = player_data:get_account(PlayerData),
    router:send(player_property, player_property_change_with_notify_client, 
		{Account, ?msg_player_property_by_furniture, NPlayerProperty, OPlayerProperty}),
    
    NPlayerData = player_data:set_property(NPlayerProperty, PlayerData),
    
    %% 房屋日志
    router:cast(SceneName, log_pick_garbage, {Account, ItemID}),
    {ItemID, GainMoney, [NPlayerProperty], NPlayerData}.



do_pick_item(ItemID, PlayerData) ->
    Account = player_data:get_account(PlayerData),
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    ItemCount = 1,
    case ItemID > 0 of
	true ->
	    %% 判断背包是否有空位
	    case pack:add_item(ItemID, ItemCount, Bag) of
		full ->
		    sys_msg:send(Account, ?err_bag_full_can_not_pick),
		    NewItemID = 0,
		    {NewItemID, [], PlayerData};
		NBag -> 
		    %% 获得物品提示
		    sys_msg:send(Account, ?msg_get_item, [item:get_item_name(ItemID), ItemCount]),
		    %% 通知背包更新    
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
	    %% 计算新money
	    NCoin = player_coin:add_coin2(?GAMECOIN, GainMoney, 
					  Coin),
	    
	    NPlayerData = player_data:set_coin(NCoin, PlayerData),
	    NewGainMoney = player_coin:diff_coin(?GAMECOIN, Coin, NCoin),

	    %% 判断钱是否满
	    case NewGainMoney =:= 0 of
		true ->  
		    Account = player_data:get_account(PlayerData),
		    sys_msg:send(Account, ?err_money_full_can_not_pick);
		_ ->
		    %% 通知更新金钱
		    player_coin:send_coin(NCoin)
	    end,	
	    
	    {NewGainMoney, [NCoin], NPlayerData};
	_ ->
	    {GainMoney, [], PlayerData}
    end.



	    

do_pick_magic_box(BoxType, HouseLevel, HouseClean, PlayerData) ->
    Account = player_data:get_account(PlayerData),

    %% 防沉迷提示
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

    %% 数据库操作
    db_common:write(WriteRecords ++ CoinRecords ++ BagRecords),

    %% 物品金钱记录
    ItemCount = 1,
    ItemInfo = {NGainItem, ItemCount, 0},
    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, NGainMoney),
    item_money_log:log(Account, pick_magic_box, ItemInfo, MoneyInfo),

    %% 通知客户端
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
	    %% 每日任务
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


