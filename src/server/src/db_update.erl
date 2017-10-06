%%%-------------------------------------------------------------------
%%% File    : db_update.erl
%%% Author  :  <>
%%% Description : 自动更新数据库结构或者数据，该模块会创建一个数据库更新表，
%%% 该表用于存储对执行过的数据结构或者数据的改动的记录，方便日后的查找，同时
%%% 该表也作为哪些语句已经被执行过，哪些未被执行的一个判断依据
%%%
%%% Created : 28 Oct 2009 by  <>
%%%-------------------------------------------------------------------
-module(db_update).

-export([start/1]).

-export([test/0]).

-include_lib("stdlib/include/qlc.hrl").


-record(db_update_history, {
	  id,      %% 顺序号(主键）	  
	  comment, %% 该动作的注释
	  time,    %% 该动作执行的时间
	  detail   %% 详细内容(便于出现问题时比对)
	 }).

start(Nodes) when is_list(Nodes) ->
    init("db_update.cfg", Nodes).
    

init(File, Nodes) ->
    init_update_history(Nodes),
    case file:consult(File) of
	{ok, Terms} ->
	    Size = mnesia:table_info(db_update_history, size),
	    {Head, Rest} = lists:split(Size, Terms),
	    waitfor_tables(Head),
	    change(Rest, Size + 1, []);
	{error, Reason} ->
	    logger:log(error, "Can't load config file ~p~n ~p. Ignoring.~n", 
		       [File, Reason]),
	    fail
    end.
    
%% 初始化数据库更新记录的表
init_update_history(Nodes)->
    case mnesia:create_table(db_update_history, 
			[{attributes, record_info(fields, db_update_history)},
			 {disc_only_copies, Nodes}]) of
	{atomic, ok} ->
	    ok;
	{aborted, {already_exists, _Tab}}->
	    ok;
	{aborted, Reason} ->
	    logger:log(error, "create table 'db_update_history' aborted, Reason:~p~n", 
		      [Reason])
    end.   

%% 创建表
%% 默认disc_only_copies, node(), 激活分片表
call_create_table(Table, Fields) when is_list(Fields), length(Fields) > 1 ->   
    Ret = mnesia:create_table(Table, [{disc_only_copies, [node()]}, 
				      {attributes, Fields}
			       ]),
    mnesia:change_table_frag(Table, {activate, [{hash_module, eq_frag_hash}
						      %%, {n_disc_only_copies, 1}
						     ]}),
    Ret.

%% 激活分片表
call_active_table_frag(TableList) when is_list(TableList) ->
    [mnesia:change_table_frag(T, {activate, []}) || 
	T <- TableList, mnesia:table_info(T, frag_properties) =/= []],
    {atomic, ok}.


%% 创建表
%% Options用于指定高级选项
call_create_table(Table, Fields, Options) 
  when is_list(Fields), is_list(Options), length(Fields) > 1 ->    
    do_create_table(Table, [{attributes, Fields} | Options]),
    mnesia:change_table_frag(Table, {activate, [{hash_module, eq_frag_hash}
								   %%, {n_disc_only_copies, 1}
								  ]}),
    {atomic, ok}.


do_create_table(Table, Options) ->
    case lists:keyfind(mem_disc, 1, Options) of
	{mem_disc, []} ->
	    NOpt = lists:keydelete(mem_disc, 1, Options),
	    db_memdisc:create_table(Table, [node()], NOpt);
	{mem_disc, Nodes} ->
	    NOpt = lists:keydelete(mem_disc, 1, Options),
	    db_memdisc:create_table(Table, Nodes, NOpt);
	false ->
	    mnesia:create_table(Table, Options)
    end.


%% 删除表
call_delete_table(Table) ->
    delete_table(Table),
    {atomic, ok}.
	
%% 加索引字段(可重复执行)
call_add_table_index(Table, Field) when is_atom(Field) ->
    TabList = get_frag_tables(Table),
    
    [case mnesia:add_table_index(T, Field) of
	 {atomic, ok} -> ok;
	 {_, {already_exists, _, _}} -> ok
     end || T <- TabList],

    {atomic, ok}.

%% 去除索引字段(可重复执行)
call_del_table_index(Table, Field) when is_atom(Field) ->
    TabList = get_frag_tables(Table),

    [case mnesia:del_table_index(T, Field) of
	 {atomic, ok} -> ok;
	 {_, {no_exists, _, _}} -> ok
     end || T <- TabList],

    {atomic, ok}.


%% 改变表格存诸形态
call_change_table_copy_type(Table, mem_disc) -> %% 特殊处理mem_disc  
    Fields = mnesia:table_info(Table, attributes),
    Opt = [{attributes, Fields}],
    db_memdisc:create_mem_table(Table, [node()], Opt),
    mnesia:change_table_copy_type(Table, node(), disc_only_copies),
    {atomic, ok};
call_change_table_copy_type(Table, NewType) ->
    mnesia:change_table_copy_type(Table, node(), NewType).

%% 改变分片hash算法
call_change_frag_hash(NewHashMod, Changes) -> 
    Tables = get_tables(lists:reverse(Changes)),
    %% 检查是否只有一个分片
    one_frag_check(Tables),
    
    %% 首先取消分片属性
    [mnesia:change_table_frag(T, deactivate) || T <- Tables],
    %% 重新设置分片属性x
    [{atomic, ok}=mnesia:change_table_frag(T, {activate, [{hash_module, NewHashMod}]}) 
     || T <- Tables],    
    {atomic, ok}.
    
    

%% 新增字段(可重复执行)
call_add_table_fields(Table, KeyVals) when is_list(KeyVals) -> 
    TabList = get_frag_tables(Table),
    [{atomic, ok} = call_add_table_fields_1(T, KeyVals) || T <- TabList],
    {atomic, ok}.

call_add_table_fields_1(Table, KeyVals) when is_list(KeyVals) -> 
    Fields = [F || {F, _V} <- KeyVals],
    Vals = [V || {_F, V} <- KeyVals],	    
    
    OldFields = mnesia:table_info(Table, attributes),
    case OldFields -- Fields of
	OldFields -> %% 没转化过
	    NewFields = lists:append(OldFields, Fields),
	    Fields = NewFields -- OldFields,
	    
	    F = fun(R) -> A = tuple_to_list(R), 
			  B = Vals, 
			  C = lists:append(A, B), 
			  list_to_tuple(C) 
		end,
    
	    mnesia:transform_table(Table, F, NewFields);
	_ -> %% 已经被转化过了
	    {atomic, ok}
    end.

%% 取消重命名，因为分片表重命名 可能会出问题
%% %% 表重命名
%% call_rename_table(OldTable, NewTable) ->

%%     Fields = mnesia:table_info(OldTable, attributes),
%%     mnesia:create_table(NewTable, [{disc_only_copies, [node()]}, {attributes, Fields}]),
%%     Records = select_table(OldTable),
%%     F = fun() ->
%% 	lists:foreach(fun(R) -> 
%% 			      mnesia:write(setelement(1, R, NewTable))		
%% 		      end,
%% 		      Records)
%% 	end,
%%     {atomic, _OpResult} = mnesia:transaction(F),
    
%%     mnesia:delete_table(OldTable).

%% 删除字段(可重复执行)
call_remove_table_field(Table, Field) when is_atom(Field) ->
    call_remove_table_fields(Table, [Field]).


%% 删除多个表字段(可重复执行)
call_remove_table_fields(Table, Fields) when is_list(Fields) -> 
    TabList = get_frag_tables(Table),
    [{atomic, ok} = call_remove_table_fields_1(T, Fields) || T <- TabList],
    {atomic, ok}.

call_remove_table_fields_1(Table, Fields) when is_list(Fields) ->
    OldFields = mnesia:table_info(Table, attributes),
    IndexArray = [pos_list_item(OldFields, F) || F <- Fields],
    %% 删除字段
    NewFields = remove_list_items(OldFields, IndexArray),
    true = length(NewFields) > 1,
    
    case OldFields =:= NewFields of
	true -> %% 已处理过了
	    {atomic, ok};
	_ -> %% 未处理
	    F = fun(R) -> 
			NewR = remove_tuple_items(R, [X + 1 || X <- IndexArray]) ,
			NewR
		end,
	    mnesia:transform_table(Table, F, NewFields)
    end.


%% 重命名表字段
call_rename_table_field(Table, {OldField, NewField}) ->
    TabList = get_frag_tables(Table),
    [{atomic, ok} = call_rename_table_field_1(T, {OldField, NewField}) || T <- TabList],
    {atomic, ok}.

%% 重命名表字段
call_rename_table_field_1(Table, {OldField, NewField}) ->
    OldFields = mnesia:table_info(Table, attributes),
    case lists:member(NewField, OldFields) of
	true -> %% 已经处理过了
	    {atomic, ok};
	false ->
	    NewFields = replace_list_item(OldFields, OldField, NewField),
	    %% 不需要做转化
	    F = fun(R) -> R end,

	    mnesia:transform_table(Table, F, NewFields)
    end.

%% 清数据
call_clear_tables(TableList) when is_list(TableList) ->
    [call_clear_frag_table(T) || T <- TableList],
    {atomic, ok}.

call_clear_frag_table(Tab) when is_atom(Tab) ->
    TabList = get_frag_tables(Tab),
    [{atomic, ok} = mnesia:clear_table(T) || T <- TabList],
    {atomic, ok}.

%% 重命名多个表字段
call_rename_table_fields(Table, KeyVals) when is_list(KeyVals) ->
    OldFields = mnesia:table_info(Table, attributes),
    
    NewFields = replace_list_items(OldFields, KeyVals),

    %% 不需要做转化
    F = ignore,
    mnesia:transform_table(Table, F, NewFields).


%% 插入记录
call_insert_data(Table, Records) when is_list(Records) ->
    F = fun() ->
		FWrite = fun(Data) ->
				AList = lists:append([Table], tuple_to_list(Data)),
				Record = list_to_tuple(AList),
				mnesia:write(Record)				
			end,
		lists:foreach(FWrite, Records)		
	end,
    {atomic, _OpResult} = mnesia:transaction(F).


%% 删除记录
call_delete_data(Table, Keys) when is_list(Keys) ->
    F = fun() -> 
		FDelete = fun(Key) ->
				  mnesia:delete({Table, Key})
			  end,
		lists:foreach(FDelete, Keys)		
	end,
    {atomic, _OpResult} = mnesia:transaction(F).

       
%% 调用module中的函数，完成一些复杂操作( 注：Mod中尽量不要直接使用Record写入数据库)
call_module_fun(Mod, Fun, Params) when is_list(Params) ->
    apply(Mod, Fun, Params).


%% 执行每一个数据库变更操作，并记录到数据库更新表中
change([], _ID, _Changes)->
    ok;
        
change([H | Rest], ID, Changes) when is_tuple(H) ->
    try
	{atomic, ok} = do_change(H, ID, Changes)
    catch 
	error: Msg ->
	    %% 处理错误，让讯息更明确
	    logger:delay_log(error, "~p~n ~p~n stack:~p~n", 
			     [H, Msg, erlang:get_stacktrace()]),
	    erlang:error(Msg)
    end,

    change(Rest, ID + 1, [H | Changes]).
    

%% 解释每个变更操作
do_change(H, ID, Changes) when is_tuple(H) ->
    Result = case H of
	{module_fun, Mod, Fun, Params, _Comment} ->
	    call_module_fun( Mod, Fun, Params );

	{delete_data, Table, Keys, _Comment} ->
	    call_delete_data(Table, Keys);

	{insert_data, Table, Records, _Comment} ->
	    call_insert_data(Table, Records);

	{create_table, Table, Fields, _Comment} ->
	    call_create_table(Table, Fields);

	{create_table, Table, Fields, Options, _Comment} ->
	    call_create_table(Table, Fields, Options);

	{active_table_frag, TableList, _Comment} ->
	    call_active_table_frag(TableList);	
		 
	{delete_table, Table, _Comment} ->
	    call_delete_table(Table);

	{add_table_fields, Table, KeyVals, _Comment}->
	    call_add_table_fields(Table, KeyVals);

	{remove_table_fields, Table, Fields, _Comment} ->
	    call_remove_table_fields(Table, Fields);

	{rename_table_fields, Table, KeyVals, _Comment} ->
	    call_rename_table_fields(Table, KeyVals);

	{remove_table_field, Table, Field, _Comment} ->
	    call_remove_table_field(Table, Field);

	{rename_table_field, Table, {OldField, NewField}, _Comment} ->
	    call_rename_table_field(Table, {OldField, NewField});

	{clear_tables, TableList, _Comment} ->
	    call_clear_tables(TableList);

	{add_table_index, Table, Field, _Comment} ->
	    call_add_table_index(Table, Field);

	{del_table_index, Table, Field, _Comment} ->
	    call_del_table_index(Table, Field);

	{change_table_copy_type, Table, NewType, _Comment} ->
	    call_change_table_copy_type(Table, NewType);

	{change_frag_hash, NewHashMod, _Comment} ->
	    call_change_frag_hash(NewHashMod, Changes);

	%% 未定义操作类型
	Other ->
	    erlang:error({bad_operate_type, Other})
    end,
    
    case Result of
	{atomic, ok} ->
	    update_history(H, ID);
	_->
	    {atomic, fail}
    end.

update_history(H, ID) ->
    Comment = lists:last(tuple_to_list(H)),
    data_helper:format("change database, comment:~p~n", [Comment]),
    Func = fun() ->
		   ok =  mnesia:write(#db_update_history{id = ID, 
					      comment = Comment, 
					      time = erlang:localtime(), 
					      detail = H})
	   end,
    {atomic, _OpResult} = mnesia:transaction(Func).


