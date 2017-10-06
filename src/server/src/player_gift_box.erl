%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 礼物系统-礼盒
%%% @end
%%% Created : 29 Nov 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(player_gift_box).

-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("player_data.hrl").

-behaviour(player_action).

-export([start/1, init/1, handle_call/3, handle_cast/2]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).

-define(MAXLOG, 1000).

%% API
start(Account) ->
    [
     router:make_event_source(?msg_req_start_gift_service, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_end_gift_service, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_get_send_box_vec, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_get_receive_box_vec, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_get_viewed_box_vec, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_clean_gift_log, Account, {self(), ?MODULE}),
     router:make_event_target(set_receive_box_vec, Account, {self(), ?MODULE}),
     router:make_event_target(delete_receive_box, Account, {self(), ?MODULE})
    ].

%% 初始化礼物
-spec init(atom()) -> tuple().
init(Account) ->
    player_gift_data:init(Account).
%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(Account, Pid) ->
    [
     router:make_event_source(?msg_req_pack_gift_box, Account, {Pid, ?MODULE}),
     router:make_event_source(?msg_req_unpack_gift_box, Account, {Pid, ?MODULE}),
     router:make_event_source(?msg_req_put_gift_box, Account, {Pid, ?MODULE}),
     router:make_event_source(?msg_req_pickup_gift_box, Account, {Pid, ?MODULE}),
     router:make_event_source(?msg_req_send_gift_express, Account, {Pid, player_gift_express}),
     router:make_event_source(?msg_req_receive_gift_express, Account, {Pid, player_gift_express})
    ].

can_start_action(_Param, _State) ->
    true.

