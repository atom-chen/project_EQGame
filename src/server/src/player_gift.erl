%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 房屋礼物动作
%%% @end
%%% Created :  2 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(player_gift).

-compile(export_all).

-include("packet_def.hrl").
-include("router.hrl").
-include("common_def.hrl").
-include("enum_def.hrl").
-include("tplt_def.hrl").
-include("sys_msg.hrl").


%%%===================================================================
%%% API
%%%===================================================================
get_name([], []) ->
    [];
get_name([], NGiftBoxVec) ->
    lists:reverse(NGiftBoxVec);
get_name([#gift_box{sender=Sender, receiver=Receiver}=GiftBox|GiftBoxVec], NGiftBoxVec) when Sender == Receiver ->
    Name = db_player:get_user_name(Sender),
    NGiftBox = GiftBox#gift_box{sender_name=Name, receiver_name=Name},
    get_name(GiftBoxVec, [NGiftBox|NGiftBoxVec]);
get_name([#gift_box{sender=Sender, receiver=Receiver}=GiftBox|GiftBoxVec], NGiftBoxVec) ->
    SenderName = db_player:get_user_name(Sender),
    ReceiverName = db_player:get_user_name(Receiver),
    NGiftBox = GiftBox#gift_box{sender_name=SenderName, receiver_name=ReceiverName},
    get_name(GiftBoxVec, [NGiftBox|NGiftBoxVec]);
get_name([#gift_express{sender=Sender, receiver=Receiver}=GiftExpress|GiftExpressVec], NGiftExpressVec) 
  when Sender == Receiver ->
    Name = db_player:get_user_name(Sender),
    NGiftExpress = GiftExpress#gift_express{sender_name=Name, receiver_name=Name},
    get_name(GiftExpressVec, [NGiftExpress|NGiftExpressVec]);
get_name([#gift_express{sender=Sender, receiver=Receiver}=GiftExpress|GiftExpressVec], NGiftExpressVec) ->
    SenderName = 
	case GiftExpress#gift_express.sender_name == [] of
	    true ->
		db_player:get_user_name(Sender);
	    false ->
		GiftExpress#gift_express.sender_name
	end,
    ReceiverName = db_player:get_user_name(Receiver),
    NGiftExpress = GiftExpress#gift_express{sender_name=SenderName, receiver_name=ReceiverName},
    get_name(GiftExpressVec, [NGiftExpress|NGiftExpressVec]).

get_index(1, _PageCount) ->
    1;
get_index(PageIndex, PageCount) ->
    (PageIndex - 1) * PageCount + 1.

has_item(PlayerGiftPack) ->
    FHasItem = fun(X) ->
		       ItemData = X#pack_grid.item_data,
		       ItemData#item.template_id > 0
	       end,
    pack:count_if(FHasItem, PlayerGiftPack) > 0.


validate_pack_count() ->
    fun({PlayerGift, GameCoin, EQCoin, Count}) ->
	    case (GameCoin > 0) or (EQCoin > 0) of %% 如果是付费的则都可以进行礼物的包装
		true ->
		    true;
		false ->
		    SendBoxVec = player_gift_data:get_send_box_vec(PlayerGift),
		    FreeBoxCount = free_box_count(SendBoxVec, 0),
		    data_helper:format("FreeBoxCount:~p~n", [FreeBoxCount]),
		    (FreeBoxCount + 1) =< Count
	    end
    end.

free_box_count([], Count) ->
    Count;
free_box_count([#gift_box{box_id=BoxId, card_id=CardId, date=Date}|SendBoxVec], Count) ->
    case datetime:is_equal(Date, datetime:localtime()) of
	true ->
	    {GiftBoxGameCoin, GiftBoxEQCoin} = get_gift_box_coin(BoxId),
	    {GiftCardGameCoin, GiftCardEQCoin} = get_gift_card_coin(CardId),
	    case ((GiftBoxGameCoin + GiftCardGameCoin) > 0) or ((GiftBoxEQCoin + GiftCardEQCoin) > 0) of
		true ->
		    free_box_count(SendBoxVec, Count);
		false ->
		    free_box_count(SendBoxVec, Count + 1)
		end;
	false ->
	    Count
    end.
    

%% 验证长度
validate_length() ->
    fun({Data, Length}) ->
	    ((util:len(Data) =< Length) and (util:len(Data) >= 0))
    end.

validate_is_contains_item() ->
    fun({Index, State}) ->
	    case Index >= 0 of
		true ->
		    Bag = player_data:get_pack(?pt_bag, State),
		    pack:is_grid_empty(Index, Bag) == false;
		false ->
		    true
	    end
    end.
	
validate_item_trade() ->    
    fun({Index, State}) ->
	    case Index >= 0 of
		true ->
		    Bag = player_data:get_pack(?pt_bag, State),
		    Item = pack:get_item(Index, Bag),
		    case item:can_trade(Item) of
			true ->
			    true;
			{false, _} ->
			    false
		    end;
		false ->
		    true
	    end
    end.

validate_gift_box() ->
    fun(BoxId) ->
	    BoxId > 0
    end.

%% 验证金币是否足够
validate_coin() ->
    fun({NeedCoin, PlayerCoin}) ->
	    PlayerCoin >= NeedCoin
    end.

%% 扣除金币
-spec dec_coin(integer(), integer(), tuple()) -> tuple().
dec_coin(GameCoin, EQCoin, PlayerData) ->
    NPlayerData = player_coin:dec_coin(?GAMECOIN, GameCoin, PlayerData),
    NNPlayerData = player_coin:dec_coin(?EQCOIN, EQCoin, NPlayerData),
    PlayerCoin = player_data:get_coin(NNPlayerData),
    {PlayerCoin, NNPlayerData}.
    
%% 获取礼物需要的金币
-spec get_coin(integer() | tuple(), tuple()) -> tuple().
get_coin(CardId, PlayerData) when is_integer(CardId) ->
    get_coin(0, CardId, PlayerData);
get_coin(#gift_box{box_id=BoxId, card_id=CardId}=PackGiftBox, PlayerData) when is_record(PackGiftBox, gift_box) ->
    get_coin(BoxId, CardId, PlayerData).
%%%===================================================================
%%% Handle cast
%%%===================================================================


%%%===================================================================
%%% Internal functions
%%%===================================================================
-spec get_coin(integer(), integer(), tuple()) -> tuple().
get_coin(BoxId, CardId, PlayerData) ->
    {GiftBoxGameCoin, GiftBoxEQCoin} = get_gift_box_coin(BoxId),
    {GiftCardGameCoin, GiftCardEQCoin} = get_gift_card_coin(CardId),
    {PlayerGameCoin, PlayerEQCoin} = get_player_coin(PlayerData),
    {GiftBoxGameCoin + GiftCardGameCoin, GiftBoxEQCoin + GiftCardEQCoin, PlayerGameCoin, PlayerEQCoin}.

%% 获取礼物盒所需要的金币
-spec get_gift_box_coin(integer()) -> tuple().
get_gift_box_coin(BoxId) when BoxId == 0 ->
    {0, 0};
get_gift_box_coin(BoxId) ->
    GiftBoxTplt = tplt:get_data(gift_box_tplt, BoxId),
    GiftBoxGameCoin = GiftBoxTplt#gift_box_tplt.game_coin,
    GiftBoxEQCoin = GiftBoxTplt#gift_box_tplt.eq_coin,
    {GiftBoxGameCoin, GiftBoxEQCoin}.

%% 获取礼物卡所需要的金币
-spec get_gift_card_coin(integer()) -> tuple().
get_gift_card_coin(CardId) when CardId == 0 ->
    {0, 0};
get_gift_card_coin(CardId) ->
    GiftCardTplt = tplt:get_data(gift_card_tplt, CardId),
    GiftCardGameCoin = GiftCardTplt#gift_card_tplt.game_coin,
    GiftCardEQCoin = GiftCardTplt#gift_card_tplt.eq_coin,
    {GiftCardGameCoin, GiftCardEQCoin}.

%% 获取玩家身上的金币
-spec get_player_coin(tuple()) -> tuple().
get_player_coin(PlayerData) ->
    PlayerCoin = player_data:get_coin(PlayerData),
    player_coin:get_coin(PlayerCoin).
    %% {10000, 10000}.
