%% Author: NoteBook
%% Created: 2009-9-18
%% Description: TODO: Add description to gen_common
-module(gen_common).

%%
%% Include files
%%

%%
%% Exported Functions
%%
-export([get_type/0, get_enum_def_list/1, get_default_table/0]).

%%
%% API Functions
%%

%%
%% TODO: Add description of get_type/function_arity
%%
get_type() -> 
	TypeList = [{int, base_type}, 
	 {unint, base_type}, 
	 {int16, base_type}, 
	 {uint16, base_type}, 
	 {int64, base_type}, 
	 {uint64, base_type}, 
	 {short, base_type}, 
	 {long, base_type}, 
	 {char, base_type}, 
	 {uchar, base_type}, 
	 {float, base_type}, 
	 {double, base_type},
	 {string, string_type}
	 ],
	TypeList ++ get_enum_def_list(net_protocal_def:get_enum_def()).

get_enum_def_list(Enums) ->
	F = fun(Enum) ->
			{element(1, Enum), enum_type}
		end,
	[F(Enum) || Enum <- Enums].

get_default_table() ->
    [{int, "0"}, 
     {unint, "0"}, 
     {int16, "0"},
     {int64, "0"},
     {uint64, "0"},
     {short, "0"},
     {long, "0"},
     {char, "\"\""},
     {uchar, "\"\""},
     {float, "0.0"}, 
     {double, "0.0"},
     {string, "\"\""}].
%%
%% Local Functions
%%

