%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 玩家等级
%%% @end
%%% Created : 17 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_level).

%% API
-export([start/1, subscribe/3, handle_call/3, handle_cast/2]).
-export([get_level/1, get_level/2, get_level/3]).

-include("packet_def.hrl").
-include("tplt_def.hrl").
-include("common_def.hrl").
-include("router.hrl").
-include("player_data.hrl").


%% 启动玩家在线时长计算
-spec start(atom()) -> any().
start(Account) ->
    [router:make_event_source(calc_player_level, Account, {self(), ?MODULE}),
     router:make_event_target(is_exceed_player_level, Account, {self(), ?MODULE})
    ].

subscribe(Account, OnlineTime, LoginTime) ->
    Timer = get_timer(OnlineTime, LoginTime),
    data_helper:format("Player Level Timer:~p~n", [Timer]),
    sys_time:subscribe(?dt_datetime, Account, Timer, calc_player_level, []).
%%%===================================================================
%%% Handle call
%%====================================================================
handle_call({#msg{event=is_exceed_player_level}, VLevel}, _From, State) ->
    Level = get_level(State),
    Ret = (VLevel =:= Level),
    {reply, Ret, State}.

%%%===================================================================
%%% Handle cast
%%====================================================================
%% 更新玩家属性
handle_cast({#msg{event=calc_player_level}, Param}, #player_data{basic_data=BasicData, login_time=LoginTime}=State) ->
    OnlineTime = BasicData#player_basic_data.online_time,
    Timer = get_timer(OnlineTime, LoginTime),
    Account = BasicData#player_basic_data.account,
    sys_time:subscribe(?dt_datetime, Account, Timer, calc_player_level, Param),
    router:send(player_level_manager, player_level_up, {Account, get_level(State)}),
    {noreply, State}.

%%%===================================================================
%%% API
%%%===================================================================
get_level(#player_data{basic_data=BasicData, login_time=LoginTime}) ->
    OnlineTime = basic_data:get_data(online_time, BasicData),
    PlayerLevelTplt = tplt:get_all_data(player_level_tplt),
    player_level:get_level(PlayerLevelTplt, OnlineTime, LoginTime).

%% 根据在线时长获取等级
-spec get_level(list(), integer()) -> integer().
get_level(PlayerLevelTplt, OnlineTime) 
  when is_list(PlayerLevelTplt), is_integer(OnlineTime) ->
    do_get_level(OnlineTime, PlayerLevelTplt, []).

get_level(PLTplt, OnlineTime, LoginTime) ->
    DiffOnlineTime = datetime:diff_time(LoginTime, datetime:localtime()),
    get_level(PLTplt, OnlineTime + DiffOnlineTime).

%% 根据等级获取在线时长
-spec get_onlinetime(list(), integer()) -> integer().
get_onlinetime(PlayerLevelTplt, Level)
  when is_list(PlayerLevelTplt), is_integer(Level) ->
    do_get_onlinetime(Level, PlayerLevelTplt).

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 获取在线时长
-spec do_get_onlinetime(integer(), list()) -> integer().
do_get_onlinetime(_CurrLevel, [#player_level_tplt{online_time=OnlineTime}]) ->
    OnlineTime;
do_get_onlinetime(CurrLevel, [#player_level_tplt{level=Level, online_time=OnlineTime}|PlayerLevelTplt]) ->
    case CurrLevel =:= Level of
	true ->
	    OnlineTime;
	_ ->
	    do_get_onlinetime(CurrLevel, PlayerLevelTplt)
    end.
 
%% 获取等级
-spec do_get_level(integer(), list(), list()) -> integer().
do_get_level(CurrentOnlineTime, [#player_level_tplt{level=Level}], 
	     [#player_level_tplt{level=PrevLevel, online_time=PrevOnlineTime}]) ->
    case CurrentOnlineTime > PrevOnlineTime of
	true ->
	    PrevLevel;
	_ ->
	    Level
    end;
do_get_level(CurrentOnlineTime, [PlayerLevel|PlayerLevelList], []) ->
    do_get_level(CurrentOnlineTime, PlayerLevelList, [PlayerLevel]);
do_get_level(CurrentOnlineTime, [#player_level_tplt{online_time=OnlineTime}=PlayerLevel|PlayerLevelList], [#player_level_tplt{level=PrevLevel, online_time=PrevOnlineTime}]) ->
    case (CurrentOnlineTime >= PrevOnlineTime) and (CurrentOnlineTime < OnlineTime) of
	true ->
	    PrevLevel;
	_ ->
	    do_get_level(CurrentOnlineTime, PlayerLevelList, [PlayerLevel])
    end.

-spec get_timer(integer(), tuple()) -> integer().
get_timer(OnlineTime, LoginTime) ->
    DiffOnlineTime = datetime:diff_time(LoginTime, datetime:localtime()),
    COnlineTime = OnlineTime + DiffOnlineTime,
    PlayerLevelTplt = tplt:get_all_data(player_level_tplt),
    CLevel = get_level(PlayerLevelTplt, COnlineTime),
    CCOnlineTime = get_onlinetime(PlayerLevelTplt, CLevel),
    if
	CCOnlineTime =:= COnlineTime ->
    	    NOnlineTime = get_onlinetime(PlayerLevelTplt, CLevel + 1),
    	    NOnlineTime - COnlineTime;
	CCOnlineTime > COnlineTime ->
	    CCOnlineTime - COnlineTime;
	CCOnlineTime < COnlineTime ->
	    0
    end.

%%%===================================================================
%%% Test case
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

calculate_test() ->
    ?assertEqual(0,  do_get_level(10, [#player_level_tplt{level=0, online_time=0}, #player_level_tplt{level=1, online_time=12}, #player_level_tplt{level=2, online_time=24}], [])),
    ?assertEqual(1, do_get_level(43199, [#player_level_tplt{level=0, online_time=0}, #player_level_tplt{level=1, online_time=12}, #player_level_tplt{level=2, online_time=24}], [])),
    ?assertEqual(1, do_get_level(43200, [#player_level_tplt{level=0, online_time=0}, #player_level_tplt{level=1, online_time=12}, #player_level_tplt{level=2, online_time=24}], [])),
    ?assertEqual(1, do_get_level(43201, [#player_level_tplt{level=0, online_time=0}, #player_level_tplt{level=1, online_time=12}, #player_level_tplt{level=2, online_time=24}], [])).

-endif.
