%% Author: NoteBook
%% Created: 2009-9-11
%% Description: Add description to client
-module(robot).

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
     {?msg_notify_create_role_result, fun process_notify_create_role_result/5},
     {?msg_notify_other_player_data, fun process_notify_other_player_data/5},
     {?msg_notify_player_data, fun process_notify_player_data/5},
     {?msg_notify_ver_validate, fun process_notify_ver_validate/5},
     {?msg_notify_get_roles_result, fun process_notify_get_roles_result/5},
     {?msg_notify_enter_scene_result, fun process_notify_enter_scene_result/5},
     {?msg_notify_player_leave_scene, fun process_notify_player_leave_scene/5},
     {?msg_notify_house_data, fun common_proc/5},
     {?msg_notify_package, fun process_notify_player_pack/5},
     {?msg_notify_npc_list, fun process_notify_npc_list/5},
     {?msg_notify_use_furniture_result, fun process_notify_use_furniture_result/5},
     {?msg_notify_task_list, fun process_notify_task_list/5},
     {?msg_notify_start_gift_service, fun process_notify_start_gift_service/5},
     {?msg_notify_gift_box, fun process_notify_gift_box/5},
     {?msg_notify_pack_gift_box, fun process_notify_pack_gift_box/5},
     {?msg_notify_put_gift_box, fun process_notify_put_gift_box/5}
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
	    printf("receive msg:{MsgType:~p, MsgData:~p}~n", [MsgType, MsgData]),
	    Processor = find_proc(MsgType, List),
	    Processor(MsgData, Socket, List, RoleName, Password),
	    loop(Socket, List, RoleName, Password);
	{tcp_closed, Socket} ->
	    printf("tcp close~n")
    end.

%%--------------------------鐧诲綍娴佺▼----------------------------------------------
process_notify_ver_validate({notify_ver_validate, ?ver_ok}, Socket, _List, RoleName, Password) ->
    Data = #req_login{account=RoleName, password=Password},
    send_data(Socket, Data);

process_notify_ver_validate({notify_ver_validate, ?ver_fail}, _Socket, _List, _RoleName, _Password) ->
    printf("Version Error~n").

process_notify_login_result({notify_login_result, ?lr_ok}, Socket, _List, _RoleName, _Password) ->
    send_data(Socket, #req_get_roles{});

process_notify_login_result({notify_login_result, ?lr_fail}, _Socket, _List, _RoleName, _Password) ->
    printf("Account login fail~n").

process_notify_get_roles_result({notify_get_roles_result, Roles, _NickName}, Socket, _List, RoleName, _Password) ->
    case Roles of
        [] ->
            PlayerBasicData = #player_basic_data{username=RoleName, 
						 sex=1,
						 skin_color=1, 
						 hair=1, 
						 face=1, 
						 beard=1},
	    Equip = #item{},
	    Equips = [Equip, Equip, Equip, Equip, Equip, Equip, Equip, Equip],
            Data = #req_create_role{basic_data=PlayerBasicData,equips=Equips},
            send_data(Socket, Data);
        _ ->
            Data = #req_enter_scene{},
            send_data(Socket, Data)
    end.

process_notify_create_role_result({notify_create_role_result, Result}, Socket, _List, _RoleName, _Password) ->
    case Result of
        ?rr_ok ->
            Data = #req_enter_scene{},
            send_data(Socket, Data);
        ?rr_fail ->
            printf("create role fail~n")
    end.

