%% Author: NoteBook
%% Created: 2009-9-9
%% Description: TODO: Add description to procotal
-module(gen_protocal).
%%
%% Include files
%%

%%
%% Exported Functions
%%
-export([start/0, test/0]).

%%
%% start/function_arity
%%
start() -> 
    make_protocal_def(),
    make_file().

make_protocal_def() ->
    Structures = net_protocal_def:get_struct_def(),
    StructureStr = get_structure(Structures, ""),
    MessageStr = create_defines(Structures, 1, ""),
    VersionStr = create_version(net_protocal_def:get_version()),
    file:write_file("packet_def.hrl", StructureStr ++ "\n" ++ MessageStr ++ "\n" ++ VersionStr).

%%
%% Local Functions
%%

%%
%% 获取结构信息
%%
get_structure([], Str) ->
    Str;
get_structure([Structure | Structures], Str) ->
    NewStr = create_record(Structure, "", 0),
    get_structure(Structures, Str ++ NewStr).

%%
%% 根据结构信息创建Record记录
%%
create_record([Data | DataStructures], Str, Flag) ->
    NewStr = create_record(Data, Str, Flag),
    create_record(DataStructures, NewStr, Flag + 1);
create_record([], Str, _Flag) ->
    Str ++ "}).\n";
create_record({Type, Name}, Str, Flag) ->
    merge_record(Type, Name, Str, Flag);
create_record({array, _Type, Name}, Str, Flag) ->
    merge_record(array, Name, Str, Flag);
create_record(RecordName, _Str, _Flag) ->
    "-record(" ++ atom_to_list(RecordName) ++ ", {".

merge_record(array, Name, Str, Flag)->
    case Flag of
	1 ->
	    Str ++ atom_to_list(Name) ++ "=[]";
	_->
	    Str ++ ", " ++ atom_to_list(Name) ++ "=[]"
    end;
merge_record(Type, Name, Str, Flag) ->
    case Flag of
	1 -> 
	    Str ++ atom_to_list(Name) ++ "=" ++ 
		get_default_value(gen_common:get_default_table(), 
				  Type, 
				  gen_common:get_enum_def_list(net_protocal_def:get_enum_def()));
	_ -> 
	    Str ++ ", " ++ atom_to_list(Name) ++ "=" ++ 
		get_default_value(gen_common:get_default_table(), 
				  Type, 
				  gen_common:get_enum_def_list(net_protocal_def:get_enum_def()))
    end.

get_default_value([], CurrentType, _EnumList) ->
    "#" ++ atom_to_list(CurrentType) ++ "{}";
get_default_value([{Type, Value}|Table], CurrentType, EnumList) ->
    if
        Type =/= CurrentType ->
            case lists:keyfind(CurrentType, 1, EnumList) of
                {_Name, enum_type} ->
                    integer_to_list(0);
                false ->
                    get_default_value(Table, CurrentType, EnumList)
            end;
        true ->
            Value
    end.

create_version(Version) ->
    "-define(proto_ver, " ++ integer_to_list(Version) ++ ").\n".

%%
%% 创建Message信息
%% 
create_defines([], _Index, Str) ->
    Str;
create_defines([Struct|Rest], Index, Str) ->
    NewStr = create_define(Struct, Index),
    create_defines(Rest, Index + 1, Str ++ NewStr).

create_define([StructName | _StructBody], Index) ->
    "-define(msg_" ++ atom_to_list(StructName) ++ ", " ++ integer_to_list(Index) ++").\n".

%%%%%%%%%%%%%%%%%%%%生成文件%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
make_file() ->
    Include = make_function_include(),
    Encode = make_function_encode(net_protocal_def:get_struct_def(), "", gen_common:get_type()),
    Decode = get_struct_member(net_protocal_def:get_struct_def(), "", gen_common:get_type()),
    file:write_file("protocal.erl", Include ++ "\n\n" ++ Encode ++ "\n\n" ++ Decode).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%生成include方法%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
make_function_include() ->
    "-module(protocal).\n" ++ 
	"-include(\"packet_def.hrl\").\n" ++ 
	"-include(\"net_type.hrl\").\n" ++ 
	"-export([encode/1, decode/2]).\n".

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%生成encode方法%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
make_function_encode([Struct | []], MergeFunc, TypeList) ->
    [StructName | StructBody] = Struct,
    NewTypeList = [{StructName, struct_type} | TypeList],
    HeaderFunc = make_function_declare(Struct),
    BodyFunc = make_function_serilize_declare(StructName, StructBody, NewTypeList),
    make_function_format(content_end, MergeFunc, HeaderFunc, BodyFunc);
