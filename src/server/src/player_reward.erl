%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% ��ҽ���
%%% @end
%%% Created :  8 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_reward).

-include("common_def.hrl").
-include("router.hrl").
-include("packet_def.hrl").
-include("player_data.hrl").
-include("enum_def.hrl").
-include("tplt_def.hrl").
-include("sys_msg.hrl").

%% API
-export([pay/4]).

%%%===================================================================
%%% API
%%%===================================================================
pay(coin, Task, PlayerData) ->
    NPlayerData = pay(game_coin, Task, PlayerData),
    NNPlayerData = pay(eq_coin, Task, NPlayerData),
    {true, NNPlayerData};
%% ֧������ҽ��    
pay(game_coin, Task, PlayerData) ->
    case Task#player_task_tplt.reward_game_coin > 0 of
	true ->
	    Count = Task#player_task_tplt.reward_game_coin,
	    CoinType = ?GAMECOIN,
	    NPlayerData = player_coin:add_coin2(CoinType, Count, PlayerData),
	    %% ����, ��Ʒ��log��¼	    
	    log_reward_money(CoinType, Count, NPlayerData),
	    NPlayerData;
	_ ->
	    PlayerData
    end;
pay(eq_coin, Task, PlayerData) ->
    case Task#player_task_tplt.reward_eq_coin > 0 of
	true ->
	    Count = Task#player_task_tplt.reward_eq_coin,
	    CoinType = ?EQCOIN,
	    NPlayerData = player_coin:add_coin2(CoinType, Count, PlayerData),
	    %% ����, ��Ʒ��log��¼
	    log_reward_money(CoinType, Count, NPlayerData),
	    NPlayerData;
	_ ->
	    PlayerData
    end.

    

%% ֧���������Ʒ
pay(item, Account, Task, PlayerData) ->
    ItemList = Task#player_task_tplt.fixed_reward_items,
    ItemCount = Task#player_task_tplt.fixed_reward_items_count,
    PayItemList = player_task_item:get_fixed_items(ItemList, ItemCount, []),
    add_item(Account, PayItemList, PlayerData);
%% ֧����ҽ���
pay(Account, Task, ItemId, PlayerData) when ItemId == 0 ->
    case pay(item, Account, Task, PlayerData) of
	{true, NPlayerData} ->
	    pay(coin, Task, NPlayerData);
	{false, Reason} ->
	    {false, Reason}
    end;
pay(Account, Task, ItemId, PlayerData) ->
    case pay(item, Account, Task, ItemId, PlayerData) of
	{true, NPlayerData} ->
	    pay(coin, Task, NPlayerData);
	{false, Reason} ->
	    {false, Reason}
    end.

%% ֧�������Ʒ(����ѡ�����Ʒ)
-spec pay(atom(), atom(), tuple(), integer(), tuple()) -> tuple().
pay(item, Account, Task, ItemId, PlayerData) ->
    ItemList = Task#player_task_tplt.select_reward_items,
    ItemCount = Task#player_task_tplt.select_reward_items_count,
    case player_task_item:contains(ItemList, ItemId) of
	true ->
	    PayItemList = player_task_item:get_selected_items(ItemList, ItemCount, []),
	    add_item(Account, PayItemList, PlayerData);
	_ ->
	    {false, not_contain_item}
    end.
    
%%%===================================================================
%%% Internal functions
%%%===================================================================
add_item(_Account, PayItemList, PlayerData) when PayItemList == [] ->
    {true, PlayerData};
add_item(Account, PayItemList, PlayerData) when PayItemList /= [] ->
    Items = [item:make_item(TempID) || TempID <- PayItemList],
    case player_pack:add_item2(Account, PayItemList, PlayerData) of
	{ok, NPlayerData} ->
	    %% ����, ��Ʒ��log��¼	    
	    Count = 1,
	    ItemInfo = [{item:get_tempid(X), Count, item:get_instid(X)} 
	     		|| X <- Items],
	    item_money_log:log_item(Account, reward_items, ItemInfo),
	    {true, NPlayerData};
	{error, _} ->
	    {false, not_enough_position}
    end.

%% ���ҵ�log��¼	    
log_reward_money(CoinType, Count, PlayerData) ->
    MoneyInfo =	item_money_log:get_money_info(CoinType, Count), 
    Account = player_data:get_account(PlayerData),
    item_money_log:log_money(Account, reward_money, MoneyInfo).