%%%===================================================================
%%% Internal functions
%%%===================================================================
waitfor_tables(Terms) ->
    Tables = get_tables(Terms),
    case mnesia:wait_for_tables(Tables, 1000) of
	{timeout, BadTables} ->
	    logger:log(warning, "wait for tables: ~p........~n", [BadTables]), 
	    mnesia:wait_for_tables(BadTables, infinity);	    
	ok ->
	    ok
    end.


get_tables(AList) when is_list(AList) ->
    F = fun(X, Acc) -> 
		Tab = element(2, X),
		case element(1, X) of
		    create_table -> [Tab | Acc];
		    delete_table -> lists:delete(Tab, Acc);
		    _ -> Acc
		end
	end,
    lists:foldl(F, [], AList).


exists_list_index(AList, Index) when is_list(AList) ->
    Len = length(AList),
    (1 =< Index) and (Index =< Len).

replace_list_items(AList, []) when is_list(AList)  ->
    AList;

replace_list_items(AList, [{Old, New} | Rest]) when is_list(AList) ->
    NewList = replace_list_item(AList, Old, New),
    replace_list_items(NewList, Rest).
    

replace_list_item(AList, OldItem, NewItem) when is_list(AList) ->
    Index = pos_list_item(AList, OldItem),
    case exists_list_index(AList, Index) of
	true ->
	    {List2, List3} = lists:split(Index - 1, AList),
	    [_H | T] = List3,
	    Last = lists:append([NewItem], T),
	    lists:append(List2, Last);
	false ->
	    AList
    end.
    

