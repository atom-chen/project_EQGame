%% Author: NoteBook
%% Created: 2009-9-3
%% Description: 该工具的作用是根据指定的结构生成策划所需要的xml源
-module(make_xml_source).

-include("xml_define.hrl").

%%
%% Exported Functions
%%
-export([start/0]).

%%
%% TODO: Add description of start/function_arity
%%
start() -> 
    make_mapping_files(),
    make_record_def_files(),
    make_tplt_field_type().

%% 生成读取xml数据需要的一些类型定义的文件
make_tplt_field_type() ->
    %L = [make_one_record_type_def(RecordName, Fields) || {RecordName, Fields} <- ?tplt_list],
    Str = "-define(field_type_list, ([" ++ make_record_type_def(?tplt_list) ++ "])).",
    file:write_file("../src/tplt_field_type.hrl", Str).

make_record_type_def([{RecordName, F}]) ->
    "{" ++ atom_to_list(RecordName) ++ ",[" ++ make_field_type("", F) ++ "]}";
make_record_type_def([{RecordName, F}|L]) ->
    "{" ++ atom_to_list(RecordName) ++ ",[" ++ make_field_type("", F) ++ "]},\n"
	++ make_record_type_def(L).

make_field_type(Str, [{Type, _Field}]) ->
    Str ++ atom_to_list(Type);
make_field_type(Str, [{Type, _Field}|Rest]) ->
    make_field_type(Str ++ atom_to_list(Type) ++ ",", Rest).

%% 生成xml对应的记录的定义文件
make_record_def_files()->
    L = [make_record_def(RecordName, get_record_fields(Fields)) || {RecordName, Fields} <- ?tplt_list],
    Str = lists:foldl(fun(E, S) -> S ++ E end, "", L),
    file:write_file("../src/tplt_def.hrl", Str).


%% 生成一个record的定义
%% -record(item_tplt, {id, name}).
make_record_def(RecordName, Fields)->
    "-record(" ++ atom_to_list(RecordName) ++ ",{" ++ make_record_content("", Fields) ++ "}).\n".

make_record_content(Str, [Field]) ->
    Str ++ atom_to_list(Field);
make_record_content(Str, [Field|Rest]) ->
    make_record_content(Str ++ atom_to_list(Field) ++ ",", Rest).

%% 生成mapping映射文件供excel映射使用
make_mapping_files()->
    [merge_content(RecordName, get_record_fields(Fields)) || {RecordName, Fields} <- ?tplt_list].

get_record_fields(Fields)->
    [FieldName||{_Type, FieldName}<- Fields].

merge_content(RecordName, Fields) ->
    Str = "<" ++ atom_to_list(RecordName) ++ ">",
    merge_content(RecordName, Fields, Str).

merge_content(RecordName, [Field|Rest], Str) ->
    Str1 = Str ++ "<" ++ atom_to_list(Field) ++ "/>",
    merge_content(RecordName, Rest, Str1);
merge_content(RecordName, [], Str) ->
    Str1 = Str ++ "</" ++ atom_to_list(RecordName) ++ ">",
    merge_root(RecordName, Str1).

merge_root(RecordName, Content) ->
    Str1 = "<root>" ++ Content ++ Content ++ "</root>",
    file:write_file(atom_to_list(RecordName) ++ "_mapping.xml", Str1).
