%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 监控npc进程
%%% @end
%%% Created : 11 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(npc_sup).

-behaviour(supervisor).

%% API
-export([start/2, get_sup_name/2]).
-export([start_link/1]).
-export([start_children/5]).

%% Supervisor callbacks
-export([init/1]).

-define(SERVER, ?MODULE).

%%%===================================================================
%%% API functions
%%%===================================================================
-spec start(integer(), integer()) -> any().
start(SceneId, CopyId) ->
    NpcSupName = get_sup_name(SceneId, CopyId),
    ChildSpec = {
      npc_sup, {?MODULE, start_link, [NpcSupName]},
      temporary, infinity, supervisor, []
     },
    supervisor:start_child(common_scene_sup:get_sup_name(SceneId, CopyId), ChildSpec).

%% 获取npc监控名称
-spec get_sup_name(integer(), integer()) -> atom().
get_sup_name(SceneId, CopyId) ->
     list_to_atom("scene_" ++ integer_to_list(SceneId) ++ "_" ++ integer_to_list(CopyId) ++ "_npc_sup").

%%--------------------------------------------------------------------
%% @doc
%% Starts the supervisor
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link(NpcSupName) ->
    supervisor:start_link({local, NpcSupName}, ?MODULE, []).

%%%===================================================================
%%% Supervisor callbacks
%%%===================================================================

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Whenever a supervisor is started using supervisor:start_link/[2,3],
%% this function is called by the new process to find out about
%% restart strategy, maximum restart frequency and child
%% specifications.
%%
%% @spec init(Args) -> {ok, {SupFlags, [ChildSpec]}} |
%%                     ignore |
%%                     {error, Reason}
%% @end
%%--------------------------------------------------------------------
init([]) ->
    process_flag(trap_exit, true),
    {ok,{{one_for_one, 1, 60}, []}}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
start_children(SceneId, CopyId, NpcKey, NpcMapMappingData, Mod) ->
    NpcSupName = get_sup_name(SceneId, CopyId),
    supervisor:start_child(NpcSupName, {NpcKey, 
				       {gen_npc, start_link, [NpcKey, NpcMapMappingData, Mod]},
				       permanent, 2000, worker, []}).
