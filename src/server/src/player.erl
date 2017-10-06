%%% -------------------------------------------------------------------
%%% Author  : NoteBook
%%% Description :
%%%
%%% Created : 2009-9-24
%%% -------------------------------------------------------------------
-module(player).

-behaviour(gen_msgproc).
%% --------------------------------------------------------------------
%% Include files
%% --------------------------------------------------------------------
-include("router.hrl").
-include("table_info.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").
-include("tplt_def.hrl").
-include("packet_def.hrl").
-include("player_data.hrl").
%% --------------------------------------------------------------------
%% External exports

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2, terminate/2, 
	 code_change/3, register/1]).
-export([start_link/5, send_to_around/2
	 %%,process_message/2, get_sex/1
	]).

-export([get_user_name/1, get_player_simple_data/1, kick_guest/3]).
-export([do_open_ui/2, do_get_player_info/1]).

-export([test_start/1]).


-define(SAVE_DATA_TIME, 2 * 60). %%单位：秒

%%---------------------------------启动,关闭玩家------------------------------------------
%% 启动玩家
start_link(MMPid, Account, BasicData, IsFirstTimeLogin, IsWallowAccount) 
  when is_record(BasicData, player_basic_data), is_atom(Account) ->
    %%data_helper:format("start player link:~p~n", [{Account, self()}]),
    gen_msgproc:start_link({local, Account}, ?MODULE,  
			  #player_data{account=Account, 
				       basic_data=BasicData,
				       mm_pid=MMPid,
				       wallow_info=IsWallowAccount,
				       is_first_time_login=IsFirstTimeLogin
				      }, 
			  []).


%% 获得性别
%% -spec get_sex(atom())-> pos_integer().
%% get_sex(Account) when is_atom(Account) ->
%%     gen_server:call(Account, get_sex).

%% 发送消息给指定玩家所在场景的玩家
send_to_around(Account, Packet)->
    gen_server:cast(Account, {broadcast_near, Packet}).

get_user_name(Account) when is_atom(Account) ->
    case util:is_process_alive(Account) of
	false ->
	    db_player:get_user_name(Account);
	_ ->
	    router:send(Account, get_user_name)
    end.    

%% 踢出家里的客人
kick_guest(Account, SceneName, Reason) ->
    Guests = router:send(SceneName, get_players),
    F = fun(Guest) -> 
		case Guest == Account of
		    true ->
			ok;
		    false ->
			router:send(Guest, you_kick_me, {SceneName, Reason})
		end
	end,
    case Guests of
	undefined ->
	    ok;
	_ ->
	    [F(G) || G <- Guests]
    end.

%% --------------------------------------------------------------------
%% Function: init/1
%% Description: Initiates the server
%% Returns: {ok, State}          |
%%          {ok, State, Timeout} |
%%          ignore               |
%%          {stop, Reason}
%% --------------------------------------------------------------------
%% 监听指定的消息
register_player_msg(Account)->
    [
     router:make_event_source(?msg_req_gm_command, Account, {self(), player_gm_cmd}),
     router:make_event_source(?msg_req_chat_around, Account, self()),
     router:make_event_source(?msg_req_chat_big_expression, Account, self()),
     router:make_event_source(?msg_req_chat_tell, Account, self()),
     router:make_event_source(?msg_req_logout, Account, self()),
     router:make_event_source(?msg_req_player_basic_data, Account, self()),
     router:make_event_source(?msg_req_other_player_info, Account, self()),
     router:make_event_source(?msg_req_open_ui, Account, self()),
     router:make_event_source(update_player_data, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_is_active_game, Account, self()),

     router:make_event_target(get_player_info, Account, {self(), ?MODULE}),
     router:make_event_target(req_logout, Account, {self(), ?MODULE}),
     router:make_event_target(get_user_name, Account, {self(), ?MODULE}),
     router:make_event_target(get_pos, Account, {self(), ?MODULE}),
     router:make_event_target(get_scene_name, Account, {self(), ?MODULE}),
     router:make_event_target(get_player_property, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_common_scene_state, Account, {self(), ?MODULE})
    ].


