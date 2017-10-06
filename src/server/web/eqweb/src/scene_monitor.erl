%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%%
%%% @end
%%% Created :  2 Aug 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(scene_monitor).

%% API
-export([get_data/1, get_total_user/1]).

%%%===================================================================
%%% API
%%%===================================================================
get_data(HostName) ->
    Scenes = rpc:call(HostName, scene_manager, get_all_scene, []),
    create(Scenes, [], HostName).

get_total_user(HostName) ->
    rpc:call(HostName, db_player, get_online_players_count, [HostName]).

%%%===================================================================
%%% Internal functions
%%%===================================================================
create([SceneName], NList, HostName) ->
    {PlayerCount, Name} = rpc:call(HostName, gm_tool, get_data, [SceneName]),
    NList ++ atom_to_list(SceneName) ++ "," ++ binary_to_list(Name)  ++ "," ++ integer_to_list(PlayerCount);
create([SceneName|Scenes], NList, HostName) ->
    {PlayerCount, Name} = rpc:call(HostName, gm_tool, get_data, [SceneName]),
    create(Scenes, NList ++ atom_to_list(SceneName) ++ "," ++ binary_to_list(Name)  ++ "," ++ integer_to_list(PlayerCount) ++ "|", HostName).
