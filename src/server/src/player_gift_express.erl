%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 礼物系统-快递礼物
%%% @end
%%% Created :  2 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(player_gift_express).

-export([start/1, send_express_vec/1, send_gm_gift/4, handle_call/3, handle_cast/2]).

-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").
-include("player_data.hrl").

-define(MAXLOG, 1000).

%% API
-spec start(atom()) -> list().
start(Account) ->
    [
     router:make_event_source(?msg_req_get_send_express_vec, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_get_receive_express_vec, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_get_viewed_express_vec, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_send_express_by_subscribe, Account, {self(), ?MODULE}),
     router:make_event_target(set_receive_express_vec, Account, {self(), ?MODULE}),
     router:make_event_target(set_subscribe_express_vec, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% API
%%%===================================================================
%% 发送快递礼物数据
send_express_vec(State) ->
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    ReceiveExpressVec = player_gift_data:get_receive_express_vec(PlayerGift),
    SubScribeExpressVec = player_gift_data:get_subscribe_express_vec(PlayerGift),
    net_helper:send2client(Account, #notify_express_vec{send_express_vec=SubScribeExpressVec, receive_express_vec=ReceiveExpressVec}).

%%%===================================================================
%%% Handle call
%%%===================================================================
handle_call({#msg{event=set_receive_express_vec}, GiftExpress}, _From, State) ->
    PlayerGift = player_data:get_gift(State),
    NPlayerGift = player_gift_data:set_receive_express_vec(PlayerGift, GiftExpress),
    NState = player_data:set_gift(NPlayerGift, State),
    {reply, NPlayerGift, NState};
handle_call({#msg{event=set_subscribe_express_vec}, GiftExpress}, _From, State) ->
    PlayerGift = player_data:get_gift(State),
    NPlayerGift = player_gift_data:set_subscribe_express_vec(PlayerGift, GiftExpress),
    NState = player_data:set_gift(NPlayerGift, State),
    {reply, NPlayerGift, NState}.

%%%===================================================================
%%% Handle cast
%%%===================================================================
%% 请求获取发送快递列表
handle_cast({_Msg, #req_get_send_express_vec{page_index=PageIndex, page_count=PageCount}}, State) ->
    NState = do_get_send_express_vec(PageIndex, PageCount, State),
    {noreply, NState};

%% 请求获取接收快递列表
handle_cast({_Msg, #req_get_receive_express_vec{page_index=PageIndex, page_count=PageCount}}, State) ->
    NState = do_get_receive_express_vec(PageIndex, PageCount, State),
    {noreply, NState};

%% 请求获取查看过的快递列表
handle_cast({_Msg, #req_get_viewed_express_vec{page_index=PageIndex, page_count=PageCount}}, State) ->
    NState = do_get_viewed_express_vec(PageIndex, PageCount, State),
    {noreply, NState};

handle_cast({_Msg, #req_send_express_by_subscribe{inst_id=InstId}}, State) ->
    PlayerGift = player_data:get_gift(State),
    SubScribeExpressVec = player_gift_data:get_subscribe_express_vec(PlayerGift),
    SubScribeExpress = lists:keyfind(InstId, 3, SubScribeExpressVec),
    NState = 
	case SubScribeExpress == false of
	    true ->
		State;
	    false ->
		SendDate = SubScribeExpress#gift_express.send_date,
		Now = datetime:localtime(),
		DiffTime = datetime:diff_time(SendDate, Now),
		case DiffTime >= 0 of
		    true ->
			NPlayerGift = player_gift_data:drop_subscribe_express(PlayerGift, SubScribeExpress),
			NNPlayerGift = player_gift_data:set_receive_express_vec(NPlayerGift, SubScribeExpress),
			db_common:write([NNPlayerGift]),
			player_data:set_gift(NNPlayerGift, State);
		    false ->
			State
		end
	end,
    {noreply, NState};

%% 请求快递礼物
handle_cast({_Msg, #req_send_gift_express{gift=GiftExpress, 
					  index1=Index1, 
					  index2=Index2}=SendGiftExpress}, State) ->
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    CardId = get_card_id(GiftExpress),
    {GameCoin, EQCoin, PlayerGameCoin, PlayerEQCoin} = player_gift:get_coin(CardId, State),
    State1 = 
	case can_send_gift_express(GiftExpress, Index1, Index2, GameCoin, EQCoin, PlayerGameCoin, PlayerEQCoin, State) of
	    true ->
		State2 = 
		    case (Index1 >= 0) and (Index2 >=0) of
			true ->
			    do_send_gift_express_contain_item(Account, 
							      SendGiftExpress, 
							      PlayerGift, 
							      GameCoin, 
							      EQCoin, 
							      State);
			false ->
			    do_send_gift_express_doesnotcontain_item(Account, 
								     SendGiftExpress, 
								     PlayerGift, 
								     GameCoin, 
								     EQCoin, 
								     State)
		    end,

		%% 后台金钱物品记录
		log_send_gift_express(Account, GameCoin, EQCoin, 
				      list_to_atom(get_receiver(GiftExpress)), Index2, State),    

		State2;
	    {false, Reason} ->
		sys_msg:send(Account, Reason),
	    State
    end,
    {noreply, State1};



handle_cast({_Msg, #req_receive_gift_express{inst_id=InstId, 
					     page_index=PageIndex, 
					     page_count=PageCount}}, #player_data{account=Account}=State) ->
    PlayerGift = player_data:get_gift(State),
    ReceiveExpressVec = player_gift_data:get_receive_express_vec(PlayerGift),
    ReceiveExpress = lists:keyfind(InstId, 3, ReceiveExpressVec),
    State1 = 
	case ReceiveExpress of
	    false ->
		State;
	    _ ->
		PlayerGiftPackGrid = get_grid_vec(ReceiveExpress),
		case player_gift:has_item(PlayerGiftPackGrid) of
		    true ->
			{PlayerPack, NPlayerGift, NState} = 
			    do_receive_gift_express_contain_item(Account, ReceiveExpress, 
								 ReceiveExpressVec, PlayerGift, State),
			db_common:write([NPlayerGift, PlayerPack]),
			send_receive_gift_express(Account, InstId, NState),
			NNState = do_get_receive_express_vec(PageIndex, PageCount, NState),
			%% 后台物品金钱记录
			{ItemTempID, ItemCount, _} = 
			    log_receive_gift_express(Account, ReceiveExpress, PlayerGiftPackGrid),

			%% 提示获得物品
			sys_msg:send(Account, ?msg_get_item, [item:get_item_name(ItemTempID), ItemCount]),

			player_data:set_gift(NPlayerGift, NNState);
		    false ->
			{NPlayerGift, NState} = 
			do_receive_gift_express_doesnotcontain_item(ReceiveExpress, ReceiveExpressVec, PlayerGift, State),
			db_common:write([NPlayerGift]),
			send_receive_gift_express(Account, InstId),
			NNState = do_get_receive_express_vec(PageIndex, PageCount, NState),
			player_data:set_gift(NPlayerGift, NNState)
		end
	end,
    {noreply, State1}.
%%%===================================================================
%%% Internal functions
%%%===================================================================
do_send_gift_express_contain_item(Account, SendGiftExpress, PlayerGift, GameCoin, EQCoin, State) ->
    case do_send_gift_express(Account, SendGiftExpress, PlayerGift, GameCoin, EQCoin, State) of
	{InstId, PlayerCoin, PlayerPack, NPlayerGift, ReceiverPlayerGift, NState} ->
	    db_common:write([NPlayerGift, ReceiverPlayerGift, PlayerCoin, PlayerPack]),
	    send_gift_express(Account, InstId, PlayerCoin, NState),
	    NState;
	{InstId, PlayerCoin, PlayerPack, NPlayerGift, NState} ->
	    db_common:write([NPlayerGift, PlayerCoin, PlayerPack]),
	    send_gift_express(Account, InstId, PlayerCoin, NState),
	    NState
    end.

do_send_gift_express_doesnotcontain_item(Account, SendGiftExpress, PlayerGift, GameCoin, EQCoin, State) ->
    case do_send_gift_express(Account, SendGiftExpress, PlayerGift, GameCoin, EQCoin, State) of
	{InstId, PlayerCoin, NPlayerGift, ReceiverPlayerGift, NState} ->	
	    db_common:write([NPlayerGift, ReceiverPlayerGift, PlayerCoin]),
	    send_gift_express(Account, InstId, PlayerCoin, NState),
	    NState;
	{InstId, PlayerCoin, NPlayerGift, NState} ->	
	    db_common:write([NPlayerGift, PlayerCoin]),
	    send_gift_express(Account, InstId, PlayerCoin, NState),
	    NState
    end.

%% 是否能够快递礼物
-spec can_send_gift_express(tuple(), integer(), integer(), integer(), integer(), integer(), integer(), tuple()) -> atom()|tuple().
can_send_gift_express(GiftExpress, Index1, Index2, GameCoin, EQCoin, PlayerGameCoin, PlayerEQCoin, State) ->
    CardId = get_card_id(GiftExpress),
    data_helper:format("CardId:~p~n", [CardId]),
    Comment = get_comment(GiftExpress),
    Receiver = get_receiver(GiftExpress),
    action:can_do([{player_gift:validate_is_contains_item(), {Index2, State}, ?msg_bag_no_item},
		   {player_gift:validate_item_trade(), {Index2, State}, ?msg_item_can_not_pack},
		   {validate_receiver(), list_to_atom(Receiver), ?msg_gift_err_sender},
		   {validate_express_item(), {CardId, Index1, Index2}, ?msg_gift_item_not_exist},  %% 礼物卡和礼物必须选择一个
		   {player_gift:validate_coin(), {GameCoin, PlayerGameCoin}, ?msg_gift_err_game_coin},  %% 验证游戏币
		   {player_gift:validate_coin(), {EQCoin, PlayerEQCoin}, ?msg_gift_err_eq_coin},        %% 验证EQ币
		   {player_gift:validate_length(), {Comment, 80}, ?msg_gift_err_comment}                %% 验证赠言长度
		  ]).

%% 发送客户端相关数据
send_gift_express(Account, InstId) ->
    net_helper:send2client(Account, #notify_send_gift_express{inst_id=InstId}).
send_gift_express(Account, InstId, PlayerCoin, State) ->
    player_coin:send_coin(PlayerCoin),
    player_pack:send(Account, State),
    send_gift_express(Account, InstId).


%% 快递礼物(存在物品)
do_send_gift_express(Account, #req_send_gift_express{gift=GiftExpress,
						     index1=Index1, 
						     pack2_type=PackType2,
						     index2=Index2},
		     PlayerGift, GameCoin, EQCoin, State) when (Index1 >= 0) and (Index2 >= 0) ->
    GridVec = player_gift_data:new_gift_express(),
    %% 扣除玩家金币
    {PlayerCoin, NState} = player_gift:dec_coin(GameCoin, EQCoin, State),
    %% 移动物品
    PlayerPackGrid = player_data:get_pack(PackType2, NState),
    {NPlayerPackGrid, PlayerGiftPackGrid} = player_pack:move_item(Account, Index2, PlayerPackGrid, Index1, GridVec),
    PlayerPack = player_pack:make_package(Account, PackType2, NPlayerPackGrid),
    NNState = player_data:set_pack(PackType2, NPlayerPackGrid, NState),
    NGiftExpress = set_grid_vec(GiftExpress, PlayerGiftPackGrid),
    {NNGiftExpress, InstId} = create_gift_express(Account, NGiftExpress),
    NPlayerGift = player_gift_data:set_send_express_vec(PlayerGift, NNGiftExpress),
    Sender = get_sender(NNGiftExpress),
    Receiver = get_receiver(NNGiftExpress),
    case Sender == Receiver of
	true ->
	    NNPlayerGift = 
		case NNGiftExpress#gift_express.send_date == #stime{} of
		    true ->
			player_gift_data:set_receive_express_vec(NPlayerGift, NNGiftExpress);
		    false ->
			player_gift_data:set_subscribe_express_vec(NPlayerGift, NNGiftExpress)
		end,
	    NNNState = player_data:set_gift(NNPlayerGift, NNState),
	    net_helper:send2client(Receiver, #notify_send_express{send_express=NNGiftExpress}),
	    {InstId, PlayerCoin, PlayerPack, NNPlayerGift, NNNState};
	false ->
	    NReceiverPlayerGift = 
		case NNGiftExpress#gift_express.send_date == #stime{} of
		    true ->
			set_receive_express_vec(Receiver, NNGiftExpress);
		    false ->
			set_subscribe_express_vec(Receiver, NNGiftExpress)
		end,
	    NNNState = player_data:set_gift(NPlayerGift, NNState),
	    net_helper:send2client(Receiver, #notify_send_express{send_express=NNGiftExpress}),
	    {InstId, PlayerCoin, PlayerPack, NPlayerGift, NReceiverPlayerGift, NNNState}
    end;
do_send_gift_express(Account, #req_send_gift_express{gift=GiftExpress}, PlayerGift, GameCoin, EQCoin, State) ->
    GridVec = player_gift_data:new_gift_express(),
    NGiftExpress = set_grid_vec(GiftExpress, GridVec),
    %% 扣除玩家金币
    {PlayerCoin, NState} = player_gift:dec_coin(GameCoin, EQCoin, State),
    {NNGiftExpress, InstId} = create_gift_express(Account, NGiftExpress),
    NPlayerGift = player_gift_data:set_send_express_vec(PlayerGift, NNGiftExpress),
    Sender = get_sender(NNGiftExpress),
    Receiver = get_receiver(NNGiftExpress),
    case Sender == Receiver of
	true ->
	    NNPlayerGift = 
		case NNGiftExpress#gift_express.send_date == #stime{} of
		    true ->
			player_gift_data:set_receive_express_vec(NPlayerGift, NNGiftExpress);
		    false ->
			player_gift_data:set_subscribe_express_vec(NPlayerGift, NNGiftExpress)
		end,
	    NNState = player_data:set_gift(NNPlayerGift, NState),
	    net_helper:send2client(Receiver, #notify_send_express{send_express=NNGiftExpress}),
	    {InstId, PlayerCoin, NNPlayerGift, NNState};
	false ->
	    NReceiverPlayerGift = 
		case NNGiftExpress#gift_express.send_date == #stime{} of
		    true ->
			set_receive_express_vec(Receiver, NNGiftExpress);
		    false ->
			set_subscribe_express_vec(Receiver, NNGiftExpress)
		end,
	    NNState = player_data:set_gift(NPlayerGift, NState),
	    net_helper:send2client(Receiver, #notify_send_express{send_express=NNGiftExpress}),
	    {InstId, PlayerCoin, NPlayerGift, NReceiverPlayerGift, NNState}
    end.

send_gm_gift(SenderName, ItemId, Comment, State) ->
    GridVec = player_gift_data:new_gift_express(),
    NGridVec = pack:add_item(ItemId, GridVec),
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    InstId = guid:make(?st_gift_express),
    Now = datetime:localtime(),
    GiftExpress = #gift_express{type=?gm, inst_id=InstId, sender_name=SenderName, 
				receiver=Account, grid_vec=NGridVec, comment=Comment, 
				send_date=Now, date=Now},
    NPlayerGift = player_gift_data:set_subscribe_express_vec(PlayerGift, GiftExpress),
    NState = player_data:set_gift(NPlayerGift, State),
    db_common:write([NPlayerGift]),
    net_helper:send2client(Account, #notify_send_express{send_express=GiftExpress}),
    send_gift_express(Account, InstId),
    NState.

%% 创建礼物盒
-spec create_gift_express(atom(), tuple()) -> tuple().
create_gift_express(Account, GiftExpress) ->
    InstId = guid:make(?st_gift_express),
    Now = datetime:localtime(),
    Receiver = list_to_atom(get_receiver(GiftExpress)),
    NGiftExpress = GiftExpress#gift_express{type=?player,
					    inst_id=InstId, 
					    sender=Account,
					    receiver = Receiver,
					    date=Now},
    {NGiftExpress, InstId}.

validate_express_item() ->
    fun({CardId, Index1, Index2}) ->
	    case CardId > 0 of
		true ->
		    true;
		false ->
		    (Index1 >= 0) and (Index2 >= 0)
	    end
    end.

validate_receiver() ->
    fun(Receiver) ->
	    db_player:is_exist(Receiver)
    end.

%% send(PlayerGift) ->
%%     SubScribeExpressVec = player_gift_data:get_subscribe_express_vec(PlayerGift),
%%     %% NPlayerGift = player_gift_data:set_subscribe_express_vec(PlayerGift, []),
%%     send(SubScribeExpressVec, PlayerGift).

%% send([], PlayerGift) ->
%%     PlayerGift;
%% send([#gift_express{send_date=SendDate}=SubScribeExpress | SubScribeExpressVec], PlayerGift) ->
%%     Now = datetime:localtime(),
%%     DiffTime = datetime:diff_time(SendDate, Now),
%%     case DiffTime >= 0 of
%% 	true ->
%% 	    NPlayerGift = player_gift_data:set_receive_express_vec(PlayerGift, SubScribeExpress),
%% 	    NNPlayerGift = player_gift_data:drop_subscribe_express(NPlayerGift, SubScribeExpress),
%% 	    send(SubScribeExpressVec, NNPlayerGift);
%% 	false ->
%% 	    NPlayerGift = player_gift_data:set_subscribe_express_vec(PlayerGift, SubScribeExpress),
%% 	    send(SubScribeExpressVec, NPlayerGift)
%%     end.

do_receive_gift_express_contain_item(Account, ReceiveExpress, ReceiveExpressVec, PlayerGift, State) ->
    PlayerGiftPackGrid = get_grid_vec(ReceiveExpress),
    PlayerPackGrid = player_data:get_pack(?pt_bag, State),
    case player_pack:find_available_grid(0, PlayerGiftPackGrid, PlayerPackGrid) of
	full ->
	    sys_msg:send(Account, ?err_bag_full),
	    State;
	Index -> 
	    {NPlayerPackGrid, _NPlayerGiftPackGrid} = 
		player_pack:move_item(Account, Index, PlayerPackGrid, 0, PlayerGiftPackGrid),
	    NState = player_data:set_pack(?pt_bag, NPlayerPackGrid, State),
	    PlayerPack = player_pack:make_package2(Account, ?pt_bag, NState),

	    NReceiveExpressVec = lists:delete(ReceiveExpress, ReceiveExpressVec),
	    NPlayerGift = player_gift_data:set_receive_express_vec(PlayerGift, NReceiveExpressVec),
	    NNPlayerGift = player_gift_data:set_viewed_express_vec(NPlayerGift, ReceiveExpress),
	    NNState = player_data:set_gift(NNPlayerGift, NState),
	    {PlayerPack, NNPlayerGift, NNState}
    end.

do_receive_gift_express_doesnotcontain_item(ReceiveExpress, ReceiveExpressVec, PlayerGift, State) ->
    NReceiveExpressVec = lists:delete(ReceiveExpress, ReceiveExpressVec),
    NPlayerGift = player_gift_data:set_receive_express_vec(PlayerGift, NReceiveExpressVec),
    NNPlayerGift = player_gift_data:set_viewed_express_vec(NPlayerGift, ReceiveExpress),
    NState = player_data:set_gift(NNPlayerGift, State),
    {NNPlayerGift, NState}.

send_receive_gift_express(Account, InstId, State) ->
    player_pack:send(Account, State),
    send_receive_gift_express(Account, InstId).
send_receive_gift_express(Account, InstId) ->
    net_helper:send2client(Account, #notify_receive_gift_express{inst_id=InstId}).

get_card_id(#gift_express{card_id=CardId}) ->
    CardId.

get_comment(#gift_express{comment=Comment}) ->
    Comment.

get_grid_vec(#gift_express{grid_vec=GridVec}) ->
    GridVec.

set_grid_vec(GiftExpress, GridVec) ->
    GiftExpress#gift_express{grid_vec=GridVec}.

get_sender(#gift_express{sender=Sender}) ->
    Sender.

get_receiver(#gift_express{receiver=Receiver}) ->
    Receiver.

do_get_send_express_vec(PageIndex, PageCount, State) ->
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    SendExpressVec = player_gift_data:get_send_express_vec(PlayerGift),
    NNPlayerGift = 
	case length(SendExpressVec) > ?MAXLOG of
	    true ->
		NSendExpressVec = lists:sublist(SendExpressVec, ?MAXLOG),
		NPlayerGift = player_gift_data:set_send_express_vec(PlayerGift, NSendExpressVec),
		NNSendExpressVec = lists:sublist(NSendExpressVec, 
						 player_gift:get_index(PageIndex, PageCount),
						 PageCount), 
		NNNSendExpressVec = player_gift:get_name(NNSendExpressVec, []),
		net_helper:send2client(Account, #notify_get_send_express_vec{
					 send_express_vec=NNNSendExpressVec,
					 total=length(NSendExpressVec)}),
		NPlayerGift;
	    false ->
		NSendExpressVec = lists:sublist(SendExpressVec, 
						player_gift:get_index(PageIndex, PageCount),
						PageCount), 
		NNSendExpressVec = player_gift:get_name(NSendExpressVec, []),
		net_helper:send2client(Account, #notify_get_send_express_vec{
					 send_express_vec=NNSendExpressVec, 
					 total=length(SendExpressVec)}),
		PlayerGift
	end,
    db_common:write(NNPlayerGift),
    player_data:set_gift(NNPlayerGift, State).

do_get_receive_express_vec(PageIndex, PageCount, State) ->
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    %% NPlayerGift = send(PlayerGift),
    ReceiveExpressVec = player_gift_data:get_receive_express_vec(PlayerGift),
    NNPlayerGift = 
	case length(ReceiveExpressVec) > ?MAXLOG of
	    true ->
		NReceiveExpressVec = lists:sublist(ReceiveExpressVec, ?MAXLOG),
		NPlayerGift = player_gift_data:set_send_express_vec(PlayerGift, NReceiveExpressVec),
		NNReceiveExpressVec = lists:sublist(NReceiveExpressVec, 
						    player_gift:get_index(PageIndex, PageCount),
						    PageCount),
		NNNReceiveExpressVec = player_gift:get_name(NNReceiveExpressVec, []),
		net_helper:send2client(Account, #notify_get_receive_express_vec{
					 receive_express_vec=NNNReceiveExpressVec,
					 total=length(NReceiveExpressVec)}),
		NPlayerGift;
	    false ->
		NReceiveExpressVec = lists:sublist(ReceiveExpressVec, 
						   player_gift:get_index(PageIndex, PageCount),
						   PageCount),
		NNReceiveExpressVec = player_gift:get_name(NReceiveExpressVec, []),
		net_helper:send2client(Account, #notify_get_receive_express_vec{
					 receive_express_vec=NNReceiveExpressVec,
					 total=length(ReceiveExpressVec)}),
		PlayerGift
	end,
    db_common:write([NNPlayerGift]),
    player_data:set_gift(NNPlayerGift, State).

do_get_viewed_express_vec(PageIndex, PageCount, State) ->
    Account = player_data:get_account(State),
    PlayerGift = player_data:get_gift(State),
    ViewedExpressVec = player_gift_data:get_viewed_express_vec(PlayerGift),
    NNPlayerGift = 
	case length(ViewedExpressVec) > ?MAXLOG of
	    true ->
		NViewedExpressVec = lists:sublist(ViewedExpressVec, ?MAXLOG),
		NPlayerGift = player_gift_data:set_viewed_express_vec(PlayerGift, NViewedExpressVec),
		NNViewedExpressVec = lists:sublist(NViewedExpressVec, 
						   player_gift:get_index(PageIndex, PageCount),
						   PageCount),
		NNNViewedExpressVec = player_gift:get_name(NNViewedExpressVec, []),
		net_helper:send2client(Account, #notify_get_viewed_express_vec{
					 viewed_express_vec=NNNViewedExpressVec,
					 total=length(NViewedExpressVec)}),
		NPlayerGift;
	    false ->
		NViewedExpressVec = lists:sublist(ViewedExpressVec, 
						  player_gift:get_index(PageIndex, PageCount),
						  PageCount),
		NNViewedExpressVec = player_gift:get_name(NViewedExpressVec, []),
		net_helper:send2client(Account, #notify_get_viewed_express_vec{
					 viewed_express_vec=NNViewedExpressVec,
					 total=length(ViewedExpressVec)}),
		PlayerGift
	end,
    db_common:write(NNPlayerGift),
    player_data:set_gift(NNPlayerGift, State).

set_receive_express_vec(Receiver, GiftExpress) ->
    case util:is_process_alive(Receiver) of
	true ->
	    router:send(Receiver, set_receive_express_vec, GiftExpress);
	false ->
	    PlayerGift = db_common:select_one(player_gift, Receiver),
	    player_gift_data:set_receive_express_vec(PlayerGift, GiftExpress)
    end.

set_subscribe_express_vec(Receiver, GiftExpress) ->
    case util:is_process_alive(Receiver) of
	true ->
	    router:send(Receiver, set_subscribe_express_vec, GiftExpress);
	false ->
	    PlayerGift = db_common:select_one(player_gift, Receiver),
	    player_gift_data:set_subscribe_express_vec(PlayerGift, GiftExpress)
    end.

%% 后台物品金钱记录
log_receive_gift_express(Account, ReceiveExpress, PlayerGiftPackGrid) ->
    GiftIndex = 0,
    GiftItem = pack:get_item(GiftIndex, PlayerGiftPackGrid),
    ItemInfo = {item:get_tempid(GiftItem), 
		pack:get_item_count(GiftIndex, PlayerGiftPackGrid), 
		item:get_instid(GiftItem)},
    Target = get_sender(ReceiveExpress),    
    
    item_money_log:log_item(Account, receive_gift_express, ItemInfo, Target),
    ItemInfo.

    
    
%% 后台金钱物品记录
log_send_gift_express(Account, GameCoin, EQCoin, Recver, GiftIndex, State) ->    
    MoneyInfo = {-GameCoin, -EQCoin},
    Bag = player_data:get_pack(?pt_bag, State),
    
    GiftItem = case GiftIndex >= 0 of
		   true ->
		       pack:get_item(GiftIndex, Bag);
		   _ ->
		       empty
	       end,

    ItemInfo = 
	case GiftItem of
	    empty ->
		{0, 0, 0};
	    _ ->
		{item:get_tempid(GiftItem), 
		 -pack:get_item_count(GiftIndex, Bag), 
		 item:get_instid(GiftItem)}
	end,
    Target = Recver, 
    item_money_log:log(Account, send_gift_express, ItemInfo, MoneyInfo, Target).
