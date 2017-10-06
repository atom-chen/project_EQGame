%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  ������ϵĻ���
%%% @end
%%% Created : 21 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_coin).

-include("common_def.hrl").
-include("router.hrl").
-include("packet_def.hrl").

-export([start/1, new/3, handle_call/3, send_coin/1, dec_coin/2,
	 add_coin/3, add_coin2/3, dec_coin/3, dec_coin2/3 ]).
-export([read/1, get_coin/1, get_coin/2, set_coin/3, get_game_coin/1, get_eq_coin/1, diff_coin/3]). 
-export([is_out_of_range/1]).

start(Account) ->
    [router:make_event_target(get_coin, Account, {self(), player_coin}),
     router:make_event_target(add_coin, Account, {self(), player_coin}),
     router:make_event_target(dec_coin, Account, {self(), player_coin})
    ].

new(AccStr, GameCoin, EQCoin) ->
    #player_coin{account= AccStr, gamecoin=GameCoin, eqcoin=EQCoin}.

%%------------handle_call--------------------------------------
handle_call({#msg{event=get_coin}, Type}, _From, State) ->
    {reply, get_coin(Type, player_data:get_coin(State)), State};
handle_call({#msg{event=add_coin}, {Type, Count}}, _From, State) ->
    Coin = add_coin(Type, Count, player_data:get_coin(State)),
    {reply, ok, player_data:set_coin(Coin, State)};
handle_call({#msg{event=dec_coin}, {Type, Count}}, _From, State) ->
    Coin = dec_coin(Type, Count, player_data:get_coin(State)),
    {reply, ok, player_data:set_coin(Coin, State)}.

send_coin(#player_coin{account=Account}=PlayerCoin) ->
    net_helper:send2client(Account, #notify_update_coin{data=PlayerCoin}).

%% ���ӻ���, ���͸��ͻ���, �������ݿ�
add_coin(?GAMECOIN, Count, PlayerCoin) when is_record(PlayerCoin, player_coin) ->
    NPlayerCoin = add_coin2(?GAMECOIN, Count, PlayerCoin),
    db_coin:write(NPlayerCoin),
    send_coin(NPlayerCoin),
    NPlayerCoin;
add_coin(?EQCOIN, Count, PlayerCoin) when is_record(PlayerCoin, player_coin) ->
    Account = get_account(PlayerCoin),
    NPlayerCoin = read(Account),
    %% NPlayerCoin = db_common:select(player_coin, Account),
    NNPlayerCoin = add_coin2(?EQCOIN, Count, NPlayerCoin),
    db_memdisc:write(NNPlayerCoin),
    %% db_coin:write(NNPlayerCoin),
    send_coin(NNPlayerCoin),
    NNPlayerCoin;
add_coin(Type, Count, PlayerData) ->
    NCoin = add_coin(Type, Count, player_data:get_coin(PlayerData)),
    player_data:set_coin(NCoin, PlayerData).

%% ���ӻ���, �����͸��ͻ���, ���������ݿ�
add_coin2(?GAMECOIN, Count, #player_coin{gamecoin=GC}=PlayerCoin) ->
    Total = erlang:min(?MAX_GAMECOIN, GC + Count),
    PlayerCoin#player_coin{gamecoin = Total};
add_coin2(?EQCOIN, Count, #player_coin{eqcoin=EC}=PlayerCoin) ->
    Total = erlang:min(?MAX_EQCOIN, EC + Count),
    PlayerCoin#player_coin{eqcoin = Total};
add_coin2(?GAMECOIN, Count, PlayerData) ->
    NCoin = add_coin2(?GAMECOIN, Count, player_data:get_coin(PlayerData)),
    player_data:set_coin(NCoin, PlayerData);
add_coin2(?EQCOIN, Count, PlayerData) ->
    Account = player_data:get_account(PlayerData),
    PlayerCoin = read(Account),
    NCoin = add_coin2(?EQCOIN, Count, PlayerCoin),
    player_data:set_coin(NCoin, PlayerData).

%% ���ٻ���, ���͸��ͻ���, �������ݿ�
dec_coin(Type, Count, Account) when is_atom(Account) ->
    PlayerCoin = read(Account),
    dec_coin(Type, Count, PlayerCoin);
dec_coin(?GAMECOIN, Count, PlayerCoin) when is_record(PlayerCoin, player_coin) ->
    NPlayerCoin = dec_coin2(?GAMECOIN, Count, PlayerCoin),
    db_coin:write(NPlayerCoin),
    send_coin(NPlayerCoin),
    NPlayerCoin;
dec_coin(?EQCOIN, Count, PlayerCoin) when is_record(PlayerCoin, player_coin) ->
    Account = get_account(PlayerCoin),
    NPlayerCoin = read(Account),
    NNPlayerCoin = dec_coin2(?EQCOIN, Count, NPlayerCoin),
    db_memdisc:write(NNPlayerCoin),
    %% db_coin:write(NNPlayerCoin),
    send_coin(NNPlayerCoin),
    NNPlayerCoin;
dec_coin(Type, Count, PlayerData) ->
    NCoin = dec_coin(Type, Count, player_data:get_coin(PlayerData)),
    player_data:set_coin(NCoin, PlayerData).
dec_coin({DecGameCoin, DecEQCoin}, PlayerData) ->
    Account = player_data:get_account(PlayerData),
    PlayerCoin = read(Account),
    NPlayerCoin = dec_coin2(?GAMECOIN, DecGameCoin, PlayerCoin),
    NNPlayerCoin = dec_coin2(?EQCOIN, DecEQCoin, NPlayerCoin),
    db_memdisc:write(NNPlayerCoin),
    send_coin(NNPlayerCoin),
    player_data:set_coin(NNPlayerCoin, PlayerData).


%% ���ٻ���, �����͸��ͻ���, Ҳ���������ݿ�
dec_coin2(?GAMECOIN, Count, #player_coin{gamecoin=GC}=PlayerCoin) ->
    Total = erlang:max(0, GC - Count),
    PlayerCoin#player_coin{gamecoin = Total};
dec_coin2(?EQCOIN, Count, #player_coin{eqcoin=EC}=PlayerCoin) ->
    Total = erlang:max(0, EC - Count),
     PlayerCoin#player_coin{eqcoin = Total};
dec_coin2(Type, Count, PlayerData) ->
    NCoin = dec_coin2(Type, Count, player_data:get_coin(PlayerData)),
    player_data:set_coin(NCoin, PlayerData).

%% ��û�������
get_coin(#player_coin{gamecoin=GC, eqcoin=EC}) ->
    {GC, EC}.

get_coin(?GAMECOIN, #player_coin{gamecoin=GC}) ->
    GC;
get_coin(?EQCOIN, #player_coin{eqcoin=EC}) ->
    EC;
get_coin(?EQCOIN, Account) when is_atom(Account) ->
    PlayerCoin = read(Account),
    get_coin(?EQCOIN, PlayerCoin);
get_coin(?GAMECOIN, PlayerData) ->
    get_coin(?GAMECOIN, player_data:get_coin(PlayerData));
get_coin(?EQCOIN, PlayerData) ->
    Account = player_data:get_account(PlayerData),
    get_coin(?EQCOIN, Account).

read(Account) ->
    db_memdisc:read(player_coin, Account).


%% ���ý�Ǯ������
set_coin(?GAMECOIN, GameCoin, PlayerCoin) when is_record(PlayerCoin, player_coin)->
    PlayerCoin#player_coin{gamecoin = GameCoin};
set_coin(?EQCOIN, EQCoin, PlayerCoin) when is_record(PlayerCoin, player_coin)->
    PlayerCoin#player_coin{eqcoin = EQCoin};
set_coin(Type, Coin, PlayerData) ->
    NCoin = set_coin(Type, Coin, player_data:get_coin(PlayerData)),
    player_data:set_coin(NCoin, PlayerData).
    

%% �����Ϸ��
get_game_coin(Data) ->
    get_coin(?GAMECOIN, Data).

get_eq_coin(Data) ->
    get_coin(?EQCOIN, Data).

%% �������PlayerData��ָ����ҵĲ�ֵ
diff_coin(Type, PlayerData1, PlayerData2) ->
    Coin1 = get_coin(Type, PlayerData1),
    Coin2 = get_coin(Type, PlayerData2),
    erlang:abs(Coin1 - Coin2).

%% �ж�Total Coin�Ƿ��Ѿ��ﵽ���߳����� 
is_out_of_range(Money) when is_integer(Money)->
    (Money < 0) or (Money > ?MAX_GAMECOIN).    
    
%%%===================================================================
%%% Internal functions
%%%===================================================================
get_account(#player_coin{account=Account}) ->
    Account.