process_notify_enter_scene_result({notify_enter_scene_result, Result}, _Socket, _List, _RoleName, _Password) ->
    case Result of
        ?sr_ok ->
            printf("enter scene success~n"),
	    %% process_req_modify_furniture_permission(Socket),
	    %% process_req_use_furniture(Socket);
	    %%Data = #req_enter_player_house{account=test1},
	    %% Data = #req_enter_common_scene{scene_id = 1},
	    %% send_data(Socket, Data),
	    %% send_data(Socket, #req_player_basic_data{}),
	    %% send_data(Socket, #req_sys_time{}),
	    %% process_req_task_list(Socket),
	    %% send_data(Socket, #req_start_gift_service{type=?gst_pack}),
	    ok;
        ?sr_fail ->
            printf("enter scene fail~n")
    end.

process_notify_start_gift_service({notify_start_gift_service, _Type}, Socket, _List, RoleName, _Password) ->
    GiftExpress = #gift_express{sender=RoleName, receiver=RoleName, comment="Test2", card_id=10000},
    send_data(Socket, #req_send_gift_express{gift=GiftExpress, index1=-1, index2=-1}).
    %% GiftBox = #gift_box{name="Test", box_id=10004, card_id=0, comment="Test2", is_anonymous=1},
    %% send_data(Socket, #req_pack_gift_box{index1=-1, index2=-1, gift=GiftBox}).

process_notify_pack_gift_box({notify_pack_gift_box, #gift_box{inst_id=InstId}=_Gift}, Socket, _List, _RoleName, _Password) ->
    send_data(Socket, #req_put_gift_box{inst_id=InstId, pos=#point{x=0, y=0, z=0}}).

process_notify_put_gift_box({notify_put_gift_box}, _Socket, _List, _RoleName, _Password) ->
    ok.

process_notify_gift_box({notify_gift_box, _Gift}, _Socket, _List, _RoleName, _Password) ->
    ok.
%%--------------------------------------------------------------------------------

process_notify_player_leave_scene({notify_player_leave_scene, Result}, _Socket, _List, _RoleName, _Password) ->
    printf("player leave scene~p~n", [Result]).

process_notify_other_player_data(Data, _Socket, _List, _RoleName, _Password) ->
    printf("Data:~p~n", [Data]).


process_notify_player_data(_Data, _Socket, _List, _RoleName, _Password) ->
    %send_data(Socket, #req_gm_command{type=?gct_add_item, params=["1"]}),
    %send_data(Socket, #req_swap_item{index1=1,index2=2}).
    ok.

process_notify_player_pack(_Data, _Socket, _List, _RoleName, _Password) ->
    ok.

process_notify_npc_list({notify_npc_list, [#npc_map_mapping_info{npc_key=_NpcKey}|_NpcMapMappings]}, _Socket, _List, _RoleName, _Password) ->
    ok.
    %% Data = #req_npc_command{npc_key=NpcKey, unique_id=1, page_index=1, command_index=1},
    %% send_data(Socket, Data).

%% process_req_use_furniture(Socket) ->
%%     Data = #req_use_furniture{instance_id=72145459250266116, function_id=33, account=test1},
%%     send_data(Socket, Data).

process_notify_use_furniture_result({notify_use_furniture_result, _Account, PositionIndex, _FurnitureId, _FunctionId}, Socket, _List, _RoleName, _Password) ->
    case PositionIndex > 0 of
	true ->
	    Data = #req_stop_using_furniture{account=test1},
	    send_data(Socket, Data);
	_ ->
	    ok
    end.

%% process_req_modify_furniture_permission(Socket) ->
%%     Data = #req_modify_furniture_permission{furniture_permission=1},
%%     send_data(Socket, Data).

%% process_req_task_list(Socket) ->
%%     Data = #req_task_list{},
%%     send_data(Socket, Data).

process_notify_task_list({notify_task_list, []}, _Socket, _List, _RoleName, _Password) ->
    ok;
process_notify_task_list({notify_task_list, [#player_task{id=Id}|_PlayerTaskList]}, Socket, _List, _RoleName, _Password) ->
    Data = #req_track_task{id=Id},
    send_data(Socket, Data),
    send_data(Socket, #req_get_track_list{}),
    send_data(Socket, #req_cancel_track_task{id=Id}),
    send_data(Socket, #req_give_up_task{id=Id}),
    ok.

%% 通用的网络消息处理函数
common_proc(_Data, _Socket, _List, _RoleName, _Password) ->
    ok.

find_proc(MsgType, List) ->
    case lists:keyfind(MsgType, 1, List) of
	{_, Proc} ->
	    Proc;
	_ ->
	    fun common_proc/5
    end.

printf(_Str) ->
    data_helper:format(_Str),
    ok.

printf(_Str, _Params) ->
    data_helper:format(_Str, _Params),
    ok.

%% %% TODO: 获得一条行走路径
%% get_path(Pos)->
%%     [].
