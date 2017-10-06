%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  鲜花
%%%     注：玩家鲜花数量会被多个进程同时修改，所以要用事务
%%% @end
%%% Created : 16 Sep 2011 by hongjx <hongjx@35info.cn>

-module(player_flower_egg).

-include("sys_msg.hrl").
-include("packet_def.hrl").
-include("router.hrl").
-include("enum_def.hrl").

-export([handle_call/3, handle_cast/2, start/1]).

-export([dec_flower/2, get_flower_count/1, notify_today_flower_egg_count/2]).

-record(player_flower_egg_log, {
	  account,
	  date = {0, 0, 0},  
	  logs = []    %% list of {friend, time}
	 }).

-record(player_flower, {
	  account,
	  flower_count = 0
	  }).

-define(flower_type, 1). 
-define(egg_type, 2). 

%%%===================================================================
%%% Api
%%%===================================================================
start(Account) ->
    [
     router:make_event_source(?msg_req_send_flower, Account, {self(), ?MODULE}),
     router:make_event_target(recv_house_flower, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_flower_logs, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_send_egg, Account, {self(), ?MODULE}),
     router:make_event_target(recv_house_egg, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_egg_logs, Account, {self(), ?MODULE})
    ].


%% 减少鲜花数量
dec_flower(N, State) ->
    %% 操作鲜花总数
    FDecFlower = fun(R) -> 
			 OldVal = R#player_flower.flower_count,
			 R#player_flower{flower_count=OldVal - N}
		 end,
    Account = player_data:get_account(State),    
    Key = Account,
    case db_common:modify(player_flower, Key, FDecFlower, 
			      #player_flower{account=Key}) of
	{ok, #player_flower{flower_count=NewCount}} ->
	    %% 鲜花数量
	    net_helper:send2client(Account, 
				   #notify_total_flower_count{
				     account=atom_to_list(Account),
				     flower_count=NewCount}),

	    player_data:set_flower(NewCount, State)
    end.

get_flower_count(Account) ->
    case db_common:select_one(player_flower, Account) of
	[] ->
	    0;
	#player_flower{flower_count=N} ->
	     N
    end.


notify_today_flower_egg_count(HouseOwner, HouseOwner) ->
    Account = HouseOwner,
    DT = datetime:local_time(),
    {Today, _Time} = DT,
    TodayLog = get_today_record(HouseOwner, Today),
    Logs = TodayLog#player_flower_egg_log.logs,
    TodayFlower = length(get_logs_by_type(Logs, ?flower_type)),
    TodayEgg = length(get_logs_by_type(Logs, ?egg_type)),
    % 通知当天鸡蛋数    
    net_helper:send2client(Account, #notify_house_egg_change{house_owner=atom_to_list(Account), 
					 egg_count=TodayEgg}),

    % 通知当天鲜花数    
    net_helper:send2client(Account, #notify_house_flower_change{house_owner=atom_to_list(Account), 
					 flower_count=TodayFlower});

notify_today_flower_egg_count(Account, HouseOwner) ->
    DT = datetime:local_time(),
    {Today, _Time} = DT,
    TodayLog = get_today_record(HouseOwner, Today),
    Logs = TodayLog#player_flower_egg_log.logs,
    TodayFlower = length(get_logs_by_type(Logs, ?flower_type)),
    TodayEgg = length(get_logs_by_type(Logs, ?egg_type)),
    UserName = db_player:get_user_name(Account),
    case can_send(UserName, TodayLog) of
	true -> %% 通知当天鲜花数    
	    net_helper:send2client(Account, 
				   #notify_today_flower_egg_count{
				     house_owner=atom_to_list(HouseOwner), 
				     flower_count=TodayFlower, 
				     egg_count=TodayEgg});
	_ ->
	    ok
    end.


