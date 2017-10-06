%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   ����ϵͳ(�����߼�)
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
	 date = {0, 0, 0},  % ����    
	 guests = []        % �������������ķÿ�  
	}).

-record(house_rand_garbage,
	{house_id, 
	 close_time,       % ���ݹر�ʱ��
	 rand_id,          % ����¼�����id
	 rand_start_time   % ���id����ʱ��
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

%% ��װʱ�������������
clear(#house_data{owner=Owner, magic_box_vec=BoxList}=HouseData, Creator) 
  when is_record(HouseData, house_data) ->
    
    %% ������������
    NewHouseData = HouseData#house_data{magic_box_vec=[]},
    db_common:write(NewHouseData),

    %% ֪ͨ�ͻ���
    case has_garbage(BoxList) of
	true -> 
	    sys_msg:send(Owner, ?msg_edit_house_clear_garbage),
	    case Creator of
		Owner ->
		    %% ֪ͨ�������������־
		    player_friends:broadcast_friends(Owner, #notify_friend_garbage{friend_account=Owner,n=0}); 
		_ -> %% GM���ѷ���, ֻ֪ͨ��ҾͿ�����
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
		%% �������������
		HouseRandN = random_box_count(HouseLevel), 
		
		GenCount = erlang:min(HouseRandN, 
				      erlang:min(MaxHouseBox - MagicBoxCount, 
						 MaxPlayerBox - PlayerGenBoxCount)),

		case GenCount > 0 of
		    true ->
			%% �Ȳ���û������ı���
			NewBoxList = BoxList ++ [#magic_box{type=?mbt_magic_box} 
						 || _X <- lists:seq(1, GenCount)],
			
			%% ���ӷ��ݱ����������
			NewGuests = [PlayerAccount | lists:delete(PlayerAccount, Guests)],
			
			NHouseMagicBox = #house_magic_box{house_id=HouseID, 
							  date=Today, guests=NewGuests},
			NPlayerMagicBoxInfo = 
			    player_magic_box:set_today_generate(Today, 
								PlayerGenBoxCount + 1, %% ������һ��
								PlayerMagicBoxInfo),
			
			%% ���ݿ����
			db_common:write([NPlayerMagicBoxInfo, 
					 NHouseMagicBox]),
			

			NewBoxList;
		    _ ->
			BoxList
		end;
	    false ->
		BoxList
    end,


    %% ֻҪ���ֱ���û���꣬����ͻ�����������
    case lists:any(fun(X) -> X#magic_box.inst_id =:= 0 end, NBoxList) of 
	true ->
	    %% ��ͻ������������, ����λ�øպñ���ռ�ã����Զ�������һ��λ��
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


%% ��ҽ������滻���ID, ���趨ʱ��
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
    
    %% ��ʱ���ĵ�һ����ʼ
    Index = 1, 
    NewTimer = do_after(TimerMinutes * 60, timer_create_garbage, {ID, Index}),    
     
    GarbageInfo = get_house_rand_garbage(HouseID),
    NGarbageInfo = GarbageInfo#house_rand_garbage{rand_id=ID, 
						  rand_start_time=datetime:local_time()},
    {NewTimer, NGarbageInfo}.
    

%% ��ʱ��������
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
	    true -> % ��װʱ������
		HouseData;
	    _ ->		
		%% ��������
		case create_garbage(GarbageCount, HouseData) of
		    {NewHouseData, GenCount, OldCount} ->
			%% ���ݿ����
			db_common:write(NewHouseData),

			Players = gen_scene:get_players(State),
			after_create_garbage(Owner, OldCount, GenCount, Players, Creator),

			NewHouseData;
		    _ ->
			HouseData
		end  	
	end,

    %% ׼��������һ������
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
    
    %% ������ʱ��
    NewTimer = do_after(TimerMinutes * 60, timer_create_garbage, {ID, NewIndex}),    
    
    NSceneData = house:set_garbage_timer(NewTimer, SceneData),
    NewSceneData = house:set_house_data(NSceneData, NHouseData),
    NewState = State#scene_data{custom_data=NewSceneData},
    
    NewState.


% ��������ʱ����������
add_offline_garbage(#house_data{owner=Owner, instance_id=HouseID}=HouseData, Creator) ->
    GarbageCount = calc_offline_garbage(HouseID),    

    %% ��������
    case create_garbage(GarbageCount, HouseData) of
	{NewHouseData, GenCount, OldCount} ->
	    Players = [], %% ��ʱ������û��
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

    %% �в�֪ͨ
    case friend_has_garbage(NHouseOwner) of 
	true ->
	    net_helper:send2client(Account, #notify_friend_garbage{friend_account=HouseOwner, 
								   n=1});
	_ ->
	    ok
    end.


%%%===================================================================
%%% ����handle_call
%%%===================================================================
%% ȡ���Ѽ�����������
handle_call(#msg{event=friend_has_garbage}, _From, 
	    #scene_data{custom_data=SceneData}=State) ->
    #house_data{magic_box_vec=BoxList} = house:get_house_data(SceneData),
    Result = has_garbage(BoxList),
    {reply, Result, State};
%% ȡ����λ��
handle_call({#msg{event=get_magic_box_pos}, BoxID}, _From, 
	    #scene_data{custom_data=SceneData}=State) ->
    HouseData = house:get_house_data(SceneData),
    {reply, get_magic_box_pos(BoxID, HouseData), State};

handle_call({#msg{event=get_magic_box}, BoxID}, _From, 
	    #scene_data{custom_data=SceneData}=State) ->
    HouseData = house:get_house_data(SceneData),    
    {reply, get_magic_box(BoxID, HouseData), State};

%% ȡ����, ����
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
		
		%% ���ݿ����
		db_common:write(NHouseData),
		
		%% �㲥ɾ������
		gen_scene:broadcast_all_players(State, 
						#notify_del_house_magic_box{box_id=BoxID}),
		%% ����������ˣ�Ҫ֪ͨ
		case (BoxType =:= ?mbt_garbage) and (not has_garbage(NewBoxList)) of
		    true ->
			case Owner of
			    Creator ->
				%% ֪ͨ����û������
				player_friends:broadcast_friends(Owner, 
								 #notify_friend_garbage{friend_account=Owner, n=0});
			    _ -> %% GM���ѷ���, ֻ֪ͨ��ҾͿ�����
				Packet = #notify_friend_garbage{friend_account=gm_friend:get_gm(Creator, Owner), n=0},
				net_helper:send2client(Creator, Packet)
			end;
		    _ -> ok
		end,	       
		NewState
	end,
    
    {reply, Result, NState}.


%%%===================================================================
%%% ����handle_cast
%%%===================================================================
%% ������־
handle_cast({#msg{event=log_pick_garbage}, {Guest, ItemID}},
	    #scene_data{custom_data=SceneData}=State) ->

    HouseData = house:get_house_data(SceneData),
    #house_data{owner=Owner} = HouseData,
    NHouseData = player_house_log:write(?hvt_pick_garbage, Guest, Owner, ItemID,
			   datetime:localtime(), HouseData),
    
    NewSceneData = house:set_house_data(SceneData, NHouseData),   
    NewState = State#scene_data{custom_data=NewSceneData},
    {noreply, NewState};

%% ȡ����λ��
handle_cast({#msg{event=fetch_empty_box_positions}, PosList},  
	    #scene_data{custom_data=SceneData}=State) ->
    HouseData = house:get_house_data(SceneData),
    #house_data{magic_box_vec=BoxList} = HouseData,
    
    %% ȥ����ռ��λ��
    FreePosList = filter_pos_list(PosList, HouseData),
    {NewBoxList, Tag} = calc_box_list_positions(BoxList, FreePosList),


    case Tag of
	unchanged ->
	    {noreply, State}; 
	ChangedList ->
	    NewHouseData = HouseData#house_data{magic_box_vec=NewBoxList},
	    NewSceneData = house:set_house_data(SceneData, NewHouseData),
	    NewState = State#scene_data{custom_data=NewSceneData},

	    %% ���ݿ����
	    db_common:write(NewHouseData),

	    %% �㲥
	    gen_scene:broadcast_all_players(State, 
					    #notify_add_house_magic_box{box_list=ChangedList}),	    
	    
	    {noreply, NewState}
    end.


%%%===================================================================
%%% �ڲ�����
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

%% ȡ���䱦����Ϣ
get_house_magic_box_info(HouseID, Today) ->
    case db_common:select_one(house_magic_box, HouseID) of
	#house_magic_box{date=Today}=Info ->
	    Info;
	_ ->
	    #house_magic_box{house_id=HouseID, date=Today}
    end.

	    
%% ���ݷ��ݵȼ�, ȡ�����������
get_max_box_limit(HouseLevel) when is_integer(HouseLevel) ->    
    #house_magic_box_tplt{max_box_count=MaxBox} 
	= tplt:get_data2(house_magic_box_tplt, HouseLevel),
    MaxBox.

%% ���ݷ��ݵȼ� �� �ܲ������ٸ�����(ÿ����ҽ���)
random_box_count(HouseLevel) when is_integer(HouseLevel) ->    
    #house_magic_box_tplt{player_box_range={Min, Max}} 
	= tplt:get_data2(house_magic_box_tplt, HouseLevel),
    rand:uniform(Min, Max).

%% ���ݷ��ݵȼ�, ȡ���������������
get_max_create_times(HouseLevel) when is_integer(HouseLevel) ->    
    #house_magic_box_tplt{max_create_box_times=Max} 
	= tplt:get_data2(house_magic_box_tplt, HouseLevel),
    Max.


%% ��û��ʵ�����ı����������
calc_box_list_positions(BoxList, PosList) ->
    %% �ҳ�ʵ��idΪ0�ı���
    {UnInstList, InstList} = lists:partition(fun(X) -> X#magic_box.inst_id =:= 0 end, 
					     BoxList),

    UnInstCount = length(UnInstList),    
    MinLen = erlang:min(length(PosList), UnInstCount),
    case MinLen > 0 of
	true ->
	    %% ����λ���б�
	    {MinPosList, _} = lists:split(MinLen, PosList),
	    {A, B} = lists:split(MinLen, UnInstList),
	    
	    %% ������ʵ��������ı���       
	    F = fun(MagicBox, Pos) -> 
			GraphicID = random_graphic_id(MagicBox#magic_box.type),
			
			MagicBox#magic_box{inst_id=guid:make(?st_magic_box), 
					   pos=Pos,
					   graphic_id=GraphicID
					  } 
		end,
	    MinList = lists:zipwith(F, A, MinPosList),

	    %% �ϲ����
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




%% �жϷ����ܷ��ٲ�������
pre_generate_box(Today, PlayerAccount, PlayerLevel, 
		 HouseID, Owner, HouseLevel, MagicBoxCount) when is_atom(PlayerAccount) ->
   
    %% ���ݷ��ݵȼ�ȡ ��������
    MaxHouseBox = get_max_box_limit(HouseLevel),

    case MagicBoxCount >=  MaxHouseBox of
	true -> % ��ǰ���������������, ���账��

	    false;
	_ ->
	    case router:send(PlayerAccount, is_friend, Owner) of
		false -> %% ���Ǻ���, ���账��

		    false;
		_ ->	    
		    %% ������ҵȼ�ȡ �ɲ�������		    
		    PlayerMagicBoxInfo = player_magic_box:get_player_magic_box_info(PlayerAccount),
		    %% ȡ����Ѳ����Ĵ���		    
		    PlayerGenBoxCount = player_magic_box:get_today_generate(Today, PlayerMagicBoxInfo),
		    
		    %% �ж�����ܷ��ٲ���
		    MaxPlayerBox = player_magic_box:get_max_box_per_day(PlayerLevel),
		    case PlayerGenBoxCount >= MaxPlayerBox of
			true -> %% ��Ҳ�������������ޣ����账��
			    false;
			_ ->
			    %% ���ݷ��ݵȼ�ȡ ��������
			    MaxHouseBoxTimes = get_max_create_times(HouseLevel),	    
			    %% �����������������
			    #house_magic_box{guests=Guests} 
				= get_house_magic_box_info(HouseID, Today),
			    %% ȡ���ݵ�ǰ���䴴������
			    TodayCreateN = length(Guests),
			    case TodayCreateN >= MaxHouseBoxTimes of
				true -> %% �����������������账��
				    false;
				_ ->
				    case lists:member(PlayerAccount, Guests)  of
					true -> %% ������Ѵ��������䣬���账��
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

%% �ж��Ƿ��������
has_garbage(BoxList) when is_list(BoxList) ->
    F = fun(#magic_box{type=Type}) ->
		Type =:= ?mbt_garbage
	end,
    lists:any(F, BoxList).

%% �ж��Ƿ���ڱ���
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

%% %% ������һ�����������������Ͳ�����ʱ���
%% next_garbage(HouseID) ->
%%     #house_rand_garbage{rand_id=ID, 
%% 			rand_start_time=StartTime}=get_house_rand_garbage(HouseID),

%%     %% ��������ID�೤ʱ����
%%     DiffSeconds = datetime:diff_time(StartTime, datetime:local_time()),
    
%%     DiffMinutes = DiffSeconds div 60,

%%     #random_garbage_tplt{interval=TimeList, garbage_count=GarbageList} 
%% 	= tplt:get_data2(random_garbage_tplt, ID),
    
%%     %% ȡ���ʱ��
%%     MaxMinute = lists:last(TimeList),
%%     %% ��������ĸ�ʱ���
%%     Minutes = trunc(DiffMinutes) rem MaxMinute,    


%%     %% ʱ���ֳɶ�����
%%     {LeftList, _RightList} = lists:splitwith(fun(X) -> X < Minutes end, TimeList),
    
%%     MidIdx = length(LeftList) + 1, 
%%     Len = length(TimeList),

%%     Idx = 
%% 	case MidIdx > Len of
%% 	    true -> 1; %% �ִ�ͷ��ʼ
%% 	    _ -> MidIdx
%% 	end,

%%     %% �´ε���������
%%     GarbageCount = lists:nth(Idx, GarbageList),

%%     %% �´ζ�ʱ���
%%     TimerSeconds = 
%% 	case Idx of
%% 	    1 -> lists:nth(Idx, TimeList) * 60;
%% 	    _ -> (lists:nth(Idx, TimeList) - lists:nth(Idx - 1, TimeList)) * 60
%% 	end,    
%%     {TimerSeconds, GarbageCount}.

%% ��������
create_garbage(GarbageCount, 
		 #house_data{magic_box_vec=BoxList, owner=Owner,
			     level=HouseLevel} = HouseData) ->

    OldCount = get_garbage_count(BoxList),

    #house_garbage_tplt{max_count=MaxCount} = tplt:get_data2(house_garbage_tplt, HouseLevel),    

    GenCount = erlang:min(MaxCount - OldCount, GarbageCount),

    case GenCount > 0 of
	true ->
	    %% �Ȳ���û�����������
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
		    %% ֪ͨ����������
		    player_friends:broadcast_friends(Owner, #notify_friend_garbage{friend_account=Owner, n=1});
		_ -> %% GM���ѷ���, ֻ֪ͨ��ҾͿ�����
		    Packet = #notify_friend_garbage{friend_account=gm_friend:get_gm(Creator, Owner), n=1},
		    net_helper:send2client(Creator, Packet)
	    end;
	_ -> ok
    end,

    %% ���һ�������������� 
    case length(Players) > 0 of
	true ->
	    PlayerAccount = hd(Players),
	    %% ��ͻ������������, ����λ�øպñ���ռ�ã����Զ�������һ��λ��
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


    %% ��CloseTime ������, �Ա���������ĸ�ʱ���
    CloseMinute = datetime:diff_time(StartTime, CloseTime) div 60,


    %% ȡ���ʱ��
    MaxMinute = lists:last(TimeList),
    
    %% ��ʼ������ʱ���
    MidMinute = (CloseMinute div MaxMinute) * MaxMinute,    
    
    NowMinute = datetime:diff_time(StartTime, Now) div 60,

    %% ����ʱ���, ��������
    F = fun({T, N}, Sum) ->
		A = MidMinute + T,

		%% �����е�ʱ��㶼���ڷ��ݹر�ʱ��
		Time = case CloseMinute > A of
			true ->  A + MaxMinute;			
			_ -> A
		    end,

		
		%% ����С�ڵ�ǰʱ�������
		case Time =< NowMinute of
		    true ->
			Sum + N;
		    _ ->
			Sum
		end		
	end,

    GarbageCount = lists:foldl(F, 0, TimeTable),
    GarbageCount.


%% ���ѵ������ж�
friend_has_garbage(Owner) when is_atom(Owner) ->
    SceneName = house:get_house_name(Owner),
    case router:send2(SceneName, friend_has_garbage) of 
	undefined -> %% ����
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
	    %% ���ش���
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
		    %% ���صȼ�������Ľ��������ݵȼ��йأ�     
		    {BoxType, HouseLevel, HouseClean}
	    end
    end.


%% ȥ����ռ��λ��(�Ҿߣ�������䣬���� λ�ò����ص�)
filter_pos_list(PosList, HouseData) ->
    [Pos || #grid_pos{x=X, z=Z, y=Y}=Pos <- PosList, house:is_pos_empty(X, Y, Z, HouseData)]. 


%%%===================================================================
%%% ���Ժ���
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
