%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  数据库内存硬盘操作模块
%%% 该模块会把数据存储与内存与硬盘中, 与mnesia内置的disc_copies不同的是, 内置的类型
%%% 是内存与硬盘的数据是一一对应的, 所以当数据量大的时候, 非常占用内存, 且程序初始化非常慢
%%% 本模块的算法是:
%%% 1. 数据在读取的时候, 如果内存里的记录不存在, 则向硬盘请求, 内存会cache住该数据
%%% 2. 数据在写入的时候, 如果内存里的记录不存在, cache住该数据, 且更新到硬盘中
%%% 3. 本模块支持只删除内存的数据, 与数据库的删除操作分开, 目的是为了减少内存占用
%%% @end
%%% Created :  3 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(db_memdisc).

-export([create_table/3, create_mem_table/3, delete_table/1, read/2, write/1, write/2, delete_mem_data/2, delete/2]).
-export([test/0]).

create_mem_table(Name, NodeList, TabDef) -> 
    MemTable = make_mem_name(Name),
    MemTabDef = [{ram_copies, NodeList}, {record_name, Name} | TabDef],
    {atomic, ok} = mnesia:create_table(MemTable, MemTabDef).

create_table(Name, NodeList, TabDef) ->
    create_mem_table(Name, NodeList, TabDef),
    MemTable = make_mem_name(Name),

    DiscTabDef = [{disc_only_copies, NodeList}, {record_name, Name} | TabDef],
    case mnesia:create_table(Name, DiscTabDef) of
	{aborted, Reason} ->
	    {atomic, ok} = mnesia:delete_table(MemTable),
	    {aborted, Reason};
	Any -> Any
    end.

delete_table(Name) ->
    MemTable = make_mem_name(Name),
    mnesia:delete_table(MemTable),
    mnesia:delete_table(Name).


%% 读数据
read(Table, KeyList) when is_list(KeyList) ->
    MemTable = make_mem_name(Table),
    F = fun() ->
		[do_read(MemTable, Table, Key) || Key <- KeyList]
	end,
    mnesia:activity(transaction, F, [], mnesia_frag);
    %% {atomic, Data} = mnesia:activity(transaction, F, [], mnesia_frag),
    %% Data;
read(Table, Key)->
    MemTable = make_mem_name(Table),
    F = fun() ->
		do_read(MemTable, Table, Key)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).
    %% {atomic, Data} = mnesia:activity(transaction, F, [], mnesia_frag),
    %% Data.

do_read(MemTable, Table, Key) ->
    case mnesia:read(MemTable, Key) of 
	[] -> 
	    case mnesia:read(Table, Key) of
		[] -> [];
		[Data] -> 
		    mnesia:write(MemTable, Data, write),
		    Data;
		Any -> Any
	    end;
	[Data] -> Data;
	Result -> Result
    end.

%% 写数据
write(ValList) when is_list(ValList) ->
    %%　支持多表同时事务写入
    F = fun() ->
		[write_impl(Val) || Val <- ValList]
	end,
    mnesia:activity(transaction, F, [], mnesia_frag);
write(Val) ->    
    %%　单表写入
    F = fun() ->
		write_impl(Val)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

write_impl(Val) ->
    Table = element(1, Val),
    MemTable = make_mem_name(Table),
    mnesia:write(MemTable, Val, write),
    mnesia:write(Table, Val, write).    

%% 支持db_memdisc类型和传统的数据库类型的事务写入
write(ValList1, ValList2)when is_list(ValList1), is_list(ValList2) ->
    F = fun() ->
		[write_impl(Val) || Val <- ValList1],
		[mnesia:write(Val) || Val <- ValList2]
	end,
    mnesia:activity(transaction, F, [], mnesia_frag);
write(Val1, Val2) ->    
    %%　单表写入
    F = fun() ->
		write_impl(Val1),
		mnesia:write(Val2)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

%% 只删除内存中的数据
delete_mem_data(Table, Key) ->
    MemTable = make_mem_name(Table),
    F = fun() ->
		mnesia:delete({MemTable, Key})
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

%% 删除内存和硬盘的数据
delete(Table, Key)->
    MemTable = make_mem_name(Table),
    F = fun() ->
		mnesia:delete({MemTable, Key}),
		mnesia:delete({Table, Key})
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

%% --------------内部函数--------------------------------
%% 生成内存表的名字
make_mem_name(Table) when is_atom(Table)->
    list_to_atom(atom_to_list(Table) ++ "$_mem$").

%% --------------test------------------------------------
-record(test_data, {id, data}).
test() ->
    Table = test_data,
    Nodes = [node()],
    mnesia:delete_schema(Nodes),
    mnesia:create_schema(Nodes),
    mnesia:start(),
    delete_table(Table),
    create_table(Table, Nodes, [{attributes,  record_info(fields, test_data)}]),
    mnesia:wait_for_tables([make_mem_name(Table), Table], 2000),
    test_read(Table),
    ok.

test_read(Table) ->
    {atomic, []} = read(Table, 1),
    {atomic, ok} = write(#test_data{id = 1, data = 2}),
    DiscTable = Table,
    F = fun() ->
		mnesia:write(DiscTable, #test_data{id=2, data=3}, write)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag),
    {atomic, {Table, 1, 2}} = read(Table, 1),
    {atomic, {Table, 2, 3}} = read(Table, 2),
    ok.
    
    
