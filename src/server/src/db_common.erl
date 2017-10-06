%%% @author  linyijie
%%% @copyright (C) 2010, 3
%%% @doc
%%%  ͨ�õ����ݿ����
%%% @end
%%% Created : 24 Mar 2010 by  linyijie

-module(db_common).

-export([select/2, select_one/2, select_list/2, write/1, delete/2, do_qlc/1, 
	 modify/4, raw_modify/4, modify_if/5, raw_modify_if/5,
	 change/1, index_select/3, select_multi_table/2, is_exist/2, all_keys/1, transaction/1]).

%%%===================================================================
%%% API
%%%===================================================================

 %% ��ȡkey��Ӧ������, ����ƥ��ĵ�һ���ļ�¼
-spec select(atom(), list()) -> list().
select(TableName, KeyList) when is_atom(TableName), is_list(KeyList) ->
    F = fun() ->
                [case mnesia:read(TableName, Key) of
		     [] -> [];
		     [Data] -> Data
		 end || Key <- KeyList]
        end,
    mnesia:activity(transaction, F, [], mnesia_frag);
select(TableName, Key) when is_atom(TableName) ->
    select_one(TableName, Key).

%% ��ȡkey��Ӧ������, ����ƥ��ĵ�һ���ļ�¼
-spec select_one(atom(), any()) -> [] | tuple().
select_one(TableName, Key) when is_atom(TableName) ->
    F = fun() ->
                mnesia:read(TableName, Key)
        end,
    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    [];
	[Data] ->
	    Data
    end.

%% ��ȡkey��Ӧ�����ݼ���, ���ؼ�¼�б�
-spec select_list(atom(), atom()) -> list().
select_list(TableName, Key) when is_atom(TableName)->
    F = fun() ->
                mnesia:read(TableName, Key)
        end,
    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    [];
	Data ->
	    Data
    end.

%% ѡ����ű��������
select_multi_table(Account, Tables) when is_list(Tables)->
    F = fun() ->
                [begin [Data] = mnesia:read(T, Account), Data end || T <- Tables]
        end,
    mnesia:activity(transaction, F, [], mnesia_frag).

%% �Ƿ���ڼ�¼
-spec is_exist(atom(), any()) -> boolean().
is_exist(TableName, Key) ->
    Lists = select_list(TableName, Key),
    has_record(Lists).

%% ���������ֶ�ȡ��¼
-spec index_select(atom(), any(), any()) -> list().
index_select(TableName, Val, IndexField) when is_atom(TableName)->
    F = fun() ->
		mnesia:index_read(TableName, Val, IndexField)
        end,
    mnesia:activity(transaction, F, [], mnesia_frag).


%% ����/���� ���ݵ�����
%% ����д, ���ڶ���ط��ĺ�����Ҫ����һ��, ������ķ�ʽ����֤��ȷ��
%% DataList��һ��[Data]���б�
-spec write(list() | any()) -> ok.
write([]) ->
    erlang:error(badarg);
write(Datas) when is_list(Datas)->
    F = fun() ->
		[ok = mnesia:write(Data) || Data <- Datas],
		ok
	end,
    ok = mnesia:activity(transaction, F, [], mnesia_frag);
write(Data) ->
    F = fun() -> mnesia:write(Data) end,
    ok = mnesia:activity(transaction, F, [], mnesia_frag).

%% ������ķ�ʽ�޸����ݿ�
%% OpList = [op]
%% op = {write, List1} | {delete, List2}
%% List1: [tuple() | tuple()]
%% List2: [{tablename, key}]
change(OpList) ->
    F = fun() -> 
		[case Op of
		    {write, List1} ->
			[ok = mnesia:write(Data) || Data <- List1];
		    {delete_object, List1} ->
			[ok = mnesia:delete_object(Data) || Data <- List1];
		    {delete, List2} ->
			[ok = mnesia:delete({TableName, Key}) || {TableName, Key} <- List2]
		 end || Op <- OpList],
		ok	   
	end,
    ok = mnesia:activity(transaction, F, [], mnesia_frag).
    

%% ɾ�����߳��� 
-spec delete(atom(), any()) -> any().
delete(TableName, Key) when is_atom(TableName) ->
    F = fun() -> mnesia:delete({TableName, Key}) end,
    ok = mnesia:activity(transaction, F, [], mnesia_frag).

do_qlc(Q) ->
    F = fun() ->
		qlc:e(Q)
	end,
    Val = mnesia:activity(transaction, F, [], mnesia_frag),
    Val.

all_keys(Tab) ->
    F = fun() ->
		mnesia:dirty_all_keys(Tab)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

%% �޸ļ�¼, �ȶ�ȡ���޸ĺ�д��
raw_modify(Table, Key, FModify, DefRecord) when is_atom(Table) ->
    case mnesia:read(Table, Key) of		    
	[Record] -> 
	    NRecord = FModify(Record),
	    {mnesia:write(NRecord), NRecord};
	_ -> %%no_record
	    NRecord = FModify(DefRecord),
	    {mnesia:write(NRecord), NRecord}			
    end.

modify(Table, Key, FModify, DefRecord) when is_atom(Table) ->
    transaction(fun() -> raw_modify(Table, Key, FModify, DefRecord) end).

%% �ȶ�ȡ������������, �޸Ĳ�д��
raw_modify_if(Table, Key, FModify, DefRecord, FCondition) when is_atom(Table) ->
    R = case mnesia:read(Table, Key) of			   
	    [Record] -> Record;
	    _ -> DefRecord
	end,
    case FCondition(R) of
	true ->
	    NRecord = FModify(R),
	    {mnesia:write(NRecord), NRecord};
	Error ->
	    Error
    end.			    
modify_if(Table, Key, FModify, DefRecord, FCondition) when is_atom(Table) ->
    transaction(fun() -> raw_modify_if(Table, Key, FModify, DefRecord, FCondition) end).



transaction(F)->
    mnesia:activity(transaction, F, [], mnesia_frag).


%%%===================================================================
%%% Internal Function
%%%===================================================================
%% �Ƿ��м�¼
has_record(Lists) ->
    case Lists of
	[] -> false;
	_ -> true
    end.    
