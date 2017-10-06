%%% @author hongjx <hongjx@china-channel.com>
%%% @copyright (C) 2010, hongjx
%%% @doc
%%%    ���ݿ�����
%%% @end
%%% Created : 17 Aug 2010 by hongjx <hongjx@china-channel.com>

-module(db_connection).

-export([open/1, close/0]).


%% ���� ���ݿ�ڵ�
open([DBAddress]) when is_list(DBAddress) ->
    data_helper:format("DBAddress ~p~n", [DBAddress]),
    DBServer = case DBAddress =:= "localhost" of
		   true -> node();
		   false -> list_to_atom(DBAddress)
	       end,
    ok = connect_db(DBServer);

open([DBAddress]) when is_atom(DBAddress) ->
    data_helper:format("DBAddress ~p~n", [DBAddress]),
    DBServer = case DBAddress of
		   localhost -> node();
		   _ -> DBAddress
	       end,
    ok = connect_db(DBServer).


%% �Ͽ�����
close() ->
    mnesia:stop().


%% ���� ���ݿ�ڵ�
connect_db(DBNode) when is_atom(DBNode) ->
    ok = rpc:call(DBNode, database, start, []),

    case DBNode =:= node() of
	false ->
	    ok = mnesia:start(),
	    {ok, [DBNode]} = mnesia:change_config(extra_db_nodes, [DBNode]);
	true ->
	    ok
    end,
    ok.




