%%%-------------------------------------------------------------------
%%% File    : player_walk.erl
%%% Author  :  <>
%%% Description : 
%%%
%%% Created : 29 Oct 2009 by  <>
%%%-------------------------------------------------------------------
-module(walk).

-include("packet_def.hrl").

%% API
-export([next_step/4, can_walk/2]).

%%====================================================================
%% API
%%====================================================================

%% 计算下一步的点
next_step(CurrPoint, _Speed, [], PosIndex) ->
    {CurrPoint, PosIndex};
next_step(CurrPoint, Speed, Path, PosIndex) ->
    NextPoint = lists:nth(PosIndex, Path),
    {Xspeed, Zspeed} = calc_speed(CurrPoint, NextPoint, Speed),
    {NewX, XState} = calc_point(CurrPoint#point.x, NextPoint#point.x, Xspeed),
    {NewZ, ZState} = calc_point(CurrPoint#point.z, NextPoint#point.z, Zspeed),
    if
	(XState =:= arrive) and (ZState =:= arrive) ->
	    {#point{x=NewX, y=0.0, z=NewZ}, PosIndex+1};
	true ->
	    {#point{x=NewX, y=0.0, z=NewZ}, PosIndex}
    end.

calc_point(CurrPoint, NextPoint, Speed) ->
    NewPoint = CurrPoint + Speed,
    if 
	abs(CurrPoint - NewPoint) >= abs(CurrPoint - NextPoint) ->
	    {NextPoint, arrive}; %%到达目的点
	true ->
	    {NewPoint, unarrive} %%还没到达目的点
    end.

%% 根据当前点和下一点计算出Speed的x和z方向的速度
calc_speed(#point{x=X1,z=Z1}, #point{x=X2,z=Z2}, Speed)->
    Dx = X2 - X1,
    Dz = Z2 - Z1,
    DD = abs(Dx) + abs(Dz),
    Xspeed = Speed * Dx / DD,
    Zspeed = Speed * Dz / DD,
    {Xspeed, Zspeed}.

%% 判断该路径是否能行走
can_walk(CurrPoint, [NextPoint|_Rest])->
    Dx = CurrPoint#point.x - NextPoint#point.z,
    Dz = CurrPoint#point.z - NextPoint#point.z,
    if
	((Dx =:= 0) and (Dz =:= 0))->
	    false;
	true ->
	    true
    end;
can_walk(_CurrPoint, []) ->
    false.
%%====================================================================
%% Internal functions
%%====================================================================
