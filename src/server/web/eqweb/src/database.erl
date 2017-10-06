%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 数据库管理
%%% @end
%%% Created : 11 Aug 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(database).

-export([list/1, status/2, start/2, close/2, restart/2, node_list/2, add_node/5, add_activity/3]).

-define(filepath, "/root/server/web/eqweb/ebin/database.txt").
-define(filepath1, "/root/server/web/eqweb/ebin/nodes.txt").
-define(maxsize, 10000).
-define(warningsize, 5000).

%%%===================================================================
%%% API
%%%===================================================================
list(Req) ->
    case file:consult(?filepath) of
	{ok, ServerList} ->
	    NList = create(ServerList, [], 1),
	    Req:respond({200, [{"Content-Type", "text/html; charset=gb2312"}], [NList]});
	{error, _Reason} ->
	    Req:respond({200, [], ["fail"]})
    end.

node_list(Req, HostName) ->
    case rpc:call(HostName, mnesia, system_info, [running_db_nodes]) of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["Error"]});
	{badrpc, Reason} ->
	    Req:respond({200, [], [Reason]});
	LiveNodes ->
	    FailNodes = rpc:call(HostName, mnesia_lib, all_nodes, []) -- LiveNodes,
	    Nodes = create_node(LiveNodes, []),
	    NFailNodes = create_fail_node(FailNodes, []),
	    Req:respond({200, [{"Content-Type", "text/html; charset=gb2312"}], [NFailNodes ++ Nodes]})
    end.

add_node(Req, HostName, IP, ServerName, NHostName) ->
    case file:write_file(?filepath1, "\r\n{'"++NHostName++"', \""++ServerName++"\", '"++IP++"'}.", [append]) of
	ok ->
	    case rpc:call(HostName, database, add_db_node, [list_to_atom(NHostName)]) of
		{badrpc, nodedown} ->
		    Req:respond({200, [], ["Error"]});
		{badrpc, _Reason} ->
		    Req:respond({200, [], ["Error"]});
		_ ->
		    Req:respond({200, [], ["ok"]})
	    end;
	{error, Reason} ->
	    Req:respond({200, [], [Reason]})
    end.

add_activity(Req, HostName, TableName) ->
    case rpc:call(HostName, database, add_table_frag, [TableName, HostName]) of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["Error"]});
	{badrpc, _Reason} ->
	    Req:respond({200, [], ["Error"]});
	_ ->
	    Req:respond({200, [], ["ok"]})
    end.

status(Req, HostName) ->
    StatusResult = rpc:call(HostName, application, which_applications, []),
    case StatusResult of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["Error"]});
	{badrpc, Reason} ->
	    Req:respond({200, [], [Reason]});
	Lists ->
	    case is_exist(Lists, mnesia) of
		true ->
		    Req:respond({200, [], ["Started"]});
		_ ->
		    Req:respond({200, [], ["Closed"]})
	    end
    end.

start(Req, HostName) ->
    StartResult = rpc:call(HostName, database, start, []),
    case StartResult of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["doesn't exist node!"]});
	{badrpc, Reason} ->
	    Req:respond({200, [], [Reason]});
	{error, _} ->
	    Req:respond({200, [], ["fail"]});
	_ ->
	    Req:respond({200, [], ["ok"]})
    end.

close(Req, HostName) ->
    CloseResult = rpc:call(HostName, database, stop, []),
    io:format("CloseResult:~p~n", [CloseResult]),
    case CloseResult of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["doesn't exist node!"]});
	{badrpc, 'EXIT'} ->
	    Req:respond({200, [], ["ok"]});
	{badrpc, Reason} ->
	    Req:respond({200, [], [Reason]});
	_ ->
	    Req:respond({200, [], ["ok"]})
    end.

restart(Req, HostName) ->
    RestartResult1 = rpc:call(HostName, database, stop, []),
    case RestartResult1 of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["doesn't exist node!"]});
	{badrpc, Reason1} ->
	    Req:respond({200, [], [Reason1]});
	_ ->
	    RestartResult2 = rpc:call(HostName, database, start, []),
	    case RestartResult2 of
		{badrpc, Reason2} ->
		    Req:respond({200, [], [Reason2]});
		{error, _} ->
		    Req:respond({200, [], ["fail"]});
		_ ->
		    Req:respond({200, [], ["ok"]})
	    end
    end.


%%%===================================================================
%%% Internal functions
%%%===================================================================
create([], NList, Index) ->
    NList ++ "<input type=hidden id=\"total_database\" value="++ integer_to_list(Index - 1) ++" >";
