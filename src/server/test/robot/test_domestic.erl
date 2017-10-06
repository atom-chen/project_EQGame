-module(test_domestic).

%%
%% Include files
%%
-include("net_type.hrl").
-include("packet_def.hrl").  
-include("enum_def.hrl").
-include("common_def.hrl").

-record(state, {bag, money,account,trade_player,recver}).
%%
%% Exported Functions
%%
-export([start/1, start/0]).   
%%
%% API Functions
%%
start() ->
    player_friends:test(),
    start(["127.0.0.1", "2222", "domestic1"]).

start([Ip, Port, RoleName]) ->
    io:format("启动玩家:~p~n", [RoleName]),
    Socket = connect(Ip, list_to_integer(Port)),
    Data = #req_ver_validate{ver=?proto_ver},
    send_data(Socket, Data),
    loop(Socket, init(), #state{account=RoleName}).

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
    [{?msg_notify_login_result, fun process_notify_login_result/4},
     {?msg_notify_create_role_result, fun process_notify_create_role_result/4},
     {?msg_notify_ver_validate, fun process_notify_ver_validate/4},
     {?msg_notify_get_roles_result, fun process_notify_get_roles_result/4},
     {?msg_notify_enter_scene_result, fun process_notify_enter_scene_result/4},
     {}
    ].

send_data(Socket, Data) ->
    {Type, Binary} = protocal:encode(Data),
    Msg = net_helper:make_net_binary(Type, Binary),
    gen_tcp:send(Socket, Msg),
    ok.


loop(Socket, List, State) ->
    receive
	{tcp, Socket, Bin} ->
	    Binary = list_to_binary(Bin),
	    {MsgType, MsgData}= net_helper:get_data(Binary),
	    show_data(MsgType, MsgData),
	    Processor = find_proc(MsgType, List),
	    NewState = Processor(MsgData, Socket, List, State),
	    loop(Socket, List, NewState);
	{tcp_closed, Socket} ->
	    printf("tcp close~n")
    end.

%%--------------------------鐧诲綍娴佺▼----------------------------------------------
process_notify_ver_validate({notify_ver_validate, ?ver_ok}, Socket, _List,
			    #state{account=RoleName}=State) ->
    Data = #req_login{account=RoleName},
    send_data(Socket, Data),
    State;

process_notify_ver_validate({notify_ver_validate, ?ver_fail}, _Socket, _List, State) ->
    printf("Version Error~n"),
    State.

process_notify_login_result({notify_login_result, ?lr_ok}, Socket, _List, State) ->
    send_data(Socket, #req_get_roles{}),
    State;

process_notify_login_result({notify_login_result, ?lr_fail}, _Socket, _List, State) ->
    printf("Account login fail~n"),
    State.

process_notify_get_roles_result(#notify_get_roles_result{player=Roles}, Socket, _List, 
				#state{account=RoleName}=State) ->
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
    end,
    State.

process_notify_create_role_result({notify_create_role_result, Result}, Socket, _List, 
				  State) ->
    case Result of
        ?rr_ok ->
            Data = #req_enter_scene{},
            send_data(Socket, Data),
	    ok;
        ?rr_fail ->
            printf("create role fail~n")
    end,
    State.

%%--------------------------------------------------------------------------------
process_notify_enter_scene_result(_Msg, Socket, _List, 
				  #state{}=State) ->
    timer:sleep(100),
    io:format("加钱\n"),
    send_data(Socket, #req_gm_command{type=?gct_get_money, 
				      params=[integer_to_list(?GAMECOIN), "10000"]}), 
    
    timer:sleep(100),
    send_data(Socket, #req_start_domestic{}),

    timer:sleep(100),
    io:format("请求家政服务\n"),
    ID = 1,
    send_data(Socket, #req_domestic_service{id=ID}),
    State.

    

%% 通用的网络消息处理函数
common_proc(_Data, _Socket, _List, State) ->
    State.

find_proc(MsgType, List) ->
    case lists:keyfind(MsgType, 1, List) of
	{_, Proc} ->
	    Proc;
	_ ->
	    fun common_proc/4
    end.

printf(_Str) ->
    io:format(_Str),
    ok.

printf(_Str, _Params) ->
    io:format(_Str, _Params),
    ok.

show_data(?msg_notify_house_data=MsgType, _Data) ->
    printf("receive msg:{MsgType:~p}~n", [MsgType]);
show_data(?msg_notify_package, #notify_package{account=Account, type=Type, grid_vec=Bag}) ->
    printf("\nplayer bag: account ~p, type ~w, bag ~w ~n", 
	   [Account, Type, get_bag_items(array:from_list(Bag))]);
show_data(MsgType, MsgData) ->
    printf("receive msg:{MsgType:~p, MsgData:~p}~n", [MsgType, MsgData]).

get_bag_items(Bag) ->
    %% 添加物品
    FAdd = fun(_Idx, #pack_grid{item_data=Item}=Grid, List) -> 		   
		   #item{template_id=TempID} = Item,
		   case TempID == 0 of
		       true ->
			   List;
		       false ->
			   [Grid | List] 
		   end
	   end,
    
    array:foldl(FAdd, [], Bag).

