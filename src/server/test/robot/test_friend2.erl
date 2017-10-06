%% Author: NoteBook
%% Created: 2009-9-11
%% Description: Add description to client
-module(test_friend2).

%%
%% Include files
%%
-include("net_type.hrl").
-include("packet_def.hrl").  
-include("enum_def.hrl").
-include("common_def.hrl").

-record(state, {account, my_friend, friend_list}).
%%
%% Exported Functions
%%
-export([start/1, start/0]).   
%%
%% API Functions
%%
start() ->
    start(["127.0.0.1", "2222", "friend2@35info.cn"]).

start([Ip, Port, RoleName]) ->
    io:format("start player:~p~n", [RoleName]),
    Socket = connect(Ip, list_to_integer(Port)),
    Data = #req_ver_validate{ver=?proto_ver},
    send_data(Socket, Data),
    loop(Socket, init(), #state{account=RoleName, my_friend="friend1@35info.cn"}).

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
     {?msg_notify_friend_list, fun process_notify_friend_list/4},
     {?msg_notify_invitation, fun process_notify_be_invite/4},
     {?msg_notify_enter_scene_result, fun process_notify_enter_scene_result/4},
     {?msg_notify_friend_online, fun process_notify_friend_online/4},
     {?msg_notify_friend_offline, fun process_notify_friend_offline/4},
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

%%--------------------------------------------------------------------------------

process_notify_enter_scene_result({notify_enter_scene_result, Result}, Socket, _List, State) ->
    case Result of
        ?sr_ok ->
	    io:format(" 请求好友列表\n"),
	    send_data(Socket, #req_friend_list{}),
	    ok;
        ?sr_fail ->
            printf("enter scene fail~n")
    end,
    State.



process_notify_friend_list(#notify_friend_list{friend_vec=FriendList}, Socket, _List, 
			   #state{my_friend=Friend1}=State) ->
    F = fun(#friend_info{basic_data=#player_basic_data{account=Friend}}) -> Friend == Friend1 end,
    case lists:any(F, FriendList) of
	true ->	 
	    io:format("5. 如果friend2存在好友friend1, 则删除之\n"),
	    send_data(Socket, #req_remove_friend{friend_account=Friend1});
	false -> 
	    io:format("6. 如果friend2存在好友friend1，则加为好友\n"),
	    send_data(Socket, #req_add_friend{friend_account=Friend1}),
	    ok	    
    end,
    State#state{friend_list=FriendList}.

process_notify_friend_online(#notify_friend_online{friend_account=Friend1}, Socket, _List, 
			   #state{my_friend=Friend1}=State) ->
    io:format("9. friend2 收到好友上线通知后对friend1说: 你上线了\n"),
    send_data(Socket, #req_chat_tell{target_player=Friend1, content="hi, 你上线了"}),
    State.    
    
process_notify_friend_offline(#notify_friend_offline{friend_account=Friend1}, Socket, _List, 
			   #state{my_friend=Friend1}=State) ->
    io:format("7. friend2 收到好友下线通知后对friend1说: 你下线了, 这里应该要发送失败\n"),
    send_data(Socket, #req_chat_tell{target_player=Friend1, content="你下线了"}),
    State.    

process_notify_be_invite({notify_invitation, Invitor, _InvitorName, ?ivt_enter_house=Type}, Socket, _List, 
			 State) ->
    io:format("同意邀请\n"),     
    send_data(Socket, #req_agree_invitation{invitor=Invitor, type=Type}),
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


