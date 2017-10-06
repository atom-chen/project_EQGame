%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  
%%% @end
%%% Created : 28 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(stress_test).

-export([start/0, start/1, start/5, loop/6]).

start() ->
    start("10.35.12.224", "2222", 0, 1000, 50).

start(Ip, Port, StartIndex, Num, ModNum) when Num > 0 ->
    start([Ip, Port, StartIndex, Num, ModNum]).

start([Ip, Port, StartIndex, Num, ModNum]) when Num > 0 ->
    spawn(stress_test, loop, [self(), Ip, Port, 
			      list_to_integer(StartIndex), list_to_integer(Num), list_to_integer(ModNum)]).

loop(_From, _Ip, _Port, StartIndex, Num, _ModNum) when StartIndex == Num ->
    ok;
loop(From, Ip, Port, StartIndex, Num, ModNum) ->
    Name = "test" ++ integer_to_list(StartIndex + Num),
    spawn(stress_robot, start, [Ip, Port, Name, Num, ModNum]),
    timer:sleep(100),
    loop(From, Ip, Port, StartIndex, Num-1, ModNum).