make_function_encode([Struct | Structs], MergeFunc, TypeList) ->
    [StructName | StructBody] = Struct,
    NewTypeList = [{StructName, struct_type} | TypeList],
    HeaderFunc = make_function_declare(Struct),
    BodyFunc = make_function_serilize_declare(StructName, StructBody, NewTypeList),
    NewMergeFunc = make_function_format(content, MergeFunc, HeaderFunc, BodyFunc),
    make_function_encode(Structs, NewMergeFunc, NewTypeList).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

make_function_format(content, MergeFunc, HeaderFunc, BodyFunc) ->
    MergeFunc ++ HeaderFunc ++ "\n\t" ++ BodyFunc ++ ";\n";
make_function_format(content_end, MergeFunc, HeaderFunc, BodyFunc) ->
    MergeFunc ++ HeaderFunc ++ "\n\t" ++ BodyFunc ++ ".\n".

%%%%%%%%%%%%%%%%%%%%生成函数头%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
%%
%% 生成函数头
%% "encode(#req_login{account=Account, pwd=Pwd}) ->"
make_function_declare([StructName | Vars]) ->
    Args = [make_function_arg(Var) || Var <- Vars],
    "encode(#" ++ atom_to_list(StructName) ++ "{" ++ merge_function_arg(Args, "") ++ "}) ->".

%%
%% 合并函数的参数
%% {account=Account, pwd=Pwd}
merge_function_arg([], _MergeArgs) ->
    "";
merge_function_arg([Arg | []], MergeArgs) ->
    MergeArgs ++ Arg;
merge_function_arg([Arg | Args], MergeArgs) ->
    NewMergeArgs = MergeArgs ++ Arg ++ ", ",
    merge_function_arg(Args, NewMergeArgs).

%%
%% 生成函数的单个参数
%%
make_function_arg({_Type, VarName}) ->
    VarNameStr = atom_to_list(VarName),
    VarNameStr ++ "=" ++ string:to_upper(VarNameStr);
make_function_arg({array, Type, VarName}) ->
    make_function_arg({Type, VarName}).

%%%%%%%%%%%%%%%%%%%%生成函数内容%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

make_function_serilize_declare(StructName, [], _TypeList) ->
    "{?msg_" ++ atom_to_list(StructName) ++ ", null}";
make_function_serilize_declare(StructName, StructBody, TypeList) ->
    Serilize = [make_function_serilize(Var, TypeList) || Var<- StructBody],
    "{?msg_" ++ atom_to_list(StructName) ++ ", <<"++ merge_function_serilize(Serilize, "") ++">>}".


%%(net_helper:encode_string(Account))/binary
%%
%% 针对单个元组生成序列化参数
%%1.基本类型不包含string, 2.string, 3.enum, 4.自定义类型, 5.array
make_function_serilize({Type, VarName}, TypeList) ->
    case lists:keyfind(Type, 1, TypeList) of
	{_Name, CustomType} -> 
	    make_function_serilize_arg({CustomType, Type}, VarName);
	false -> 
	    io:format("Not Found CustomType in type list, TypeName:~p, term:~p~n", [Type, VarName])
    end;
make_function_serilize({array, Type, VarName}, TypeList) ->
    case lists:keyfind(Type, 1, TypeList) of
	{_Name, CustomType} -> 
	    make_function_serilize_array({CustomType, Type}, VarName);
	false -> 
	    io:format("Not Found CustomType in type list, TypeName:~p, term:~p~n", [Type, VarName])
    end.

%% 
%% 合并元组生成序列化参数
%%
merge_function_serilize([Arg | []], MergeArgs) ->
    MergeArgs ++ Arg;
merge_function_serilize([Arg | Args], MergeArgs) ->
    NewMergeArgs = MergeArgs ++ Arg ++ ", ",
    merge_function_serilize(Args, NewMergeArgs).

%%
%% 生成函数二进制内容
%%1.string:(net_helper:encode_string(Account))/binary
%%2.基本类型不包含string: Value:?uint
%%3.自定义类型:encode(ReqLogin)/binary
%% 4.enum:  Value:?int
make_function_serilize_arg({base_type, Type}, VarName) ->
    string:to_upper(atom_to_list(VarName)) ++ ":?" ++ string:to_upper(atom_to_list(Type));
make_function_serilize_arg({string_type, _Type}, VarName) ->
    "(net_helper:encode_string(" ++ string:to_upper(atom_to_list(VarName)) ++ "))/binary";
make_function_serilize_arg({enum_type, _Type}, VarName) ->
    string:to_upper(atom_to_list(VarName)) ++ ":?INT";
make_function_serilize_arg({struct_type, _Type}, VarName) ->
    "(net_helper:get_encode_binary_data(encode(" ++ string:to_upper(atom_to_list(VarName)) ++ ")))/binary".

