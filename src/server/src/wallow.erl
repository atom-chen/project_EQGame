%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   防沉迷模块
%%% @end
%%% Created : 16 Jun 2011 by hongjx <hongjx@35info.cn>

-module(wallow).
-export([new/1, calc_income/2, calc_item/2, save_online_time/2,
	 income_percent/1, on_login/3, on_logout/3,
	 in_normal_time/1, in_warn_time/1, in_bad_time/1 
	 ]).

-include("packet_def.hrl").

%%%===================================================================
%%% Api
%%%===================================================================

%% 计算收入
%% 传入金钱或物品
%% 如果返回物品为空的话,返回[]
calc_income(Income, PlayerData) when is_integer(Income) -> 
    case income_percent(PlayerData) of 
	100 ->
	    Income;
	Percent ->   
	    Result = Income * Percent div 100,
	    Result
    end.

calc_item(Item, PlayerData) -> 
    %% 判断是否处在正常游戏时间
    case in_normal_time(PlayerData) of
	true ->
	    Item;
	_ ->
	    %% 判断是否物品ID
	    case is_integer(Item) of
		true -> 0;
		_ -> []
	    end
    end.

income_percent(PlayerData) -> 
    %% 判断是否启用防沉迷系统
    IsWallowAccount =
	case get_wallow_info(PlayerData) of
	    {_LastOnlineTime, _LoginTime} ->
		true;
	    _ -> false
	end,

    case IsWallowAccount of
	true ->   
	    Time = get_wallow_time(PlayerData),
	    get_percent(Time);
	_ ->
	    100
    end.

%% 判断是否处在零收益时间
in_bad_time(PlayerData) ->
    Percent = income_percent(PlayerData),
    Percent =:= 0. 


%% 判断是否处在收益减少时间
in_warn_time(PlayerData) ->
    Percent = income_percent(PlayerData),
    Percent =:= 50. 
     

%% 判断是否正常游戏时间
in_normal_time(PlayerData) ->
    Percent = income_percent(PlayerData),
    Percent =:= 100. 

on_login(Account, SystemFlag, IsWallowAccount) ->  
    %%data_helper:format("wallow on login ~p~n~n", [Account]),
    Send2Client = case SystemFlag of 
		      farm -> farm2client;
		      _ -> send2client
		  end,

    %% 判断是否启用防沉迷系统
    case is_wallow_account(Account, IsWallowAccount) of
	true -> 
	    WallowInfo = db_wallow:on_login(Account, SystemFlag),
	    data_helper:format("wallow ~p~n~n", [WallowInfo]),
	    {LastOnlineTime, LoginTime} = WallowInfo,

	    DiffTime = datetime:diff_time(LoginTime, datetime:localtime()),
	    Seconds = LastOnlineTime + DiffTime,
	    net_helper:Send2Client(Account, #notify_wallow_time{wallow_seconds=Seconds}),
	    start_timer(),
	    WallowInfo;
	_ ->
	    net_helper:Send2Client(Account, #notify_wallow_time{wallow_seconds=-1}),
	    undefined
    end.


on_logout(Account, SystemFlag, WallowInfo) when is_tuple(WallowInfo) ->    
    %%data_helper:format("wallow on logout ~n~n"),
    db_wallow:on_logout(Account, SystemFlag, WallowInfo);
on_logout(_Account, _SystemFlag, _) ->
    ok.

new(Account) ->
    db_wallow:new(Account).
%%%===================================================================
%%% 内部函数
%%%===================================================================
%% 判断是否防沉迷账号 
is_wallow_account(Account, IsWallowAccount) when is_atom(Account) ->
    List = common_def:get_val(wallow_accounts),
    case lists:member(Account, List) of 
	true ->
	     true;
	_ ->
	    IsWallowAccount and	common_def:get_val(wallow_system_on)
    end.


%% 算沉迷时间
get_wallow_time(PlayerData)  ->
    case get_wallow_info(PlayerData) of
	{LastOnlineTime, LoginTime} ->
	    DiffTime = datetime:diff_time(LoginTime, datetime:localtime()),
	    LastOnlineTime + DiffTime;
	_ ->
	    0
    end.

%%根据沉迷时间算收益
get_percent(WallowTime) ->
    Hour = WallowTime div common_def:get_val(wallow_seconds_per_hour),
    case (0 =< Hour) and (Hour < 5) of
	true ->
	    case Hour < 3 of
		true -> 100;   %% 3小时以内 100% 收益
		_ -> 50        %% 3~5小时   50% 收益
	    end;
	_ ->
	    0                  %% 其它时间   0% 收益
    end. 


get_wallow_info(PlayerData) when element(1, PlayerData) == player_data ->
    player_data:get_wallow_info(PlayerData);
get_wallow_info(FarmData) -> 
    farm:get_wallow_info(FarmData).

get_account(PlayerData) when element(1, PlayerData) == player_data ->
    player_data:get_account(PlayerData);
get_account(FarmData) -> 
    farm:get_account(FarmData).


do_after(Seconds, Fun, Arg) ->   
    Event = {timer, ?MODULE, Fun, Arg},
    erlang:send_after(Seconds * 1000, self(), Event).    

start_timer()->
    Seconds = common_def:get_val(wallow_seconds_auto_save),
    do_after(Seconds, save_online_time, Seconds).

%% 保存上线时间 (回调函数)
save_online_time(Interval, State) ->
  
    db_wallow:save(get_account(State), get_wallow_info(State)),
    %% 设定下次保存时间
    do_after(Interval, save_online_time, Interval),
    State.
