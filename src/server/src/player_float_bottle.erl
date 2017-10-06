%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   Ʈ��ƿ
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
%%% ����handle_cast
%%%===================================================================
%% ��Ư��ƿ���Ա��ñ���ȥ��
handle_cast({_Msg, #req_drop_bottle{msg=Msg, 
				    type=Type, 
				    sex=Sex, 
				    account_visible=Visible}}, 
	    State) ->
    Account = player_data:get_account(State),
    Today = datetime:date(),

    MaxDrop = common_def:get_val(max_drop_float_bottle_per_day),   %% ÿ�������Լ���ٴ�Ʈ��ƿ
    {PickN, DropN} = get_today_bottle_info(Account, Today),
    RemainDropN = MaxDrop - DropN,

    %% �ж��ܷ��ٶ�ƿ��
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

	    MaxMsg = common_def:get_val(max_float_bottle_message_count),  %% Ʈ��ƿ��Ϣ�������

	    %% ���������Ŀ����ɾ����ɵ�
	    NDropList =
		case length(DropList) >= MaxMsg of
		    true -> 
			[Bottle | lists:sublist(DropList, 1, length(DropList) - 1)];	
		    _ ->
			[Bottle | DropList]
		end,

	    %% ���ݿ����
	    set_player_bottle_log(Account, PickList, NDropList), 
	    set_today_bottle_info(Account, Today, PickN, DropN + 1),

	    MaxPick = common_def:get_val(max_pick_float_bottle_per_day),   %% ÿ�������Լ���ٴ�Ʈ��ƿ
	    notify_remain_bottle_info(Account, Today, MaxPick - PickN, MaxDrop - DropN - 1),
	    net_helper:send2client(Account, #notify_drop_bottle{bottle=Bottle}),
	    %% ÿ������
	    NewState = daily_task:inc_action_amount(?tat_drop_float_bottle, State),
	    {noreply, NewState}
    end;

%% ��Ư��ƿ
handle_cast({_Msg, #req_pick_bottle{}}, 
	    State) ->
    Account = player_data:get_account(State),
    Today = datetime:date(),

    MaxPick = common_def:get_val(max_pick_float_bottle_per_day),   %% ÿ�������Լ���ٴ�Ʈ��ƿ
    {PickN, DropN} = get_today_bottle_info(Account, Today),
    RemainPickN = MaxPick - PickN,

    %% �ж��ܷ��ټ�Ʈ��ƿ
    case RemainPickN > 0 of
	false ->
	    sys_msg:send(Account, ?err_pick_float_bottle_fail),
	    {noreply, State};
	_ ->
	    {PickList, DropList} = get_player_bottle_log(Account),
	    BasicData = player_data:get_basic_data(State),
	    PickerSex = basic_data:get_data(sex, BasicData),
	    %% ��Ʈ����ȡƮ��ƿ
	    case fetch_bottle_from_pool(Account, PickerSex) of
		Bottle=#bottle_info{}->		    
		    MaxMsg = common_def:get_val(max_float_bottle_message_count),  %% Ʈ��ƿ��Ϣ�������
		    
		    NewBottle = Bottle#bottle_info{time=datetime:localtime()},
		    %% ���������Ŀ����ɾ����ɵ�
		    NPickList =
			case length(PickList) >= MaxMsg of
			    true -> 
				[NewBottle | lists:sublist(PickList, 1, length(PickList) - 1)];	
			    _ ->
				[NewBottle | PickList]
			end,
		    
		    %% ���ݿ����
		    set_player_bottle_log(Account, NPickList, DropList),
		    
		    net_helper:send2client(Account, #notify_pick_bottle{bottle=NewBottle});
		_ -> 
		    sys_msg:send(Account, ?err_pick_float_bottle_fail)
	    end,

	    %% ���Ӵ���
	    set_today_bottle_info(Account, Today, PickN + 1, DropN),
	    MaxDrop = common_def:get_val(max_drop_float_bottle_per_day), %% ÿ�������Լ���ٴ�Ʈ��ƿ
	    notify_remain_bottle_info(Account, Today, MaxPick - PickN - 1, MaxDrop - DropN),	    

	    %% ÿ������
	    NewState = daily_task:inc_action_amount(?tat_pick_float_bottle, State),
	    {noreply, NewState}
    end;
    
%% ɾ��Ʈ��ƿ��־
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
%% ����Ʈ��ƿ��Ϣ
handle_cast({_Msg, #req_all_bottle_info{}}, 
	    State) ->
    Account = player_data:get_account(State),
    {PickList, DropList} = get_player_bottle_log(Account),

    %% ƿ����Ϣ
    Packet = #notify_bottles_info{pick_bottles=PickList,  % �̵���ƿ����Ϣ
				  drop_bottles=DropList}, % ������ƿ����Ϣ
    net_helper:send2client(Account, Packet),
    
    %% ��ҵ����Ʈ��ƿ�����Ϣ
    Today = datetime:date(),
    {RemainPickN, RemainDropN} = get_today_remain_bottle(Account, Today),

    notify_remain_bottle_info(Account, Today, RemainPickN, RemainDropN),
    
    {noreply, State}.

%%%===================================================================
%%% �ڲ�����
%%%===================================================================

notify_remain_bottle_info(Account, Today, RemainPickN, RemainDropN) ->
    {Y, M, D} = Today,
    TodayInfo = #notify_today_bottle_info{    
      year=Y,           % ��������(����ڿ��������������Ҫ֪��ʱ��)
      month=M,          % 
      day=D,            % 
      pick_count=RemainPickN, % ���ٴ��̴���
      drop_count=RemainDropN  % ���ٶ�ƿ����
     },
    net_helper:send2client(Account, TodayInfo).

%% %% ɾ��ĳ��Ԫ��, �±��1��ʼ
%% lists_remove_at(Idx, List) ->
%%     lists:sublist(List, 1, Idx - 1)
%% 		++ lists:nthtail(Idx, List).

%% ɾ������ID������
remove_id_equal(DelID, List) ->
    NList = [X || #bottle_info{id=ID}=X <- List, ID =/= DelID],
    Changed = length(NList) =/= length(List),
    {NList, Changed}.

%% ��Ʈ����ȡƮ��ƿ
fetch_bottle_from_pool(Picker, PickerSex) ->
    %% Ʈ��ƿ�Ļ���
    Rate = common_def:get_val(pick_float_bottle_rate),    
    
    Hit = rand:uniform(100) =< Rate,
    case Hit of
	true -> %% ����	    
	    do_fetch_bottle_from_pool(Picker, PickerSex);
	_ -> 
	    []
    end.

do_fetch_bottle_from_pool(Picker, PickerSex) when is_atom(Picker) ->
    float_bottle_pool:rand_pick_bottle(Picker, PickerSex).


%% ���� ���ٴ��̴��� �� ���ٶ�ƿ����
get_today_remain_bottle(Account, Today) ->
    MaxDrop = common_def:get_val(max_drop_float_bottle_per_day),   %% ÿ���������Ӷ��ٴ�Ʈ��ƿ
    MaxPick = common_def:get_val(max_pick_float_bottle_per_day),   %% ÿ�������Լ���ٴ�Ʈ��ƿ
    {PickN, DropN} = get_today_bottle_info(Account, Today),
    {MaxPick - PickN, MaxDrop - DropN}.


%%%===================================================================
%%% ���ݿ����
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
%% ������ ���̴��� �� ��ƿ����
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





