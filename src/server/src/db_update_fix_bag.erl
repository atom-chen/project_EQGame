%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   修复背包
%%% @end
%%% Created : 22 Nov 2011 by hongjx <hongjx@35info.cn>

-module(db_update_fix_bag).

-export([fix_bag/0]).


fix_bag() ->
    Players = get_table_keys(player_pack),
    [init_record(Account) || Account <- Players],
    {atomic, ok}.


%%%===================================================================
%%% 内部函数
%%%===================================================================

get_table_keys(Tab) ->
    F = fun() ->
		mnesia:all_keys(Tab)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

init_record(Account) ->
    case db_common:select_one(player_pack, Account) of
	{_Tab, _Acc, full} ->
	    Record = player_init:init_player_pack(Account),
	    db_common:write(Record);
	_ -> ok
    end.
    
