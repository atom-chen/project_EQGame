%% Author: NoteBook
%% Created: 2009-9-18
%% Description: TODO: Add description to gen_protocal_c
-module(gen_protocal_cpp).

%%
%% Include files
%%

%%
%% Exported Functions
%%
-export([start/0]).
-export([test/0]).
%%
%% API Functions
%%

start() ->
    StructList = net_protocal_def:get_struct_def(),
    TypeList = gen_common:get_type(),
    FileStr = make_include() ++ make_protocal(StructList, "", TypeList),
    file:write_file("NetPacket.h",  FileStr),
    EnumStr = make_protocal_enum(net_protocal_def:get_enum_def(), ""),
    ConstStr = make_protocal_const(net_protocal_def:get_version()),
    file:write_file("EnumDef.h",  EnumStr ++ "\n" ++ ConstStr),
    MsgTypeStr = make_protocal_msg_type(StructList, ""),
    file:write_file("NetMsgType.h", MsgTypeStr).

%%
%% Local Functions
%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%生成包含文件%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

make_include() ->
    "#pragma once
    #include\"ByteArray.h\"
    #include\"BaseType.h\"
    #include\"INetPacket.h\"
    #include\"NetMsgType.h\"
    #include<string>
    #include<vector>
    ".
%%%%%%%%%%%%%%%%%%%%%%%%%%%%生成函数的方法%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

make_protocal([], StructStr, _TypeList) ->
    StructStr;
make_protocal([Struct | Structs], StructStr, TypeList) ->
    [StructName | NewStructList] = Struct,
    VariableStr = make_protocal_Variable(NewStructList, "", TypeList),
    EncodeStr = make_protocal_encode(NewStructList, "", TypeList),
    DecodeStr = make_protocal_decode(NewStructList, "", TypeList),
    BuildStr = make_protocal_build(StructName),
    StructStr1 = StructStr ++ "\nstruct " ++ atom_to_list(StructName) ++ 
	" : public INetPacket\n{\n" ++ VariableStr ++ EncodeStr ++ DecodeStr ++ BuildStr ++ "\n};\n",
    make_protocal(Structs, StructStr1, TypeList).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%生成变量%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

make_protocal_Variable([], VariableStr, _TypeList) ->
    VariableStr;
make_protocal_Variable([{Type, Name} | Vars], VariableStr, TypeList) ->
    VariableStr1 = 
	case lists:keyfind(Type, 1, TypeList) of
	    {Type1, base_type} ->
		VariableStr ++ "\t" ++ atom_to_list(Type1) ++ " " ++ atom_to_list(Name) ++ ";\n";
	    {Type1, string_type}  -> 
		VariableStr ++ "\tstd::" ++ atom_to_list(Type1) ++ " " ++ atom_to_list(Name) ++ ";\n";
	    {_Type, enum_type} ->
		VariableStr ++ "\tint " ++ atom_to_list(Name) ++ ";\n";
	    _ -> 
		VariableStr ++ "\t" ++ atom_to_list(Type) ++ " " ++ atom_to_list(Name) ++ ";\n"
	end,
    make_protocal_Variable(Vars, VariableStr1, TypeList);
make_protocal_Variable([{array, Type, Name} | Vars], VariableStr, TypeList) ->
    VariableStr1 = 
	case lists:keyfind(Type, 1, TypeList) of
	    {Type1, base_type} ->
		VariableStr ++ "\tstd::vector<" ++ atom_to_list(Type1) ++ "> " ++ atom_to_list(Name) ++ ";\n";
	    {Type1, string_type}  -> 
		VariableStr ++ "\tstd::vector<std::" ++ atom_to_list(Type1) ++ "> " ++ atom_to_list(Name) ++ ";\n";
	    {_Type, enum_type} ->
		VariableStr ++ "\tstd::vector<int> " ++ atom_to_list(Name) ++ ";\n";
	    _ -> 
		VariableStr ++ "\tstd::vector<" ++ atom_to_list(Type) ++ "> " ++ atom_to_list(Name) ++ ";\n"
	end,
    make_protocal_Variable(Vars, VariableStr1, TypeList).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


