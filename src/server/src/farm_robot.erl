%% Author: NoteBook
%% Created: 2009-9-11
%% Description: Add description to client
-module(farm_robot).

%%
%% Include files
%%
-include("net_type.hrl").
-include("packet_def.hrl").  
-include("enum_def.hrl").

%%
%% Exported Functions
%%
-export([start/1, start/4]).   
%%
%% API Functions
%%
start(Ip, Port, RoleName, Password) ->
    start([Ip, Port, RoleName, Password]).

start([Ip, Port, RoleName, Password]) ->
    data_helper:format("start player:~p~n", [RoleName]),
    Socket = connect(Ip, list_to_integer(Port)),
    printf("client version:~p~n", [?proto_ver]),
    Data = #req_ver_validate{ver=?proto_ver},
    send_data(Socket, Data),
    loop(Socket, init(), RoleName, Password);
start([Ip, Port, RoleName]) ->
    start([Ip, Port, RoleName, []]).

%%
%% Add description of start/function_arity
%%
connect(Ip, Port) -> 
    %application:start(tcp_server),
    {ok, Socket} = gen_tcp:connect(Ip, Port, [{packet, 4}]),
    Socket.
%%
%% Local Functions
%%

init() ->
    [{?msg_notify_login_result, fun process_notify_login_result/5},
     {?msg_notify_ver_validate, fun process_notify_ver_validate/5},
     {?msg_notify_farm_data, fun process_notify_farm_data/5},
     {?msg_notify_sys_time, fun process_notify_sys_time/5},
     {?msg_notify_friend_farm_status, fun process_notify_friend_farm_status/5},
     {?msg_notify_eq_user_info, fun process_notify_eq_user_info/5},
     {?msg_notify_farm_task_list, fun process_notify_farm_task_list/5},
     {?msg_notify_package, fun process_notify_package/5},
     {?msg_notify_farm_money, fun process_notify_farm_money/5},
     {?msg_notify_sys_msg, fun process_notify_sys_msg/5}
    ].

send_data(Socket, Data) ->
    {Type, Binary} = protocal:encode(Data),
    Msg = net_helper:make_net_binary(Type, Binary),
    gen_tcp:send(Socket, Msg),
    ok.


loop(Socket, List, RoleName, Password) ->
    receive
	{tcp, Socket, Bin} ->
	    Binary = list_to_binary(Bin),
	    {MsgType, MsgData}= net_helper:get_data(Binary),
	    Processor = find_proc(MsgType, List),
	    printf("receive msg:{MsgType:~p, MsgData:~p}~n", [MsgType, MsgData]),
	    Processor(MsgData, Socket, List, RoleName, Password),
	    loop(Socket, List, RoleName, Password);
	{tcp_closed, Socket} ->
	    printf("tcp close~n")
    end.

process_notify_ver_validate({notify_ver_validate, ?ver_ok}, Socket, _List, RoleName, Password) ->
    Data = #req_login{account=RoleName, password=Password},
    send_data(Socket, Data);

process_notify_ver_validate({notify_ver_validate, ?ver_fail}, _Socket, _List, _RoleName, _Password) ->
    printf("Version Error~n").

process_notify_login_result({notify_login_result, ?lr_ok}, Socket, _List, RoleName, _password) ->
    send_data(Socket, #req_enter_farm{owner=RoleName});
    %% send_data(Socket, #req_enter_farm{owner="linyj1@35info.cn"});

process_notify_login_result({notify_login_result, ?lr_fail}, _Socket, _List, _RoleName, _Password) ->
    printf("Account login fail~n").

process_notify_farm_data({notify_farm_data, _Data}, Socket, _List, _RoleName, _password) ->
    Packet = #req_get_friend_farm_state{},
    send_data(Socket, Packet),
    send_data(Socket, #req_farm_complete_task{task_id=20000}).

process_notify_package({notify_package, _, _, _Data}, _Socket, _List, _RoleName, _password) ->
    ok.

process_notify_sys_time({notify_sys_time, _Data}, _Socket, _List, _RoleName, _password) ->
    ok.

process_notify_friend_farm_status({notify_friend_farm_status, _Data}, _Socket, _List, _RoleName, _password) ->
    ok.

process_notify_eq_user_info({notify_eq_user_info, _Account, _Data}, _Socket, _List, _RoleName, _password) ->
    ok.

process_notify_farm_money({notify_farm_money, _, _, _}, _Socket, _List, _RoleName, _password) ->
    ok.

process_notify_sys_msg({notify_sys_msg, _, _}, _Socket, _List, _RoleName, _Password) ->
    ok.

process_notify_farm_task_list({notify_farm_task_list, []}, _Socket, _List, _RoleName, _password) ->
    ok;
process_notify_farm_task_list({notify_farm_task_list, _TaskList}, _Socket, _List, _RoleName, _password) ->
    ok.
    %% [send_data(Socket, #req_farm_complete_task{task_id=TaskId}) || #player_task{task_id=TaskId} <- TaskList].
    
%%--------------------------------------------------------------------------------
%% 通用的网络消息处理函数
common_proc(_Data, _Socket, _List, _RoleName) ->
    ok.

find_proc(MsgType, List) ->
    case lists:keyfind(MsgType, 1, List) of
	{_, Proc} ->
	    Proc;
	_ ->
	    fun common_proc/4
    end.

printf(_Str) ->
    data_helper:format(_Str),
    ok.

printf(_Str, _Params) ->
    data_helper:format(_Str, _Params),
    ok.
