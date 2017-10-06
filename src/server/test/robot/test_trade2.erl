%% Author: NoteBook
%% Created: 2009-9-11
%% Description: Add description to client
-module(test_trade2).

%%
%% Include files
%%
-include("net_type.hrl").
-include("packet_def.hrl").  
-include("enum_def.hrl").
-include("common_def.hrl").

-record(state, {bag, money,account,trade_player}).
%%
%% Exported Functions
%%
-export([start/1, start/0]).   
%%
%% API Functions
%%
start() ->
    start(["127.0.0.1", "2222", "hjx2"]).

start([Ip, Port, RoleName]) ->
    io:format("start player:~p~n", [RoleName]),
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
     {?msg_notify_player_data, fun process_notify_player_data/4},
     {?msg_notify_ver_validate, fun process_notify_ver_validate/4},
     {?msg_notify_get_roles_result, fun process_notify_get_roles_result/4},
     {?msg_notify_enter_scene_result, fun process_notify_enter_scene_result/4},
     {?msg_notify_player_leave_scene, fun process_notify_player_leave_scene/4},
     {?msg_notify_house_data, fun common_proc/4},
     {?msg_notify_package, fun process_notify_player_pack/4},
     {?msg_notify_enter_common_scene, fun process_notify_enter_common_scene/4},
     {?msg_notify_invitation, fun process_notify_be_invite/4},
     {?msg_notify_trade_success, fun process_notify_trade_success/4},
     {?msg_notify_unput_trade_item, fun process_notify_unput_trade_item/4},
     {?msg_notify_other_update_trade_money, fun process_notify_other_update_trade_money/4},
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
    io:format(" 加物品117\n"),
    send_data(Socket, #req_gm_command{type=?gct_add_item, 
				      params=["117"]}),
    io:format("加物品118\n"),
    send_data(Socket, #req_gm_command{type=?gct_add_item, 
				      params=["118"]}),
    io:format("加钱1000\n"),
    send_data(Socket, #req_gm_command{type=?gct_get_money, 
				      params=[integer_to_list(?GAMECOIN), "1000"]}),
    State.

process_notify_other_update_trade_money(#notify_other_update_trade_money{money=Money},
					Socket, _List, 
					#state{bag=Bag}=State) ->
    case Money of
	2 ->
	    io:format(" 8. Hjx2 看到2块钱后说, 我看到2块钱了\n"),
	    send_data(Socket, #req_chat_around{content="我看到2块钱了"}),
	    
	    io:format(" 8. Hjx2 看到2块钱后, 将118号物品放到交易栏第二格\n"),   
	    F = fun(#pack_grid{item_data=#item{template_id=ID}}) -> ID == 118 end,
	    Idx = util:array_index_of(F, Bag),
	    send_data(Socket, #req_put_trade_item{packet_index=Idx, trade_index=2}),

	    io:format("9. Hjx2 又将118号物品从交易栏取下\n"),
	    send_data(Socket, #req_unput_trade_item{packet_index=-1, trade_index=2}),

	    io:format("9. Hjx2 将BagIdx为1物品放到交易栏1\n"),
	    send_data(Socket, #req_put_trade_item{packet_index=1, trade_index=1}),
	    io:format("9. Hjx2 将BagIdx为0物品放到交易栏1，取代原先物品\n"),
	    send_data(Socket, #req_put_trade_item{packet_index=0, trade_index=1}),
	    io:format("9. Hjx2 将BagIdx为1物品放到交易栏1，取代原先物品\n"),
	    send_data(Socket, #req_put_trade_item{packet_index=1, trade_index=1}),

	    io:format("9. Hjx2 将BagIdx为0物品放到交易栏0\n"),
	    send_data(Socket, #req_put_trade_item{packet_index=0, trade_index=0}),

	    io:format("9. Hjx2 交换交易栏的两个物品\n"),
	    send_data(Socket, #req_swap_trade_item{index1=0, index2=1}),
	    io:format("9. Hjx2 将BagIdx为0物品从交易栏取\n"),
	    send_data(Socket, #req_unput_trade_item{packet_index=-1, trade_index=0}),
	    ok;
 	5 ->
	    io:format("13. Hjx2 看到5块钱后, 将117号物品放到交易栏第3格\n"),
	    io:format("bag ~w\n", [get_bag_items(Bag)]),
	    F = fun(#pack_grid{item_data=#item{template_id=ID}}) -> ID == 117 end,
	    Idx = util:array_index_of(F, Bag),
	    send_data(Socket, #req_put_trade_item{packet_index=Idx, trade_index=3}),
	    io:format("14. Hjx2 将30块钱放到交易栏\n"),    
	    send_data(Socket, #req_update_trade_money{money=30}),
	    io:format("15. Hjx2 确定交易\n"),
	    send_data(Socket, #req_sure_trade{});
	_  ->
	    io:format("15. Hjx2 确定交易\n"),
	    send_data(Socket, #req_sure_trade{}),
	    ok
    end,

    State.

process_notify_unput_trade_item(#notify_unput_trade_item{trade_index=0}, Socket, _List, State) ->
    io:format("10. Hjx2 取消交易\n"),
    send_data(Socket, #req_cancel_trade{}),    
    State;
process_notify_unput_trade_item(#notify_unput_trade_item{trade_index=_Idx}, _Socket, _List, State) ->
    State.

%%--------------------------------------------------------------------------------

process_notify_player_leave_scene({notify_player_leave_scene, Result}, _Socket, _List, State) ->
    printf("player leave scene~p~n", [Result]),
    State.

process_notify_trade_success(#notify_trade_success{}, Socket, _List, 
			     #state{account=Account}=State) ->
    io:format("首次交易成功，Hjx2 状态应为 (117, 118) - 117, 1000 - 30 + 5 = 975\n"),    
    io:format("17. 交易成功后， Hjx2回到自已的家里\n"),
    send_data(Socket, #req_enter_player_house{account=Account}),
    State.

process_notify_other_player_data(Data, _Socket, _List, State) ->
    printf("Data:~p~n", [Data]), 
    State.

process_notify_be_invite({notify_invitation, Invitor, _InvitorName, ?ivt_trade=Type}=MsgData, Socket, _List, 
			 State) ->
    printf("call be invite:~p~n", [MsgData]),
    io:format(" 同意邀请\n"),     
    send_data(Socket, #req_agree_invitation{invitor=Invitor, type=Type}),
    State.    



process_notify_player_data(_Data, _Socket, _List, State) ->
    %send_data(Socket, #req_gm_command{type=?gct_add_item, params=["1"]}),
    %send_data(Socket, #req_swap_item{index1=1,index2=2}).
    State.

process_notify_player_pack(#notify_package{account=Account, 
					  type=Type, grid_vec=Bag}, _Socket, _List, 
			   #state{account=RoleName}=State) ->

    case (Account == RoleName) and (Type == ?pt_bag) of
	true ->
	    State#state{bag=array:from_list(Bag)};
	_ ->
	    State
    end.




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