remove_list_items(AList, IndexArray) when is_list(AList), is_list(IndexArray) ->
    NIndexArray = lists:sort(IndexArray),

    F = fun(Index, Acc) -> 
		remove_list_item(Acc, Index)
	end,
 
    lists:foldr(F, AList, NIndexArray).

    

remove_tuple_items(ATuple, IndexArray) when is_tuple(ATuple), is_list(IndexArray) ->
    List = remove_list_items(tuple_to_list(ATuple), IndexArray),
    list_to_tuple(List).
 
remove_list_item(AList, Index) when is_list(AList) ->
    case exists_list_index(AList, Index) of
	true->
	    {List2, List3} = lists:split(Index - 1, AList),
	    [_H | T] = List3,
	    lists:append(List2, T);
	false ->
	    AList
    end.

remove_tuple_item(ATuple, Index) when is_tuple(ATuple) ->
    List = remove_list_item(tuple_to_list(ATuple), Index),
    list_to_tuple(List).

%% 查找列表中某个值的位置, 没找到返回0
pos_list_item(AList, Val) when is_list(AList) ->
    pos_loop(AList, Val, 1).
    

%% 没找到，返回0
pos_loop([], _Val, _N) ->
    0;

%% 找到，返回位置，从1开始
pos_loop([_Val | _T], _Val, N) ->
    N;

