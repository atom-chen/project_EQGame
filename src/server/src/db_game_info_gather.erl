%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   游戏信息收集(玩家上线时间, 编辑房屋时间)
%%% @end
%%% Created : 21 Apr 2011 by hongjx <hongjx@35info.cn>

-module(db_game_info_gather).

-export([add_edit_house_seconds/1,
	 avg_edit_house_seconds/0,
	 avg_play_game_seconds/1
%%	 add_play_game_seconds/1
]).



add_edit_house_seconds(Seconds) ->
    Key = edit_house_time,
    F = fun() -> 
		{TotalSeconds, NTimes} = 
		    case get_val(Key) of
			undefined -> {0, 0};
			Val -> Val
		    end,
		NewVal = {TotalSeconds + Seconds, NTimes + 1},
		set_val(Key, NewVal)		
	end,
    run_trans(F).

%% add_play_game_seconds(Seconds) ->
%%     Key = play_game_time,
%%     F = fun() -> 
%% 		{TotalSeconds, InitTime} = 
%% 		    case get_val(Key) of
%% 			undefined -> {0, datetime:local_time()};
%% 			Val -> Val
%% 		    end,
%% 		NewVal = {TotalSeconds + Seconds, InitTime},
%% 		set_val(Key, NewVal)		
%% 	end,
%%     run_trans(F).



%% 平均每次编辑房屋多久
avg_edit_house_seconds() ->
    Key = edit_house_time,
    case db_common:select_one(game_info_gather, Key) of
	[] -> 0;
	{_Tab, Key, Val} -> 
	    {Seconds, Times} = Val,
	    case Times =:= 0 of
		true -> 0;
		_ -> Seconds div Times
	    end
    end.

%% 平均每人每天玩多久
avg_play_game_seconds(0) ->
    0;
avg_play_game_seconds(PlayerCount) ->    
    FSum = fun(PlayerBasicData, Acc) ->
		   OnlineSeconds = basic_data:get_data(online_time, PlayerBasicData),
		   OnlineSeconds + Acc
	   end,

    Total = db_common:transaction(
	       fun() -> 
		       mnesia:foldl(FSum, 0, player_basic_data) 
	       end),
    Total div PlayerCount.
%% avg_play_game_seconds(PlayerCount) ->    
%%     Key = play_game_time,
%%     case db_common:select_one(game_info_gather, Key) of
%% 	[] -> 0;
%% 	{_Tab, Key, Val} -> 
%% 	    {Seconds, InitTime} = Val,
%% 	    Now = datetime:local_time(),
%% 	    NDay = (datetime:diff_time(InitTime, Now) div (24 * 60 * 60)) + 1,
%% 	    Seconds div NDay div PlayerCount
%%     end.


%%%===================================================================
%%% 内部函数
%%%===================================================================
get_val(Key) ->    
    case mnesia:read({game_info_gather, Key}) of
	[] -> undefined;
	[{_Tab, Key, Val}] -> Val
    end.

set_val(Key, Val) ->	    
    R = {game_info_gather, Key, Val},
    mnesia:write(R).

run_trans(F) ->
    mnesia:activity(transaction, F, [], mnesia_frag).
