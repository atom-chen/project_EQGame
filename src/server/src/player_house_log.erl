%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 房屋日志
%%% @end
%%% Created :  4 Aug 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(player_house_log).

-include("enum_def.hrl").
-include("packet_def.hrl").

-define(LOGSAVETIME, 3 * 24 * 60 * 60).

%% API
-export([write/5, write/6, 
	 clear/1, 
	 reset/1]).


%%%===================================================================
%%% API
%%%===================================================================
%% 记录访问日志
-spec write(integer(), atom(), atom(), tuple(), tuple()) -> tuple().
write(Type, Account, Owner, DateTime, HouseData) 
  when is_integer(Type), is_atom(Account), is_atom(Owner), is_tuple(HouseData) ->
    write(Type, Account, Owner, 0, DateTime, HouseData).
-spec write(integer(), atom(), atom(), integer(), tuple(), tuple()) -> tuple().
write(Type, Account, Owner, ItemID, DateTime, HouseData) 
  when is_integer(Type), is_atom(Account), is_atom(Owner), is_tuple(HouseData) ->
    case Account /= Owner of
	true ->
	    Log = HouseData#house_data.visit_log,
	    NLog = filter(Log, datetime:localtime()),
	    NNLog = filter(Type, Account, ItemID, DateTime, NLog, 200),
	    HouseData#house_data{visit_log=NNLog};
	_ ->
	    HouseData
    end.

%% 清空记录
clear(HouseData)
  when is_tuple(HouseData) ->
	    HouseData#house_data{visit_log=[]}.

%% 重新设置访问记录相关内容
reset(#house_data{visit_log=[]}=HouseData) ->
    HouseData;
reset(#house_data{visit_log=VisitLog}=HouseData) ->
    NVisitLog = [set_username(Log)  || Log <- VisitLog],
    HouseData#house_data{visit_log=NVisitLog}.


%%%===================================================================
%%% Internal functions
%%%===================================================================
set_username(#house_visit_log{account=Account}=VisitLog) ->
    UserName = player:get_user_name(Account),
    VisitLog#house_visit_log{username=UserName}.

%% 过滤超过三天的列表内容
-spec filter(list(), tuple()) -> list().
filter(List, Now) when is_list(List) ->
    F = fun(#house_visit_log{datetime=DateTime}) ->
		DiffTime = datetime:diff_time(DateTime, Now),
		DiffTime =< ?LOGSAVETIME
	end,
    lists:filter(F, List).

%% 过滤超过200条的列表内容
filter(Type, Account, ItemID, DateTime, Log, MaxLog) when is_list(Log) ->
    LogLength = length(Log),
    NewLog = [#house_visit_log{type=Type, 
			       datetime=DateTime, 
			       account=Account,
			       item_id=ItemID} | Log],
    case LogLength  < MaxLog of
	true ->
	    NewLog;
	_ ->
	    lists:sublist(NewLog, 1, MaxLog)
    end.

%%%===================================================================
%%% Test case
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").

-ifdef(TEST).

filter(Type, Account, DateTime, Log, MaxLog) when is_list(Log) ->
    filter(Type, Account, 0, DateTime, Log, MaxLog).


filter1_test() ->
    ?assertEqual([#house_visit_log{type=?hvt_visit, datetime=1, account=test1}], 
		 filter(?hvt_visit, test1, 1, [], 1)),
    ?assertEqual([#house_visit_log{type=?hvt_visit, datetime=1, account=test3},
		  #house_visit_log{type=?hvt_visit, datetime=1, account=test1}],
		 filter(?hvt_visit, test3, 1, [#house_visit_log{type=?hvt_visit, datetime=1, account=test1},
					       #house_visit_log{type=?hvt_visit, datetime=1, account=test2}], 2)),
    ?assertEqual([#house_visit_log{type=?hvt_visit, datetime=1, account=test3},
		  #house_visit_log{type=?hvt_visit, datetime=1, account=test1},
		  #house_visit_log{type=?hvt_visit, datetime=1, account=test2}],
		 filter(?hvt_visit, test3, 1, [#house_visit_log{type=?hvt_visit, datetime=1, account=test1},
					       #house_visit_log{type=?hvt_visit, datetime=1, account=test2}], 3)),
    ?assertEqual([#house_visit_log{type=?hvt_visit, datetime=1, account=test3},
		  #house_visit_log{type=?hvt_visit, datetime=1, account=test1},
		  #house_visit_log{type=?hvt_visit, datetime=1, account=test2}],
		 filter(?hvt_visit, test3, 1, [#house_visit_log{type=?hvt_visit, datetime=1, account=test1},
					       #house_visit_log{type=?hvt_visit, datetime=1, account=test2}], 10)).

filter_test() ->
    Now = #stime{year=2010, month=8, day=5, hour=12, minute=10, second=10},
    List = [#house_visit_log{type=?hvt_visit, datetime=#stime{year=2010, month=8, day=1, hour=10, minute=10, second=10}, account=test3},
	    #house_visit_log{type=?hvt_visit, datetime=#stime{year=2010, month=8, day=2, hour=10, minute=10, second=10}, account=test1},
	    #house_visit_log{type=?hvt_visit, datetime=#stime{year=2010, month=8, day=3, hour=12, minute=10, second=10}, account=test2}],
    ?assertEqual([#house_visit_log{type=?hvt_visit, datetime=#stime{year=2010, month=8, day=3, hour=12, minute=10, second=10}, account=test2}], filter(List, Now)).

-endif.