%%数组
make_function_serilize_array({base_type, Type}, VarName) ->
    "(net_helper:encode_list(fun(E)-> <<E:?" ++ string:to_upper(atom_to_list(Type)) ++ ">>end, " 
	++ string:to_upper(atom_to_list(VarName)) ++ "))/binary";
make_function_serilize_array({string_type, _Type}, VarName) ->
    "(net_helper:encode_list(fun(E)-> net_helper:encode_string(E) end, " 
	++ string:to_upper(atom_to_list(VarName)) ++ "))/binary";
make_function_serilize_array({enum_type, _Type}, VarName) ->
    "(net_helper:encode_list(fun(E)-> <<E:?INT>>end, " 
	++ string:to_upper(atom_to_list(VarName)) ++ "))/binary";
make_function_serilize_array({struct_type, _Type}, VarName) ->
    "(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, " 
	++ string:to_upper(atom_to_list(VarName)) ++ "))/binary".
%%%%%%%%%%%%%%%%%%%%%%生成解码函数%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% 获得每个结构体的成员变量名
%% 返回成员的列表
%% [{array, string}, string, {user_define, fun(Bin)->decode(?player_data, Bin)end}]
%% [{array, user_define, fun(Bin)->decode(?player_data, Bin)end}]
get_struct_member([Struct | []], FuncStr, TypeList) ->
    [StructName | NewStruct] = Struct,
    FuncStr ++ make_function_decode_declare(Struct) ++ 
	"\t" ++ make_function_decode_body(StructName, get_struct_member_type(NewStruct, TypeList)) ++ ".\n";
get_struct_member([Struct | StructList], FuncStr, TypeList) ->
    [StructName | NewStruct] = Struct,
    NewTypeList = [{StructName, struct_type}| TypeList],
    FuncHeader = make_function_decode_declare(Struct),
    FuncBody = make_function_decode_body(StructName, get_struct_member_type(NewStruct, NewTypeList)),
    NewFuncStr = FuncStr ++ FuncHeader ++ "\t" ++ FuncBody ++ ";\n",
    get_struct_member(StructList, NewFuncStr, NewTypeList).

get_struct_member_type(Vars, TypeList) ->
    StrList = get_struct_member_type_list(Vars, [], TypeList),
    "[" ++make_type_list_string(StrList, "") ++ "]".

get_struct_member_type_list([], StrList, _TypeList) ->
    lists:reverse(StrList);
get_struct_member_type_list([{array, Type, Name} | Rest], StrList, TypeList) ->	
    case lists:keyfind(Type, 1, TypeList) of
	{_Name, struct_type} -> %% 结构体类型
	    Str1 = "{array, user_define, fun(Bin)->decode(?msg_" ++ atom_to_list(Type) ++ ", Bin)end}",
	    get_struct_member_type_list(Rest,  [Str1 | StrList], TypeList);
	{_Name, enum_type} -> %% 枚举类型
	    get_struct_member_type_list(Rest,  ["{array, int}" | StrList], TypeList);
	false -> 
	    io:format("Not Found CustomType in type list, TypeName:~p, term:~p~n", [Type, Name]);
	_ -> 
	    get_struct_member_type_list(Rest,  ["{array, " ++ atom_to_list(Type) ++ "}" | StrList], TypeList)
    end;
get_struct_member_type_list([{Type, Name} | Rest], StrList, TypeList) ->
    case lists:keyfind(Type, 1, TypeList) of
	{_Name, struct_type} -> %% 结构体类型
	    Str1 = "{user_define, fun(Bin)->decode(?msg_" ++ atom_to_list(Type) ++ ", Bin)end}",
	    get_struct_member_type_list(Rest, [Str1 | StrList], TypeList);
	{_Name, enum_type} -> %% 枚举类型
	    get_struct_member_type_list(Rest, ["int" | StrList], TypeList);
	false -> 
	    io:format("Not Found CustomType in type list, TypeName:~p, term:~p~n", [Type, Name]);
	_ -> 
	    get_struct_member_type_list(Rest, [atom_to_list(Type) | StrList], TypeList)
    end.


%%生成解码函数的函数头
make_function_decode_declare([StructName | _Vars]) ->
    "decode(?msg_" ++ atom_to_list(StructName) ++ ", Binary) ->\n".

%%生成解码函数的函数体
make_function_decode_body(StructName, TypeListStr) ->
    "net_helper:decode(" ++ TypeListStr ++ ", Binary, [" 
	++ atom_to_list(StructName) ++ "])".

%%根据原子类型列表转换成字符串
make_type_list_string([], Str) ->
    Str;
make_type_list_string([Type | Rest], []) ->
    make_type_list_string(Rest, Type);
make_type_list_string([Type | Rest], Str) ->
    Str1 = Str ++", " ++ Type,
    make_type_list_string(Rest, Str1).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