make_protocal_build(StructName) ->
    "\tvoid build(ByteArray &byteArray)\n" ++ 
	"\t{\n" ++
        "\t\tbyteArray.write_uint16(NetMsgType::msg_" ++ atom_to_list(StructName) ++ ");\n" ++
        "\t\tencode(byteArray);\n" ++
	"\t}".

%%%%%%%%%%%%%%%%%%%%%%%%%%%%生成encode%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% byteArray.write_uint16(account.size());
%%         for(int i=0; i<account.size();i++)
%%         {
%%             byteArray.write_string(account[i]);
%%         } 
make_protocal_encode([], EncodeStr, _TypeList) ->
    "\tvoid encode(ByteArray &byteArray)\n\t{\n" ++ EncodeStr ++ "\t}\n\n";
make_protocal_encode([Struct | Vars], EncodeStr, TypeList) ->
    EncodeStr1 = EncodeStr ++ make_protocal_encode_item(Struct, TypeList),
    make_protocal_encode(Vars, EncodeStr1, TypeList).

%% 生成decode中的数组 
make_protocal_encode_item({Type, Name}, TypeList) ->
    case lists:keyfind(Type, 1, TypeList) of
	{Type1, base_type} ->
	    "\t\tbyteArray.write_" ++ atom_to_list(Type1) ++ "(" ++ atom_to_list(Name) ++ ");\n";
	{Type1, string_type}  -> 
	    "\t\tbyteArray.write_" ++ atom_to_list(Type1) ++ "(" ++ atom_to_list(Name) ++ ");\n";
	{_Type, enum_type} ->
	    make_protocal_encode_item({int, Name}, TypeList);
	_ -> 
	    "\t\t" ++ atom_to_list(Name) ++ ".encode(byteArray);\n\n"
    end;
make_protocal_encode_item({array, Type, Name}, TypeList) ->
    case lists:keyfind(Type, 1, TypeList) of
	{Type1, base_type} ->
	    make_protocal_encode_item_base_type(Name, Type1);
	{Type1, string_type}  -> 
	    make_protocal_encode_item_base_type(Name, Type1);
	{_Type, enum_type} ->
	    make_protocal_encode_item({array, int, Name}, TypeList);
	_ -> 
	    NameStr = atom_to_list(Name),
	    "\t\tbyteArray.write_uint16(" ++ NameStr ++ ".size());\n" ++
		"\t\tfor(size_t i=0; i<" ++ NameStr ++ ".size();i++)\n\t\t{\n" ++
		"\t\t\t" ++ NameStr ++ "[i].encode(byteArray);\n\t\t}\n\n"
    end.

make_protocal_encode_item_base_type(Name, Type) ->
    NameStr = atom_to_list(Name),
    TypeStr = atom_to_list(Type),
    "\t\tbyteArray.write_uint16(" ++ NameStr ++ ".size());\n" ++
	"\t\tfor(size_t i=0; i<" ++ NameStr ++ ".size();i++)\n\t\t{\n" ++ 
	"\t\t\tbyteArray.write_" ++ TypeStr ++"(" ++ NameStr ++ "[i]);\n\t\t}\n\n".

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%生成decode%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

make_protocal_decode([], DecodeStr, _TypeList) ->
    "\tvoid decode(ByteArray &byteArray)\n\t{\n" ++ DecodeStr ++ "\t}\n\n";
make_protocal_decode([Struct | Vars], DecodeStr, TypeList) ->
    DecodeStr1 = DecodeStr ++  make_protocal_decode_item(Struct, TypeList),
    make_protocal_decode(Vars, DecodeStr1, TypeList).

