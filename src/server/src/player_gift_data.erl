%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% �����������
%%% @end
%%% Created :  3 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(player_gift_data).

-include("packet_def.hrl").
-include("common_def.hrl").

-record(player_gift, {account,                      %% ����ʺ�
		      send_box = #gift_box{},       %% ��ǰ�������
		      send_box_vec = [],            %% ��ҷ��͹�������б�
		      receive_box_vec = [],         %% ��ҽ��յ�(δ�鿴)����б�
		      viewed_box_vec = [],          %% ����Ѳ鿴��������б�
		      send_express_vec = [],        %% ��ҷ��͵Ŀ���б�
		      receive_express_vec = [],     %% ��ҽ��չ��Ŀ���б�
		      viewed_express_vec = [],      %% �Ѳ鿴���Ŀ���б�
		      subscribe_express_vec = []    %% ���ĵĿ���б�
		     }).

%% API
-compile(export_all).

%%%===================================================================
%%% API
%%%===================================================================
%% ��ʼ���������
-spec init(atom()) -> tuple().
init(Account) ->
    SendBox = new_send_box(),
    #player_gift{account=Account, send_box=SendBox}.

new_send_box() ->
    GridVec = pack:new(?init_gift_grid_count),
    #gift_box{grid_vec=GridVec}.

new_gift_express() ->
    pack:new(?init_gift_grid_count).

%% ��ȡ��������
get_send_box(#player_gift{send_box=SendBox}) ->
    SendBox.

%% ������������
set_send_box(PlayerGift, SendBox) ->
    PlayerGift#player_gift{send_box=SendBox}.

drop_send_box(PlayerGift) ->
    SendBox = new_send_box(),
    PlayerGift#player_gift{send_box=SendBox}. 

%% ��ȡ��ҷ��͹���������б�
get_send_box_vec(#player_gift{send_box_vec=SendBoxVec}) ->
    SendBoxVec.

%% ������ҷ��͹���������б�
set_send_box_vec(PlayerGift, SendBoxVec) when is_list(SendBoxVec) ->
    PlayerGift#player_gift{send_box_vec=SendBoxVec};
set_send_box_vec(PlayerGift, SendBox) ->
    SendBoxVec = get_send_box_vec(PlayerGift),
    set_send_box_vec(PlayerGift, [SendBox | SendBoxVec]).

%% ��ȡ��ҽ��ܹ���������б�
get_receive_box_vec(#player_gift{receive_box_vec=ReceiveBoxVec}) ->
    ReceiveBoxVec.

%% ������ҽ��չ���������б�
set_receive_box_vec(PlayerGift, ReceiveBoxVec) when is_list(ReceiveBoxVec) ->
    PlayerGift#player_gift{receive_box_vec=ReceiveBoxVec};
set_receive_box_vec(PlayerGift, ReceiveBox) ->
    ReceiveBoxVec = get_receive_box_vec(PlayerGift),
    set_receive_box_vec(PlayerGift, [ReceiveBox | ReceiveBoxVec]).

drop_receive_box(PlayerGift, ReceiveBox) ->
    ReceiveExpressVec = get_receive_box_vec(PlayerGift),
    lists:delete(ReceiveBox, ReceiveExpressVec).

%% ��ȡ����Ѿ��鿴����������б�
get_viewed_box_vec(#player_gift{viewed_box_vec=ViewedBoxVec}) ->
    ViewedBoxVec.

set_viewed_box_vec(PlayerGift, ViewedBoxVec) when is_list(ViewedBoxVec) ->
    PlayerGift#player_gift{viewed_box_vec=ViewedBoxVec};
set_viewed_box_vec(PlayerGift, ViewedBox) ->
    ViewedBoxVec = get_viewed_box_vec(PlayerGift),
    set_viewed_box_vec(PlayerGift, [ViewedBox | ViewedBoxVec]).

%% ��ȡ��ҷ��͹����������б�
get_send_express_vec(#player_gift{send_express_vec=SendExpressVec}) ->
    SendExpressVec.

%% ������ҷ��͹����������б�
set_send_express_vec(PlayerGift, SendExpressVec) when is_list(SendExpressVec) ->
    PlayerGift#player_gift{send_express_vec=SendExpressVec};
set_send_express_vec(PlayerGift, SendExpress) ->
    SendExpressVec = get_send_express_vec(PlayerGift),
    set_send_express_vec(PlayerGift, [SendExpress | SendExpressVec]).

%% ��ȡ��ҽ��չ����������б�
get_receive_express_vec(#player_gift{receive_express_vec=ReceiveExpressVec}) ->
    ReceiveExpressVec.

%% ������ҽ��չ����������б�
set_receive_express_vec(PlayerGift, ReceiveExpressVec) when is_list(ReceiveExpressVec) ->
    PlayerGift#player_gift{receive_express_vec=ReceiveExpressVec};
set_receive_express_vec(PlayerGift, ReceiveExpress) ->
    ReceiveExpressVec = get_receive_express_vec(PlayerGift),
    set_receive_express_vec(PlayerGift, [ReceiveExpress | ReceiveExpressVec]).

drop_receive_express(PlayerGift, ReceiveExpress) ->
    ReceiveExpressVec = get_receive_express_vec(PlayerGift),
    lists:delete(ReceiveExpress, ReceiveExpressVec).

get_viewed_express_vec(#player_gift{viewed_express_vec=ViewedExpressVec}) ->
    ViewedExpressVec.

set_viewed_express_vec(PlayerGift, ViewedExpressVec) when is_list(ViewedExpressVec) ->
    PlayerGift#player_gift{viewed_express_vec=ViewedExpressVec};
set_viewed_express_vec(PlayerGift, ViewedExpress) ->
    ViewedExpressVec = get_viewed_express_vec(PlayerGift),
    set_viewed_express_vec(PlayerGift, [ViewedExpress | ViewedExpressVec]).

%% ��ȡ��Ҷ��ĵ��������б�
get_subscribe_express_vec(#player_gift{subscribe_express_vec=SubScribeExpressVec}) ->
    SubScribeExpressVec.

%% ������Ҷ��ĵ��������б�
set_subscribe_express_vec(PlayerGift, SubScribeExpressVec) when is_list(SubScribeExpressVec) ->
    PlayerGift#player_gift{subscribe_express_vec=SubScribeExpressVec};
set_subscribe_express_vec(PlayerGift, SubScribeExpress) ->
    SubScribeExpressVec = get_subscribe_express_vec(PlayerGift),
    set_subscribe_express_vec(PlayerGift, [SubScribeExpress | SubScribeExpressVec]).

drop_subscribe_express(PlayerGift, SubExpress) ->
    SubExpressVec = get_subscribe_express_vec(PlayerGift),
    NSubExpressVec = lists:delete(SubExpress, SubExpressVec),
    set_subscribe_express_vec(PlayerGift, NSubExpressVec).

get_sender(#gift_box{sender=Sender}) ->
    Sender.

get_grid_vec(#gift_box{grid_vec=GridVec}) ->
    GridVec.

set_grid_vec(GiftBox, GridVec) ->
    GiftBox#gift_box{grid_vec=GridVec}.
%%%===================================================================
%%% Internal functions
%%%===================================================================
