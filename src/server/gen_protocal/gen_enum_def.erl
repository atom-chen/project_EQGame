%% Author: NoteBook
%% Created: 2009-9-17
%% Description: TODO: Add description to gen_enum_def
-module(gen_enum_def).

%%
%% Include files
%%

%%
%% Exported Functions
%%
-export([start/0, test/0]).

%%
%% API Functions
%%

start() ->
	EnumStr = make_enum_def(net_protocal_def:get_enum_def(), ""),
	file:write_file("enum_def.hrl", EnumStr).

%%
%% Local Functions
%%
make_enum_def([], EnumStr) ->
	EnumStr;
make_enum_def([{_EnumType, EnumList} | Enums], EnumStr) ->
	NewEnumStr = EnumStr ++ make_enum_def_record(EnumList, 1, ""),
	make_enum_def(Enums, NewEnumStr).

make_enum_def_record([], _Index, EnumStr) ->
	EnumStr;
make_enum_def_record([Enum | EnumList], Index, EnumStr) ->
	NewEnumStr = EnumStr ++ "-define(" ++ atom_to_list(Enum) ++ ", "++ integer_to_list(Index) ++").\n",
	make_enum_def_record(EnumList, Index + 1, NewEnumStr).

test() ->
	test_make_enum_def_record(),
	test_make_enum_def(),
	start().

test_make_enum_def_record() ->
	"-define(lr_ok, 1).\n-define(lr_fail, 2).\n" = make_enum_def_record([lr_ok, lr_fail], 1, "").

test_make_enum_def() ->
	"-define(lr_ok, 1).\n-define(lr_fail, 2).\n-define(d_ok, 1).\n-define(d_fail, 2).\n" = make_enum_def([
	 	{login_result, [lr_ok, lr_fail]},
		{notify_result, [d_ok, d_fail]}
	 ], "").
