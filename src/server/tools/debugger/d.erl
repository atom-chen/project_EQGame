%%%-------------------------------------------------------------------
%%% @author linyijie <linyijie@china-channel.com>
%%% @copyright (C) 2010, linyijie
%%% @doc
%%%
%%% @end
%%% Created : 27 Jul 2010 by linyijie <linyijie@china-channel.com>
%%%-------------------------------------------------------------------
-module(d).

-export([s/0, start/1, start/2, startpath/1, startpath/2]).


start(Module, Line) ->
    i:im(),
    {module, _Module} = i:ii(Module),
    ib(Module, Line),
    i:iaa([break, exit]).

%% L = [{Module, Line} | ...]
start(L) when is_list(L) ->
    i:im(),
    [begin {module, _Module} = i:ii(Module), ib(Module, Line) end || {Module, Line} <- L],
    i:iaa([break, exit]).

s() ->
    startpath("../src", []).

startpath(Path) ->
    startpath(Path, []).

%% 遍历指定文件夹下的所有erl, 然后自动装载文件来调试
%% L = [{Module, Line} | ...]
startpath(Path, L) ->
    {ok, Files} = file:list_dir(Path),
    Modules = [filename:join(Path, filename:basename(F, ".erl"))
	       || F <- Files, filename:extension(F) == ".erl"],
    i:im(),
    [{module, _Module} = i:ii(M) || M <- Modules],
    [ib(Module, Line) || {Module, Line} <- L],
    i:iaa([break, exit]),
    ok.

ib(Module, Line) when is_list(Module) ->
    M = filename:basename(Module, ".erl"),
    i:ib(list_to_atom(M), Line);
ib(Module, Line) when is_atom(Module) ->
    i:ib(Module, Line).


    
    