%%%===================================================================
%%% handle_cast
%%%===================================================================
%% 请求送鸡蛋
handle_cast({_Msg, #req_send_egg{}}, State) ->
    Account = player_data:get_account(State),
    SceneName = player_data:get_scene_name(State),
    NState =
	case router:send(SceneName, get_house_owner) of
	    undefined -> %% 不在房屋中
		State;
	    HouseOwner ->
		BasicData = player_data:get_basic_data(State),
		UserName = basic_data:get_data(username, BasicData),    
		send_egg(Account, UserName, HouseOwner, SceneName, State)
	end,
    {noreply, NState};
%% 请求送花
handle_cast({_Msg, #req_send_flower{}}, State) ->
    Account = player_data:get_account(State),
    SceneName = player_data:get_scene_name(State),
    NState =
	case router:send(SceneName, get_house_owner) of
	    undefined -> %% 不在房屋中
		State;
	    HouseOwner ->
		BasicData = player_data:get_basic_data(State),
		UserName = basic_data:get_data(username, BasicData),    
		
		send_flower(Account, UserName, HouseOwner, SceneName, State)
	end,
    {noreply, NState};

%% 查看自已当天的收到的鸡蛋
handle_cast({_Msg, #req_egg_logs{}}, State) ->
    Account = player_data:get_account(State),
    notify_today_logs(Account, ?egg_type),
    {noreply, State};
%% 查看自已当天的收到的鲜花
handle_cast({_Msg, #req_flower_logs{}}, State) ->
    Account = player_data:get_account(State),
    notify_today_logs(Account, ?flower_type),
    {noreply, State}.

%%%===================================================================
%%% handle_call
%%%===================================================================
handle_call({#msg{event=recv_house_egg}, TodayEgg}, 
	    _From, State) ->
    Account = player_data:get_account(State),

    % 通知当天鸡蛋数    
    net_helper:send2client(Account, #notify_house_egg_change{house_owner=atom_to_list(Account), 
					 egg_count=TodayEgg}),

    {reply, ok, State};
handle_call({#msg{event=recv_house_flower}, {TodayFlower, TotalFlower}}, 
	    _From, State) ->
    Account = player_data:get_account(State),

    % 通知当天鲜花数    
    net_helper:send2client(Account, #notify_house_flower_change{house_owner=atom_to_list(Account), 
					 flower_count=TodayFlower}),
    % 通知鲜花总数    
    net_helper:send2client(Account, #notify_total_flower_count{ 
			     account=atom_to_list(Account),
			     flower_count=TotalFlower}),

    NState = player_data:set_flower(TotalFlower, State),
    {reply, ok, NState}.

%%%===================================================================
%%% 内部函数
%%%===================================================================
get_today_logs(Account, Type) ->
    DT = datetime:local_time(),
    {Today, _Time} = DT,
    TodayLog = get_today_record(Account, Today),
    Logs = TodayLog#player_flower_egg_log.logs,
    get_logs_by_type(Logs, Type).


%% 通知送花记录
notify_today_logs(Account, Type) ->
    Logs = [convert_log(X) || X <- get_today_logs(Account, Type)],
    %% 通知客户端
    case Type of
	?flower_type ->
	    net_helper:send2client(Account, 
				   #notify_flower_logs{logs=Logs});
	?egg_type ->
	    net_helper:send2client(Account, 
				   #notify_egg_logs{logs=Logs})
    end.


%% 好友送鲜花
send_flower(Account, _UserName, Account, _SceneName, State) -> %% 不能给自已送花   
    sys_msg:send(Account, ?err_only_friend_can_send_flower),
    State;    
send_flower(Account, UserName, HouseOwner, SceneName, State) ->    
    Friends = db_friends:get_friends(HouseOwner),
    case lists:member(Account, Friends) of
	false ->
	    sys_msg:send(Account, ?err_only_friend_can_send_flower),
	    State;
	_ ->
	    do_send_flower(Account, UserName, HouseOwner, SceneName, State)	    
    end.
    

do_send_flower(Account, UserName, HouseOwner, SceneName, State) ->
    DT = datetime:local_time(),
    {Today, _Time} = DT,
    KeyVal = {UserName, datetime:make_time(DT), ?flower_type},


    %% 条件判断
    FCondition = fun(R) ->
			 TodayLog = get_today_record(HouseOwner, Today, R), 
			 can_send(UserName, TodayLog)
		 end,
 
    %% 操作当天记录
    FAddLog = fun(R) -> 
		      TodayLog = get_today_record(HouseOwner, Today, R), 

		      Logs = TodayLog#player_flower_egg_log.logs,
 		      TodayLog#player_flower_egg_log{logs=[KeyVal| Logs]}
 	      end,    
    
    %% 操作鲜花总数
    FAddFlower = fun(R) -> 
			 OldVal = R#player_flower.flower_count,
			 R#player_flower{flower_count=OldVal + 1}
		 end,

    %% 将整个事务组织起来
    F = fun() ->
		case db_common:raw_modify_if(player_flower_egg_log, HouseOwner, FAddLog, 
					 #player_flower_egg_log{account=HouseOwner, date=Today}, 
					 FCondition) of
		    {ok, R1} ->			
			case db_common:raw_modify(player_flower, HouseOwner, FAddFlower, 
					     #player_flower{account=HouseOwner}) of
			    {ok, R2} ->
				{ok, R1, R2}
			end;
		    Error -> Error
		end
	end,
    
    %% 提交事务
    case db_common:transaction(F) of
	{ok, NewLogs, NewFlower} -> 
	    TodayFlower = length(get_logs_by_type(NewLogs#player_flower_egg_log.logs, ?flower_type)),
	    TotalFlower = NewFlower#player_flower.flower_count,

	    %% 通知送花的人送花成功
	    notify_send_flower_success(Account, HouseOwner),
	    %% 通知房间里的所有人
	    Packet = #notify_house_flower_change{house_owner=atom_to_list(HouseOwner), 
						 flower_count=TodayFlower},
	    router:cast(undefined, SceneName, broadcast_all_players, Packet),

	    %% 每日任务
	    NewState = daily_task:inc_action_amount(?tat_send_flower, State),

	    %% 通知屋主收到鲜花
	    router:send2(HouseOwner, recv_house_flower, {TodayFlower, TotalFlower}),
	    NewState;
	{false, Err} -> 
	    sys_msg:send(Account, Err),
	    State
    end.
    

%% 取今天记录
get_today_record(Account, Today) when is_atom(Account) ->
    R = db_common:select_one(player_flower_egg_log, Account),
    get_today_record(Account, Today, R).


get_today_record(Account, Today, 
		      #player_flower_egg_log{account=Account, date=Today}=R) ->
    R;
get_today_record(Account, Today, _R) ->
    #player_flower_egg_log{account=Account, date=Today}.


%% 判断是否已可以送鲜花or蛋
can_send(FriendName, #player_flower_egg_log{logs=Logs}) when is_list(FriendName) ->
    case lists:keyfind(FriendName, 1, Logs) of
	{FriendName, _Time, ?flower_type} ->
	    {false, ?err_today_has_send_flower};
	{FriendName, _Time, ?egg_type} ->
	    {false, ?err_today_has_send_egg};
	false ->
	    true
    end.


convert_log({Who, STime, _Type}) when is_list(Who) ->
    #flower_log{time=STime, who=Who}.


get_logs_by_type(Logs, Type) ->
    [X || {_Who, _Time, T}=X <- Logs, T =:= Type].


%% 好友送鸡蛋
send_egg(Account, _UserName, Account, _SceneName, State) -> %% 不能给自已送蛋   
    sys_msg:send(Account, ?err_only_friend_can_send_egg),
    State;    
send_egg(Account, UserName, HouseOwner, SceneName, State) ->    
    Friends = db_friends:get_friends(HouseOwner),
    case lists:member(Account, Friends) of
	false ->
	    sys_msg:send(Account, ?err_only_friend_can_send_egg),
	    State;
	_ ->
	    do_send_egg(Account, UserName, HouseOwner, SceneName, State)	    
    end.
    
do_send_egg(Account, UserName, HouseOwner, SceneName, State) ->
    DT = datetime:local_time(),
    {Today, _Time} = DT,
    KeyVal = {UserName, datetime:make_time(DT), ?egg_type},

    %% 条件判断
    FCondition = fun(R) ->
			 TodayLog = get_today_record(HouseOwner, Today, R), 
			 can_send(UserName, TodayLog)
		 end,
 
    %% 操作当天记录
    FAddLog = fun(R) -> 
		      TodayLog = get_today_record(HouseOwner, Today, R), 
		      Logs = TodayLog#player_flower_egg_log.logs,
 		      TodayLog#player_flower_egg_log{logs=[KeyVal| Logs]}
 	      end,    
    

    %% 提交事务
    case db_common:modify_if(player_flower_egg_log, HouseOwner, FAddLog, 
				 #player_flower_egg_log{account=HouseOwner, date=Today}, 
				 FCondition) of
	{ok, NewLogs} -> 
	    TodayEgg = length(get_logs_by_type(NewLogs#player_flower_egg_log.logs, ?egg_type)),

	    %% 影响清洁度
	    EffectHouseClean = common_def:get_val(egg_effect_house_clean),
	    {_, NHouseData} = router:send(SceneName, add_house_clean, EffectHouseClean),
	    db_common:write(NHouseData),

	    %% 通知送蛋的人送蛋成功
	    notify_send_egg_success(Account, HouseOwner),


	    %% 通知房间里的所有人
	    Packet = #notify_house_egg_change{house_owner=atom_to_list(HouseOwner), 
					      egg_count=TodayEgg},
	    router:cast(undefined, SceneName, broadcast_all_players, Packet),

	    %% 每日任务
	    NewState = daily_task:inc_action_amount(?tat_send_egg, State),

	    %% 通知屋主收到鸡蛋
	    router:send2(HouseOwner, recv_house_egg, TodayEgg),
	    NewState;
	{false, Err} -> 
	    sys_msg:send(Account, Err),
	    State
    end.

notify_send_flower_success(Account, TargetPlayer) ->
    case player:do_get_player_info(TargetPlayer) of
	undefined ->
	    ok;
	{{BasicData, _PlayerProperty, BodyGrids}, _IsOnline} ->	    
	    %% 通知基本信息
	    net_helper:send2client(Account, #notify_send_flower_success{
				     player=#player_info{basic_data=BasicData}, 
				    body=BodyGrids}) 	    
    end.

notify_send_egg_success(Account, TargetPlayer) ->
    case player:do_get_player_info(TargetPlayer) of
	undefined ->
	    ok;
	{{BasicData, _PlayerProperty, BodyGrids}, _IsOnline} ->	    
	    %% 通知基本信息
	    net_helper:send2client(Account, #notify_send_egg_success{
				     player=#player_info{basic_data=BasicData}, 
				    body=BodyGrids}) 	    
    end.