test() ->
    test_make_function_arg(),
    test_merge_function_arg(),
    test_make_function_declare(), 
    test_make_function_serilize_arg(),
    test_make_function_serilize_array(),
    test_make_function_serilize(),
    test_make_function_serilize_declare(),
    test_make_function(),
    test_make_type_list_string(),
    test_get_struct_member_type(),
    test_get_struct_member(),
    start().

test_make_function_arg() ->
    "account=ACCOUNT" = make_function_arg({string, account}),
    "account=ACCOUNT" = make_function_arg({int, account}),
    "account=ACCOUNT" = make_function_arg({array, int, account}),
    "account=ACCOUNT" = make_function_arg({array, string, account}).

test_merge_function_arg() ->
    "account=ACCOUNT, pwd=PWD, data=DATA" = merge_function_arg(["account=ACCOUNT","pwd=PWD","data=DATA"], "").

test_make_function_declare() ->
    "encode(#req_login{account=ACCOUNT, pwd=PWD, data=DATA}) ->" = 
	make_function_declare([req_login, 
			       {array, string, account},
			       {string, pwd},
			       {player_data, data}]).

test_make_function_serilize_arg() ->
    "ACCOUNT:?INT" = make_function_serilize_arg({base_type, int}, account),
    "(net_helper:encode_string(ACCOUNT))/binary" = make_function_serilize_arg({string_type, []}, account),
    "ACCOUNT:?INT" = make_function_serilize_arg({enum_type, []}, account),
    "(net_helper:get_encode_binary_data(encode(ACCOUNT)))/binary" = make_function_serilize_arg({struct_type, []}, account).

test_make_function_serilize_array() ->
    "(net_helper:encode_list(fun(E)-> <<E:?INT>>end, ACCOUNT))/binary" =
	make_function_serilize_array({base_type, int}, account),
    "(net_helper:encode_list(fun(E)-> net_helper:encode_string(E) end, ACCOUNTS))/binary" = 
	make_function_serilize_array({string_type, []}, accounts),
    "(net_helper:encode_list(fun(E)-> <<E:?INT>>end, ACCOUNTS))/binary" = 
	make_function_serilize_array({enum_type, []}, accounts),
    "(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, PLAYER_DATA))/binary" = 
	make_function_serilize_array({struct_type, []}, player_data).

test_make_function_serilize() ->
    "ACCOUNT:?INT" = make_function_serilize({int, account}, gen_common:get_type()),
    "(net_helper:encode_string(ACCOUNT))/binary" = make_function_serilize({string, account}, gen_common:get_type()), 
    "ACCOUNT:?INT" = make_function_serilize({login_result, account}, gen_common:get_type()).

test_make_function_serilize_declare() ->
    "{?msg_player_data, <<(net_helper:encode_string(NAME))/binary>>}" = make_function_serilize_declare(player_data, [{string, name}], gen_common:get_type()),
    "{?msg_player_data, <<(net_helper:encode_list(fun(E)-> net_helper:encode_string(E) end, ACCOUNT))/binary, (net_helper:encode_string(PWD))/binary, (net_helper:get_encode_binary_data(encode(DATA)))/binary>>}"
	= make_function_serilize_declare(player_data, [ 
			{array, string, account},
		 	{string, pwd},
		 	{player_data, data}
	 	], [{player_data, struct_type}|gen_common:get_type()]).

test_make_function() ->
	make_function_encode([
	 	[player_data, 
		 	{string, name}
		],
		
		[req_login, 
			{array, string, account},
		 	{string, pwd},
		 	{player_data, data}
	 	],
		
	 	[notify_login_result,
	  		{int, result}
	 	]
	 ], "", gen_common:get_type()).

test_make_type_list_string() ->
	"string, int, float" = make_type_list_string(["string", "int", "float"], "").

test_get_struct_member_type() ->
	"[{array, string}, string, {user_define, fun(Bin)->decode(?msg_player_data, Bin)end}]" = get_struct_member_type([
			{array, string, account},
		 	{string, pwd},
		 	{player_data, data}
	 	], [{player_data, struct_type}|gen_common:get_type()]).

test_get_struct_member() ->
	"decode(?msg_player_data, Binary) ->\n\tnet_helper:decode([string], Binary, [player_data]);\ndecode(?msg_req_login, Binary) ->\n\tnet_helper:decode([{array, string}, string, {user_define, fun(Bin)->decode(?msg_player_data, Bin)end}], Binary, [req_login]);\ndecode(?msg_notify_login_result, Binary) ->\n\tnet_helper:decode([int], Binary, [notify_login_result]).\n"
		= get_struct_member([
	 	[player_data, 
		 	{string, name}
		],
		
		[req_login, 
			{array, string, account},
		 	{string, pwd},
		 	{player_data, data}
	 	],
		
	 	[notify_login_result,
	  		{int, result}
	 	]
	 ], "", gen_common:get_type()).






