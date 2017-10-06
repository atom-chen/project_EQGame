%% Author: NoteBook
%% Created: 2009-9-3
%% Description: TODO: Add description to xml_data
-module(xml_data).

%%
%% Include files
%%
-include_lib("xmerl/include/xmerl.hrl").
-include_lib("xmerl/include/xmerl_xsd.hrl").

-record(table_info, {name="", %%表名称
		     info_complete = false, %%是否全部获取表的字段 
		     info=[], %%表结构信息
		     record_temp = [], 
		     records=[] %%记录信息
		    }).
%%
%% Exported Functions
%%
-export([start/0, test/0]).

%%
%% Add description of start/function_arity
%%
start() ->
    FilePath = "xml/",
    {ok, Files} = get_file_list(FilePath),
    ok = get_file(FilePath, Files).

%%
%% Local Functions
%%

test() ->
    mnesia:create_schema([node()|nodes()]),
    mnesia:start(),
    mnesia:wait_for_tables([book], 20000),
    start().

%% 
%% 创建表格信息
%% 
create_table(#table_info{name = Name, info = Info, records = Records}) ->
    io:format("Records Info:~p~n", [Records]),
    TableName = list_to_atom(Name),
    mnesia:delete_table(TableName),
    case mnesia:create_table(TableName, [{disc_copies, [node() | nodes()]}, {attributes, Info}]) of
	{aborted, Reason} -> 
	    io:format("~p~n", [Reason]),
	    fail;
	{atomic, ok} ->
	    insert(Records),
	    ok
    end.
%% 
%% 将数据插入表
%% 
insert(Records) ->
    F = fun() ->
		lists:foreach(fun mnesia:write/1, Records)
	end,
    Result = mnesia:transaction(F),
    case Result of
	{aborted, _} -> 
	    io:format("Insert Record Error:~p~n", [Result]), 
	    error;
	{atomic, _} ->
	    ok
    end.

%% 
%% 获取文件列表
%% 
get_file_list(FilePath) ->
    case file:list_dir(FilePath) of
	{ok, Files} -> {ok, Files};
	_ -> error
    end.

%% 
%% 获取文件
%% 
get_file(_FilePath, []) ->
    ok;
get_file(FilePath, [File|Files]) ->
    case filename:extension(File) of
	".xml" -> 
	    State = parse_file(FilePath, File),
	    create_table(State);
	_ -> error
    end,
    get_file(FilePath, Files).

%% 
%% 解析文件
%% 
parse_file(FilePath, FileName) ->
    {ok, #table_info{info = Info, records = Records}=EventState} = 
	xmerl_sax_parser:file(FilePath ++ FileName, [{event_fun, fun listen_node_event/3}]),
    EventState#table_info{info = lists:reverse(Info), records = lists:reverse(Records)}.

%% 
%% 监听事件信息
%% 
listen_node_event({startElement, _Uri, "root", _QualifiedName, _Attributes}, _Location, _State) -> 
    #table_info{};
listen_node_event({startElement, _Uri, LocalName, _QualifiedName, _Attributes}, _Location, 
		  #table_info{name=TableName,info=Info,record_temp=RecordTemp,info_complete=InfoComplete}=State) -> 
    case TableName of
	"" -> 
	    State#table_info{name = LocalName};
	LocalName -> 
	    State#table_info{record_temp = [list_to_atom(TableName)]};
	_ ->
	    case InfoComplete of
		false ->
		    case RecordTemp of
			[] -> 
			    State#table_info{info=[list_to_atom(LocalName)|Info], 
					     record_temp = [list_to_atom(TableName)]};
			_ -> 
			    State#table_info{info=[list_to_atom(LocalName)|Info]}
		    end;
		true -> 
		    State
	    end
    end;
listen_node_event({endElement, _Uri, LocalName, _QualifiedName}, _Location, 
		  #table_info{name = TableName, record_temp = RecordTemp, records=Records} = State) -> 
    case TableName of
	LocalName -> 
	    State#table_info{info_complete = true, records = [list_to_tuple(lists:reverse(RecordTemp))|Records]};
	_ -> 
	    State
    end;
listen_node_event({characters, Value}, _Location, #table_info{record_temp = RecordTemp} = State) ->
    case Value of
	"" -> 
	    State#table_info{record_temp = []};
	_ ->
	    State#table_info{record_temp = [Value|RecordTemp]}
    end;
listen_node_event(_Event, _Location, State) ->
    State.