make_protocal_decode_item({Type, Name}, TypeList) ->
    case lists:keyfind(Type, 1, TypeList) of
	{Type1, base_type} ->
	    "\t\t" ++ atom_to_list(Name) ++ " = byteArray.read_"++ atom_to_list(Type1) ++"();\n";
	{Type1, string_type}  -> 
	    "\t\t" ++ atom_to_list(Name) ++ " = byteArray.read_"++ atom_to_list(Type1) ++"();\n";
	{_Type, enum_type} ->
	    make_protocal_decode_item({int, Name}, TypeList);
	_ -> 
	    "\t\t" ++ atom_to_list(Name) ++ ".decode(byteArray);\n\n"
    end;
make_protocal_decode_item({array, Type, Name}, TypeList) ->
    case lists:keyfind(Type, 1, TypeList) of
	{Type1, base_type} ->
	    make_protocal_encode_item_decode_type(Name, Type1);
	{Type1, string_type}  -> 
	    make_protocal_encode_item_decode_type(Name, Type1);
	{_Type, enum_type} ->
	    make_protocal_decode_item({array, int, Name}, TypeList);
	_ -> 
	    NameStr = atom_to_list(Name),
	    "\t\t{\n\t\tUINT16 size = byteArray.read_uint16();\n" ++
		"\t\t" ++ NameStr ++ ".resize(size);\n" ++
    		"\t\tfor(int i=0; i<size;i++)\n" ++
    		"\t\t{\n" ++
    		"\t\t\t" ++ NameStr ++ "[i].decode(byteArray);\n\t\t}\n\t\t}\n\n"
    end.

make_protocal_encode_item_decode_type(Name, Type) ->
    NameStr = atom_to_list(Name),
    TypeStr = atom_to_list(Type),
    "\t\t{\n\t\tUINT16 size = byteArray.read_uint16();\n" ++
	"\t\t" ++ NameStr ++ ".reserve(size);\n" ++
	"\t\tfor(int i=0; i<size;i++)\n" ++
	"\t\t{\n" ++
	"\t\t\t" ++ NameStr ++ ".push_back(byteArray.read_" ++ TypeStr ++ "());\n\t\t}\n\t\t}\n\n".

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%生成网络类型%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
make_protocal_msg_type([], MsgTypeStr) ->
    "
#pragma once
class NetMsgType{
public:
enum net_type
{
    msg_none,
    " ++ MsgTypeStr ++ 
    "
};
};";
make_protocal_msg_type([Struct|Structs], MsgTypeStr) ->
    [StructName | _NewStructList] = Struct,
    make_protocal_msg_type(Structs, MsgTypeStr ++ "\tmsg_" ++ atom_to_list(StructName) ++ ",\n").

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%生成枚举类型%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

make_protocal_const(Ver) ->
    "const int PROTO_VER = " ++ integer_to_list(Ver) ++ ";\n".

make_protocal_enum([], EnumStr) ->
    "#pragma once\n" ++ EnumStr;
make_protocal_enum([Enum | Enums], EnumStr) ->
    {EnumName , EnumItems} = Enum,
    EnumStr1 = EnumStr ++ "enum " ++
	string:to_upper(atom_to_list(EnumName)) ++ "\n{\n" ++ 
	make_protocal_enum_item(EnumItems, 1, "") ++ "};\n\n",
    make_protocal_enum(Enums, EnumStr1).

make_protocal_enum_item([EnumItem | []], Index, EnumItemStr) ->
    EnumItemStr ++ "\t" ++ atom_to_list(EnumItem) ++ " = " ++ integer_to_list(Index) ++ "\n";
make_protocal_enum_item([EnumItem | EnumItems], Index, EnumItemStr) ->
    EnumItemStr1 = EnumItemStr ++ "\t" ++ atom_to_list(EnumItem) ++ " = " ++ integer_to_list(Index) ++ ",\n",
    make_protocal_enum_item(EnumItems, Index + 1, EnumItemStr1).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%测试函数%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

