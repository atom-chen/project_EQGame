%% Author: NoteBook
%% Created: 2009-9-11
%% Description: Add description to client
-module(test_trade1).

%%
%% Include files
%%
-include("net_type.hrl").
-include("packet_def.hrl").  
-include("enum_def.hrl").
-include("common_def.hrl").
-record(state, {bag, money,account,trade_player,trade_times=0}).

%%
%% Exported Functions
%%
-export([start/1, start/0]).   
%%
%% API Functions
%%
start() ->
    start(["127.0.0.1", "2222", "hjx1"]).

start([Ip, Port, RoleName]) ->
    io:format("启动玩家:~p~n", [RoleName]),
    Socket = connect(Ip, list_to_integer(Port)),
    printf("client version:~p~n", [?proto_ver]),
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
     {?msg_notify_other_player_data, fun process_notify_other_player_data/4},
     {?msg_notify_ver_validate, fun process_notify_ver_validate/4},
     {?msg_notify_get_roles_result, fun process_notify_get_roles_result/4},
     {?msg_notify_enter_scene_result, fun process_notify_enter_scene_result/4},
     {?msg_notify_player_leave_scene, fun process_notify_player_leave_scene/4},
     {?msg_notify_house_data, fun common_proc/4},
     {?msg_notify_enter_common_scene, fun process_notify_enter_common_scene/4},
     {?msg_notify_start_trade, fun process_notify_start_trade/4},
     {?msg_notify_trade_success, fun process_notify_trade_success/4},
     {?msg_notify_cancel_trade, fun process_notify_cancel_trade/4},
     {?msg_notify_other_sure_trade, fun process_notify_other_sure_trade/4},
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
process_notify_ver_validate({notify_ver_validate, ?ver_ok}, Socket, _List, #state{account=RoleName}=State) ->
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

process_notify_create_role_result({notify_create_role_result, Result}, Socket, _List, State) ->
    case Result of
        ?rr_ok ->
            Data = #req_enter_scene{},
            send_data(Socket, Data);
        ?rr_fail ->
            printf("create role fail~n")
    end,
    State.

process_notify_enter_scene_result({notify_enter_scene_result, Result}, Socket, _List, State) ->
    case Result of
        ?sr_ok ->
	    timer:sleep(100),
	    Data = #req_enter_common_scene{scene_id = 1},
	    send_data(Socket, Data),
	    ok;
        ?sr_fail ->
            printf("enter scene fail~n")
    end,
    State.
process_notify_enter_common_scene(#notify_enter_common_scene{}, Socket, _List, 
				  #state{account=RoleName}=State) ->    
    timer:sleep(100),
    io:format("\n~p enter common scene, wait for player Hjx2 to trade.\n", [RoleName]),
    io:format(" 加物品30006\n"),
    send_data(Socket, #req_gm_command{type=?gct_add_item, 
				      params=["30006"]}),
    io:format(" 加物品30007\n"),
    send_data(Socket, #req_gm_command{type=?gct_add_item, 
				      params=["30007"]}),
    io:format("加钱100\n"),
    send_data(Socket, #req_gm_command{type=?gct_get_money, 
				      params=[integer_to_list(?GAMECOIN), "100"]}), 
    State.

process_notify_cancel_trade(#notify_cancel_trade{}, Socket, _List, 
			    #state{trade_player=TradePlayer, trade_times=Times}=State) ->
    io:format("State: ~p \n", [State]),
    case Times of
	1 ->
	    io:format("11. Hjx1 发现Hjx2取消交易又再次发起邀请\n"),
	    Packet = #req_invite_someone{invitee=TradePlayer, type=?ivt_trade},
	    send_data(Socket, Packet);
	_ ->
	    ok
    end,
    State.

process_notify_other_sure_trade(#notify_other_sure_trade{}, Socket, _List, State) ->
    io:format("16. Hjx1 也确定交易\n"),
    send_data(Socket, #req_sure_trade{}),
    State.

process_notify_start_trade(#notify_start_trade{account=TradePlayer}, Socket, _List, 
			   #state{trade_times=Times}=State) ->
    io:format("trade_player:~p\n", [TradePlayer]),
    case Times of
    	0 ->
    	    io:format("7. Hjx1 开始交易后，将2块钱放到交易栏\n"),    
    	    send_data(Socket, #req_update_trade_money{money=2});
    	1 ->
    	    io:format("14. Hjx1 开始交易后，将5块钱放到交易栏\n"),    
    	    send_data(Socket, #req_update_trade_money{money=5});
    	_N ->
    	    io:format("14. Hjx1 开始交易后，将10块钱放到交易栏\n"),    
    	    send_data(Socket, #req_update_trade_money{money=10})
    end,
    State#state{trade_player=TradePlayer, trade_times=Times + 1}.
%%--------------------------------------------------------------------------------

process_notify_player_leave_scene({notify_player_leave_scene, Account}, Socket, _List, State) ->
    printf("player leave scene~p~n", [Account]),
    io:format("18. hjx1移动到距离大于5 * 80 的地方\n"),
    Pos = #point{x= 5 * 80 + 1, y = 0},
    send_data(Socket, #req_sync_position{pos=Pos}),
    io:format("19. 此时两玩家处在不同场景中, Hjx1 又向Hjx2发出交易请求，结果失败\n"),
    Packet = #req_invite_someone{invitee=Account, type=?ivt_trade},
    send_data(Socket, Packet),
    State.

process_notify_other_player_data(#notify_other_player_data{player=PlayerInfo}, Socket, _List, State) ->
   
    #player_basic_data{account=Account, username=_UserName} = PlayerInfo#player_info.basic_data,
    io:format("请求对方交易\n"),
    Packet = #req_invite_someone{invitee=Account, type=?ivt_trade},
    send_data(Socket, Packet),
    State.

process_notify_trade_success(#notify_trade_success{}, _Socket, _List, 
			     #state{account=_Account}=State) ->
    io:format("首次交易成功，Hjx1 状态应为 (30006, 30007) + 117, 100 + 30 - 5 = 125\n"),    
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
