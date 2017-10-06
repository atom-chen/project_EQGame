%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   ÿ�����ֵ
%%% @end
%%% Created : 21 Sep 2011 by hongjx <hongjx@35info.cn>

-module(player_every_day_active).
-export([add_active_value/3, calc_offline_gain/1]).

-include("sys_msg.hrl").
-include("enum_def.hrl").

-record(player_visit_friends_log, {
	  account,
	  date = {0, 0, 0},  
	  logs = []    %% list of {friend, time}
	 }).

%% ���������ջ�
calc_offline_gain(State) ->
    %% ÿ�θ���ʱ��
    BasicData = player_data:get_player_basic_data(State),
    LogoutTime = basic_data:get_data(logout_time, BasicData),
    
    Interval = common_def:get_val(offline_seconds_per_active_value),
    
    case LogoutTime == datetime:empty() of
	true -> State;
	_ ->
	    OfflineSeconds = datetime:diff_time(LogoutTime, datetime:localtime()),
	    %%  io:format("call calc_offline_gain offlineSeconds ~p ~n", [OfflineSeconds]),	    
	    
	    %% ������һ��
	    ActiveValue = OfflineSeconds div Interval,
	    
	    %%Account = player_data:get_account(State),

	    NState = update_property(ActiveValue, State),		    
	    %% 

	    NState
    end.


%% ÿ���״ν����Ѽң����ӻ���ֵ
add_active_value(HouseOwner, HouseOwner, PlayerData) ->
    PlayerData;
add_active_value(Account, HouseOwner, PlayerData) ->    
    case router:send(Account, is_friend, HouseOwner) of
	true -> %% ����Ǻ���
	    DT = datetime:local_time(),
	    {Today, _Time} = DT,

	    MaxTimes = common_def:get_val(visit_friend_add_active_max_times),
	    %% �����ж�
	    FCanAdd = fun(R) ->
			      TodayLog = get_today_log(Account, Today, R), 
			      #player_visit_friends_log{logs=Logs} = TodayLog, 
			      case (length(Logs) < MaxTimes) of 
				  false -> {false, ?msg_visit_friend_over_times};
				  _ -> 
				      case lists:member(HouseOwner, Logs) of
					  true -> {false, ?msg_visit_friend_no_gain};
					  _ ->
					      true
				      end
			      end				 
		      end,

	    %% ���Ӱݷü�¼
	    FAddLog = fun(R) ->
			      TodayLog = get_today_log(Account, Today, R), 
			      #player_visit_friends_log{logs=Logs} = TodayLog, 
			      TodayLog#player_visit_friends_log{logs=[HouseOwner | Logs]}    
		      end,

	    %% �����ύ
	    case db_common:modify_if(player_visit_friends_log, Account, FAddLog, 
				 #player_visit_friends_log{account=Account, date=Today}, 
				 FCanAdd) of
		{ok, _NewR} -> 
		    ActiveValue = common_def:get_val(visit_friend_add_active_value),
		    NState = update_property(ActiveValue, PlayerData),
		    %% ÿ������
		    NNState = daily_task:inc_action_amount(?tat_visit_friend, NState),

		    %% ֪ͨ�ͻ��˵õ�����ֵ
		    sys_msg:send(Account, ?msg_visit_friend_gain_active_val, 
				 [integer_to_list(ActiveValue)]),
		    NNState;
		{false, Err} -> 

		    sys_msg:send(Account, Err),
		    PlayerData
	    end;
	_ ->
	    PlayerData
    end.

get_today_log(Account, Today, 
		      #player_visit_friends_log{account=Account, date=Today}=R) ->
    R;
get_today_log(Account, Today, _R) ->
    #player_visit_friends_log{account=Account, date=Today}.

update_property(ActiveValue, PlayerData) ->
    Account = player_data:get_account(PlayerData),

    OPlayerProperty = player_data:get_property(PlayerData),
		    

    NPlayerProperty = player_property_by_action:calculate(OPlayerProperty, 
							  0, 0, 0, ActiveValue, PlayerData),

    %% ֪ͨ�������Ա仯
    router:send(player_property, player_property_change, {Account, NPlayerProperty, OPlayerProperty}),

%%    io:format("update Property:~p~n", [{OPlayerProperty, NPlayerProperty}]),
    player_data:set_property(NPlayerProperty, PlayerData).		
