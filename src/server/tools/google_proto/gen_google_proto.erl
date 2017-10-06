%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   ×ª³Égoogle proto
%%% @end
%%% Created : 24 Jun 2011 by hongjx <hongjx@35info.cn>

-module(gen_google_proto).


-export([start/0]).

start() ->
    Structures = net_protocal_def:get_struct_def(),
    NameList = [hd(X) || X <- Structures],
    Content = lists:foldl(fun(X, Str) -> Str ++ convert(X, NameList) end, 
			  "", 
			  Structures),
    file:write_file("packet_def.proto", Content ++ "\n").

%% message Location {
%%     optional string region = 1;
%%     required string country = 2;
%% }


convert([MsgName | Fields], NameList) ->
    "message " ++ atom_to_list(MsgName) ++ "{\n" ++ convert_fields(Fields, NameList) ++ "}\n".

convert_fields(Fields, NameList) ->    
    lists:foldl(fun({X, Index}, Str) -> Str ++ do_convert(X, Index, NameList) end, 
		"", 
		lists:zip(Fields, lists:seq(1, length(Fields)))).

do_convert({array, Type, Field}, Index, NameList) ->
    "\t repeated " ++ get_type(Type, NameList) ++ " " ++ atom_to_list(Field)
	++ " = " ++ integer_to_list(Index) ++ ";\n";
do_convert({Type, Field}, Index, NameList) ->
    "\t optional " ++ get_type(Type, NameList) ++ " " ++ atom_to_list(Field) 
	++ " = " ++ integer_to_list(Index) ++ ";\n".

    
get_type(float, _NameList) ->
    "float";
get_type(uint64, _NameList) ->
    "uint64";
get_type(int, _NameList) ->
    "int32";
get_type(string, _NameList) ->
    "string";
get_type(Other, NameList) ->
    case lists:member(Other, NameList) of
	true ->
	    atom_to_list(Other);
	_ ->
	    "int32"
    end.

