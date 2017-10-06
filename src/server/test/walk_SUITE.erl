%%%-------------------------------------------------------------------
%%% File    : walk_SUITE.erl
%%% Author  :  <>
%%% Description : 
%%%
%%% Created : 29 Oct 2009 by  <>
%%%-------------------------------------------------------------------
-module(walk_SUITE).

-compile(export_all).

-include("ct.hrl").
-include("packet_def.hrl").

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
    [validate_walk_path].

%%--------------------------------------------------------------------
%% Function: TestCase() -> Info
%% Info = [tuple()]
%%--------------------------------------------------------------------
validate_walk_path() -> 
    [].

%%--------------------------------------------------------------------
%% Function: TestCase(Config0) ->
%%               ok | exit() | {skip,Reason} | {comment,Comment} |
%%               {save_config,Config1} | {skip_and_save,Reason,Config1}
%% Config0 = Config1 = [tuple()]
%% Reason = term()
%% Comment = term()
%%--------------------------------------------------------------------
validate_walk_path(_Config) ->
    CurrPoint = #point{x=0.0,y=0.0,z=0.0},
    Path = [#point{x=5.0,y=0.0,z=-5.0}, #point{x=2.0,y=0.0,z=2.0},
	    #point{x=-6.0,y=0.0,z=6.0}, #point{x=-1.0,y=0.0,z=-1.0},
        #point{x=2.0,y=0.0,z=-2.0}, #point{x=2.0,y=0.0,z=-11.0}],
    Speed = 8,
    % ValidateData是期望的数据， 如果不符合该数据，证明代码有问题 
    ValidateData = [{4.0, -4.0}, {5.0, -5.0}, 
		    {2.6, 0.5999999999999996}, {2.0, 2.0},
		    {-3.333333333333333, 4.666666666666666}, {-6.0, 6.0},
		    {-2.6666666666666665, 1.333333333333333},{-1.0, -1.0},
            {2.0,-2.0}, {2.0, -10.0}],
    validate_walk_path(CurrPoint, Speed, Path, ValidateData, 1).

validate_walk_path(_CurrPoint, _Speed, _Path, [], _PosIndex)->
    ok;
validate_walk_path(CurrPoint, Speed, Path, [{X,Z}| Rest], PosIndex) ->
    {NextPoint, PosIndex2} = walk:next_step(CurrPoint, Speed, Path, PosIndex),
    X = NextPoint#point.x,
    Z = NextPoint#point.z,
    validate_walk_path(NextPoint, Speed, Path, Rest, PosIndex2).
    
    
    
