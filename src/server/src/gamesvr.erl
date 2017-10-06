%%%-------------------------------------------------------------------
%%% @author linyijie <linyijie@china-channel.com>
%%% @copyright (C) 2010, linyijie
%%% @doc
%%%  ÓÎÏ··şÎñÆ÷
%%% @end
%%% Created : 17 Aug 2010 by linyijie <linyijie@china-channel.com>
%%%-------------------------------------------------------------------
-module(gamesvr).

-export([start/0, stop/0, stop/1]).


start()->
    data_helper:format("start game server...~n"),
    application:start(gamesvr).

stop() ->
    stop(gamesvr).

stop(AppFile) ->
    application:stop(AppFile).


