-module(tcp_server_app).
-author('saleyn@gmail.com').

-behaviour(application).

%% Internal API
-export([start_client/0]).

%% Application and Supervisor callbacks
-export([start/0, start/2, stop/1, init/1]).

%最大启动次数
-define(MAX_RESTART,    5).
-define(MAX_TIME,      3).
-define(DEF_PORT,    2222).

%% A startup function for spawning new client connection handling FSM.
%% To be called by the TCP listener process.
start_client() ->
    supervisor:start_child(tcp_client_sup, []).

%%----------------------------------------------------------------------
%% Application behaviour callbacks
%%----------------------------------------------------------------------
start() ->
    start([], []).

start(_Type, _Args) ->
    ListenPort = get_app_env(listen_port, ?DEF_PORT),
    data_helper:format("listen port:~p~n", [ListenPort]),
    supervisor:start_link({local, ?MODULE}, ?MODULE, 
			  [ListenPort, get_app_env(callback, tcp_mm)]).

stop(_S) ->
    ok.

%%----------------------------------------------------------------------
%% Supervisor behaviour callbacks
%%----------------------------------------------------------------------
init([Port, Module]) ->
    {ok,
     {_SupFlags = {one_for_one, ?MAX_RESTART, ?MAX_TIME},
      [{tcp_server_sup,                         % TCP Listener
	{tcp_listener,start_link,[Port,Module]}, % StartFun = {M, F, A}
	permanent,                               % Restart  = permanent | transient | temporary
	2000,                                    % Shutdown = brutal_kill | int() >= 0 | infinity
	worker,                                  % Type     = worker | supervisor
	[tcp_listener]                           % Modules  = [Module] | dynamic
       },
       {tcp_client_sup,	                         % Client instance supervisor
	{supervisor,start_link,[{local, tcp_client_sup}, ?MODULE, [Module]]},
	permanent,                               % Restart  = permanent | transient | temporary
	infinity,                                % Shutdown = brutal_kill | int() >= 0 | infinity
	supervisor,                              % Type     = worker | supervisor
	[]                                       % Modules  = [Module] | dynamic
       }
      ]
     }
    };

init([Module]) ->
    {ok,
        {_SupFlags = {simple_one_for_one, ?MAX_RESTART, ?MAX_TIME},
            [
              % TCP Client
              {   undefined,                 % Id       = internal id
                  {Module,start_link,[]},    % StartFun = {M, F, A}
                  temporary,                 % Restart  = permanent | transient | temporary
                  2000,                      % Shutdown = brutal_kill | int() >= 0 | infinity
                  worker,                    % Type     = worker | supervisor
                  []                         % Modules  = [Module] | dynamic
              }
            ]
        }
    }.

%%----------------------------------------------------------------------
%% Internal functions
%%----------------------------------------------------------------------
get_app_env(Opt, Default) ->
    case application:get_env(Opt) of
	{ok, Val} -> Val;
	_ ->
	    case init:get_argument(Opt) of
		{ok, [[Value]]} -> Value;
		error         -> Default
	    end
    end.