pos_loop([_H | T], Val, N) ->
    pos_loop( T, Val, N + 1 ).


%% 取得所有分片表
get_frag_tables(Tab) when is_atom(Tab) ->
    KVList = do_tran(fun mnesia:table_info/2, [Tab, frag_size]),
    [Table || {Table, _Size} <- KVList].

%% 事务处理
do_tran(Fun, Args) when is_list(Args) ->
    mnesia:activity(transaction, Fun, Args, mnesia_frag).


one_frag_check(Tables) ->
    F = fun(T, Acc) -> 
		case catch mnesia:table_info(T, frag_properties) of
		    Props when is_list(Props) ->
			NFrag = 
			    case lists:keyfind(n_fragments, 1, Props) of
				{n_fragments, N} -> N;
				_ -> 1
			    end,
			
			case NFrag > 1 of 
			    true -> [T | Acc];
			    _ -> Acc
			end;
		    _  ->
			Acc
		end
	end,
	 	 
    ErrorTables = lists:foldl(F, [], Tables),
    case length(ErrorTables) > 0 of
	true ->
	    erlang:error({too_many_frags_can_not_change_hash, [ErrorTables]});
	_ ->
	    ok
    end.


%%%===================================================================
%%% 测试代码
%%%===================================================================

test() ->    
    test_pos(),
    test_remove(),
    test_replace(),
    test_sort(),
    test_db().

