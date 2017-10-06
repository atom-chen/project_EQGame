%%%-------------------------------------------------------------------
%%% File    : log.hrl
%%% Author  :  <>
%%% Description : 日志系统
%%%
%%% Created : 20 Oct 2009 by  <>
%%%-------------------------------------------------------------------
-module(logger).

-export([start/0, start/1, log/2, log/3, delay_log/3]).

start() ->
    start("./error_logs/").


start(LogDir)->
    TailFileName = "game.log",
    delete_overtime_log_file(LogDir, 12, TailFileName),
    LogFile = build_log_file_name(LogDir, TailFileName),
    
    %% 设置错误日志
    error_logger:swap_handler({logfile, LogFile}).


%%记录日志
%%Level有四个等级（warn, info, debug, error ）
log(Type, Str) ->
    log(Type, Str, []).


log(Type, Str, Args) when is_atom(Type), is_list(Str), is_list(Args) ->
    S1 = string:concat(atom_to_list(Type), ": "),
    Format = string:concat(S1, Str),
    error_logger:error_msg(Format, Args).

delay_log(Type, Str, Args) ->
    log(Type, Str, Args),
    %% 延时，防止有时没记录到日志中, 特别是在整个应用程序开始运行时
    timer:sleep(10).



%%%===================================================================
%%% log4erl相关代码
%%%===================================================================
%%开始运行日志系统
%% start(ConfFile)->
%%     application:start(log4erl),
%%     log4erl:conf(ConfFile).

%%记录日志
%%Level有四个等级（warn, info, debug）
%% log(warn, Str)->
%%     log4erl:warn(Str);
%% log(info, Str) ->
%%     log4erl:info(Str);
%% log(error, Str) ->
%%     log4erl:fatal(Str);
%% log(debug, Str) ->
%%     log4erl:debug(Str).

%% log(warn, Str, Param)->
%%     log4erl:warn(Str, Param);
%% log(info, Str, Param) ->
%%     log4erl:info(Str, Param);
%% log(error, Str, Param) ->    
%%     log4erl:fatal(Str, Param);
%% log(debug, Str, Param) ->
%%     log4erl:debug(Str, Param).


build_log_file_name(LogDir, Tail) ->
    {{Y, M, D}, {H, N, S}} = datetime:local_time(),
    FileName = string:right(integer_to_list(Y), 4, $0) 
	++ string:right(integer_to_list(M), 2, $0) 
	++ string:right(integer_to_list(D), 2, $0) 
	++ string:right(integer_to_list(H), 2, $0) 
	++ string:right(integer_to_list(N), 2, $0) 
	++ string:right(integer_to_list(S), 2, $0) 
	++ Tail,

    LogFile = LogDir ++ FileName,
    LogFile.

delete_overtime_log_file(LogDir, LimitCount, Tail) ->
    List = 
	case file:list_dir(LogDir) of
	    {ok, Files} -> Files;
	    _ -> []
	end,

    LogFiles = [X || X <- List, string:rstr(X, Tail) > 0],
    
    FAsc = fun(E1, E2) -> 
		   E1 < E2 
	   end,

    SortedFiles = lists:sort(FAsc, LogFiles),
    FileCount = length(SortedFiles),
    case FileCount =< LimitCount of
	true ->
	    ok;
	_ ->
	    {DeleteFiles, _RemainFiles} = lists:split(FileCount - LimitCount, SortedFiles),
	    [ok = file:delete(LogDir ++ F) || F <- DeleteFiles]
    end,
	
    ok.
    
    
