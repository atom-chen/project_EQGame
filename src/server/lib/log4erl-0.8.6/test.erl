-module(test).

-export([fib/1]).

-include_lib("eunit/include/eunit.hrl").

fib(0) ->
    1;
fib(1) ->
    1;
fib(N) ->
    fib(N-1) + fib(N-2).

fib_test_()->
    [
     ?_assert(fib(0), 2)
    ].
