%% @author author <author@example.com>
%% @copyright YYYY author.

%% @doc TEMPLATE.

-module(eqweb).
-author('author <author@example.com>').
-export([start/0, stop/0]).

ensure_started(App) ->
    case application:start(App) of
        ok ->
            ok;
        {error, {already_started, App}} ->
            ok
    end.

%% @spec start() -> ok
%% @doc Start the eqweb server.
start() ->
    eqweb_deps:ensure(),
    ensure_started(crypto),
    application:start(eqweb).

%% @spec stop() -> ok
%% @doc Stop the eqweb server.
stop() ->
    Res = application:stop(eqweb),
    application:stop(crypto),
    Res.