register(#player_data{account=Account})-> 
    data_helper:format("player register:~n"),
    player_pack:register(Account)++
	player_coin:start(Account) ++ 
	mutex_actions:start(Account) ++
	player_task:start(Account) ++
	player_trade:start(Account) ++
	player_invite:start(Account) ++
	player_be_invite:start(Account) ++
	player_walk:start(Account) ++
	player_edit_house:start(Account) ++
	player_change_scene:start(Account) ++
	player_be_kick:start(Account) ++
	player_mail:start(Account) ++
	player_domestic:start(Account) ++
	player_change_looks:start(Account) ++
	player_work:start(Account) ++
	player_search:start(Account) ++
	player_magic_box:start(Account) ++
	player_body_action:start(Account) ++
	player_use_furniture:start(Account) ++
	player_level:start(Account) ++
	sysshop:start(Account) ++
	npcshop:start(Account) ++
	register_player_msg(Account)++
	player_property_by_stime:start(Account) ++
	player_property_by_action:start(Account) ++
	player_property_by_item:start(Account) ++
	player_property_interaction:start(Account) ++
	player_disease:start(Account) ++
	doctor:start(Account) ++
	house_transaction:start(Account) ++
	player_depot:register(Account) ++
	player_house:start(Account) ++
	player_gift_box:start(Account) ++
	player_gift_express:start(Account) ++
	house_exist_gift:start(Account) ++
	player_npc:start(Account) ++ 
	player_change_house:start(Account)++
	player_weibo:start(Account) ++
	player_preview_house:start(Account) ++
	player_flower_egg:start(Account) ++
	player_float_bottle:start(Account).

%% 上线监听模块
-spec login_listeners() -> list().
login_listeners() ->
    [player_mail].
 
init(#player_data{account=Account, is_first_time_login=IsFirstTimeLogin,
		 wallow_info=IsWallowAccount}=State) ->

    data_helper:format("player start :~p, ~p~n", [Account, self()]),
    PlayerData = init_player_data(Account, State),
    %% 发送礼物盒给客户端
    PlayerGift = player_data:get_gift(PlayerData),
    SendBox = player_gift_data:get_send_box(PlayerGift),
    net_helper:send2client(Account, #notify_gift_box{gift=SendBox}),

    OnlineTime = player_data:get_onlinetime(PlayerData),
    LoginTime = player_data:get_login_time(PlayerData),
    player_level:subscribe(Account, OnlineTime, LoginTime),
    TimerRef = player_property_by_stime:subscribe(Account, OnlineTime, LoginTime),
    player_property_interaction:subscribe(Account),
    sys_time:subscribe(?dt_interval, Account, ?SAVE_DATA_TIME, update_player_data),
    player_disease:init(Account),
    player_task:get_task_list(PlayerData),
    
    %% 如果没完成过某号任务，会被强制为白天
    TaskId = common_def:get_val(stop_force_day_task_id),
    case player_task:is_exist_complete_task(TaskId, PlayerData) of
	true -> ok;
	_ -> net_helper:send2client(Account, #notify_day_or_night{flag=?dn_day})	    
    end,
    
    [Mod:notify_login(PlayerData) || Mod <- login_listeners()],
    NPlayerData = player_pack:start(Account, PlayerData),
    NNPlayerData = player_change_scene:enter_player_scene(Account, NPlayerData),
    SceneName = player_data:get_scene_name(NNPlayerData),
    CustomData = router:send(SceneName, get_custom_data),
    router:cast(Account, start_house_gift, house:get_gift_box_vec(CustomData)),
    player_gift_express:send_express_vec(NNPlayerData),
    
    case IsFirstTimeLogin of
	true -> 
	    notify_get_items(Account, NNPlayerData);
	_ -> ok
    end,

    %% 通知货币
    PlayerCoin = player_coin:read(Account),
    player_coin:send_coin(PlayerCoin),

    %% 离线邀请
    player_invite:notify_offline_invite_list(Account),
    
    %% 处理离线消息
    sys_msg:load_offline_sys_msg(Account), 
    NNNPlayerData = player_data:set_property_timer_ref(TimerRef, NNPlayerData),
    WallowInfo = wallow:on_login(Account, eq, IsWallowAccount),

    %% 处理每日任务
    NNNNPlayerData = daily_task:update_today(NNNPlayerData),

    %% 通知基础属性
    BasicData = get_player_basic_data(NNNNPlayerData),
    PlayerProperty = player_data:get_property(NNNNPlayerData),
    net_helper:send2client(Account, #notify_player_basic_data{basic_data=BasicData, 
							      property=PlayerProperty}),
    
    %% 请求EQ用户信息
    router:cast(Account, req_eq_vcard, atom_to_list(Account)),

    {ok, NNNNPlayerData#player_data{wallow_info=WallowInfo}};
init([]) ->
    data_helper:format("player init empty params~n"),
    {ok, #player_data{}}.

%%---------------------------------处理玩家发送的数据-------------------------------------
%%

%% 处理消息
%% MsgType 消息类型
%% NetData 消息对应的结构体
%% process_message(Account, {MsgType, NetData}) when is_atom(Account) ->
%%     gen_server:cast(Account, {MsgType, NetData}).
%% ====================================================================
%% Server functions
%% ====================================================================


%% --------------------------------------------------------------------
%% Function: handle_call/3
%% Description: Handling call messages
%% Returns: {reply, Reply, State}          |
%%          {reply, Reply, State, Timeout} |
%%          {noreply, State}               |
%%          {noreply, State, Timeout}      |
%%          {stop, Reason, Reply, State}   | (terminate/2 is called)
%%          {stop, Reason, State}            (terminate/2 is called)
%% --------------------------------------------------------------------
%% handle_call(get_sex, _From, #player_data{basic_data=BasicData}=State)->
%%     {reply, basic_data:get_data(sex, BasicData), State};
handle_call(#msg{event=get_user_name}, _From, #player_data{basic_data=BasicData}=State) ->
    UserName = BasicData#player_basic_data.username,
    {reply, UserName, State};
handle_call(#msg{event=get_pos}, _From, State) ->
    Pos = player_data:get_pos(State),
    {reply, Pos, State};
handle_call(#msg{event=get_scene_name}, _From, State) ->
    SceneName = player_data:get_scene_name(State),
    {reply, SceneName, State};
%% 取玩家信息
handle_call(#msg{event=get_player_info}, _From, State) ->
    NBasicData = get_player_basic_data(State),
    #player_data{player_property=Property} = State,

    BodyGrids = player_data:get_pack(?pt_body, State),

    {reply, {NBasicData, Property, BodyGrids}, State};

handle_call({#msg{event=get_player_property}, Type}, _From, 
	    #player_data{player_property=PlayerProperty}=State) ->
    Property = player_property:get_player_property(Type, PlayerProperty),
    {reply, Property, State};
%% 请求退出
handle_call(#msg{event=req_logout}, _From, #player_data{mm_pid=MMPid}=State) ->
    Account = player_data:get_account(State),
    Packet = protocal:encode(#notify_repeat_login{account=Account}),
    tcp_mm:send(MMPid, Packet),
    {stop , normal, true, State};
handle_call(Request, _From, State) ->
    data_helper:format("player not process handle_call request:~p~n", [Request]),
    {reply, ok, State}.

%% --------------------------------------------------------------------
%% Function: handle_cast/2
%% Description: Handling cast messages
%% Returns: {noreply, State}          |
%%          {noreply, State, Timeout} |
%%          {stop, Reason, State}            (terminate/2 is called)
%% --------------------------------------------------------------------
handle_cast({broadcast_near, Packet}, #player_data{scene_name=SceneName}=State)->
    router:cast(undefined, SceneName, broadcast_all_players, Packet),
    {noreply, State};
handle_cast({send2client, Packet}, #player_data{mm_pid=MMPid}=State) ->
    tcp_mm:send(MMPid, Packet),
    {noreply, State};
%% 任何一个玩家(包括自己)进入场景, 其他玩家(不包括自己)都会接受到该事件
handle_cast({player_enter_scene, {_HeScene, HeAccount}}, 
	    #player_data{scene_name=SceneName}=State) ->
    case SceneName =:= router:send(HeAccount, get_scene_name) of
	true -> %% 在同一场景才通知
	    Packet = get_player_simple_data(State), 
	    net_helper:send2client(HeAccount, Packet),
	    player_pack:player_enter_scene(HeAccount, State),
	    player_body_action:player_enter_scene(HeAccount, State);		
	false ->
	    ok
    end,
    {noreply, State};
%% 回到自已家中(注：不处理离开场景)
%% 处理进入自己家园
handle_cast(enter_my_home, #player_data{account=Owner}=State)->
    {noreply, player_change_scene:enter_player_scene(Owner, State)};
%% 处理玩家在场景中说话
handle_cast({#msg{event=?msg_req_chat_around}, #req_chat_around{content=Content}}, 
	    #player_data{account=Account, basic_data=BasicData, scene_name=SceneName} = State) ->
    UserName = basic_data:get_data(username, BasicData),    
    Packet = #notify_chat_around{account=atom_to_list(Account), player_name=UserName, content=Content},
    router:cast(undefined, SceneName, broadcast_other_players, {Account, Packet}),
    {noreply, State};
%% 处理玩家在场景中说话
handle_cast({#msg{event=?msg_req_chat_big_expression}, #req_chat_big_expression{exp_id=Content}}, 
	    #player_data{account=Account, basic_data=BasicData, scene_name=SceneName} = State) ->
    UserName = basic_data:get_data(username, BasicData),    
    Packet = #notify_chat_big_expression{account=atom_to_list(Account), player_name=UserName, exp_id=Content},
    router:cast(undefined, SceneName, broadcast_other_players, {Account, Packet}),
    {noreply, State};
%% 查看其他玩家属性
handle_cast({_Msg, #req_other_player_info{account=OtherAccount}}, 
	    #player_data{account=Account}=State) ->
    %% 请求EQ用户信息
    router:cast(Account, req_eq_vcard, OtherAccount),

    TargetPlayer = list_to_atom(OtherAccount),    

    case do_get_player_info(TargetPlayer) of
	undefined ->
	    ok;
	{{BasicData, PlayerProperty, BodyGrids}, IsOnline} ->
	    NBasicData = case IsOnline of
			     true -> BasicData;
			     _ -> calc_offline_level(BasicData)
			 end,

	    %% %% 鲜花数量
	    %% FlowerCount = player_flower_egg:get_flower_count(TargetPlayer),
	    %% net_helper:send2client(Account, #notify_total_flower_count{
	    %% 			     account=OtherAccount,
	    %% 			     flower_count=FlowerCount}),

	    %% %% 家装值
	    %% player_decoration:notify_decoration(Account, TargetPlayer),

	    %% 通知基本信息
	    net_helper:send2client(Account, #notify_other_player_info{
				     player=#player_info{basic_data=NBasicData}, 
				     property=PlayerProperty, 
				    body=BodyGrids}) 	    
    end,
    {noreply, State};
%% 处理玩家对玩家说话(密聊)
handle_cast({#msg{event=?msg_req_chat_tell}, 
	     #req_chat_tell{target_player=TargetAccount, content=Content}},
	    #player_data{account=Account, basic_data=BasicData} = State) ->
    UserName = basic_data:get_data(username, BasicData),    
    Packet = #notify_chat_tell{speaker=atom_to_list(Account), speaker_name=UserName, content=Content},
    TargetPlayer = list_to_atom(TargetAccount),   
    case util:is_process_alive(TargetPlayer) of
	false ->
	    sys_msg:send(Account, ?err_player_offline);
	_ ->
	    net_helper:send2client(TargetPlayer, Packet)
    end,
    {noreply, State};


%% 处理退出场景的请求
handle_cast({#msg{event=?msg_req_logout}, _NetData}, State) ->
    {stop, normal, State};

%% 玩家打开界面
handle_cast({_Msg, #req_open_ui{type=Type}}, State)->
    do_open_ui(Type, State),
    {noreply, State};
handle_cast({_Msg, #req_is_active_game{type=Type}}, #player_data{account=Account}=State)->
    case Type of
	?sgt_farm -> 
	    case db_farm:select([Account]) of
		[[]] -> net_helper:send2client(Account, #notify_active_game_result{result=0});
		_ -> net_helper:send2client(Account, #notify_active_game_result{result=1})
	    end
    end,
    {noreply, State};

handle_cast({#msg{event=?msg_req_player_basic_data}, #req_player_basic_data{}}, 
	    #player_data{account=Account, player_property=PlayerProperty}=State) ->    
    %% 请求EQ用户信息
    router:cast(Account, req_eq_vcard, atom_to_list(Account)),

    %% 鲜花数量
    FlowerCount = player_data:get_flower(State),
    net_helper:send2client(Account, #notify_total_flower_count{
			     account=atom_to_list(Account),
			     flower_count=FlowerCount}),

    %% 家装值
    player_decoration:notify_decoration(Account),

    
    NBasicData = get_player_basic_data(State),
    Packet = #notify_player_basic_data{basic_data=NBasicData, property=PlayerProperty},
    net_helper:send2client(Account, Packet),

    {noreply, State};
%% 更新玩家数据
handle_cast({#msg{event=update_player_data}, _TimerRef}, 
	    #player_data{basic_data=BasicData, player_property=PlayerProperty}=State) ->
    Account = basic_data:get_data(account, BasicData),
    UserName = basic_data:get_data(username, BasicData),
    Sex = basic_data:get_data(sex, BasicData),
    HealthStatus = basic_data:get_data(health_status, BasicData),
    db_player:update(Account, UserName, Sex, PlayerProperty, HealthStatus),
    {noreply, State};

%% 请求场景副本
handle_cast({#msg{event=?msg_req_common_scene_state}, #req_common_scene_state{teleport_id=TeleportID}}, #player_data{account=Account}=State)->
    TeleportTplt = tplt:get_data(teleport_tplt, TeleportID),
    #teleport_tplt{dest_scene_id=SceneId} = TeleportTplt,
    SceneList = ets:foldl(fun({{SID, CID}, _PID, Count, CopyName}, AccOut)-> 
				  case SID=:=SceneId of 
				      true ->
					  Item = tplt:get_data(common_scene,SID),
					  {_, SID, _, _, _, Max,Busy,Loose} = Item,
					  Status = if
						       Count>=Max -> 3;
						       Count>=Busy -> 2;
						       Count>=Loose -> 1;
						       true -> 0
						   end,
			      [#scene_state{scene_id=SID, 
					    copy_id=CID, 
					    count=Count,
					    copy_name=CopyName,
					    status=Status,
					    max_players=Max,
					    busy_players=Busy,
					    loose_players=Loose}|AccOut];
				      _-> 
					  AccOut
				  end 
			  end, [], common_scene_copy),
    Packet = #notify_common_scene_state{scene_list=lists:sort(fun(ItemA, ItemB)-> ItemA#scene_state.status < ItemB#scene_state.status end,SceneList)},
    net_helper:send2client(Account, Packet),
    {noreply, State};

handle_cast(Request, State) ->
    logger:log(error, "handle cast not process, Request:~p~n", [Request]),
    {noreply, State}.

%%---------------------handle info ------------------------------------------
handle_info({timer, Mod, Fun, Arg}, State) ->
    NewState = Mod:Fun(Arg, State),
    {noreply, NewState};
handle_info(_Info, State) ->
    {noreply, State}.

terminate(normal, State) ->
    NewState = do_before_logout(State),
    do_logout(NewState);
terminate(shutdown, State) ->
    NewState = do_before_logout(State),
    do_logout(NewState);
terminate({shutdown, _Reason}, State) ->
    NewState = do_before_logout(State),
    do_logout(NewState);
terminate(_Reason, #player_data{mm_pid=Pid}=State) ->
    NewState = do_before_logout(State),
    do_logout(NewState),
    tcp_mm:stop(Pid).

code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

%%%===================================================================
%%% Internal functions
%%%===================================================================



%% 获取玩家的简单信息
get_player_simple_data(#player_data{basic_data=BasicData, curr_pos=Pos, dest_pos=DPos,
				    used_furni_inst_id=UsedFurniInstId, 
				    used_furni_func_id=UsedFurniFuncId,
				    player_property=Property,
				    used_furni_index = UsedFurniIndex}) ->
    #notify_other_player_data{player=#player_info{basic_data=basic_data:convert(BasicData)}, 
			      curr_pos=Pos, 
			      dest_pos=DPos, 
			      property=Property,
			      used_furni_inst_id=UsedFurniInstId, 
			      used_furni_func_id=UsedFurniFuncId,
			      used_furni_index=UsedFurniIndex}.

%% %% 发送当前玩家数据给场景中的其他玩家
%% send_to_other_players(OperateType, Data) ->
%%     gen_scene:handle_player_message(OperateType, Data).

%% 初始化玩家数据
init_player_data(Account, State) ->
    [BasicData, PlayerProperty, PlayerCoin, PlayerPack, PlayerBody, Depot, Task] = 
	db_common:select_multi_table(Account, [player_basic_data, 
					       player_property,
					       player_coin,
					       player_pack,
					       player_body,
					       player_depot,
					       task
					      ]),

    %% 发现部分账号的player_gift为空, 原因未明，所以在这里将gift_box独立出来处理
    Gift = 
	case db_common:select_one(player_gift, Account) of
	    [] -> player_gift_box:init(Account);
	    GiftRecord -> GiftRecord
	end,

    FlowerCount = player_flower_egg:get_flower_count(Account),
    Bag = pack:unlock_all(player_pack:get_grids(PlayerPack)),
    NState = State#player_data{account=Account, 
    		      basic_data=BasicData,
    		      player_property=PlayerProperty,
    		      login_time=datetime:localtime(),		      
    		      player_coin=PlayerCoin,
    		      bag_grids=Bag,
    		      body_grids=player_pack:get_grids(PlayerBody),
    		      depot=Depot,
		      gift=Gift,
		      flower_count=FlowerCount,
		      task=Task
		     },
    player_every_day_active:calc_offline_gain(NState).

do_before_logout(#player_data{account=Account}=State) ->
    NewState = mutex_actions:clear_actions(logout, State),
    #player_data{scene_name=SceneName} = NewState,    
    router:send(SceneName, leave_scene, Account),
    NewState.

do_logout(#player_data{account=Account, 
		       basic_data=BasicData, login_time=LoginTime,
		       wallow_info=WallowInfo,
		       player_property=PlayerProperty,
		       property_timer_ref=TimerRef}) ->
    data_helper:format("player '~p' logout~n", [Account]),
    cupid:cancel_participate(Account),
    UserName = basic_data:get_data(username, BasicData),
    Sex = basic_data:get_data(sex, BasicData),
    HealthStatus = basic_data:get_data(health_status, BasicData),
    LogoutTime = datetime:localtime(),
    DiffTime = datetime:diff_time(LoginTime, LogoutTime),

    %% 防沉迷, 保存上线时间
    wallow:on_logout(Account, eq, WallowInfo),
    erlang:cancel_timer(TimerRef),
    db_player:update(Account, UserName, Sex, DiffTime, PlayerProperty, HealthStatus, LogoutTime).


do_open_ui(Type, #player_data{account=Account}=State) ->
    case Type of
	?uit_sysshop ->
	    %% Coin = player_data:get_coin(State),
	    PlayerCoin = player_coin:read(Account),
	    player_coin:send_coin(PlayerCoin);
	?uit_farmdepot ->
	    Farm = player_farm:get_farm_data(Account),
	    Pack = player_data:get_pack(?pt_farmdepot, State),
	    pack:send_package(Account, ?pt_farmdepot, Pack),
	    net_helper:send2client(Account, #notify_farm_money{account = Account, 
							       add_money = 0,
							       money = player_farm:get_coin(Farm)});
	?uit_depot -> 
	    player_depot:open(Account, State);
	?uit_bag ->
	    player_pack:send(Account, State),
	    %% PlayerCoin = player_coin:get(Account),
	    Coin = player_data:get_coin(State),
	    player_coin:send_coin(Coin);
	?uit_npcshop ->
	    %% Coin = player_data:get_coin(State),
	    PlayerCoin = player_coin:read(Account),
	    player_coin:send_coin(PlayerCoin);
	_ ->
	    ok
    end,
    net_helper:send2client(Account, #notify_open_ui{type=Type}).


get_player_basic_data(#player_data{basic_data=BasicData, 
			 login_time=LoginTime}) ->
    OnlineTime = basic_data:get_data(online_time, BasicData),
    PlayerLevelTplt = tplt:get_all_data(player_level_tplt),
    Level = player_level:get_level(PlayerLevelTplt, OnlineTime, LoginTime),  
    NBasicData = BasicData#player_basic_data{level=Level},
    basic_data:convert(NBasicData).

calc_offline_level(BasicData) ->
    OnlineTime = basic_data:get_data(online_time, BasicData),
    PlayerLevelTplt = tplt:get_all_data(player_level_tplt),
    Level = player_level:get_level(PlayerLevelTplt, OnlineTime),    
    NBasicData = BasicData#player_basic_data{level=Level},
    NBasicData.


%% 取玩家基本信息 
do_get_player_info(TargetPlayer) when is_atom(TargetPlayer) ->
    case router:send2(TargetPlayer, get_player_info) of
	undefined ->
	    %% 取玩家装备
	    case db_body:get_body(TargetPlayer) of
		undefined -> %% 数据库里没有该玩家
		    undefined;
		Body -> 
		    %% 再去数据库取基本信息
		    [BasicData, PlayerProperty] = 
			db_common:select_multi_table(TargetPlayer, 
						     [player_basic_data, player_property]),
		    {{BasicData, PlayerProperty, Body}, _Online=false}
		end;
	PlayerInfo ->
	    {PlayerInfo, _Online=true}
    end.


%% 提示获得物品
notify_get_items(Account, State) ->
    %% 装备
    Body = player_data:get_pack(?pt_body, State),
    F = fun(_Index, {Item, Count}, Acc) ->
		case Count > 0 of 
		    true -> 
			sys_msg:send(Account, ?msg_get_item, [item:get_item_name(Item), Count]),
			Acc;
		    _ ->			
			Acc
		end
	end,
    pack:foldl(F, [], Body),
    %% 家具
    house:notify_get_items(Account),
    ok.


%%---------------------测试函数--------------------------------------------
%% 测试函数的入口
-spec test_start(atom()) -> any().
test_start(Account)->
    gen_server:start_link({local, Account}, ?MODULE, [], []).
