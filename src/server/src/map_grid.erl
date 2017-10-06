%%%-------------------------------------------------------------------
%%% File    : map_grid.erl
%%% Author  : 
%%% Description : 
%%%
%%% Created : 15 Nov 2009 by  <>
%%%-------------------------------------------------------------------
-module(map_grid).

-include("packet_def.hrl").   
%% API
-export([calc_grid_index/2, carry/1, get_near_grid/4,
	 calc_total_grids/2]).

%%每个格子的宽度和高度
-define(GRID_WIDTH, 30).
-define(GRID_HEIGHT, 30).

%%====================================================================
%% API
%%====================================================================
%% 计算出指定矩形包含的网格的列表
%% 返回该矩形的网格列表
calc_total_grids(Width, Length) ->
    ColGrids = carry(Width / ?GRID_WIDTH),
    RowGrids = carry(Length / ?GRID_HEIGHT),
    make_list({1, ColGrids}, {1, RowGrids}, ColGrids, []).

%%计算格子索引
calc_grid_index(#point{x=X,z=Z}, MapWidth)
  when MapWidth >= ?GRID_WIDTH ->
    ColGrids = carry(MapWidth / ?GRID_WIDTH),
    ColIndex = carry(X / ?GRID_WIDTH),
    RowIndex = carry(Z / ?GRID_HEIGHT),
    (RowIndex-1) * ColGrids + ColIndex.
    

%%获得感知范围的格子的索引列表集合
%%返回感知范围对象所在的格子索引列表
%%point为指定的坐标，以该坐标为中心，获得范围内的对象
%% MapWidth, MapHeight分别为地图的宽和高
%%Range为范围，以格子为单位（例如，2代表两个格子，0代表本格子），是一个周边边长半径
get_near_grid(#point{x=X,z=Z}, MapWidth, MapHeight, Range)
  when Range >= 0 ->
    ColIndex = carry(X / ?GRID_WIDTH),
    RowIndex = carry(Z / ?GRID_HEIGHT),
    ColGrids = carry(MapWidth / ?GRID_WIDTH),
    RowGrids = carry(MapHeight / ?GRID_HEIGHT),
    {ColStart, ColEnd} = bound(1, ColGrids, Range, ColIndex),
    {RowStart, RowEnd} = bound(1, RowGrids, Range, RowIndex),
    ColStartIndex1 = (RowStart-1) * ColGrids + ColStart,
    ColEndIndex1 = (RowStart-1) * ColGrids + ColEnd,
    make_list({ColStartIndex1, ColEndIndex1},
	      {RowStart,RowEnd}, ColGrids, []).
    
%%====================================================================
%% Internal functions
%%====================================================================
%% make_list({ColStart, ColEnd}=ColLimit, {RowStart, RowEnd}, [])->
%%     make_list(ColLimit, {RowStart+1,RowEnd},lists:seq(ColStart, ColEnd));
make_list(_ColLimit, {RowStart, RowEnd}, _ColGrids, L)
  when RowStart =:= RowEnd + 1->
    lists:flatten(lists:reverse(L));
make_list({ColStart, ColEnd},{RowStart, RowEnd}, ColGrids, L)->
    make_list({ColStart+ColGrids, ColEnd+ColGrids}, {RowStart+1,RowEnd},
	      ColGrids, [lists:seq(ColStart, ColEnd)|L]).

bound(Min, Max, Range, Value)->
    MinValue = erlang:max(Value - Range, Min),
    MaxValue = erlang:min(Value + Range, Max),
    {MinValue, MaxValue}.

%取整，进位
%% carry(1.2) -> 2
%% carry(1.8)-> 2
carry(Value) ->
    Integer = trunc(Value),
    Float = Value - Integer,
    if 
	Float > 0.0 ->
	    Integer + 1;
	true ->
	    Integer
    end.
