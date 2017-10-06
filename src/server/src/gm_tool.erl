%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% GM工具
%%% @end
%%% Created :  6 Aug 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(gm_tool).

-behaviour(gen_server).

%% API
-export([start_link/0]).
-export([search_player/1, search_farm_player/1, is_online/1, 
	 add_coin/2, add_item/3, add_property/6, get_data/1,
	game_info_gather/0, edit_gm_level/2]).
-export([search_sys_broadcast/1, delete_sys_broadcast/2]).
-export([add_farm_exp/2]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-define(SERVER, ?MODULE). 

-include("packet_def.hrl").
-include("enum_def.hrl").

-record(state, {}).

%%%===================================================================
%%% API
%%%===================================================================
%% 搜索玩家
-spec search_player(atom()) -> tuple().
search_player(Account) when is_atom(Account) ->
    gen_server:call(?SERVER, {search_player, Account}).

search_farm_player(Account) when is_atom(Account) ->
    gen_server:call(?SERVER, {search_farm_player, Account}).

is_online(Account) when is_atom(Account) ->
    gen_server:call(?SERVER, {is_online, Account}).

add_coin(Account, Coin) when is_atom(Account), is_integer(Coin) ->
    gen_server:call(?SERVER, {add_coin, Account, Coin}).

add_item(Account, ItemId, InstId) when is_atom(Account), is_integer(ItemId) ->
    gen_server:call(?SERVER, {add_item, Account, ItemId, InstId}).

add_property(Account, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, HouseClean) ->
    gen_server:call(?SERVER, {add_property, Account, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, HouseClean}).

get_data(SceneName) ->
    gen_server:call(SceneName, get_data).

add_farm_exp(Account, Exp) when is_atom(Account) ->
    gen_server:call(?SERVER, {add_farm_exp, Account, Exp}).

search_sys_broadcast(Type) ->
    gen_server:call(?SERVER, {search_sys_broadcast, Type}).

delete_sys_broadcast(InstId, Type) ->
    gen_server:call(?SERVER, {delete_sys_broadcast, InstId, Type}).

edit_gm_level(Account, Level) ->
    gen_server:call(?SERVER, {edit_gm_level, Account,Level}).

%% 游戏信息收集，统计
game_info_gather() ->
    gen_server:call(?SERVER, game_info_gather, infinity).

			     
%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link() ->
    gen_server:start_link({local, ?SERVER}, ?MODULE, [], []).

%%%===================================================================
%%% gen_server callbacks
%%%===================================================================

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Initializes the server
%%
%% @spec init(Args) -> {ok, State} |
%%                     {ok, State, Timeout} |
%%                     ignore |
%%                     {stop, Reason}
%% @end
%%--------------------------------------------------------------------
init([]) ->
    {ok, #state{}}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling call messages
%%
%% @spec handle_call(Request, From, State) ->
%%                                   {reply, Reply, State} |
%%                                   {reply, Reply, State, Timeout} |
%%                                   {noreply, State} |
%%                                   {noreply, State, Timeout} |
%%                                   {stop, Reason, Reply, State} |
%%                                   {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
%% 查询玩家
handle_call({search_player, Account}, _From, State) ->
    BasicData = db_gm_tool:search_player(Account),
    {reply, BasicData, State};
handle_call({search_farm_player, Account}, _From, State) ->
    BasicData = db_gm_tool:search_farm_player(Account),
    {reply, BasicData, State};
%% 判断玩家是否在线
handle_call({is_online, Account}, _From, State) ->
    IsOnline = db_player:is_online(Account),
    {reply, IsOnline, State};
%% 增加玩家的金币
handle_call({add_coin, Account, Coin}, _From, State) ->
    PlayerCoin = db_common:select_one(player_coin, Account),
    OldCoin = PlayerCoin#player_coin.gamecoin,
    NPlayerCoin = PlayerCoin#player_coin{gamecoin=OldCoin+Coin},
    {reply, db_common:write(NPlayerCoin), State};
%% 增加玩家的物品
handle_call({add_item, Account, ItemId, InstId}, _From, State) ->
    PlayerPack = db_common:select_one(player_pack, Account),
    BagGrid = player_pack:get_grids(PlayerPack),
    Items = [item:new(ItemId, InstId)],
    Result = 
	case player_pack:add_item3(Account, Items, BagGrid) of
	    {ok, NPack} ->
		player_pack:save(Account, ?pt_bag, NPack);
	    {error, 'id not found', _} ->
		{fail, "fail"}
	end,

    {reply, Result, State};
handle_call({add_property, Account, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, HouseClean}, _From, State) ->
    PlayerProperty = db_common:select_one(player_property, Account),
    MaxPlayerClean = player_property_by_action:get_max_value(player_clean),
    NPlayerClean = add_property(PlayerProperty#player_property.player_clean, PlayerClean, MaxPlayerClean),
    MaxPlayerHealth = player_property_by_action:get_max_value(player_health),
    NPlayerHealth = add_property(PlayerProperty#player_property.player_health, PlayerHealth, MaxPlayerHealth),
    MaxPlayerCharm =  player_property_by_action:get_max_value(player_charm),
    NPlayerCharm = add_property(PlayerProperty#player_property.player_charm, PlayerCharm, MaxPlayerCharm),
    MaxActiveValue =  player_property_by_action:get_max_value(active_value),
    NActiveValue = add_property(PlayerProperty#player_property.active_value, ActiveValue, MaxActiveValue),
    NPlayerProperty = PlayerProperty#player_property{player_clean=NPlayerClean, 
						     player_health=NPlayerHealth, 
						     player_charm=NPlayerCharm, 
						     active_value=NActiveValue}, 
    HouseData = db_common:select_one(house_data, Account),
    NHouseData = house_clean:add(Account, HouseData, HouseClean),

    {reply, db_common:write([NPlayerProperty, NHouseData]), State};
handle_call({add_farm_exp, Account, Exp}, _From, State) ->
    PlayerFarm = db_farm:select(Account),
    NPlayerFarm = player_farm:add_exp(PlayerFarm, Exp),
    Ret = db_farm:write(NPlayerFarm),
    {reply, Ret, State};
handle_call({search_sys_broadcast, Type}, _From, State) ->
    Reply = db_gm_tool:search_sys_broadcast(Type),
    {reply, Reply, State};
handle_call({delete_sys_broadcast, InstId, Type}, _From, State) ->
    data_helper:format("InstId:~p, Ret:~p~n", [InstId, is_integer(InstId)]),
    Reply = db_common:delete(sys_broadcast, InstId),
    sys_broadcast:gm_change_broadcast(InstId, Type),
    {reply, Reply, State};
handle_call(game_info_gather, _From, State) ->
    {PlayerCount, ItemMoneyInfo} = item_money_log:gather(),

    Result = {PlayerCount, 
	     db_game_info_gather:avg_play_game_seconds(PlayerCount),
	     db_game_info_gather:avg_edit_house_seconds(),
	     ItemMoneyInfo},

    Msg = format_game_info(Result),

    {reply, Msg, State};

handle_call({edit_gm_level, Account, Level}, _From, State) ->
    BasicData = db_common:select_one(player_basic_data, Account),
    NBasicData=BasicData#player_basic_data{gm_enabled=Level},
    db_common:write(NBasicData),
    {reply, ok, State};

handle_call(_Request, _From, State) ->
    Reply = ok,
    {reply, Reply, State}.


%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling cast messages
%%
%% @spec handle_cast(Msg, State) -> {noreply, State} |
%%                                  {noreply, State, Timeout} |
%%                                  {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
handle_cast(_Msg, State) ->
    {noreply, State}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling all non call/cast messages
%%
%% @spec handle_info(Info, State) -> {noreply, State} |
%%                                   {noreply, State, Timeout} |
%%                                   {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
handle_info(_Info, State) ->
    {noreply, State}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% This function is called by a gen_server when it is about to
%% terminate. It should be the opposite of Module:init/1 and do any
%% necessary cleaning up. When it returns, the gen_server terminates
%% with Reason. The return value is ignored.
%%
%% @spec terminate(Reason, State) -> void()
%% @end
%%--------------------------------------------------------------------
terminate(_Reason, _State) ->
    ok.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Convert process state when code is changed
%%
%% @spec code_change(OldVsn, State, Extra) -> {ok, NewState}
%% @end
%%--------------------------------------------------------------------
code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
add_property(OldProperty, [], _MaxProperty) ->
    OldProperty;
add_property(OldProperty, NewProperty, MaxProperty) ->
    Result = OldProperty + NewProperty,
    if
	Result =< 0 ->
	    0;
	(Result > 0) and (Result < MaxProperty) ->
	    Result;
	Result >= MaxProperty ->
	    MaxProperty
    end.



format_game_info({0, _AvgGameSeconds, _AvgEditHouseSeconds, _ItemMoneyInfo}) ->
    "无玩家";
format_game_info({PlayerCount, AvgGameSeconds, AvgEditHouseSeconds, ItemMoneyInfo}) ->
    {TopInfo, IncomeInfo, AvgIncome, ConsumeInfo, AvgConsume} = ItemMoneyInfo,
    {TotalIncome, WorkIncome, FarmIncome} = IncomeInfo,
    {PlayerAvgIncome, AvgWorkIncome, AvgFarmIncome} = AvgIncome,
    {TotalConsume, FarmConsume, FurConsume, 
     HouseConsume, DressConsume, ScriptConsume} = ConsumeInfo,
    {PlayerAvgConsume, AvgFarmConsume, AvgFurConsume, 
     AvgHouseConsume, AvgDressConsume, AvgScriptConsume} = AvgConsume,

    {TopDress, TopFur, TopFarm} = TopInfo,

    Msg = 
	write_line("玩家个数: " ++ integer_to_list(PlayerCount) ++ "人") ++
	write_line("") ++
	write_line("总收入: " ++ get_money_str(TotalIncome)) ++ 
	write_line("\t打工收入: " ++ get_money_str(WorkIncome)) ++ 
	write_line("\t农场收入: " ++ get_money_str(FarmIncome)) ++ 
	write_line("") ++
	write_line("人均收入: " ++ get_money_str(PlayerAvgIncome)) ++ 
	write_line("\t人均打工收入: " ++ get_money_str(AvgWorkIncome)) ++ 
	write_line("\t人均农场收入: " ++ get_money_str(AvgFarmIncome)) ++
	write_line("") ++
	write_line("总支出: " ++ get_money_str(TotalConsume)) ++ 
	write_line("\t农场支出: " ++ get_money_str(FarmConsume)) ++ 
	write_line("\t家具支出: " ++ get_money_str(FurConsume)) ++ 
	write_line("\t购房支出: " ++ get_money_str(HouseConsume)) ++ 
	write_line("\t服装支出: " ++ get_money_str(DressConsume)) ++ 
	write_line("\t脚本(含疾病)支出: " ++ get_money_str(ScriptConsume)) ++ 
	write_line("") ++
	write_line("人均支出: " ++ get_money_str(PlayerAvgConsume)) ++ 
	write_line("\t人均农场支出: " ++ get_money_str(AvgFarmConsume)) ++ 
	write_line("\t人均家具支出: " ++ get_money_str(AvgFurConsume)) ++ 
	write_line("\t人均购房支出: " ++ get_money_str(AvgHouseConsume)) ++ 
	write_line("\t人均服装支出: " ++ get_money_str(AvgDressConsume)) ++
	write_line("\t人均脚本(含疾病)支出: " ++ get_money_str(AvgScriptConsume)) ++ 
	write_line("") ++
	write_line("服装消费前10: " ++ get_items_str(TopDress)) ++ 
	write_line("") ++
	write_line("家具消费前10: " ++ get_items_str(TopFur)) ++ 
	write_line("") ++
	write_line("农场消费前10: " ++ get_items_str(TopFarm)) ++ 
	write_line("") ++
	write_line("每人游戏时间: " ++ integer_to_list(AvgGameSeconds) ++ "秒") ++ 
	write_line("") ++
	write_line("每次家装时间: " ++ integer_to_list(AvgEditHouseSeconds) ++ "秒"),
    Msg.

get_items_str(List) ->
    F = fun({ItemID, Count}, Str) ->
		Str ++ write_line("") ++ "\t" ++ 
		    integer_to_list(ItemID) ++ ":" ++ integer_to_list(Count) ++ "个"	
	end,
    lists:foldl(F, "", List).

get_money_str({GameMoney, EQMoney}) ->
    integer_to_list(GameMoney) ++ "," ++ integer_to_list(EQMoney).

write_line(Str) ->
    Str ++ "\r\n".
