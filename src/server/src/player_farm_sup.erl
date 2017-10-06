%%% @author lyj <lyj@35info.cn>
%%% @copyright (C) 2010, lyj
%%% @doc
%%%
%%% @end
%%% Created : 12 Nov 2010 by lyj <lyj@35info.cn>

-module(player_farm_sup).  

-export([start/4, stop/1, stop/2]).

-define(SUP, tcp_server_app).

start(Account, MMPid, EQPid, IsWallowAccount) ->
    ChildSpec = {get_sup_name(Account), {farm, start_link, [Account, MMPid, EQPid, IsWallowAccount]},
      transient, 2000, worker, [farm]},
    supervisor:start_child(?SUP, ChildSpec).
    
stop(Node, Account) ->
    rpc:call(Node, ?MODULE, stop, [Account]).

stop(Account) ->
    Name = get_sup_name(Account),
    ok = supervisor:terminate_child(?SUP, Name),
    supervisor:delete_child(?SUP, Name).
 
get_sup_name(Account) ->
     list_to_atom(atom_to_list(Account) ++ "_farm_sup").