test_sort() ->
    FGreater = fun(A, B) -> A > B end,
    A = [2, 892, 83, 992, 0],
    OrderArray = lists:sort(FGreater, A ),
    [992, 892, 83, 2, 0] = OrderArray,
    ok.


test_replace() ->
    A = [a,b,c,d],
    [hjx,b,c,d] = replace_list_item(A, a, hjx),
    [a,hjx,c,d] = replace_list_item(A, b, hjx),
    [a,b,hjx,d] = replace_list_item(A, c, hjx),
    [a,b,c,hjx] = replace_list_item(A, d, hjx),
    [a,b,c,d] = replace_list_item(A, e, akk),
    [a,b,c,d] = replace_list_item(A, f, akk),

    [age, b, money, d] = replace_list_items(A, [{a, age}, {c, money}]),
    [a, age, c, d] = replace_list_items(A, [{b, age}]),
    [age, bb, money, dd] = replace_list_items(A, [{a, age}, {b, bb}, {c, money}, {d, dd}]),
    [a, b, c, d] = replace_list_items(A, [{aaa, age}, {ccc, money}]),
     
    ok.

test_remove()->
    A = [1,2,3,4],
    [2,3,4] = remove_list_item(A, 1),
    [1,3,4] = remove_list_item(A, 2),
    [1,2,4] = remove_list_item(A, 3),
    [1,2,3] = remove_list_item(A, 4),
    [1,2,3,4] = remove_list_item(A, 5),
    [1,2,3,4] = remove_list_item(A, 0),

    [1,4] = remove_list_items(A, [2, 3]),
    [2,3,4] = remove_list_items(A, [1]),
    [2,4] = remove_list_items(A, [1, 3]),
    [4] = remove_list_items(A, [1, 2, 3]),
    [] = remove_list_items(A, [1, 2, 3, 4]),
    [] = remove_list_items(A, [1, 2, 3, 4, 5]),
    [1, 4] = remove_list_items(A, [2, 3, 5, 7, 9]),

    B = {a,b,c,d},
    {b,c,d} = remove_tuple_item(B, 1),
    {a,c,d} = remove_tuple_item(B, 2),
    {a,b,d} = remove_tuple_item(B, 3),
    {a,b,c} = remove_tuple_item(B, 4),
    {a,b,c,d} = remove_tuple_item(B, 5),
    {a,b,c,d} = remove_tuple_item(B, 0),

    {a,d} = remove_tuple_items(B, [2, 3]),
    {b,c,d} = remove_tuple_items(B, [1]),
    {b,d} = remove_tuple_items(B, [1, 3]),
    {d} = remove_tuple_items(B, [1, 2, 3]),
    {} = remove_tuple_items(B, [1, 2, 3, 4]),
    {} = remove_tuple_items(B, [1, 2, 3, 4, 5]),
    {a, d} = remove_tuple_items(B, [2, 3, 5, 7, 9]),
    ok.

