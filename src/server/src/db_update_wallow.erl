%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   防沉迷系统数据升级
%%% @end
%%% Created : 16 Jun 2011 by hongjx <hongjx@35info.cn>

-module(db_update_wallow).
-export([start/0]).

start() ->
    Players = get_table_keys(player_basic_data),
    [init_wallow(Account) || Account <- Players],
    {atomic, ok}.

%%%===================================================================
%%% 内部函数
%%%===================================================================

get_table_keys(Tab) ->
    F = fun() ->
		mnesia:all_keys(Tab)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

init_wallow(Account) ->
    Record = wallow:new(Account),
    db_common:write(Record).
    
