%%%-------------------------------------------------------------------
%%% File    : map_grid_SUITE.erl
%%% Author  :  <>
%%% Description : 
%%%
%%% Created : 15 Nov 2009 by  <>
%%%-------------------------------------------------------------------
-module(map_grid_SUITE).

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
    [test_calc_grid_index, test_get_near_grid, test_calc_total_grids].

%%--------------------------------------------------------------------
%% Function: TestCase() -> Info
%% Info = [tuple()]
%%--------------------------------------------------------------------
test_calc_grid_index() -> 
    [].
%%--------------------------------------------------------------------
%% Function: TestCase(Config0) ->
%%               ok | exit() | {skip,Reason} | {comment,Comment} |
%%               {save_config,Config1} | {skip_and_save,Reason,Config1}
%% Config0 = Config1 = [tuple()]
%% Reason = term()
%% Comment = term()
%%--------------------------------------------------------------------
test_calc_grid_index(_Config) -> 
    19 = map_grid:calc_grid_index(#point{x=100,z=100}, 150),
    25 = map_grid:calc_grid_index(#point{x=149,z=149}, 150),
    1 = map_grid:calc_grid_index(#point{x=15,z=15}, 150),
    ok.

test_get_near_grid()-> 
    [].
test_get_near_grid(_Config) ->
    [13,14,15,18,19,20,23,24,25] = 
	map_grid:get_near_grid(#point{x=100,z=100}, 149, 149, 1),
    [11,12,16,17,21,22] = 
	map_grid:get_near_grid(#point{x=15,z=100}, 149, 149, 1),
    [16,17,21,22] = 
	map_grid:get_near_grid(#point{x=15,z=130}, 149, 149, 1),
    [19,20,24,25] = 
	map_grid:get_near_grid(#point{x=130,z=130}, 149, 149, 1),
    [1,2,6,7] = 
	map_grid:get_near_grid(#point{x=10,z=10}, 149, 149, 1),
    [2,3,4,7,8,9] = 
	map_grid:get_near_grid(#point{x=70,z=10}, 149, 149, 1),
    [6,7,11,12,16,17] = 
	map_grid:get_near_grid(#point{x=10,z=70}, 149, 149, 1),
    ok.

test_calc_total_grids()->
    [].
test_calc_total_grids(_config) ->
    [1,2,3,4,5,6,7,8,9] = map_grid:calc_total_grids(70, 70),
    ok.