test_pos() ->
    A = [1, 2, 3, 4],
    0 = pos_list_item(A, 0),
    1 = pos_list_item(A, 1),
    2 = pos_list_item(A, 2),
    3 = pos_list_item(A, 3),
    4 = pos_list_item(A, 4),
    0 = pos_list_item(A, 99),    
    ok.
    
test_db() ->
    mnesia:create_schema([node()]),
    ok = mnesia:start(),

    %% 对测试所需的表做wait for
    mnesia:wait_for_tables([person_table, palyer_table, rm_person], 2000),
    test_create_table(), 
    test_delete_table(),
    test_add_remove_record(),
    test_rename_table(),
    test_modify_table_field(),    
    mnesia:stop(),
    ok.
      

test_add_remove_record() ->
    PersonFields = [name, age],
    delete_table(person_table),
    call_create_table(person_table, PersonFields),
    call_insert_data(person_table, [{hjx, 20}, {ye, 30}]),
    Records = select_table(person_table),
    [] = Records -- [{person_table, hjx, 20}, {person_table, ye, 30}],  

    call_delete_data(person_table, [hjx]),
    [{person_table, ye, 30}] = select_table(person_table),     

    ok.

test_create_table() ->
    PersonFields = [age, height],
    PlayerFields = [username, money],
    delete_table(person_table),
    delete_table(player_table),
    call_create_table(person_table, PersonFields),
    call_create_table(player_table, PlayerFields),

    PersonFields = mnesia:table_info(person_table, attributes),    
    PlayerFields = mnesia:table_info(player_table, attributes),
    insert_record({person_table, 19, 28}),
    insert_record({person_table, 20, 30}),
    [] = [{person_table, 20, 30},{person_table, 19, 28}] 
	-- select_table(person_table),
    ok.

test_delete_table() ->
    PersonFields = [age, height],
    delete_table(person_table),
    call_create_table(person_table, PersonFields),
    
    call_delete_table(person_table),
    {timeout, _} = mnesia:wait_for_tables([person_table], 200),

    ok.

test_rename_table() ->
    PersonFields = [name, age],
    delete_table(person_table),
    call_create_table(person_table, PersonFields),
    insert_record({person_table, hjx, 31}),
    insert_record({person_table, y3, 22}),
    Records = select_table(person_table),
    [] = Records -- [{person_table, hjx, 31}, {person_table, y3, 22}],
    %% call_rename_table(person_table, rm_person),

    %%  NewRecords = select_table(rm_person),
    %% [] = NewRecords -- [{rm_person, hjx, 31}, {rm_person, y3, 22}],
    %% {timeout, _} = mnesia:wait_for_tables([person_table], 200),
    
    ok.

