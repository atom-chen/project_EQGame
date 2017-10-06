%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   飘流瓶
%%% @end
%%% Created : 23 May 2011 by hongjx <hongjx@35info.cn>

-module(player_float_bottle).

-include("packet_def.hrl").
-include("sys_msg.hrl").
-include("tplt_def.hrl").
-include("common_def.hrl").
-include("enum_def.hrl").


-export([handle_cast/2, start/1]).


start(Account) ->
    [
     router:make_event_source(?msg_req_del_bottle_log, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_all_bottle_info, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_pick_bottle, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_drop_bottle, Account, {self(), ?MODULE})
    ].


%%%===================================================================
%%% api
%%%===================================================================


%%%===================================================================
%%% 处理handle_cast
%%%===================================================================
%% 丢漂流瓶，以便让别人去捡
handle_cast({_Msg, #req_drop_bottle{msg=Msg, 
				    type=Type, 
				    sex=Sex, 
				    account_visible=Visible}}, 
	    State) ->
    Account = player_data:get_account(State),
    Today = datetime:date(),

    MaxDrop = common_def:get_val(max_drop_float_bottle_per_day),   %% 每天最多可以捡多少次飘流瓶
    {PickN, DropN} = get_today_bottle_info(Account, Today),
    RemainDropN = MaxDrop - DropN,

    %% 判断能否再丢瓶子
    case RemainDropN > 0 of
	false ->
    	    sys_msg:send(Account, ?err_drop_float_bottle_fail),
	    {noreply, State};
	_ ->
	    {PickList, DropList} = get_player_bottle_log(Account),
	    AccountStr = case Visible of 
			     ?TRUE -> atom_to_list(Account);
			     _ -> ""
			 end,
    
	    ID = float_bottle_pool:add_bottle(Msg, Type, Sex, Account, Visible),

	    Bottle = #bottle_info{id=ID, msg=Msg, type=Type, time=datetime:localtime(), 
				  account=AccountStr},

	    MaxMsg = common_def:get_val(max_float_bottle_message_count),  %% 飘流瓶信息最大条数

	    %% 超过最大条目，就删除最旧的
	    NDropList =
		case length(DropList) >= MaxMsg of
		    true -> 
			[Bottle | lists:sublist(DropList, 1, length(DropList) - 1)];	
		    _ ->
			[Bottle | DropList]
		end,

	    %% 数据库操作
	    set_player_bottle_log(Account, PickList, NDropList), 
	    set_today_bottle_info(Account, Today, PickN, DropN + 1),

	    MaxPick = common_def:get_val(max_pick_float_bottle_per_day),   %% 每天最多可以捡多少次飘流瓶
	    notify_remain_bottle_info(Account, Today, MaxPick - PickN, MaxDrop - DropN - 1),
	    net_helper:send2client(Account, #notify_drop_bottle{bottle=Bottle}),
	    %% 每日任务
	    NewState = daily_task:inc_action_amount(?tat_drop_float_bottle, State),
	    {noreply, NewState}
    end;

%% 捡漂流瓶
handle_cast({_Msg, #req_pick_bottle{}}, 
	    State) ->
    Account = player_data:get_account(State),
    Today = datetime:date(),

    MaxPick = common_def:get_val(max_pick_float_bottle_per_day),   %% 每天最多可以捡多少次飘流瓶
    {PickN, DropN} = get_today_bottle_info(Account, Today),
    RemainPickN = MaxPick - PickN,

    %% 判断能否再捡飘流瓶
    case RemainPickN > 0 of
	false ->
	    sys_msg:send(Account, ?err_pick_float_bottle_fail),
	    {noreply, State};
	_ ->
	    {PickList, DropList} = get_player_bottle_log(Account),
	    BasicData = player_data:get_basic_data(State),
	    PickerSex = basic_data:get_data(sex, BasicData),
	    %% 从飘流池取飘流瓶
	    case fetch_bottle_from_pool(Account, PickerSex) of
		Bottle=#bottle_info{}->		    
		    MaxMsg = common_def:get_val(max_float_bottle_message_count),  %% 飘流瓶信息最大条数
		    
		    NewBottle = Bottle#bottle_info{time=datetime:localtime()},
		    %% 超过最大条目，就删除最旧的
		    NPickList =
			case length(PickList) >= MaxMsg of
			    true -> 
				[NewBottle | lists:sublist(PickList, 1, length(PickList) - 1)];	
			    _ ->
				[NewBottle | PickList]
			end,
		    
		    %% 数据库操作
		    set_player_bottle_log(Account, NPickList, DropList),
		    
		    net_helper:send2client(Account, #notify_pick_bottle{bottle=NewBottle});
		_ -> 
		    sys_msg:send(Account, ?err_pick_float_bottle_fail)
	    end,

	    %% 增加次数
	    set_today_bottle_info(Account, Today, PickN + 1, DropN),
	    MaxDrop = common_def:get_val(max_drop_float_bottle_per_day), %% 每天最多可以捡多少次飘流瓶
	    notify_remain_bottle_info(Account, Today, MaxPick - PickN - 1, MaxDrop - DropN),	    

	    %% 每日任务
	    NewState = daily_task:inc_action_amount(?tat_pick_float_bottle, State),
	    {noreply, NewState}
    end;
    
%% 删除飘流瓶日志
handle_cast({_Msg, #req_del_bottle_log{id=ID}}, 
	    State) ->
    Account = player_data:get_account(State),
    {PickList, DropList} = get_player_bottle_log(Account),
    

    case remove_id_equal(ID, PickList) of
	{NPickList, true} ->
	    set_player_bottle_log(Account, NPickList, DropList);
	_ -> ok
    end,
    
    case remove_id_equal(ID, DropList) of
	{NDropList, true} ->
	    set_player_bottle_log(Account, PickList, NDropList);
	_ -> ok
    end,

    {noreply, State};
%% 请求飘流瓶信息
handle_cast({_Msg, #req_all_bottle_info{}}, 
	    State) ->
    Account = player_data:get_account(State),
    {PickList, DropList} = get_player_bottle_log(Account),

    %% 瓶子信息
    Packet = #notify_bottles_info{pick_bottles=PickList,  % 捞到的瓶子信息
				  drop_bottles=DropList}, % 丢出的瓶子信息
    net_helper:send2client(Account, Packet),
    
    %% 玩家当天的飘流瓶相关信息
    Today = datetime:date(),
    {RemainPickN, RemainDropN} = get_today_remain_bottle(Account, Today),

    notify_remain_bottle_info(Account, Today, RemainPickN, RemainDropN),
    
    {noreply, State}.

%%%===================================================================
%%% 内部函数
%%%===================================================================

notify_remain_bottle_info(Account, Today, RemainPickN, RemainDropN) ->
    {Y, M, D} = Today,
    TodayInfo = #notify_today_bottle_info{    
      year=Y,           % 当天日期(因存在跨天情况，所以需要知道时间)
      month=M,          % 
      day=D,            % 
      pick_count=RemainPickN, % 可再打捞次数
      drop_count=RemainDropN  % 可再丢瓶次数
     },
    net_helper:send2client(Account, TodayInfo).

%% %% 删除某个元素, 下标从1开始
%% lists_remove_at(Idx, List) ->
%%     lists:sublist(List, 1, Idx - 1)
%% 		++ lists:nthtail(Idx, List).

%% 删除包含ID的数据
remove_id_equal(DelID, List) ->
    NList = [X || #bottle_info{id=ID}=X <- List, ID =/= DelID],
    Changed = length(NList) =/= length(List),
    {NList, Changed}.

%% 从飘流池取飘流瓶
fetch_bottle_from_pool(Picker, PickerSex) ->
    %% 飘流瓶的机率
    Rate = common_def:get_val(pick_float_bottle_rate),    
    
    Hit = rand:uniform(100) =< Rate,
    case Hit of
	true -> %% 命中	    
	    do_fetch_bottle_from_pool(Picker, PickerSex);
	_ -> 
	    []
    end.

do_fetch_bottle_from_pool(Picker, PickerSex) when is_atom(Picker) ->
    float_bottle_pool:rand_pick_bottle(Picker, PickerSex).


%% 今天 可再打捞次数 和 可再丢瓶次数
get_today_remain_bottle(Account, Today) ->
    MaxDrop = common_def:get_val(max_drop_float_bottle_per_day),   %% 每天最多可以扔多少次飘流瓶
    MaxPick = common_def:get_val(max_pick_float_bottle_per_day),   %% 每天最多可以捡多少次飘流瓶
    {PickN, DropN} = get_today_bottle_info(Account, Today),
    {MaxPick - PickN, MaxDrop - DropN}.


%%%===================================================================
%%% 数据库相关
%%%===================================================================
-record(player_bottle_log, {account, pick_list, drop_list}).
set_player_bottle_log(Account, PickList, DropList) -> 
    db_common:write(#player_bottle_log{account=Account, pick_list=PickList, drop_list=DropList}).

get_player_bottle_log(Account) ->
    case db_common:select_one(player_bottle_log, Account) of
	#player_bottle_log{pick_list=Picks, drop_list=Drops} -> {Picks, Drops};
	_ -> {[], []}
    end.

-record(player_bottle_info, {account, today, pick_count, drop_count}). 
%% 今天已 打捞次数 和 丢瓶次数
get_today_bottle_info(Account, Today) ->
    case db_common:select_one(player_bottle_info, Account) of
	#player_bottle_info{today=Today, pick_count=PickCount, drop_count=DropCount} -> 
	    {PickCount, DropCount};
	_ -> 
	    {0, 0}
    end.

set_today_bottle_info(Account, Today, PickCount, DropCount) ->
    db_common:write(#player_bottle_info{account=Account, 
					today=Today,
					pick_count=PickCount,
					drop_count=DropCount
				       }).