create([{HostName, ServerName, IP}|ServerList], NList, Index) ->
    LHostName = atom_to_list(HostName),
    LIndex = integer_to_list(Index),
    List1 = 
	"<tr id=\"database_row\" ondblclick=\"node_list('"++LHostName++"')\" style=\"background-color: #E7E8EB\">" ++
	    "<td>" ++ atom_to_list(IP) ++ "</td>" ++
	    "<td id=\"hostname_"++LIndex++"\">" ++ LHostName ++ "</td>"++
	    "<td>" ++ ServerName ++ "</td>"++
	    "<td><input type=\"radio\" id=\"start_"++LIndex++"\" name=\"database_"++LIndex++"\" checked=\"checked\" value=\"start\" />
                 <label for=\"start_"++LIndex++"\">开启数据库</label><br />
	         <input type=\"radio\" id=\"close_"++LIndex++"\" name=\"database_"++LIndex++"\" value=\"close\" />"++
	         "<label for=\"close_"++LIndex++"\">关闭数据库</label><br />
                 <input type=\"radio\" id=\"restart_"++LIndex++"\" name=\"database_"++LIndex++"\" value=\"restart\" />"++
	         "<label for=\"restart_"++LIndex++"\">重启数据库</label></td>"++
	    "<td><img id=\"status_"++LIndex++"\" src=\"images/closed.png\" width=\"20px\" height=\"20px\" /></td>"++
	    "<td><input type=\"submit\" name=\"submit\" id=\"submit\" onclick=\"database_submit('"++LHostName++"', "++LIndex++")\"  value=\" 提交 \" /></td>"++
	"</tr>",
    create(ServerList, NList ++ List1, Index + 1).

is_exist([], _Type) ->
    false;
is_exist([{LType, _, _}|Lists], Type) ->
    case LType == Type of
	true ->
	    true;
	_ ->
	    is_exist(Lists, Type)
    end.

create_node([], NList) ->
    NList;
create_node([HostName|NodeList], NList) ->
    List1 = "<tr id=\"node_row\" style=\"background-color: #E7E8EB;height: 28px;\"><td colspan=\"2\">" ++
	"<table width=\"100%\">" ++
	    "<tr style=\"background-color: #333333; height: 28px;\">"++
	        "<td style=\"color: White;\">" ++ atom_to_list(HostName) ++ "</td>" ++
	        "<td style=\"color: White;\">" ++ atom_to_list(HostName) ++ "</td>"++
	        "<td style=\"color: White;\">"++
	            "<a id=\"add_node\" onclick=\"show_add_node('"++atom_to_list(HostName)++"');\" href=\"#\">增加节点</a></br>"++

	        "</td>" ++
	    "</tr>"++ create_activity(HostName)++
	"</table></td></tr>",
    create_node(NodeList, NList ++ List1).

create_activity(Node) ->
    create_activity_1(Node, rpc:call(Node, mnesia, system_info, [tables]), []).

create_activity_1(_Node, [], NList) ->
    NList;
create_activity_1(Node, [TableName|Tables], NList) when TableName /= schema ->
    case rpc:call(Node, mnesia, table_info, [TableName, where_to_commit]) of
	{badrpc, nodedown} ->
	    ok;
	{badrpc, _Reason} ->
	    ok;
	ContainList ->
	    case is_contains(ContainList, Node, TableName) of
		{true, Size} ->
		    List1 = create_activity_2(Node, Size, TableName),
		    create_activity_1(Node, Tables, NList ++ List1);
		_ ->
		    create_activity_1(Node, Tables, NList)
	    end
    end;
create_activity_1(Node, [TableName|Tables], NList) when TableName == schema ->
    create_activity_1(Node, Tables, NList).

create_activity_2(Node, Size, TableName) ->
    case Size >= ?maxsize of
	true ->
	    create_activity_3(TableName, Node, Size, " style=\"color:Red\"");
	_ ->
	    case (Size >= ?warningsize) and (Size < ?maxsize) of
		true ->
		    create_activity_3(TableName, Node, Size, " style=\"color:Blue\"");
		_ ->
		    create_activity_3(TableName, Node, Size, [])
	    end
    end.

create_activity_3(TableName, Node, Size, Style) ->
    "<tr><td>"++atom_to_list(TableName)++"</td><td"++Style++">"++integer_to_list(Size)++"</td><td><a id=\"add_activity\" onclick=\"add_activity('"++atom_to_list(Node)++"', '"++atom_to_list(TableName)++"');\" href=\"#\">增加分片表</a></td></tr>".


is_contains([], _Node, _TableName) ->
    false;
is_contains([{CNode, Type}|Lists], Node, TableName) when Type == disc_only_copies ->
    case CNode == Node of
	true ->
	    Size = rpc:call(Node, dets, info, [TableName, file_size]),
	    {true, Size};
	_ ->
	    is_contains(Lists, Node, TableName)
    end;
is_contains([{CNode, _Type}|Lists], Node, TableName) ->
    case CNode == Node of
	true ->
	    Size = rpc:call(Node, ets, info, [TableName, memory]),
	    {true, Size};
	_ ->
	    is_contains(Lists, Node, TableName)
    end.


create_fail_node([], NNode) ->
    NNode;
create_fail_node([HostName|FailNodes], NNode) ->
    List1 = "<tr id=\"node_row\" style=\"background-color: #E7E8EB;height: 28px;\"><td colspan=\"2\">" ++
	"<table width=\"100%\" style=\"text-align:center;\">" ++
	"<tr style=\"background-color: red; height: 28px;\">"++
	"<td style=\"color: White;\">"++atom_to_list(HostName)++"</td>" ++
	"<td style=\"color: White;\">"++atom_to_list(HostName)++"</td>"++
	"<td style=\"color: White;\">节点出错</td>" ++
	"</tr>"++
	"</table></td></tr>",

    create_fail_node(FailNodes, List1 ++ NNode).
