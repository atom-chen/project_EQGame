%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   宝箱系统(房屋逻辑)
%%% @end
%%% Created : 28 Mar 2011 by hongjx <hongjx@35info.cn>

-module(house_magic_box).

-export([handle_cast/2, handle_call/3, start/1]).

-export([clear/2, on_generate_box/3, save_close_time/1, 
	 reset_garbage_timer/2, timer_create_garbage/2, notify_friend_garbage_info/2,
	 init_house_rand_garbage/1, add_offline_garbage/2]).

-include("router.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").
-include("tplt_def.hrl").
-include("gen_scene.hrl").
-include("enum_def.hrl").

-record(house_magic_box, 
	{house_id,  
	 date = {0, 0, 0},  % 日期    
	 guests = []        % 当天产生过宝箱的访客  
	}).

-record(house_rand_garbage,
	{house_id, 
	 close_time,       % 房屋关闭时间
	 rand_id,          % 最后记录的随机id
	 rand_start_time   % 随机id启用时间
	}).

start(SceneName)->
    [     
     router:make_event_source(fetch_empty_box_positions, SceneName, {self(), ?MODULE}),
     router:make_event_source(log_pick_garbage, SceneName, {self(), ?MODULE}),
     router:make_event_target(friend_has_garbage, SceneName, {self(), ?MODULE}),
     router:make_event_target(get_magic_box_pos, SceneName, {self(), ?MODULE}),
     router:make_event_target(get_magic_box, SceneName, {self(), ?MODULE}),
     router:make_event_target(fetch_magic_box, SceneName, {self(), ?MODULE})
    ].


%%%===================================================================
%%% api
%%%===================================================================

