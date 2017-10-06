%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 验证列表
%%% @end
%%% Created : 12 Jul 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(action).

%% API
-export([can_do/1]).

%%%===================================================================
%%% API
%%%===================================================================
%%是否能够执行
-spec can_do(list()) -> tuple() | atom().
can_do([]) ->
    true;
can_do([{Mod, Args, Type}|Lists]) ->
    case Mod(Args) of
	true ->
	    can_do(Lists);
	false ->          %% 错误返回时不带参数
	    {false, Type};
	{false, Datas} -> %% 错误返回时带参数
	    {false, Type, Datas}
    end.
%%%===================================================================
%%% Internal functions
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

can_do_test() ->
    Func1 = fun({_Account, _Params}) ->
		    true
	    end,
    Func2 = fun({_Account, _Params}) ->
		    false
	    end,
    Func3 = fun({_Account, _Params}) ->
		    {false, {1, 2, 3}}
	    end,
    Lists1 = [{Func1, {test1, test}, 1},
	      {Func1, {test1, test}, 2}
	     ],
    ?assertEqual(true, can_do(Lists1)),
    List2 = [{Func1, {test1, test}, 1},
    	     {Func2, {test1, test}, 2}],
    ?assertEqual({false, 2}, can_do(List2)),
    List3 = [{Func2, {test1, test}, 1},
    	     {Func1, {test1, test} ,2}],
    ?assertEqual({false, 1}, can_do(List3)),
    List4 = [{Func2, {test1, test}, 1},
    	     {Func2, {test1, test}, 2}],
    ?assertEqual({false, 1}, can_do(List4)),
    List5 = [{Func3, {test1, test}, 1},
	     {Func3, {test1, test}, 2}],
    ?assertEqual({false, 1, {1, 2, 3}}, can_do(List5)).


-endif.