test() ->
    %% 	test_make_protocal_encode_item(),
    %% 	test_make_protocal_decode_item(),
    %% 	test_make_protocal_encode(),
    %% 	test_make_protocal_decode().
    start().

%% test_make_protocal_encode_item() ->
%% 	"\t\tbyteArray.write_uint16(result.size());\n\t\tfor(int i=0; i<result.size();i++)\n\t{\n\t\t\tresult[i].encode(byteArray);\n\t}\n"
%% 		= make_protocal_encode_item({array, notify_login_result, result}, gen_common:get_type()),
%% 	"\t\tbyteArray.write_uint16(account.size());\n\t\tfor(int i=0; i<account.size();i++)\n\t{\n\t\t\tbyteArray.write_string(account[i]);\n\t}\n" 
%% 		= make_protocal_encode_item({array, string, account}, gen_common:get_type()),
%% 	"\t\tbyteArray.write_uint16(account.size());\n\t\tfor(int i=0; i<account.size();i++)\n\t{\n\t\t\tbyteArray.write_int(account[i]);\n\t}\n" 
%% 		= make_protocal_encode_item({array, login_result, account}, gen_common:get_type()),
%% 	"\t\tbyteArray.write_string(account);\n" = make_protocal_encode_item({string, account}, gen_common:get_type()),
%% 	"\t\tbyteArray.write_int(account);\n" = make_protocal_encode_item({login_result, account}, gen_common:get_type()),
%% 	"\t\taccount.encode(byteArray);\n" = make_protocal_encode_item({notify_login_result, account}, gen_common:get_type()).
%% 
%% test_make_protocal_decode_item() ->
%% 	"uint16 size = byteArray.read_uint16();\naccount.reserve(size);\nfor(int i=0; i<size;i++)\n{\n\taccount.push_back(byteArray.read_string());\n}\n"
%% 		= make_protocal_decode_item({array, string, account}, gen_common:get_type()),
%% 	"uint16 size = byteArray.read_uint16();\nresult.resize(size);\nfor(int i=0; i<size;i++)\n{\n\tresult[i].decode(byteArray);\n}\n"
%% 		= make_protocal_decode_item({array, notify_login_result, result}, gen_common:get_type()),
%% 	"uint16 size = byteArray.read_uint16();\naccount.reserve(size);\nfor(int i=0; i<size;i++)\n{\n\taccount.push_back(byteArray.read_int());\n}\n"
%% 		= make_protocal_decode_item({array, login_result, account}, gen_common:get_type()),
%% 	"account = byteArray.read_string();\n" = make_protocal_decode_item({string, account}, gen_common:get_type()),
%% 	"account = byteArray.read_int();\n" = make_protocal_decode_item({login_result, account}, gen_common:get_type()),
%% 	"account.decode(byteArray);\n" = make_protocal_decode_item({notify_login_result, account}, gen_common:get_type()).
%% 
%% test_make_protocal_encode() ->
%% 	Result = make_protocal_encode([{array, string, account},
%% 		 	{string, pwd},
%% 		 	{player_data, data},
%% 		 	{array, notify_login_result, result}], "", gen_common:get_type()),
%% 	io:format("~p~n", [Result]).
%% 
%% test_make_protocal_decode() ->
%% 	Result = make_protocal_decode([{array, string, account},
%% 		 	{string, pwd},
%% 		 	{player_data, data},
%% 		 	{array, notify_login_result, result}], "", gen_common:get_type()),
%% 	io:format("~p~n", [Result]).
%% 
%% test_make_protocal_Variable() ->
%% 	Result = make_protocal_Variable([{array, string, account},
%% 		 	{string, pwd},
%% 		 	{player_data, data},
%% 		 	{array, notify_login_result, result}], "", gen_common:get_type()),
%% 	io:format("~p~n", [Result]).