%% 家装时会清理宝箱和垃圾
clear(#house_data{owner=Owner, magic_box_vec=BoxList}=HouseData, Creator) 
  when is_record(HouseData, house_data) ->
    
    %% 清除宝箱和垃圾
    NewHouseData = HouseData#house_data{magic_box_vec=[]},
    db_common:write(NewHouseData),

    %% 通知客户端
    case has_garbage(BoxList) of
	true -> 
	    sys_msg:send(Owner, ?msg_edit_house_clear_garbage),
	    case Creator of
		Owner ->
		    %% 通知好友清除垃圾标志
		    player_friends:broadcast_friends(Owner, #notify_friend_garbage{friend_account=Owner,n=0}); 
		_ -> %% GM好友房间, 只通知玩家就可以了
		    Packet = #notify_friend_garbage{friend_account=gm_friend:get_gm(Creator, Owner), n=0},
		    net_helper:send2client(Creator, Packet)
	    end;
	_ -> ok
    end,
    
    case has_magic_box(BoxList) of
	true -> sys_msg:send(Owner, ?msg_edit_house_clear_magic_box);
	_ -> ok
    end,

    net_helper:send2client(Owner, #notify_house_magic_box{box_list=[]}),

    NewHouseData.

on_generate_box(PlayerAccount, PlayerLevel, HouseData) when is_atom(PlayerAccount) ->
    #house_data{instance_id=HouseID, 
		owner=Owner, 
		magic_box_vec=BoxList, 
		level=HouseLevel} = HouseData,
    {Today, _} = datetime:local_time(),

    MagicBoxCount = get_magic_box_count(BoxList),

    NBoxList = 
	case pre_generate_box(Today, PlayerAccount, PlayerLevel, 
		 HouseID, Owner, HouseLevel, MagicBoxCount) of
	    {MaxHouseBox, MaxPlayerBox, PlayerGenBoxCount, Guests, PlayerMagicBoxInfo} ->
		%% 房屋随机生成数
		HouseRandN = random_box_count(HouseLevel), 
		
		GenCount = erlang:min(HouseRandN, 
				      erlang:min(MaxHouseBox - MagicBoxCount, 
						 MaxPlayerBox - PlayerGenBoxCount)),

		case GenCount > 0 of
		    true ->
			%% 先产生没有坐标的宝箱
			NewBoxList = BoxList ++ [#magic_box{type=?mbt_magic_box} 
						 || _X <- lists:seq(1, GenCount)],
			
			%% 增加房屋宝箱次数记数
			NewGuests = [PlayerAccount | lists:delete(PlayerAccount, Guests)],
			
			NHouseMagicBox = #house_magic_box{house_id=HouseID, 
							  date=Today, guests=NewGuests},
			NPlayerMagicBoxInfo = 
			    player_magic_box:set_today_generate(Today, 
								PlayerGenBoxCount + 1, %% 增加了一次
								PlayerMagicBoxInfo),
			
			%% 数据库操作
			db_common:write([NPlayerMagicBoxInfo, 
					 NHouseMagicBox]),
			

			NewBoxList;
		    _ ->
			BoxList
		end;
	    false ->
		BoxList
    end,


    %% 只要发现宝箱没坐标，就向客户端请求坐标
    case lists:any(fun(X) -> X#magic_box.inst_id =:= 0 end, NBoxList) of 
	true ->
	    %% 向客户端请求空坐标, 因怕位置刚好被人占用，所以多请求了一个位置
	    net_helper:send2client(PlayerAccount, 
				   #req_client_empty_box_positions{need_n=length(NBoxList) + 1});
	_ ->
	    ok
    end,   
    
    HouseData#house_data{magic_box_vec=NBoxList}.
    
save_close_time(HouseID) ->
    Now = datetime:local_time(),
    RandGarbage = get_house_rand_garbage(HouseID),
    set_house_rand_garbage(RandGarbage#house_rand_garbage{close_time=Now}).


%% 玩家进房间替换随机ID, 重设定时器
reset_garbage_timer(HouseID, OldTimer) ->        
    case OldTimer of 
	undefined -> ok;
	_ -> erlang:cancel_timer(OldTimer)
    end,

    ID = rand:uniform(tplt:get_size(random_garbage_tplt)),
    %ID = 1,
    #random_garbage_tplt{interval=TimeList} 
     	= tplt:get_data2(random_garbage_tplt, ID),
    
    TimerMinutes = hd(TimeList),


    %% NextTime = datetime:gregorian_seconds_to_datetime(datetime:datetime_to_gregorian_seconds(
    %% 					     datetime:local_time()) + TimerMinutes * 60),
    %% data_helper:format("next create_garbage ~p~n", [{NextTime, 1}]),
    
    %% 从时间表的第一个开始
    Index = 1, 
    NewTimer = do_after(TimerMinutes * 60, timer_create_garbage, {ID, Index}),    
     
    GarbageInfo = get_house_rand_garbage(HouseID),
    NGarbageInfo = GarbageInfo#house_rand_garbage{rand_id=ID, 
						  rand_start_time=datetime:local_time()},
    {NewTimer, NGarbageInfo}.
    

%% 定时产生垃圾
timer_create_garbage({ID, Index}, 
		     #scene_data{custom_data=SceneData}=State) 
  when is_integer(Index) ->
    %%data_helper:format("timer_create_garbage ~p~n", [{ID, Index}]),
    HouseData = house:get_house_data(SceneData),
    Creator = house:get_creator(SceneData),
    #house_data{owner=Owner} = HouseData,

    #random_garbage_tplt{interval=TimeList, garbage_count=GarbageList} 
     	= tplt:get_data2(random_garbage_tplt, ID),
    
    GarbageCount = lists:nth(Index, GarbageList),
    
    NHouseData = 
	case house:is_editing(SceneData) of
	    true -> % 家装时不产生
		HouseData;
	    _ ->		
		%% 产生垃圾
		case create_garbage(GarbageCount, HouseData) of
		    {NewHouseData, GenCount, OldCount} ->
			%% 数据库操作
			db_common:write(NewHouseData),

			Players = gen_scene:get_players(State),
			after_create_garbage(Owner, OldCount, GenCount, Players, Creator),

			NewHouseData;
		    _ ->
			HouseData
		end  	
	end,

    %% 准备产生下一次垃圾
    %%{TimerSeconds, GarbageCount} = next_garbage(HouseID),    
    {TimerMinutes, NewIndex} = 
	case Index >= length(GarbageList) of
	    true -> {hd(TimeList), 1};
	    _ ->
		Minutes = lists:nth(Index + 1, TimeList) - lists:nth(Index, TimeList),
		{Minutes, Index + 1}
	end,
    
    %% NextTime = datetime:gregorian_seconds_to_datetime(datetime:datetime_to_gregorian_seconds(
    %% 					     datetime:local_time()) + TimerMinutes * 60),
    %% data_helper:format("next create_garbage ~p~n", [{NextTime, NewIndex}]),
    
    %% 启动定时器
    NewTimer = do_after(TimerMinutes * 60, timer_create_garbage, {ID, NewIndex}),    
    
    NSceneData = house:set_garbage_timer(NewTimer, SceneData),
    NewSceneData = house:set_house_data(NSceneData, NHouseData),
    NewState = State#scene_data{custom_data=NewSceneData},
    
    NewState.


% 增加离线时产生的垃圾
add_offline_garbage(#house_data{owner=Owner, instance_id=HouseID}=HouseData, Creator) ->
    GarbageCount = calc_offline_garbage(HouseID),    

    %% 产生垃圾
    case create_garbage(GarbageCount, HouseData) of
	{NewHouseData, GenCount, OldCount} ->
	    Players = [], %% 此时场景还没人
	    after_create_garbage(Owner, OldCount, GenCount, Players, Creator),
	    NewHouseData;
	_ ->
	    HouseData
    end.  	

notify_friend_garbage_info(Account, HouseOwner) ->
    NHouseOwner = 
	case gm_friend:is_gm_account(HouseOwner) of
	    true -> gm_friend:get_house_owner(Account, HouseOwner);
	    _ -> HouseOwner
	end,

    %% 有才通知
    case friend_has_garbage(NHouseOwner) of 
	true ->
	    net_helper:send2client(Account, #notify_friend_garbage{friend_account=HouseOwner, 
								   n=1});
	_ ->
	    ok
    end.


%%%===================================================================
%%% 处理handle_call
%%%===================================================================
%% 取好友家里垃圾个数
handle_call(#msg{event=friend_has_garbage}, _From, 
	    #scene_data{custom_data=SceneData}=State) ->
    #house_data{magic_box_vec=BoxList} = house:get_house_data(SceneData),
    Result = has_garbage(BoxList),
    {reply, Result, State};
%% 取宝箱位置
handle_call({#msg{event=get_magic_box_pos}, BoxID}, _From, 
	    #scene_data{custom_data=SceneData}=State) ->
    HouseData = house:get_house_data(SceneData),
    {reply, get_magic_box_pos(BoxID, HouseData), State};

handle_call({#msg{event=get_magic_box}, BoxID}, _From, 
	    #scene_data{custom_data=SceneData}=State) ->
    HouseData = house:get_house_data(SceneData),    
    {reply, get_magic_box(BoxID, HouseData), State};

%% 取宝箱, 垃圾
handle_call({#msg{event=fetch_magic_box}, {Guest, BoxID}}, _From, 
	    #scene_data{custom_data=SceneData}=State) ->

    Creator = house:get_creator(SceneData),

    HouseData = house:get_house_data(SceneData),
    #house_data{owner=Owner, magic_box_vec=BoxList, 
		level=HouseLevel, house_clean=HouseClean} = HouseData,

    Result = pre_fetch_magic_box(BoxID, BoxList, Guest, Owner, HouseLevel, HouseClean),
    NState = 
	case Result of
	    {false, _Err} ->	
		State;
	    {BoxType, _HouseLevel, _HouseClean} ->
		NewBoxList = lists:keydelete(BoxID, #magic_box.inst_id, BoxList),
		NHouseData = HouseData#house_data{magic_box_vec=NewBoxList},

		NewSceneData = house:set_house_data(SceneData, NHouseData),   
		NewState = State#scene_data{custom_data=NewSceneData},
		
		%% 数据库操作
		db_common:write(NHouseData),
		
		%% 广播删除宝箱
		gen_scene:broadcast_all_players(State, 
						#notify_del_house_magic_box{box_id=BoxID}),
		%% 垃圾被捡光了，要通知
		case (BoxType =:= ?mbt_garbage) and (not has_garbage(NewBoxList)) of
		    true ->
			case Owner of
			    Creator ->
				%% 通知好友没有垃圾
				player_friends:broadcast_friends(Owner, 
								 #notify_friend_garbage{friend_account=Owner, n=0});
			    _ -> %% GM好友房间, 只通知玩家就可以了
				Packet = #notify_friend_garbage{friend_account=gm_friend:get_gm(Creator, Owner), n=0},
				net_helper:send2client(Creator, Packet)
			end;
		    _ -> ok
		end,	       
		NewState
	end,
    
    {reply, Result, NState}.


%%%===================================================================
%%% 处理handle_cast
%%%===================================================================
%% 房屋日志
handle_cast({#msg{event=log_pick_garbage}, {Guest, ItemID}},
	    #scene_data{custom_data=SceneData}=State) ->

    HouseData = house:get_house_data(SceneData),
    #house_data{owner=Owner} = HouseData,
    NHouseData = player_house_log:write(?hvt_pick_garbage, Guest, Owner, ItemID,
			   datetime:localtime(), HouseData),
    
    NewSceneData = house:set_house_data(SceneData, NHouseData),   
    NewState = State#scene_data{custom_data=NewSceneData},
    {noreply, NewState};

%% 取到空位置
handle_cast({#msg{event=fetch_empty_box_positions}, PosList},  
	    #scene_data{custom_data=SceneData}=State) ->
    HouseData = house:get_house_data(SceneData),
    #house_data{magic_box_vec=BoxList} = HouseData,
    
    %% 去掉被占用位置
    FreePosList = filter_pos_list(PosList, HouseData),
    {NewBoxList, Tag} = calc_box_list_positions(BoxList, FreePosList),


    case Tag of
	unchanged ->
	    {noreply, State}; 
	ChangedList ->
	    NewHouseData = HouseData#house_data{magic_box_vec=NewBoxList},
	    NewSceneData = house:set_house_data(SceneData, NewHouseData),
	    NewState = State#scene_data{custom_data=NewSceneData},

	    %% 数据库操作
	    db_common:write(NewHouseData),

	    %% 广播
	    gen_scene:broadcast_all_players(State, 
					    #notify_add_house_magic_box{box_list=ChangedList}),	    
	    
	    {noreply, NewState}
    end.


%%%===================================================================
%%% 内部函数
%%%===================================================================

get_magic_box_pos(BoxID, HouseData) ->
    case get_magic_box(BoxID, HouseData) of
	#magic_box{pos=Pos} -> Pos;
	Err -> Err
    end.  

get_magic_box(BoxID, HouseData) ->
    BoxList = HouseData#house_data.magic_box_vec,
    case lists:keyfind(BoxID, #magic_box.inst_id, BoxList) of
	#magic_box{}=Box ->
	    Box;
	_ ->
	    {false, ?err_not_find_magic_box}
    end.

%% 取房间宝箱信息
get_house_magic_box_info(HouseID, Today) ->
    case db_common:select_one(house_magic_box, HouseID) of
	#house_magic_box{date=Today}=Info ->
	    Info;
	_ ->
	    #house_magic_box{house_id=HouseID, date=Today}
    end.

	    
%% 根据房屋等级, 取宝箱个数限制
get_max_box_limit(HouseLevel) when is_integer(HouseLevel) ->    
    #house_magic_box_tplt{max_box_count=MaxBox} 
	= tplt:get_data2(house_magic_box_tplt, HouseLevel),
    MaxBox.

%% 根据房屋等级 算 能产生多少个宝箱(每次玩家进入)
random_box_count(HouseLevel) when is_integer(HouseLevel) ->    
    #house_magic_box_tplt{player_box_range={Min, Max}} 
	= tplt:get_data2(house_magic_box_tplt, HouseLevel),
    rand:uniform(Min, Max).

%% 根据房屋等级, 取创建宝箱次数限制
get_max_create_times(HouseLevel) when is_integer(HouseLevel) ->    
    #house_magic_box_tplt{max_create_box_times=Max} 
	= tplt:get_data2(house_magic_box_tplt, HouseLevel),
    Max.


%% 给没有实例化的宝箱标上坐标
calc_box_list_positions(BoxList, PosList) ->
    %% 找出实例id为0的宝箱
    {UnInstList, InstList} = lists:partition(fun(X) -> X#magic_box.inst_id =:= 0 end, 
					     BoxList),

    UnInstCount = length(UnInstList),    
    MinLen = erlang:min(length(PosList), UnInstCount),
    case MinLen > 0 of
	true ->
	    %% 对齐位置列表
	    {MinPosList, _} = lists:split(MinLen, PosList),
	    {A, B} = lists:split(MinLen, UnInstList),
	    
	    %% 创建有实例及坐标的宝箱       
	    F = fun(MagicBox, Pos) -> 
			GraphicID = random_graphic_id(MagicBox#magic_box.type),
			
			MagicBox#magic_box{inst_id=guid:make(?st_magic_box), 
					   pos=Pos,
					   graphic_id=GraphicID
					  } 
		end,
	    MinList = lists:zipwith(F, A, MinPosList),

	    %% 合并结果
	    NewBoxList = 
		case UnInstCount > MinLen of
		    true ->
			MinList ++ B ++ InstList ;
		    _ ->
			MinList ++ InstList	    
		end,
	    {NewBoxList, MinList};
	_ ->
	    {BoxList, unchanged}
    end.




%% 判断房屋能否再产生宝箱
pre_generate_box(Today, PlayerAccount, PlayerLevel, 
		 HouseID, Owner, HouseLevel, MagicBoxCount) when is_atom(PlayerAccount) ->
   
    %% 根据房屋等级取 个数限制
    MaxHouseBox = get_max_box_limit(HouseLevel),

    case MagicBoxCount >=  MaxHouseBox of
	true -> % 当前宝箱个数超过限制, 无需处理

	    false;
	_ ->
	    case router:send(PlayerAccount, is_friend, Owner) of
		false -> %% 不是好友, 无需处理

		    false;
		_ ->	    
		    %% 根据玩家等级取 可产生个数		    
		    PlayerMagicBoxInfo = player_magic_box:get_player_magic_box_info(PlayerAccount),
		    %% 取玩家已产生的次数		    
		    PlayerGenBoxCount = player_magic_box:get_today_generate(Today, PlayerMagicBoxInfo),
		    
		    %% 判断玩家能否再产生
		    MaxPlayerBox = player_magic_box:get_max_box_per_day(PlayerLevel),
		    case PlayerGenBoxCount >= MaxPlayerBox of
			true -> %% 玩家产生宝箱次数超限，无需处理
			    false;
			_ ->
			    %% 根据房屋等级取 次数限制
			    MaxHouseBoxTimes = get_max_create_times(HouseLevel),	    
			    %% 当天产生过宝箱的玩家
			    #house_magic_box{guests=Guests} 
				= get_house_magic_box_info(HouseID, Today),
			    %% 取房屋当前宝箱创建次数
			    TodayCreateN = length(Guests),
			    case TodayCreateN >= MaxHouseBoxTimes of
				true -> %% 创建次数超过，无需处理
				    false;
				_ ->
				    case lists:member(PlayerAccount, Guests)  of
					true -> %% 该玩家已创建过宝箱，无需处理
					    false;
					_ ->
					    {MaxHouseBox, MaxPlayerBox, PlayerGenBoxCount, 
					     Guests, PlayerMagicBoxInfo}
				    end    
			    end
		    end
	    end
    end.


get_magic_box_count(BoxList) when is_list(BoxList) ->
    F = fun(#magic_box{type=Type}, Sum) ->
		case Type of
		    ?mbt_magic_box -> Sum + 1;
		    _ -> Sum
		end
	end,
    lists:foldl(F, 0, BoxList).

get_garbage_count(BoxList) when is_list(BoxList) ->
    F = fun(#magic_box{type=Type}, Sum) ->
		case Type of
		    ?mbt_garbage -> Sum + 1;
		    _ -> Sum
		end
	end,
    lists:foldl(F, 0, BoxList).

%% 判断是否存在垃圾
has_garbage(BoxList) when is_list(BoxList) ->
    F = fun(#magic_box{type=Type}) ->
		Type =:= ?mbt_garbage
	end,
    lists:any(F, BoxList).

%% 判断是否存在宝箱
has_magic_box(BoxList) when is_list(BoxList) ->
    F = fun(#magic_box{type=Type}) ->
		Type =:= ?mbt_magic_box
	end,
    lists:any(F, BoxList).

random_graphic_id(?mbt_magic_box) ->
    random:uniform(common_def:get_val(magic_box_graphic_count));
random_graphic_id(?mbt_garbage) ->
    random:uniform(common_def:get_val(garbage_graphic_count)).


get_house_rand_garbage(HouseID) ->
    case db_common:select_one(house_rand_garbage, HouseID) of
	[] -> 
	    init_house_rand_garbage(HouseID);
	R ->
	    R
    end. 

init_house_rand_garbage(HouseID) ->
    Now = datetime:local_time(),
    #house_rand_garbage{house_id=HouseID, 
			close_time=Now,      
			rand_id=1, 
			rand_start_time=Now}.
				    

set_house_rand_garbage(#house_rand_garbage{}=R) ->
    db_common:write(R).

%% %% 计算下一次垃圾产生的数量和产生的时间点
%% next_garbage(HouseID) ->
%%     #house_rand_garbage{rand_id=ID, 
%% 			rand_start_time=StartTime}=get_house_rand_garbage(HouseID),

%%     %% 算启用随ID多长时间了
%%     DiffSeconds = datetime:diff_time(StartTime, datetime:local_time()),
    
%%     DiffMinutes = DiffSeconds div 60,

%%     #random_garbage_tplt{interval=TimeList, garbage_count=GarbageList} 
%% 	= tplt:get_data2(random_garbage_tplt, ID),
    
%%     %% 取最大时间
%%     MaxMinute = lists:last(TimeList),
%%     %% 算出落在哪个时间段
%%     Minutes = trunc(DiffMinutes) rem MaxMinute,    


%%     %% 时间表分成二部分
%%     {LeftList, _RightList} = lists:splitwith(fun(X) -> X < Minutes end, TimeList),
    
%%     MidIdx = length(LeftList) + 1, 
%%     Len = length(TimeList),

%%     Idx = 
%% 	case MidIdx > Len of
%% 	    true -> 1; %% 又从头开始
%% 	    _ -> MidIdx
%% 	end,

%%     %% 下次的垃圾数量
%%     GarbageCount = lists:nth(Idx, GarbageList),

%%     %% 下次定时多久
%%     TimerSeconds = 
%% 	case Idx of
%% 	    1 -> lists:nth(Idx, TimeList) * 60;
%% 	    _ -> (lists:nth(Idx, TimeList) - lists:nth(Idx - 1, TimeList)) * 60
%% 	end,    
%%     {TimerSeconds, GarbageCount}.

%% 产生垃圾
create_garbage(GarbageCount, 
		 #house_data{magic_box_vec=BoxList, owner=Owner,
			     level=HouseLevel} = HouseData) ->

    OldCount = get_garbage_count(BoxList),

    #house_garbage_tplt{max_count=MaxCount} = tplt:get_data2(house_garbage_tplt, HouseLevel),    

    GenCount = erlang:min(MaxCount - OldCount, GarbageCount),

    case GenCount > 0 of
	true ->
	    %% 先产生没有坐标的垃圾
	    NewBoxList = BoxList ++ [#magic_box{type=?mbt_garbage} 
				     || _X <- lists:seq(1, GenCount)],

	    
	    Cost = common_def:get_val(garbage_cost_house_clean) * GenCount,
	    HouseData1 = house_clean:dec(Owner, HouseData, Cost),

	    NewHouseData = HouseData1#house_data{magic_box_vec=NewBoxList},
	    

	    {NewHouseData, GenCount, OldCount};
	_ ->
	    HouseData
    end.   

do_after(RawSeconds, Fun, Arg) ->   
    %%Seconds = RawSeconds div 60,
    Seconds = RawSeconds,
    Event = {timer, ?MODULE, Fun, Arg},
    erlang:send_after(Seconds * 1000, self(), Event).    

after_create_garbage(Owner, OldCount, GenCount, Players, Creator) ->
    case OldCount =:= 0 of
	true ->
	    case Owner of
		Creator ->
		    %% 通知好友有垃圾
		    player_friends:broadcast_friends(Owner, #notify_friend_garbage{friend_account=Owner, n=1});
		_ -> %% GM好友房间, 只通知玩家就可以了
		    Packet = #notify_friend_garbage{friend_account=gm_friend:get_gm(Creator, Owner), n=1},
		    net_helper:send2client(Creator, Packet)
	    end;
	_ -> ok
    end,

    %% 向第一个玩家请求空坐标 
    case length(Players) > 0 of
	true ->
	    PlayerAccount = hd(Players),
	    %% 向客户端请求空坐标, 因怕位置刚好被人占用，所以多请求了一个位置
	    net_helper:send2client(PlayerAccount, 
				   #req_client_empty_box_positions{need_n=GenCount + 1});
	_ ->
	    ok
    end.
    

calc_offline_garbage(HouseID) when is_integer(HouseID) ->
    GarbageInfo = get_house_rand_garbage(HouseID),
    #house_rand_garbage{rand_id=ID, 
			close_time=CloseTime,
			rand_start_time=StartTime}=GarbageInfo,
    
    #random_garbage_tplt{interval=TimeList, garbage_count=GarbageList} 
	= tplt:get_data2(random_garbage_tplt, ID),


    Now = datetime:local_time(),
    do_calc_offline_garbage(TimeList, GarbageList, StartTime, CloseTime, Now).

do_calc_offline_garbage(TimeList, GarbageList, StartTime, CloseTime, Now) ->
    TimeTable = lists:zip(TimeList, GarbageList),    


    %% 算CloseTime 分钟数, 以便算出落在哪个时间段
    CloseMinute = datetime:diff_time(StartTime, CloseTime) div 60,


    %% 取最大时间
    MaxMinute = lists:last(TimeList),
    
    %% 开始遍历的时间点
    MidMinute = (CloseMinute div MaxMinute) * MaxMinute,    
    
    NowMinute = datetime:diff_time(StartTime, Now) div 60,

    %% 遍历时间表, 加总垃圾
    F = fun({T, N}, Sum) ->
		A = MidMinute + T,

		%% 让所有的时间点都大于房屋关闭时间
		Time = case CloseMinute > A of
			true ->  A + MaxMinute;			
			_ -> A
		    end,

		
		%% 加总小于当前时间的垃圾
		case Time =< NowMinute of
		    true ->
			Sum + N;
		    _ ->
			Sum
		end		
	end,

    GarbageCount = lists:foldl(F, 0, TimeTable),
    GarbageCount.


%% 好友的垃圾判定
friend_has_garbage(Owner) when is_atom(Owner) ->
    SceneName = house:get_house_name(Owner),
    case router:send2(SceneName, friend_has_garbage) of 
	undefined -> %% 离线
	    case db_common:select_one(house_data, Owner) of    
		#house_data{instance_id=HouseID, magic_box_vec=BoxList} ->
		    case has_garbage(BoxList) of
			true -> true;
			_ ->
			    GarbageCount = calc_offline_garbage(HouseID),
			    
			    GarbageCount > 0
		    end;
		_ ->
		    false
	    end;
	Result ->
	    Result
    end.

pre_fetch_magic_box(BoxID, BoxList, Guest, Owner, HouseLevel, HouseClean) ->
    case lists:keyfind(BoxID, #magic_box.inst_id, BoxList) of
	false ->
	    %% 返回错误
	    {false, ?err_not_find_magic_box};
	#magic_box{type=BoxType} ->
	    case (Owner =:= Guest) or router:send(Guest, is_friend, Owner) of
		false ->
		    case BoxType of
			?mbt_magic_box ->
			    {false, ?err_only_friend_can_pick_magic_box};
			?mbt_garbage ->
			    {false, ?err_only_friend_can_pick_garbage}
		    end;				
		true ->
		    %% 返回等级（宝箱的奖励跟房屋等级有关）     
		    {BoxType, HouseLevel, HouseClean}
	    end
    end.


%% 去掉被占用位置(家具，礼物，宝箱，垃圾 位置不能重叠)
filter_pos_list(PosList, HouseData) ->
    [Pos || #grid_pos{x=X, z=Z, y=Y}=Pos <- PosList, house:is_pos_empty(X, Y, Z, HouseData)]. 


%%%===================================================================
%%% 测试函数
%%%===================================================================

-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

    %% {{2011,04,14}, {11,00,00}},

calc_offline_garbage1_test() ->
    TimeList = [5, 10, 15, 30], 
    GarbageList = [2, 1, 3, 5], 
    StartTime = {{2011,04,14}, {11,00,00}}, 
    CloseTime = {{2011,04,14}, {12,00,00}}, 
    Now = {{2011,04,14}, {12,31,00}},

    A = do_calc_offline_garbage(TimeList, GarbageList, StartTime, CloseTime, Now),

    ?assertEqual(11, A),
    ok.

calc_offline_garbage2_test() ->
    TimeList = [5, 10, 15, 30], 
    GarbageList = [2, 1, 3, 5], 
    StartTime = {{2011,04,14}, {11,00,00}}, 
    CloseTime = {{2011,04,14}, {11,00,00}}, 
    Now = {{2011,04,14}, {11,09,00}},

    A = do_calc_offline_garbage(TimeList, GarbageList, StartTime, CloseTime, Now),

    ?assertEqual(2, A),
    ok.


calc_offline_garbage3_test() ->
    TimeList = [5, 10, 15, 30], 
    GarbageList = [2, 1, 3, 5], 
    StartTime = {{2011,04,14}, {11,00,00}}, 
    CloseTime = {{2011,04,14}, {11,00,00}}, 
    Now = {{2011,04,14}, {11,10,00}},

    A = do_calc_offline_garbage(TimeList, GarbageList, StartTime, CloseTime, Now),

    ?assertEqual(3, A),
    ok.

calc_offline_garbage4_test() ->
    TimeList = [5, 10, 15, 30], 
    GarbageList = [2, 1, 3, 5], 
    StartTime = {{2011,04,14}, {11,00,00}}, 
    CloseTime = {{2011,04,14}, {12,00,00}}, 
    Now = {{2011,04,14}, {12,10,00}},

    A = do_calc_offline_garbage(TimeList, GarbageList, StartTime, CloseTime, Now),

    ?assertEqual(3, A),
    ok.

calc_offline_garbage5_test() ->
    TimeList = [5, 10, 15, 30], 
    GarbageList = [2, 1, 3, 5], 
    StartTime = {{2011,04,14}, {11,00,00}}, 
    CloseTime = {{2011,04,14}, {11,58,00}}, 
    Now = {{2011,04,14}, {12,01,00}},

    A = do_calc_offline_garbage(TimeList, GarbageList, StartTime, CloseTime, Now),

    ?assertEqual(5, A),
    ok.

calc_offline_garbage6_test() ->
    TimeList = [5, 10, 15, 30], 
    GarbageList = [2, 1, 3, 5], 
    StartTime = {{2011,04,14}, {11,00,00}}, 
    CloseTime = {{2011,04,14}, {11,58,00}}, 
    Now = {{2011,04,14}, {12,16,00}},

    A = do_calc_offline_garbage(TimeList, GarbageList, StartTime, CloseTime, Now),

    ?assertEqual(11, A),
    ok.

-endif.