do_start_action(_Param, State) ->
    Account = player_data:get_account(State),
    Type = player_data:get_gift_service_type(State),
    net_helper:send2client(Account, #notify_start_gift_service{type=Type}),  
    State.

%% 结束动作
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action({auto_stop, _Reason}, State) ->
    do_stop_action(stop, State);

%% 结束动作
do_stop_action(stop, State) ->
    Account = player_data:get_account(State),
    Type = player_data:get_gift_service_type(State),
    net_helper:send2client(Account, #notify_end_gift_service{type=Type}),
    player_data:set_gift_service_type(?gst_normal, State).

%%%===================================================================
%%% Handle call
%%%===================================================================
handle_call({#msg{event=set_receive_box_vec}, SendBox}, _From, State) ->
    data_helper:format("set receive box vec~n"),
    PlayerGift = player_data:get_gift(State),
    ReceiveBoxVec = player_gift_data:get_receive_box_vec(PlayerGift),
    NReceiveBoxVec = [SendBox | ReceiveBoxVec],
    NPlayerGift = player_gift_data:set_receive_box_vec(PlayerGift, NReceiveBoxVec),
    NState = player_data:set_gift(NPlayerGift, State),
    {reply, NPlayerGift, NState};
handle_call({#msg{event=delete_receive_box}, SendBox}, _From, State) ->
    PlayerGift = player_data:get_gift(State),
    ReceiveBoxVec = player_gift_data:get_receive_box_vec(PlayerGift),
    NReceiveBoxVec = lists:delete(SendBox, ReceiveBoxVec),
    NPlayerGift = player_gift_data:set_receive_box_vec(PlayerGift, NReceiveBoxVec),
    NState = player_data:set_gift(NPlayerGift, State),
    {reply, NPlayerGift, NState}.

%%%===================================================================
%%% Handle cast
%%%===================================================================
%% 请求礼物服务
handle_cast({_Msg, #req_start_gift_service{type=Type}}, State) ->  
    NState = player_data:set_gift_service_type(Type, State),
    NNState = mutex_actions:try_start_action(?action_type_gift, undefined, NState),
    {noreply, NNState};

%% 请求结束礼物服务
handle_cast({_Msg, #req_end_gift_service{}}, State) ->
    NState = mutex_actions:call_stop_action(?action_type_gift, stop, State),
    {noreply, NState};

%% 请求清空日志
handle_cast({_Msg, #req_clean_gift_log{type=Type}}, State) ->
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    NState = do_clean_gift_log(Type, Account, PlayerGift, State),
    {noreply, NState};

%% 请求获取发送礼物盒列表
handle_cast({_Msg, #req_get_send_box_vec{page_index=PageIndex, page_count=PageCount}}, State) ->
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    SendBoxVec = player_gift_data:get_send_box_vec(PlayerGift),
    NNPlayerGift = 
	case length(SendBoxVec) > ?MAXLOG of
	    true ->
		NSendBoxVec = lists:sublist(SendBoxVec, ?MAXLOG),
		NPlayerGift = player_gift_data:set_send_box_vec(PlayerGift, NSendBoxVec),
		NNSendBoxVec = lists:sublist(NSendBoxVec, 
					     player_gift:get_index(PageIndex, PageCount), 
					     PageCount),
		NNNSendBoxVec = player_gift:get_name(NNSendBoxVec, []),
		net_helper:send2client(Account, #notify_get_send_box_vec{
					 send_box_vec=NNNSendBoxVec,
					 total=length(NSendBoxVec)}),
		db_common:write(NPlayerGift),
		NPlayerGift;
	    false ->
		NSendBoxVec = lists:sublist(SendBoxVec, 
					     player_gift:get_index(PageIndex, PageCount), 
					     PageCount),
		NNSendBoxVec = player_gift:get_name(NSendBoxVec, []),
		net_helper:send2client(Account, #notify_get_send_box_vec{
					 send_box_vec=NNSendBoxVec,
					 total=length(SendBoxVec)}),
		PlayerGift
	end,
    NState = player_data:set_gift(NNPlayerGift, State),
    {noreply, NState};

%% 请求获取接受礼物盒列表
handle_cast({_Msg, #req_get_receive_box_vec{page_index=PageIndex, page_count=PageCount}}, State) ->
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    ReceiveBoxVec = player_gift_data:get_receive_box_vec(PlayerGift),
    NNPlayerGift = 
	case length(ReceiveBoxVec) > ?MAXLOG of
	    true ->
		NReceiveBoxVec = lists:sublist(ReceiveBoxVec, ?MAXLOG),
		NPlayerGift = player_gift_data:set_receive_box_vec(PlayerGift, NReceiveBoxVec),
		NNReceiveBoxVec = lists:sublist(NReceiveBoxVec, 
						player_gift:get_index(PageIndex, PageCount), 
						PageCount),
		NNNReceiveBoxVec = player_gift:get_name(NNReceiveBoxVec, []),
		net_helper:send2client(Account, #notify_get_receive_box_vec{
					 receive_box_vec=NNNReceiveBoxVec,
					 total=length(NReceiveBoxVec)}),
		db_common:write(NPlayerGift),
		NPlayerGift;
	    false ->
		NReceiveBoxVec = lists:sublist(ReceiveBoxVec, 
						player_gift:get_index(PageIndex, PageCount), 
						PageCount),
		NNReceiveBoxVec = player_gift:get_name(NReceiveBoxVec, []),
		net_helper:send2client(Account, #notify_get_receive_box_vec{
					 receive_box_vec=NNReceiveBoxVec,
					 total=length(ReceiveBoxVec)}),
		PlayerGift
	end,
    NState = player_data:set_gift(NNPlayerGift, State),
    {noreply, NState};

%% 请求获取查看过的礼物盒列表
handle_cast({_Msg, #req_get_viewed_box_vec{page_index=PageIndex, page_count=PageCount}}, State) ->
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    ViewedBoxVec = player_gift_data:get_viewed_box_vec(PlayerGift),
    NNPlayerGift = 
    case length(ViewedBoxVec) > ?MAXLOG of
	true ->
	    NViewedBoxVec = lists:sublist(ViewedBoxVec, ?MAXLOG),
	    NPlayerGift = player_gift_data:set_viewed_box_vec(PlayerGift, NViewedBoxVec),
	    NNViewedBoxVec = lists:sublist(NViewedBoxVec, 
					   player_gift:get_index(PageIndex, PageCount), 
					   PageCount), 
	    NNNViewedBoxVec = player_gift:get_name(NNViewedBoxVec, []),
	    net_helper:send2client(Account, 
				   #notify_get_viewed_box_vec{
				     viewed_box_vec=NNNViewedBoxVec,
				     total=length(NViewedBoxVec)}),
	    db_common:write(NPlayerGift),
	    NPlayerGift;
	false ->
	    NViewedBoxVec = lists:sublist(ViewedBoxVec, 
					   player_gift:get_index(PageIndex, PageCount), 
					   PageCount), 
	    NNViewedBoxVec = player_gift:get_name(NViewedBoxVec, []),
	    net_helper:send2client(Account, #notify_get_viewed_box_vec{
				     viewed_box_vec=NNViewedBoxVec, 
				     total=length(ViewedBoxVec)}),
	    PlayerGift
    end,
    NState = player_data:set_gift(NNPlayerGift, State),
    {noreply, NState};

%% 请求制作礼物 
handle_cast({_Msg, #req_pack_gift_box{gift=Gift}=PackGiftBox}, State) ->
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    SendBox = player_gift_data:get_send_box(PlayerGift),
    NState =
	case box_is_empty(SendBox) of
	    true ->
		{GameCoin, EQCoin, PlayerGameCoin, PlayerEQCoin} = player_gift:get_coin(Gift, State),
		case can_pack_gift_box(PlayerGift, Gift, GameCoin, EQCoin, 
				       PlayerGameCoin, PlayerEQCoin, PackGiftBox, State) of
		    true ->
			State1 = do_pack_gift_box(PackGiftBox, Account, GameCoin, EQCoin, PlayerGift, State),
			%% 后台金钱物品记录
			MoneyInfo = {-GameCoin, -EQCoin},
			item_money_log:log_money(Account, pack_gift_box, MoneyInfo),
			State1;
		    {false, Reason} ->
			sys_msg:send(Account, Reason),
			State
		end;
	    false ->
		sys_msg:send(Account, ?msg_gift_err_pack),
		State
	end,
    {noreply, NState};
%% 请求取消礼物
handle_cast({_Msg, #req_unpack_gift_box{}}, #player_data{account=Account}=State) ->
    PlayerGift = player_data:get_gift(State),
    SendBox = player_gift_data:get_send_box(PlayerGift),
    NState = 
	case box_is_empty(SendBox) of
	    true ->
		sys_msg:send(Account, ?msg_gift_err_unpack),
		State;
	    false ->
		PlayerGiftPack = player_gift_data:get_grid_vec(SendBox),
		case player_gift:has_item(PlayerGiftPack) of
		    true ->
			do_unpack_gift_box_contain_item(Account, SendBox, PlayerGift, State);
		    false ->
			do_unpack_gift_box_doesnotcontain_item(Account, PlayerGift, State)
		end
	end,
    {noreply, NState};
%% 请求放置礼物
handle_cast({_Msg, #req_put_gift_box{pos=Pos}}, #player_data{account=Account, scene_name=SceneName}=State) ->
    HouseOwner = router:send(SceneName, get_house_owner),
    PlayerGift = player_data:get_gift(State),
    NState =
	case Account /= HouseOwner of
	    true ->
		{NPlayerGift, NHPlayerGift, NSendBox} = do_put_gift_box(Account, HouseOwner, PlayerGift, Pos, State),
		case router:send(SceneName, put_gift_box, NSendBox) of
		    {false, Err} -> 
			sys_msg:send(Account, Err),
			State;		    
		    HouseData ->
			NNPlayerGift = player_gift_data:drop_send_box(NPlayerGift),
			NNSendBox = player_gift_data:get_send_box(NNPlayerGift),
			db_common:write([NNPlayerGift, HouseData, NHPlayerGift]),
			send_put_gift_box(Account, NNSendBox),
			log_put_gift_box(Account, PlayerGift),
			player_data:set_gift(NNPlayerGift, State)
		end;
	    false ->
		{NPlayerGift, NSendBox} = do_put_gift_box_by_self(Account, HouseOwner, PlayerGift, Pos, State),
		case router:send(SceneName, put_gift_box, NSendBox) of
		    {false, Err} -> 
			sys_msg:send(Account, Err),
			State;		    
		    HouseData ->
			NNPlayerGift = player_gift_data:drop_send_box(NPlayerGift),
			NNSendBox = player_gift_data:get_send_box(NNPlayerGift),
			db_common:write([NNPlayerGift, HouseData]),
			send_put_gift_box(Account, NNSendBox),
			log_put_gift_box(Account, PlayerGift),
			player_data:set_gift(NNPlayerGift, State)
		end
	end,
    
    {noreply, NState};
%% 请求捡起物品
handle_cast({_Msg, #req_pickup_gift_box{inst_id=InstId}}, #player_data{account=Account, scene_name=SceneName}=State) ->
    PlayerGift = player_data:get_gift(State),
    GiftBox = router:send(SceneName, get_gift_box, InstId),
    NState = 
    case GiftBox == false of
	true ->
	    sys_msg:send(Account, ?msg_gift_box_has_been_pickup),
	    net_helper:send2client(Account, #notify_pickup_gift_box{inst_id=InstId}),
	    State;
	_ ->
	    PlayerGiftPackGrid = player_gift_data:get_grid_vec(GiftBox),
	    case player_gift:has_item(PlayerGiftPackGrid) of
		true ->
		    State1 = do_pickup_gift_box_contain_item(Account, PlayerGiftPackGrid, PlayerGift, 
						    GiftBox, SceneName, InstId, State),
		    %% 后台金钱物品日志
		    GiftIndex = 0,
		    GiftItem = pack:get_item(GiftIndex, PlayerGiftPackGrid),
		    ItemCount = pack:get_item_count(GiftIndex, PlayerGiftPackGrid), 
		    ItemInfo = {item:get_tempid(GiftItem), 
				ItemCount,
				item:get_instid(GiftItem)},
		    item_money_log:log_item(Account, pickup_gift_box, ItemInfo),

		    %% 提示获得物品
		    sys_msg:send(Account, ?msg_get_item, [item:get_item_name(GiftItem), ItemCount]),

		    State1;
		false ->
		    do_pickup_gift_box_doesnotcontain_item(Account, PlayerGift, GiftBox, SceneName, InstId, State)
	    end
    end,
    {noreply, NState}.

%%%===================================================================
%%% Internal functions
%%%===================================================================

%% 打包礼物盒
-spec do_pack_gift_box(tuple(), atom(), integer(), integer(), tuple(), tuple()) -> tuple().
do_pack_gift_box(#req_pack_gift_box{gift=Gift,
				    pack1_type=PackType1, 
				    index1=Index1, 
				    pack2_type=PackType2, 
				    index2=Index2}, Account, GameCoin, EQCoin, PlayerGift, State) 
  when (Index1 >= 0) and (Index2 >=0) ->
    SendBox = player_gift_data:get_send_box(PlayerGift),
    %% 移动物品
    {PlayerPackGrid, PlayerGiftPackGrid} = player_pack:move_item(PackType2, Index2, PackType1, Index1, State),
    PlayerPack = player_pack:make_package(Account, PackType2, PlayerPackGrid),
    NState = player_data:set_pack(PackType2, PlayerPackGrid, State),
    NSendBox = player_gift_data:set_grid_vec(SendBox, PlayerGiftPackGrid),
    %% 扣除金币
    {PlayerCoin, NNState} = player_gift:dec_coin(GameCoin, EQCoin, NState),
    %% 创建礼物盒
    NPlayerGift = create_gift_box(Account, NSendBox, PlayerGift, Gift, State),
    db_common:write([NPlayerGift, PlayerCoin, PlayerPack]),   
    send_pack_gift_box(Account, PlayerCoin, NPlayerGift, NNState),

 
    player_data:set_gift(NPlayerGift, NNState);
do_pack_gift_box(#req_pack_gift_box{gift=Gift}, Account, GameCoin, EQCoin, PlayerGift, State) ->
    SendBox = player_gift_data:get_send_box(PlayerGift),
    %% 扣除金币
    {PlayerCoin, NState} = player_gift:dec_coin(GameCoin, EQCoin, State),
    %% 创建礼物盒
    NPlayerGift = create_gift_box(Account, SendBox, PlayerGift, Gift, State),
    db_common:write([NPlayerGift, PlayerCoin]),
    send_pack_gift_box(Account, PlayerCoin, NPlayerGift),
    player_data:set_gift(NPlayerGift, NState).



%% 能否包装礼物
-spec can_pack_gift_box(tuple(), tuple(), integer(), integer(), integer(), integer(), tuple(), tuple()) -> atom() | tuple().
can_pack_gift_box(PlayerGift, Gift, GameCoin, EQCoin, PlayerGameCoin, PlayerEQCoin, PackGiftBox, State) ->
    #gift_box{name=Name, box_id=BoxId, comment=Comment} = Gift,
    #req_pack_gift_box{index2=Index2} = PackGiftBox,
    action:can_do([{player_gift:validate_is_contains_item(), {Index2, State}, ?msg_bag_no_item},
		   {player_gift:validate_item_trade(), {Index2, State}, ?msg_item_can_not_pack},
		   {player_gift:validate_pack_count(), {PlayerGift, GameCoin, EQCoin, 5}, ?msg_gift_err_pack_count},
		   {player_gift:validate_length(), {Name, 16}, ?msg_gift_err_name},                     %% 验证名称长度
		   {player_gift:validate_gift_box(), BoxId, ?msg_gift_err_giftbox},                     %% 验证礼物盒
		   {player_gift:validate_coin(), {GameCoin, PlayerGameCoin}, ?msg_gift_err_game_coin},  %% 验证游戏币
		   {player_gift:validate_coin(), {EQCoin, PlayerEQCoin}, ?msg_gift_err_eq_coin},        %% 验证EQ币
		   {player_gift:validate_length(), {Comment, 80}, ?msg_gift_err_comment}                %% 验证赠言长度
		  ]).

%% 创建礼物盒
-spec create_gift_box(atom(), tuple(), tuple(), tuple(), tuple()) -> tuple().
create_gift_box(Account, SendBox, PlayerGift, PackGiftBox, State) ->
    #gift_box{name = Name, 
    	      box_id = BoxId, 
    	      card_id = CardId,
    	      comment = Comment, 
    	      is_anonymous = IsAnonymous} = PackGiftBox,
    InstId = guid:make(?st_gift_box),
    NSendBox = SendBox#gift_box{name=Name,
				box_id=BoxId,
				card_id=CardId,
				comment=Comment,
				is_anonymous=IsAnonymous,
				inst_id = InstId, 
				sender = Account, 
				sender_name=player_data:get_name(State),
				date = datetime:localtime()},
    player_gift_data:set_send_box(PlayerGift, NSendBox).

%% 发送客户端信息
send_pack_gift_box(Account, PlayerCoin, PlayerGift, State) ->
    player_pack:send(Account, State),
    send_pack_gift_box(Account, PlayerCoin, PlayerGift).
send_pack_gift_box(Account, PlayerCoin, PlayerGift) ->
    player_coin:send_coin(PlayerCoin),
    net_helper:send2client(Account, #notify_pack_gift_box{}),
    SendBox = player_gift_data:get_send_box(PlayerGift),
    net_helper:send2client(Account, #notify_gift_box{gift=SendBox}).

%% 取消玩家包裹(带物品)
-spec do_unpack_gift_box_contain_item(atom(), tuple(), tuple(), tuple()) -> tuple().
do_unpack_gift_box_contain_item(Account, SendBox, PlayerGift, State) ->
    PlayerGiftPackGrid = player_gift_data:get_grid_vec(SendBox),
    PlayerPackGrid = player_data:get_pack(?pt_bag, State),
    case player_pack:find_available_grid(0, PlayerGiftPackGrid, PlayerPackGrid) of
	full ->
	    sys_msg:send(Account, ?err_bag_full),
	    State;
	Index -> 
	    {NPlayerPackGrid, _NPlayerGiftPackGrid} = player_pack:move_item(?pt_bag, Index, ?pt_gift_bag, 0, State),
	    NState = player_data:set_pack(?pt_bag, NPlayerPackGrid, State),
	    NPlayerPack = player_pack:make_package2(Account, ?pt_bag, NState),
	    NPlayerGift = player_gift_data:drop_send_box(PlayerGift),
	    NSendBox = player_gift_data:get_send_box(NPlayerGift),
	    db_common:write([NPlayerGift, NPlayerPack]),
	    send_unpack_gift_box(Account, NSendBox, State),
	    player_data:set_gift(NPlayerGift, NState)
    end.
%% 取消玩家包裹(不带物品)
-spec do_unpack_gift_box_doesnotcontain_item(atom(), tuple(), tuple()) -> tuple().
do_unpack_gift_box_doesnotcontain_item(Account, PlayerGift, State) ->
    NPlayerGift = player_gift_data:drop_send_box(PlayerGift),
    SendBox = player_gift_data:get_send_box(NPlayerGift),
    send_unpack_gift_box(Account, SendBox),
    db_common:write(NPlayerGift),
    player_data:set_gift(NPlayerGift, State).

%% 发送客户端信息
-spec send_unpack_gift_box(atom(), tuple(), tuple()) -> any().
send_unpack_gift_box(Account, SendBox, State) ->
    player_pack:send(Account, State),
    send_unpack_gift_box(Account, SendBox).
-spec send_unpack_gift_box(atom(), tuple()) -> any().
send_unpack_gift_box(Account, SendBox) ->
    net_helper:send2client(Account, #notify_unpack_gift_box{}),
    net_helper:send2client(Account, #notify_gift_box{gift=SendBox}).


%% 捡起有物品的礼物盒
do_pickup_gift_box_contain_item(Account, PlayerGiftPack, PlayerGift, #gift_box{sender=Sender, sender_name=SenderName}=SendBox,
 		   SceneName, InstId, State) when (Account == Sender)  ->
    PlayerGiftPackGrid = player_gift_data:get_grid_vec(SendBox),
    PlayerPackGrid = player_data:get_pack(?pt_bag, State),
    case player_pack:find_available_grid(0, PlayerGiftPackGrid, PlayerPackGrid) of
	full ->
	    sys_msg:send(Account, ?err_bag_full),
	    State;
	Index -> 
	    Receiver = get_receiver(SendBox),
	    case Account == Receiver of
		true ->
		    NPlayerGift = player_gift_data:set_viewed_box_vec(PlayerGift, SendBox),
		    ReceiveBoxVec = player_gift_data:get_receive_box_vec(NPlayerGift),
		    NReceiveBoxVec = lists:delete(SendBox, ReceiveBoxVec),
		    NNPlayerGift = player_gift_data:set_receive_box_vec(NPlayerGift, NReceiveBoxVec),
		    {NNPlayerPack, NHouseData, NNNPlayerGift, NState} = 
		    do_pickup_gift_box_contain_item(PlayerGiftPack, Index, InstId, SceneName, NNPlayerGift, State),
		    db_common:write([NNPlayerPack, NHouseData, NNNPlayerGift]),
		    player_pack:send(Account, NState),
		    net_helper:send2client(Account, #notify_pickup_gift_box{inst_id=InstId}),
		    player_data:set_gift(NNNPlayerGift, NState);
		false ->
		    NPlayerGift = player_gift_data:set_viewed_box_vec(PlayerGift, SendBox),
		    ReceiverPlayerGift = delete_receive_box_vec(Receiver, SendBox),
		    {NNPlayerPack, NHouseData, NNPlayerGift, NState} = 
		    do_pickup_gift_box_contain_item(PlayerGiftPack, Index, InstId, SceneName, NPlayerGift, State),
		    db_common:write([NNPlayerPack, NHouseData, NNPlayerGift, ReceiverPlayerGift]),
		    sys_msg:send(Receiver, ?msg_gift_box_owner_pickup, [SenderName]),
		    player_pack:send(Account, NState),
		    net_helper:send2client(Account, #notify_pickup_gift_box{inst_id=InstId}),
		    player_data:set_gift(NNPlayerGift, NState)
	    end
    end;
do_pickup_gift_box_contain_item(Account, PlayerGiftPack, PlayerGift, SendBox, SceneName, InstId, State) ->
    PlayerGiftPackGrid = player_gift_data:get_grid_vec(SendBox),
    PlayerPackGrid = player_data:get_pack(?pt_bag, State),
    GiftIndex = 0,
    case player_pack:find_available_grid(GiftIndex, PlayerGiftPackGrid, PlayerPackGrid) of
	full ->
	    sys_msg:send(Account, ?err_bag_full),
	    State;
	Index -> 
	    NPlayerGift = do_pickup_gift_box_log(PlayerGift, SendBox),
	    {NNPlayerPack, NHouseData, NNPlayerGift, NState} = 
		do_pickup_gift_box_contain_item(PlayerGiftPack, Index, InstId, SceneName, NPlayerGift, State),
	    db_common:write([NNPlayerPack, NHouseData, NNPlayerGift]),
	    player_pack:send(Account, NState),
	    net_helper:send2client(Account, #notify_pickup_gift_box{inst_id=InstId}),
	    player_data:set_gift(NNPlayerGift, NState)
    end.

do_pickup_gift_box_contain_item(PlayerGiftPack, Index, InstId, SceneName, PlayerGift, State) ->
    Account = player_data:get_account(State),
    PlayerPack = player_data:get_pack(?pt_bag, State),
    {NPlayerPack, _PlayerGiftPack} = player_pack:move_item(Account, Index, PlayerPack, 0, PlayerGiftPack),
    NState = player_data:set_pack(?pt_bag, NPlayerPack, State),
    NNPlayerPack = player_pack:make_package2(Account, ?pt_bag, NState),
    NHouseData = router:send(SceneName, dec_gift_box, InstId),
    {NNPlayerPack, NHouseData, PlayerGift, NState}.


%% 捡起没有物品的礼物盒
do_pickup_gift_box_doesnotcontain_item(Account, PlayerGift, #gift_box{sender = Sender, sender_name=SenderName}=SendBox,
				       SceneName, InstId, State) when (Account == Sender)  ->
    Receiver = get_receiver(SendBox),
    case Account == Receiver of
	true ->
	    NPlayerGift = player_gift_data:set_viewed_box_vec(PlayerGift, SendBox),
	    ReceiveBoxVec = player_gift_data:get_receive_box_vec(NPlayerGift),
	    NReceiveBoxVec = lists:delete(SendBox, ReceiveBoxVec),
	    NNPlayerGift = player_gift_data:set_receive_box_vec(NPlayerGift, NReceiveBoxVec),
	    do_pickup_gift_box_doesnotcontain_item(Account, SceneName, InstId, NNPlayerGift, State);
	false ->
	    NPlayerGift = player_gift_data:set_viewed_box_vec(PlayerGift, SendBox),
	    ReceiverPlayerGift = delete_receive_box_vec(Receiver, SendBox),
	    NHouseData = router:send(SceneName, dec_gift_box, InstId),
	    db_common:write([PlayerGift, ReceiverPlayerGift, NHouseData]),
	    sys_msg:send(Receiver, ?msg_gift_box_owner_pickup, [SenderName]),
	    net_helper:send2client(Account, #notify_pickup_gift_box{inst_id=InstId}),
	    player_data:set_gift(NPlayerGift, State)
    end;
do_pickup_gift_box_doesnotcontain_item(Account, PlayerGift, SendBox,
		    SceneName, InstId, State)  ->
    NPlayerGift = do_pickup_gift_box_log(PlayerGift, SendBox),
    do_pickup_gift_box_doesnotcontain_item(Account, SceneName, InstId, NPlayerGift, State).

do_pickup_gift_box_doesnotcontain_item(Account, SceneName, InstId, PlayerGift, State) ->
    NHouseData = router:send(SceneName, dec_gift_box, InstId),
    db_common:write([PlayerGift, NHouseData]),
    net_helper:send2client(Account, #notify_pickup_gift_box{inst_id=InstId}),
    player_data:set_gift(PlayerGift, State).

-spec do_pickup_gift_box_log(tuple(), tuple()) -> tuple().
do_pickup_gift_box_log(PlayerGift, SendBox) ->
    %% 去除未查看的礼物盒
    NPlayerGift = drop_receive_box(PlayerGift, SendBox),
    %% 增加已查看的礼物盒
    player_gift_data:set_viewed_box_vec(NPlayerGift, SendBox).

%% 判断发送的礼物盒是否为空
-spec box_is_empty(tuple()) -> atom().
box_is_empty(GiftBox) ->
    Sender = player_gift_data:get_sender(GiftBox),
    Sender == [].

%% 去除未查看的礼物盒
-spec drop_receive_box(tuple(), tuple()) -> tuple().
drop_receive_box(PlayerGift, SendBox) ->
    ReceiveBoxVec = player_gift_data:get_receive_box_vec(PlayerGift),
    data_helper:format("ReceiveBoxVec:~p~n, SendBox:~p~n", [ReceiveBoxVec, SendBox]),
    NReceiveBoxVec = lists:delete(SendBox, ReceiveBoxVec),
    player_gift_data:set_receive_box_vec(PlayerGift, NReceiveBoxVec).

%% %% 是否能否放礼物
%% -spec can_put_gift_box(atom(), atom()) -> atom().
%% can_put_gift_box(Account, HouseOwner) when Account == HouseOwner ->
%%     true;
%% can_put_gift_box(_Account, _HouseOwner) ->
%%     true.

%% 放置礼物盒
do_put_gift_box(Account, HouseOwner, PlayerGift, Pos, State) when Account /= HouseOwner ->
    SendBox = player_gift_data:get_send_box(PlayerGift),
    %% 增加本人发送的日志
    NSendBox = SendBox#gift_box{sender_name=player_data:get_name(State),
				receiver=HouseOwner, 
				receiver_name=player:get_user_name(HouseOwner), 
				pos=Pos},
    NHPlayerGift = set_receive_box_vec(HouseOwner, NSendBox#gift_box{receiver=HouseOwner, pos=Pos}),
    NPlayerGift = player_gift_data:set_send_box_vec(PlayerGift, NSendBox),
    {NPlayerGift, NHPlayerGift, NSendBox}.
do_put_gift_box_by_self(Account, HouseOwner, PlayerGift, Pos, State) when Account == HouseOwner ->
    SendBox = player_gift_data:get_send_box(PlayerGift),
    %% 增加本人发送的日志
    NSendBox = SendBox#gift_box{sender_name=player_data:get_name(State),
				receiver=HouseOwner, 
				receiver_name=player_data:get_name(State), 
				pos=Pos},
    NPlayerGift = player_gift_data:set_receive_box_vec(PlayerGift, NSendBox),
    NNPlayerGift = player_gift_data:set_send_box_vec(NPlayerGift, NSendBox),
    {NNPlayerGift, NSendBox}.

%% 发送客户端信息
-spec send_put_gift_box(atom(), tuple()) -> any().
send_put_gift_box(Account, SendBox) ->
    net_helper:send2client(Account, #notify_gift_box{gift=SendBox}),
    net_helper:send2client(Account, #notify_put_gift_box{}).

get_receiver(#gift_box{receiver=Receiver}) ->
    Receiver.

%% 执行清空日志
do_clean_gift_log(?glt_send_box, Account, PlayerGift, State) ->
    NPlayerGift = player_gift_data:set_send_box_vec(PlayerGift, []),
    net_helper:send2client(Account, #notify_get_send_box_vec{send_box_vec=[], total=0}),
    db_common:write(NPlayerGift),
    player_data:set_gift(NPlayerGift, State);
do_clean_gift_log(?glt_receive_box, Account, PlayerGift, State) ->
    NPlayerGift = player_gift_data:set_receive_box_vec(PlayerGift, []),
    net_helper:send2client(Account, #notify_get_receive_box_vec{receive_box_vec=[], total=0}),
    db_common:write(NPlayerGift),
    player_data:set_gift(NPlayerGift, State);
do_clean_gift_log(?glt_viewed_box, Account, PlayerGift, State) ->
    NPlayerGift = player_gift_data:set_viewed_box_vec(PlayerGift, []),
    net_helper:send2client(Account, #notify_get_viewed_box_vec{viewed_box_vec=[], total=0}),
    db_common:write(NPlayerGift),
    player_data:set_gift(NPlayerGift, State);
do_clean_gift_log(?glt_send_express, Account, PlayerGift, State) ->
    NPlayerGift = player_gift_data:set_send_express_vec(PlayerGift, []),
    net_helper:send2client(Account, #notify_get_send_express_vec{send_express_vec=[], total=0}),
    db_common:write(NPlayerGift),
    player_data:set_gift(NPlayerGift, State);
do_clean_gift_log(?glt_receive_express, Account, PlayerGift, State) ->
    NPlayerGift = player_gift_data:set_receive_express_vec(PlayerGift, []),
    net_helper:send2client(Account, #notify_get_receive_express_vec{receive_express_vec=[], total=0}),
    db_common:write(NPlayerGift),
    player_data:set_gift(NPlayerGift, State);
do_clean_gift_log(?glt_viewed_express, Account, PlayerGift, State) ->
    NPlayerGift = player_gift_data:set_viewed_express_vec(PlayerGift, []),
    net_helper:send2client(Account, #notify_get_viewed_express_vec{viewed_express_vec=[], total=0}),
    db_common:write(NPlayerGift),
    player_data:set_gift(NPlayerGift, State).

set_receive_box_vec(Receiver, SendBox) ->
    case util:is_process_alive(Receiver) of
	true ->
	    router:send(Receiver, set_receive_box_vec, SendBox);
	false ->
	    PlayerGift = db_common:select_one(player_gift, Receiver),
	    ReceiveBoxVec = player_gift_data:get_receive_box_vec(PlayerGift),
	    NReceiveBoxVec = [SendBox | ReceiveBoxVec],
	    player_gift_data:set_receive_box_vec(PlayerGift, NReceiveBoxVec)
    end.

delete_receive_box_vec(Receiver, SendBox) ->
    case util:is_process_alive(Receiver) of
	true ->
	    router:send(Receiver, delete_receive_box, SendBox);
	false ->
	    PlayerGift = db_common:select_one(player_gift, Receiver),
	    ReceiveBoxVec = player_gift_data:get_receive_box_vec(PlayerGift),
	    NReceiveBoxVec = lists:delete(SendBox, ReceiveBoxVec),
	    player_gift_data:set_receive_box_vec(PlayerGift, NReceiveBoxVec)
    end.


log_put_gift_box(Account, PlayerGift) ->
    %% 后台金钱物品日志
    GiftBox = player_gift_data:get_send_box(PlayerGift),
    PlayerGiftPackGrid = player_gift_data:get_grid_vec(GiftBox),
    GiftIndex = 0,
    GiftItem = pack:get_item(GiftIndex, PlayerGiftPackGrid),
    ItemInfo = 
	case GiftItem of
	    empty -> {0, 0, 0};
	    _ ->
		{item:get_tempid(GiftItem), 
		 -pack:get_item_count(GiftIndex, PlayerGiftPackGrid), 
		 item:get_instid(GiftItem)}
	end,
    item_money_log:log_item(Account, drop_gift_box, ItemInfo).
