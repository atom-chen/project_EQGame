%%%-------------------------------------------------------------------
%%% @author  linyibin
%%% @copyright (C) 2010, 
%%% @doc
%%%  过滤打印信息
%%% @end
%%% Created : 26 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(data_helper).

-export([start/0, format/1, format/2]).

-include("packet_def.hrl"). 

start() ->
    filter().

%% 增加过滤条件
filter() ->
    %%put(消息类型， 消息名称）
    [put(Type, ok) || Type <- [?msg_req_start_walk,
			       ?msg_req_stop_walk,
			       ?msg_req_sync_position
			      ]].


%% 格式化数据并打印数据
-spec format(list(), any()) -> atom().
format(Format, {Type, _Data} = Args) ->
    case get(Type) of
	undefined -> %%如果没有过滤条件，则打印过滤信息
	    io:format(Format, [Args]);
	_ ->
	    ok
    end;
format(Format, {Type} = Args) ->
    case get(Type) of
	undefined -> %%如果没有过滤条件，则打印过滤信息
	    io:format(Format, [Args]);
	_ ->
	    ok
    end;
format(Format, Args) when is_list(Args) ->
    io:format(Format, Args);
format(Format, Args) ->
    io:format(Format, [Args]).

format(Format) ->
    io:format(Format).

%% %% 格式化数据并打印数据
%% -spec format(list(), any()) -> atom().
%% format(Format, {Type, _Data} = Args) ->
%%     case get(Type) of
%% 	undefined -> %%如果没有过滤条件，则打印过滤信息
%% 	    ok;
%% 	_ ->
%% 	    ok
%%     end;
%% format(Format, {Type} = Args) ->
%%     case get(Type) of
%% 	undefined -> %%如果没有过滤条件，则打印过滤信息
%% 	    ok;
%% 	_ ->
%% 	    ok
%%     end;
%% format(Format, Args) when is_list(Args) ->
%%     ok;
%% format(Format, Args) ->
%%     ok.

%% format(Format) ->
%%     ok.

%%%===================================================================
%%% Test case
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

start_test() ->
    put(?msg_req_sync_position, req_sync_position).

format_test() ->
    ok == format("Data:~p~n", {30, {req_start_walk, {point, 0, 0, 0}, {point, 10, 10, 10}}}).

-endif.
