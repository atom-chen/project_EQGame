%%%-------------------------------------------------------------------
%%% File    : player_SUITE.erl
%%% Author  :  <>
%%% Description : 
%%%
%%% Created : 28 Oct 2009 by  <>
%%%-------------------------------------------------------------------
-module(player_SUITE).

-compile(export_all).

-include("packet_def.hrl").
-include("ct.hrl").

%%--------------------------------------------------------------------
%% Function: suite() -> Info
%% Info = [tuple()]
%%--------------------------------------------------------------------
suite() ->
    [{timetrap,{seconds,30}}].

%%--------------------------------------------------------------------
%% Function: init_per_suite(Config0) ->
%%               Config1 | {skip,Reason} | {skip_and_save,Reason,Config1}
%% Config0 = Config1 = [tuple()]
%% Reason = term()
%%--------------------------------------------------------------------
init_per_suite(Config) ->
    Config.

%%--------------------------------------------------------------------
%% Function: end_per_suite(Config0) -> void() | {save_config,Config1}
%% Config0 = Config1 = [tuple()]
%%--------------------------------------------------------------------
end_per_suite(_Config) ->
    ok.

%%--------------------------------------------------------------------
%% Function: init_per_group(GroupName, Config0) ->
%%               Config1 | {skip,Reason} | {skip_and_save,Reason,Config1}
%% GroupName = atom()
%% Config0 = Config1 = [tuple()]
%% Reason = term()
%%--------------------------------------------------------------------
init_per_group(_GroupName, Config) ->
    Config.

%%--------------------------------------------------------------------
%% Function: end_per_group(GroupName, Config0) ->
%%               void() | {save_config,Config1}
%% GroupName = atom()
%% Config0 = Config1 = [tuple()]
%%--------------------------------------------------------------------
end_per_group(_GroupName, _Config) ->
    ok.

%%--------------------------------------------------------------------
%% Function: init_per_testcase(TestCase, Config0) ->
%%               Config1 | {skip,Reason} | {skip_and_save,Reason,Config1}
%% TestCase = atom()
%% Config0 = Config1 = [tuple()]
%% Reason = term()
%%--------------------------------------------------------------------
init_per_testcase(_TestCase, Config) ->
    Config.

%%--------------------------------------------------------------------
%% Function: end_per_testcase(TestCase, Config0) ->
%%               void() | {save_config,Config1}
%% TestCase = atom()
%% Config0 = Config1 = [tuple()]
%%--------------------------------------------------------------------
end_per_testcase(_TestCase, _Config) ->
    ok.

%%--------------------------------------------------------------------
%% Function: groups() -> [Group]
%% Group = {GroupName,Properties,GroupsAndTestCases}
%% GroupName = atom()
%% Properties = [parallel | sequence | Shuffle | {RepeatType,N}]
%% GroupsAndTestCases = [Group | {group,GroupName} | TestCase]
%% TestCase = atom()
%% Shuffle = shuffle | {shuffle,{integer(),integer(),integer()}}
%% RepeatType = repeat | repeat_until_all_ok | repeat_until_all_fail |
%%              repeat_until_any_ok | repeat_until_any_fail
%% N = integer() | forever
%%--------------------------------------------------------------------
groups() ->
    [].

%%--------------------------------------------------------------------
%% Function: all() -> GroupsAndTestCases | {skip,Reason}
%% GroupsAndTestCases = [{group,GroupName} | TestCase]
%% GroupName = atom()
%% TestCase = atom()
%% Reason = term()
%%--------------------------------------------------------------------
all() -> 
    [test_start_walk, test_stop_walk, test_sync_pos].

%% 测试开始行走
test_start_walk() -> 
    [].
test_start_walk(_Config) ->
    Account = test_acc,
    player:test_start(Account),
    CurrPos = #point{x=0.0,y=0.0,z=0.0},
    DestPos = #point{x=1.0,y=0.0,z=1.0},
    player:process_message(Account, {?msg_req_start_walk, 
				     #req_start_walk{curr_pos=CurrPos,
						    dest_pos=DestPos}}),
    ok.
test_stop_walk()->
    [].
test_stop_walk(_Config) ->
    Account = test_acc,
    player:test_start(Account),
    CurrPos = #point{x=0.0,y=0.0,z=0.0},
    player:process_message(Account, {?msg_req_stop_walk, 
				     #req_stop_walk{pos=CurrPos}}),
    ok.

test_sync_pos() ->
    [].
test_sync_pos(_Config)->
    Account = test_acc,
    player:test_start(Account),
    CurrPos = #point{x=0.0,y=0.0,z=0.0},
    player:process_message(Account, {?msg_req_sync_position, 
				     #req_sync_position{pos=CurrPos}}),
    ok.
    
    
