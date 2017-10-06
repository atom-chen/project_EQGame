-module(gamesvr_app).

-behaviour(application).

%% Internal API
-export([start_client/0]).

%% Application and Supervisor callbacks
-export([start/0, start/2, stop/1, init/1]).

						%最大启动次数
-define(MAX_RESTART,    5).
-define(MAX_TIME,      3).

start_client() ->
    supervisor:start_child(gamesvr_sup, []).

%%----------------------------------------------------------------------
%% Application behaviour callbacks
%%----------------------------------------------------------------------
start() ->
    start([], []).

start(_Type, _Args) ->
    supervisor:start_link({local, ?MODULE}, ?MODULE, []).

stop(_S) ->
    ok.

%%----------------------------------------------------------------------
%% Supervisor behaviour callbacks
%%----------------------------------------------------------------------
init([]) ->
    {ok,
     {_SupFlags = {one_for_one, ?MAX_RESTART, ?MAX_TIME},
      [{router,                                  % 消息路由器
	{router, start_link,[]},                 % StartFun = {M, F, A}
	permanent,                               % Restart  = permanent | transient | temporary
	2000,                                    % Shutdown = brutal_kill | int() >= 0 | infinity
	worker,                                  % Type     = worker | supervisor
	[router]                                 % Modules  = [Module] | dynamic
       },
       %% {guid,                                    % guid生成器
       %% 	{guid, start_link,[]},                   % StartFun = {M, F, A}
       %% 	permanent,                               % Restart  = permanent | transient | temporary
       %% 	2000,                                    % Shutdown = brutal_kill | int() >= 0 | infinity
       %% 	worker,                                  % Type     = worker | supervisor
       %% 	[guid]                                   % Modules  = [Module] | dynamic
       %% },
       {tplt,                                    % 模板表进程
	{tplt, start_link,[]},                   % StartFun = {M, F, A}
	permanent,                               % Restart  = permanent | transient | temporary
	2000,                                    % Shutdown = brutal_kill | int() >= 0 | infinity
	worker,                                  % Type     = worker | supervisor
	[tplt]                                   % Modules  = [Module] | dynamic
       },
       {common_def,                              % 公用变量进程
	{common_def, start_link,[]},                 % StartFun = {M, F, A}
	permanent,                               % Restart  = permanent | transient | temporary
	2000,                                    % Shutdown = brutal_kill | int() >= 0 | infinity
	worker,                                  % Type     = worker | supervisor
	[common_def]                             % Modules  = [Module] | dynamic
       },
       {scene_sup,	                         % 场景监控进程
	{supervisor,start_link,[{local, scene_sup}, scene_sup, []]},
	permanent,                               % Restart  = permanent | transient | temporary
	infinity,                                % Shutdown = brutal_kill | int() >= 0 | infinity
	supervisor,                              % Type     = worker | supervisor
	[]                                       % Modules  = [Module] | dynamic
       },
       {scene_manager,                           % 场景管理器
	{scene_manager, start_link,[]},          % StartFun = {M, F, A}
	permanent,                               % Restart  = permanent | transient | temporary
	2000,                                    % Shutdown = brutal_kill | int() >= 0 | infinity
	worker,                                  % Type     = worker | supervisor
	[scene_manager]                          % Modules  = [Module] | dynamic
       },
       {player_property,                         % 玩家属性
	{player_property, start_link, []},
	permanent,
	2000,
	worker,
	[player_property]
       },
       {npc_manager,                             % npc管理器
	{npc_manager, start_link, []},
	permanent,
	2000,
	worker,
	[scene_manager]
       },
       {player_level_manager,                    % 玩家等级管理器
	{player_level_manager, start_link, []},
	permanent,
	2000,
	worker,
	[player_level_manager]
       },
       {sys_time,                                % 系统时间
	{sys_time, start_link, []},
	permanent,
	2000,
	worker,
	[sys_time]
       },
       {sys_broadcast,                                    % 系统广播
	{sys_broadcast, start_link,[]},                   % StartFun = {M, F, A}
	permanent,                               % Restart  = permanent | transient | temporary
	2000,                                    % Shutdown = brutal_kill | int() >= 0 | infinity
	worker,                                  % Type     = worker | supervisor
	[sys_broadcast]                                   % Modules  = [Module] | dynamic
       },
       {float_bottle_pool,                                    % 飘流池
	{float_bottle_pool, start_link,[]},                   % StartFun = {M, F, A}
	permanent,                               % Restart  = permanent | transient | temporary
	2000,                                    % Shutdown = brutal_kill | int() >= 0 | infinity
	worker,                                  % Type     = worker | supervisor
	[float_bottle_pool]                                   % Modules  = [Module] | dynamic
       },
       {gm_tool,                                 % GM工具
	{gm_tool, start_link, []},
	permanent,
	2000,
	worker,
	[gm_tool]
       },
       {cupid,                                 % GM工具
	{cupid, start_link, []},
	permanent,
	2000,
	worker,
	[cupid]
       }
      ]
     }
    }.

