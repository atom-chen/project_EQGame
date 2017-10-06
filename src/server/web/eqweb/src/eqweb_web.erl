%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 
%%% @end 
%%% Created :  6 Aug 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(eqweb_web).
-author('author <author@example.com>').

-export([start/1, stop/0, loop/2]).

%% External API

start(Options) ->
    {DocRoot, Options1} = get_option(docroot, Options),
    Loop = fun (Req) ->
                   ?MODULE:loop(Req, DocRoot)
           end,
    mochiweb_http:start([{name, ?MODULE}, {loop, Loop} | Options1]),
    guid:start_link(9999).

stop() ->
    mochiweb_http:stop(?MODULE).

loop(Req, DocRoot) ->
    "/" ++ Path = Req:get(path),
    case Req:get(method) of
        Method when Method =:= 'GET'; Method =:= 'HEAD' ->
            case Path of
		"server_list" ->
		    server:list(Req);
		"start_server/" ++ HostName ->
		    server:start(Req, list_to_atom(HostName));
		"close_server/" ++ HostName ->
		    server:close(Req, list_to_atom(HostName));
		"restart_server/" ++ HostName ->
		    server:restart(Req, list_to_atom(HostName));
		"server_status/" ++ HostName ->
		    server:status(Req, list_to_atom(HostName));
		"template_update/" ++ HostName ->
		    Ret = rpc:call(list_to_atom(HostName), os, cmd, ["svn up /root/server/ebin/template --username linyb --password 821109"]),
		    case Ret of
			{badrpc, nodedown} ->
			    Req:respond({200, [], ["don't exist node!"]});
			_ ->
			    Req:respond({200, [], [Ret]})
		    end;
		"scene_monitor/" ++ HostName ->
		    SceneData = scene_monitor:get_data(list_to_atom(HostName)),
		    Req:respond({200, [], [lists:flatten(SceneData)]});
		"total_user/" ++ HostName ->
		    Users = scene_monitor:get_total_user(list_to_atom(HostName)),
		    Req:respond({200, [{"Content-Type", "text/html; charset=utf-8"}], [integer_to_list(Users)]});
		"house_monitor" ->
		    ok;
		"search_player" ->
		    List = Req:parse_qs(),
		    Account = get_param(List, "account"),
		    HostName = get_param(List, "hostname"),
		    player:search(Account, Req, list_to_atom(HostName)); 
		"search_farm_player" ->
		    List = Req:parse_qs(),
		    Account = get_param(List, "account"),
		    HostName = get_param(List, "hostname"),
		    farm:search(Account, Req, list_to_atom(HostName)); 
		"server_add" ->
		    List = Req:parse_qs(),
		    IP = get_param(List, "ip"),
		    HostName = get_param(List, "hostname"),
		    ServerName = get_param(List, "servername"),
		    server:add(Req, IP, HostName, ServerName);
		"server_delete" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    server:delete(Req, list_to_atom(HostName));
		"database_list" ->
		    database:list(Req);
		"database_node_list/" ++ HostName ->
		    database:node_list(Req, list_to_atom(HostName));
		"database_status/" ++ HostName ->
		    database:status(Req, list_to_atom(HostName));
		"start_database/" ++ HostName ->
		    database:start(Req, list_to_atom(HostName));
		"close_database/" ++ HostName ->
		    database:close(Req, list_to_atom(HostName));
		"restart_database/" ++ HostName ->
		    database:restart(Req, list_to_atom(HostName));
		"database_add_node" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    NHostName = get_param(List, "nhostname"),
		    ServerName = get_param(List, "servername"),
		    IP = get_param(List, "ip"),
		    database:add_node(Req, list_to_atom(HostName), IP, ServerName, NHostName);
		"database_add_activity" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    TableName = get_param(List, "tablename"),
		    database:add_activity(Req, list_to_atom(HostName), list_to_atom(TableName));
		"player_add_item" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    ItemId= get_param(List, "item_id"),
		    Account = get_param(List, "account"),
		    player:add_item(list_to_atom(Account), Req, list_to_atom(HostName), list_to_integer(ItemId));
		"player_add_coin" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    Coin = get_param(List, "coin"),
		    Account = get_param(List, "account"),
		    player:add_coin(list_to_atom(Account), Req, list_to_atom(HostName), list_to_integer(Coin));
		"player_add_farm_exp" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    Account = get_param(List, "account"),
		    Exp = get_param(List, "farm_exp"),
		    player:add_farm_exp(list_to_atom(Account), Req, list_to_atom(HostName), list_to_integer(Exp));
		"game_svr_list" ->
		    game_svr:list(Req);
		"game_svr_status/" ++ HostName ->
		    game_svr:status(Req, list_to_atom(HostName));
		"start_game_svr/" ++ HostName ->
		    game_svr:start(Req, list_to_atom(HostName));
		"close_game_svr/" ++ HostName ->
		    game_svr:close(Req, list_to_atom(HostName));
		"restart_game_svr/" ++ HostName ->
		    game_svr:restart(Req, list_to_atom(HostName));
		"game_svr_add" ->
		    List = Req:parse_qs(),
		    IP = get_param(List, "ip"),
		    HostName = get_param(List, "hostname"),
		    ServerName = get_param(List, "servername"),
		    game_svr:add(Req, IP, HostName, ServerName);
		"game_svr_delete" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    game_svr:delete(Req, list_to_atom(HostName));
		"farm_svr_list" ->
		    farm_svr:list(Req);
		"farm_svr_status/" ++ HostName ->
		    farm_svr:status(Req, list_to_atom(HostName));
		"farm_svr_time/" ++ HostName ->
		    farm_svr:get_time(Req, list_to_atom(HostName));
		"start_farm_svr/" ++ HostName ->
		    farm_svr:start(Req, list_to_atom(HostName));
		"close_farm_svr/" ++ HostName ->
		    farm_svr:close(Req, list_to_atom(HostName));
		"restart_farm_svr/" ++ HostName ->
		    farm_svr:restart(Req, list_to_atom(HostName));
		"farm_svr_add" ->
		    List = Req:parse_qs(),
		    IP = get_param(List, "ip"),
		    HostName = get_param(List, "hostname"),
		    ServerName = get_param(List, "servername"),
		    farm_svr:add(Req, IP, HostName, ServerName);
		"farm_svr_delete" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    farm_svr:delete(Req, list_to_atom(HostName));
		"player_add_property" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    Account = get_param(List, "account"),
		    PlayerClean = get_param(List, "player_clean"),
		    PlayerHealth = get_param(List, "player_health"),
		    PlayerCharm = get_param(List, "player_charm"),
		    ActiveValue = get_param(List, "active_value"),
		    HouseClean = get_param(List, "house_clean"),
		    player:add_property(Req, list_to_atom(HostName), list_to_atom(Account), list_to_integer(PlayerClean), list_to_integer(PlayerHealth), list_to_integer(PlayerCharm), list_to_integer(ActiveValue), list_to_integer(HouseClean));
		"player_add_sys_broadcast" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    Type = list_to_integer(get_param(List, "type")),
		    Content = get_param(List, "content"),
		    Y = list_to_integer(get_param(List, "y")),
		    M = list_to_integer(get_param(List, "m")),
		    D = list_to_integer(get_param(List, "d")),
		    H = list_to_integer(get_param(List, "h")),
		    Mi = list_to_integer(get_param(List, "mi")),
		    S = list_to_integer(get_param(List, "s")),
		    IsBroadcast = list_to_atom(get_param(List, "is_broadcast")),
		    DateTime ={{Y,M,D}, {H,Mi,S}},
		    Count= list_to_integer(get_param(List, "count")),
		    Priority = list_to_integer(get_param(List, "priority")),
		    sys_broadcast:add(Req, list_to_atom(HostName), Type, Content, DateTime, Count, Priority, IsBroadcast);
		"player_modify_sys_broadcast" ->
		    List = Req:parse_qs(),
		    HostName = get_param(List, "hostname"),
		    InstId = list_to_integer(get_param(List, "instid")),
		    Type = list_to_integer(get_param(List, "type")),
		    Content = get_param(List, "content"),
		    Y = list_to_integer(get_param(List, "y")),
		    M = list_to_integer(get_param(List, "m")),
		    D = list_to_integer(get_param(List, "d")),
		    H = list_to_integer(get_param(List, "h")),
		    Mi = list_to_integer(get_param(List, "mi")),
		    S = list_to_integer(get_param(List, "s")),
		    IsBroadcast = list_to_atom(get_param(List, "is_broadcast")),
		    DateTime ={{Y,M,D}, {H,Mi,S}},
		    Count= list_to_integer(get_param(List, "count")),
		    Priority = list_to_integer(get_param(List, "priority")),
		    sys_broadcast:modify(InstId, Req, list_to_atom(HostName), Type, Content, DateTime, Count, Priority, IsBroadcast);
		"search_sys_broadcast" ->
		    List = Req:parse_qs(),
		    Type = list_to_integer(get_param(List, "type")),
		    HostName = list_to_atom(get_param(List, "hostname")),
		    sys_broadcast:search(Type, Req, HostName);
		"game_info_gather" ->
		    List = Req:parse_qs(),
		    HostName = list_to_atom(get_param(List, "hostname")),
		    game_info_gather:run(Req, HostName);
		"get_gm_bottle_count" ->
		    List = Req:parse_qs(),
		    HostName = list_to_atom(get_param(List, "hostname")),
		    gm_float_pool:get_gm_bottle_count(Req, HostName);
		"clear_all_gm_bottle" ->
		    List = Req:parse_qs(),
		    HostName = list_to_atom(get_param(List, "hostname")),
		    gm_float_pool:clear_all_gm_bottle(Req, HostName);
		"add_gm_bottle" ->
		    List = Req:parse_qs(),
		    HostName = list_to_atom(get_param(List, "hostname")),
		    BottleCount = list_to_integer(get_param(List, "bottle_count")),
		    GMName = get_param(List, "gm_name"),
		    Msg = get_param(List, "msg"),
		    RewardID = list_to_integer(get_param(List, "reward_id")),
		    gm_float_pool:add_gm_bottle(Req, HostName, BottleCount, GMName, Msg, RewardID);
		"delete_sys_broadcast" ->
		    List = Req:parse_qs(),
		    InstId = list_to_integer(get_param(List, "instid")),
		    Type = list_to_integer(get_param(List, "type")),
		    HostName = list_to_atom(get_param(List, "hostname")),
		    sys_broadcast:delete(InstId, Type, Req, HostName);
		"edit_gm_level"->
		    List = Req:parse_qs(),
		    Level = list_to_atom(get_param(List, "level")),
		    Account = list_to_atom(get_param(List, "account")),
		    HostName = list_to_atom(get_param(List, "hostname")),
		    Result = player:edit_gm_level(HostName,Account, case Level of enable -> 1; _->2 end),
		    Req:respond({200, [], [atom_to_list(Result)]});
                _ ->
                    Req:serve_file(Path, DocRoot)
            end;
        'POST' ->
            case Path of
		"upload" -> uploads:do(Req, DocRoot);
                _ -> Req:not_found()
            end;
        _ ->
            Req:respond({501, [], []})
    end.

%% Internal API

get_option(Option, Options) ->
    {proplists:get_value(Option, Options), proplists:delete(Option, Options)}.


get_param([], _Param) ->
    [];
get_param([{Key, Value}|List], Param) ->
    case Key == Param of
	true ->
	    Value;
	_ ->
	    get_param(List, Param)
    end.

%%
%% Tests
%%
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).
-endif.
