%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% ϵͳ����
%%% @end 
%%% Created :  6 Aug 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(sys_broadcast).

-export([add/8, modify/9, delete/4, search/3]).

add(Req, HostName, Type, Content, DateTime, Count, Priority, IsBroadcast) ->
    NNContent = iolist_to_binary(Content),
    io:format("NNContent:~p, NNNContent:~p~n", [NNContent, unicode:characters_to_list(NNContent)]),
    case IsBroadcast of
	true ->
	    ServerList = game_svr:get_server(),
	    Id = rpc:call(HostName, sys_broadcast, add, [Type, Content, DateTime, Count, Priority]),
	    [rpc:call(HostName1, sys_broadcast, gm_change_broadcast, [Id, Type]) || HostName1 <- ServerList],
	    Req:respond({200, [], ["ok"]});
	false ->
	    Id = rpc:call(HostName, sys_broadcast, add, [Type, NNContent, DateTime, Count, Priority]),
	    Result = rpc:call(HostName, sys_broadcast, gm_change_broadcast, [Id, Type]), 
	    case Result of
		{badrpc, nodedown} ->
		    Req:respond({200, [], ["doesn't exist node!"]});
		{badrpc, Reason} ->
		    Req:respond({200, [], [Reason]});
		{fail, Reason} ->
		    Req:respond({200, [], [Reason]});
		_ ->
		    Req:respond({200, [], ["ok"]})
	    end
end.

modify(Id, Req, HostName, Type, Content, DateTime, Count, Priority, IsBroadcast) ->
    NNContent = iolist_to_binary(Content),
    case IsBroadcast of
	true ->
	    ServerList = game_svr:get_server(),
	    rpc:call(HostName, sys_broadcast, modify, [Id, Type, Content, DateTime, Count, Priority]),
	    [rpc:call(HostName1, sys_broadcast, gm_change_broadcast, [Id, Type]) || HostName1 <- ServerList],
	    Req:respond({200, [], ["ok"]});
	false ->
	    rpc:call(HostName, sys_broadcast, modify, [Id, Type, NNContent, DateTime, Count, Priority]),
	    Result = rpc:call(HostName, sys_broadcast, gm_change_broadcast, [Id, Type]), 
	    case Result of
		{badrpc, nodedown} ->
		    Req:respond({200, [], ["doesn't exist node!"]});
		{badrpc, Reason} ->
		    Req:respond({200, [], [Reason]});
		{fail, Reason} ->
		    Req:respond({200, [], [Reason]});
		_ ->
		    Req:respond({200, [], ["ok"]})
	    end
end.

search(Type, Req, HostName) ->
    SearchResult = rpc:call(HostName, gm_tool, search_sys_broadcast, [Type]),
    case SearchResult of
	[] ->
	    Req:respond({200, [], ["fail"]});
	List ->
	    Req:respond({200, [{"Content-Type", "text/html; charset=gb2312"}], 
			 [create(HostName, List, [])]})
    end.

delete(InstId, Type, Req, HostName) ->
    SearchResult = rpc:call(HostName, gm_tool, delete_sys_broadcast, [InstId, Type]),
    case SearchResult of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["doesn't exist node!"]});
	{badrpc, Reason} ->
	    Req:respond({200, [], [Reason]});
	{fail, Reason} ->
	    Req:respond({200, [], [Reason]});
	_ ->
	    Req:respond({200, [], ["ok"]})
    end.

create(_HostName, [], NList) ->
    NList;
create(HostName, [{STable, SInstId, SType, SContent, SDateTime, SCount, SPriority}|List], NList) ->
    List1 = create(HostName, STable, SInstId, SType, SContent, SDateTime, SCount, SPriority),
    create(HostName, List, List1 ++ NList).

create(_HostName, _STable, SInstId, SType, SContent, SDateTime, SCount, SPriority) ->
    {{Y,M,D}, {H,Mi,S}} = SDateTime, 
    DateTime = integer_to_list(M) ++ "/" ++ integer_to_list(D) ++ "/" ++ integer_to_list(Y) ++
	" " ++ integer_to_list(H) ++ ":" ++ integer_to_list(Mi) ++ ":" ++ integer_to_list(S),
    "<tr id=\"row\" class=\"repeat\" style=\"background-color:#E7E8EB\">" ++ 
	"<td style=\"text-align:center; width:50px\">" ++ integer_to_list(SInstId) ++ "</td>" ++
	"<td style=\"text-align:center; width:50px\">" ++ create_type(SType) ++ "</td>" ++
	"<td style=\"text-align:center; width:300px\">" ++ create_content(SContent) ++ "</td>" ++
	"<td style=\"text-align:center; width:50px\">" ++ integer_to_list(SCount) ++ "</td>" ++
	"<td style=\"text-align:center; width:50px\">" ++ DateTime ++ "</td>" ++
	"<td style=\"text-align:center; width:50px\">" ++ create_priority(SPriority) ++ "</td>" ++
	"<td style=\"text-align:center; width:50px\"><a href=\"#\" onclick=\"show_modify_sys_broadcast('"++ integer_to_list(SInstId) ++"', 
                                                          '"++ integer_to_list(SType) ++"',
                                                          '"++ create_content(SContent) ++"',
                                                          '"++ DateTime ++"',
                                                          '"++ integer_to_list(SCount) ++"',
                                                          '"++ integer_to_list(SPriority) ++"')\">�޸�</a></br>
<a href=\"#\" onclick=\"delete_sys_broadcast('"++ integer_to_list(SInstId) ++"', '"++ integer_to_list(SType) ++"')\">ɾ��</a></td>" ++
    "</tr>".

create_content(Content) ->
    case is_binary(Content) of
	true ->
	    binary_to_list(Content);
	false ->
	    Content
    end.

create_type(1) ->
    "ϵͳ�Զ���ʾ���ŵĹ���";
create_type(2) ->
    "ϵͳ���㲥�ŵĹ���";
create_type(3) ->
    "�����Ϊ����Ĺ���";
create_type(4) ->
    "GM����";
create_type(5) ->    
    "ũ������";
create_type(6) ->
    "ÿ�춨�㹫��".

create_priority(1) ->
    "��";
create_priority(2) ->
    "ƫ��";
create_priority(3) ->
    "��";
create_priority(4) ->
    "��";
create_priority(5) ->    
    "ƫ��".