test_modify_table_field() ->
    PersonFields = [name, age],
    delete_table(person_table),
    call_create_table(person_table, PersonFields),
     insert_record({person_table, hjx, 31}),
    insert_record({person_table, y3, 22}),
    Records = select_table(person_table),
    [] = Records -- [{person_table, hjx, 31}, {person_table, y3, 22}],

    %% 测试新增
    call_add_table_fields(person_table, [{height, 177}, {power, 99}]),
    [name, age, height, power] = mnesia:table_info(person_table, attributes),
    NewRecords = select_table(person_table),
    [] = NewRecords -- [{person_table, hjx, 31, 177, 99}, {person_table, y3, 22, 177, 99}],    

    %% 测试删除
    call_remove_table_field(person_table, height),
    NotHeightRecords = select_table(person_table), 
    [name, age, power] = mnesia:table_info(person_table, attributes),
    [] = [{person_table, hjx, 31, 99}, {person_table, y3, 22, 99}] 
	-- NotHeightRecords,

    %% 测试删除
    call_remove_table_field(person_table, power), 
    NotPowerRecords = select_table(person_table),   
    [name, age] = mnesia:table_info(person_table, attributes),
    [] = [{person_table, hjx, 31}, {person_table, y3, 22}] 
	-- NotPowerRecords,

    %% 测试新增
    call_add_table_fields(person_table, [{height, 177}, {power, 99}, {money, 123}]),
    [name, age, height, power, money] = mnesia:table_info(person_table, attributes),
    MoneyRecords = select_table(person_table),    
    [] = MoneyRecords -- [{person_table, hjx, 31, 177, 99, 123}, {person_table, y3, 22, 177, 99, 123}],    

    %% 测试删除
    call_remove_table_fields(person_table, [height, power]),
    DeleteTestRecords = select_table(person_table),   
    [name, age, money] = mnesia:table_info(person_table, attributes),
    [] = [{person_table, hjx, 31, 123}, {person_table, y3, 22, 123}] 
	-- DeleteTestRecords,

    %% 测试新增
    call_add_table_fields(person_table, [{height, 160}, {power, 25}]),
    [name, age, money, height, power] = mnesia:table_info(person_table, attributes),
    [] = [{person_table, hjx, 31, 123, 160, 25}, {person_table, y3, 22, 123, 160, 25}] 
	-- select_table(person_table),               
    %% 测试重命名
    call_rename_table_field(person_table, {power, wid}),
    [] = [{person_table, hjx, 31, 123, 160, 25}, {person_table, y3, 22, 123, 160, 25}] 
	-- select_table(person_table),

    [name, age, money, height, wid] = mnesia:table_info(person_table, attributes),

    %% 测试重命名
    call_rename_table_fields(person_table, [{wid, power}, {age, myage}, {name, myname}]),
    [] = [{person_table, hjx, 31, 123, 160, 25}, {person_table, y3, 22, 123, 160, 25}] 
	-- select_table(person_table),

    [myname, myage, money, height, power] = mnesia:table_info(person_table, attributes),
    insert_record({person_table, jy, 22, 1123, 1160, 225}),
    [] = [{person_table, jy, 22, 1123, 1160, 225}, 
	  {person_table, hjx, 31, 123, 160, 25},
	  {person_table, y3, 22, 123, 160, 25}] 
	-- select_table(person_table),
    %% 测试清除表数据
    call_clear_tables([person_table]),
    0 = mnesia:table_info(person_table, size),
    ok.

insert_record(Record) ->
    F = fun() -> ok = mnesia:write( Record ) end,
    {atomic, OpResult} = mnesia:transaction(F),
    OpResult.


delete_table(Table)->
    mnesia:delete_table(Table).
     

select_table(Table) ->
    F = fun() -> qlc:e(qlc:q([X || X <- mnesia:table(Table)])) end,
    {atomic, Records} = mnesia:transaction(F),
    Records.

