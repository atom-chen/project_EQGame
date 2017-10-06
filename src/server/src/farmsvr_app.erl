-module(farmsvr_app).

-behaviour(application).

%% Internal API
-export([start_client/0]).

%% Application and Supervisor callbacks
-export([start/0, start/2, stop/1, init/1]).

						%最大启动次数
-define(MAX_RESTART,    5).
-define(MAX_TIME,      3).

start_client() ->
    supervisor:start_child(farmsvr_sup, []).

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
      [
       {guid,                                    % guid生成器
	{guid, start_link,[]},                   % StartFun = {M, F, A}
	permanent,                               % Restart  = permanent | transient | temporary
	2000,                                    % Shutdown = brutal_kill | int() >= 0 | infinity
	worker,                                  % Type     = worker | supervisor
	[guid]                                   % Modules  = [Module] | dynamic
       },
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
       {sys_broadcast,                                    % 系统广播
	{sys_broadcast, start_link,[]},                   % StartFun = {M, F, A}
	permanent,                               % Restart  = permanent | transient | temporary
	2000,                                    % Shutdown = brutal_kill | int() >= 0 | infinity
	worker,                                  % Type     = worker | supervisor
	[sys_broadcast]                                   % Modules  = [Module] | dynamic
       },
       {gm_tool,                                 % GM工具
	{gm_tool, start_link, []},
	permanent,
	2000,
	worker,
	[gm_tool]
       }
      ]
     }
    }.

