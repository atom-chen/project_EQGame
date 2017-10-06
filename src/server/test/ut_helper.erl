 %% 单元测试的帮助模块
%% 该模块主要是防止一些经常使用的功能, 方便写测试代码

-module(ut_helper).
-author('schuett@zib.de').
-vsn('$Id$ ').

-include("packet_def.hrl").
-include("enum_def.hrl").
 
-export([start/1, send_data/2, add_item/2]).


%% 启动一个玩家的测试进程, 该进程在玩家登陆服务器后, 
-spec start(list()) -> port().
start(Account) when is_list(Account)->
    login(Account).

-spec send_data(port(), any())-> any().
send_data(Socket, Data) when is_port(Socket)->
    {Type, Binary} = protocal:encode(Data),
    Msg = net_helper:make_net_binary(Type, Binary),
    gen_tcp:send(Socket, Msg).

%% 向服务端请求增加物品
-spec add_item(port(), pos_integer())->any().	      
add_item(Socket, ItemTempID) ->
    send_data(Socket, 
	      #req_gm_command{type=?gct_add_item, params=[integer_to_list(ItemTempID)]}).

%%%===================================================================
%%% Internal functions
%%%===================================================================

%% 玩家登陆进入游戏
%% 返回玩家对应的socket    
login(Account)  ->
    login(Account, 1000).

login(Account, Timeout)->
    {ok, Socket} = gen_tcp:connect({127,0,0,1}, 3333, [{packet, 2}]),
    send_data(Socket, #req_ver_validate{ver=?proto_ver}),
    loop(Socket, init(), Account, Timeout),
    Socket.

init() ->
    [{?msg_notify_login_result, fun process_notify_login_result/4},
     {?msg_notify_create_role_result, fun process_notify_create_role_result/4},
     {?msg_notify_ver_validate, fun process_notify_ver_validate/4},
     {?msg_notify_get_roles_result, fun process_notify_get_roles_result/4},
     {?msg_notify_enter_scene_result, fun process_notify_enter_scene_result/4},
     {?msg_notify_house_data, fun common_proc/4}
    ].


loop(Socket, List, Account, Timeout) ->
    receive
	{tcp, Socket, Bin} ->
	    Binary = list_to_binary(Bin),
	    {MsgType, MsgData}= net_helper:get_data(Binary),
	    Processor = find_proc(MsgType, List),
	    Processor(MsgData, Socket, List, Account),
	    loop(Socket, List, Account, Timeout);
	{send_data, Data} -> send_data(Socket, Data);
	{close} -> io:format("client close~n"),close;
	{tcp_closed, Socket} -> tcp_close
    after Timeout ->
	ok
    end.

process_notify_ver_validate({notify_ver_validate, ?ver_ok}, Socket, _List, Account) ->
    Data = #req_login{account=Account},
    send_data(Socket, Data).

process_notify_login_result({notify_login_result, ?lr_ok}, Socket, _List, _Account) ->
    send_data(Socket, #req_get_roles{}).

process_notify_get_roles_result({notify_get_roles_result, Roles}, Socket, _List, Account) ->
    case Roles of
        [] ->
            PlayerBasicData = #player_basic_data{username=Account, 
						 sex=1,
						 skin_color=1, 
						 hair=1, 
						 face=1, 
						 beard=1},
	    Equip = #item{},
	    Equips = [Equip, Equip, Equip, Equip, Equip, Equip, Equip, Equip],
            send_data(Socket, #req_create_role{basic_data=PlayerBasicData,equips=Equips});
        _ ->
            send_data(Socket, #req_enter_scene{})
    end.

process_notify_enter_scene_result(_Data, _Socket, _List, _Account)->
    ok.

process_notify_create_role_result({notify_create_role_result, ?rr_ok}, Socket, _List, _Account) ->
    send_data(Socket, #req_enter_scene{}).

%% 通用的网络消息处理函数
common_proc(_Data, _Socket, _List, _Account) ->
    ok.

find_proc(MsgType, List) ->
    case lists:keyfind(MsgType, 1, List) of
	{_, Proc} ->
	    Proc;
	_ ->
	    fun common_proc/4
    end.

