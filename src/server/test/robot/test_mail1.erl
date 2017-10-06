-module(test_mail1).

%%
%% Include files
%%
-include("net_type.hrl").
-include("packet_def.hrl").  
-include("enum_def.hrl").
-include("common_def.hrl").

-record(state, {bag, money,account,trade_player,recver, mails}).
%%
%% Exported Functions
%%
-export([start/1, start/0]).   
%%
%% API Functions
%%
start() ->
    player_friends:test(),
    start(["127.0.0.1", "2222", "mail1"]).

start([Ip, Port, RoleName]) ->
    io:format("启动玩家:~p~n", [RoleName]),
    Socket = connect(Ip, list_to_integer(Port)),
    Data = #req_ver_validate{ver=?proto_ver},
    send_data(Socket, Data),
    loop(Socket, init(), #state{account=RoleName, recver="mail2"}).

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
     {?msg_notify_mail_not_read, fun process_notify_mail_not_read/4},
     {?msg_notify_mail_list, fun process_notify_mail_list/4},
     {?msg_notify_mail_content, fun process_notify_mail_content/4},
     {?msg_notify_add_mail, fun process_notify_add_mail/4},
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
				  #state{account=RoleName}=State) ->
    timer:sleep(100),
    io:format("给自已发邮件，结果失败\n"),
    send_data(Socket, #req_send_mail{recver_name=RoleName, title="hi", content="abc", items=[]}),
    timer:sleep(100),
    io:format("不写收件人名字，结果失败\n"),
    send_data(Socket, #req_send_mail{recver_name="", title="", content="", items=[]}),
    timer:sleep(100),
    io:format("找不到收件人，结果失败\n"),
    send_data(Socket, #req_send_mail{recver_name="t___tammgih332", title="gg", content="eee", items=[]}),
    State.

process_notify_mail_not_read(_Msg, Socket, _List, 
			     State) ->
    
    io:format("邮件未读，请求邮件列表\n"),
    send_data(Socket, #req_mail_list{}),
    State.

process_notify_mail_list(#notify_mail_list{mails=Mails}, Socket, _List, 
			     State) ->
    
    io:format("对于每封新邮件, 取得邮件具体内容, 通知服务端看过标题\n"),
    IDList = [ID || #mail_info{id=ID, read_status=Status} <- Mails, Status == ?mrs_not_read],
    send_data(Socket, #req_know_new_mail_title{id_list=IDList}),

    [send_data(Socket, #req_mail_content{id=ID}) || 
	#mail_info{id=ID, read_status=Status} <- Mails, Status == ?mrs_not_read],

    State#state{mails=Mails}.

process_notify_mail_content(#notify_mail_content{id=ID}, Socket, _List, 
			    #state{mails=Mails}=State) ->
    io:format("对于每封邮件内容, 提取邮件物品\n"),
    F = fun(#mail_info{id=MailID}) -> MailID == ID end ,
    [#mail_info{items=Items}] = lists:filter(F, Mails),
    [send_data(Socket, #req_get_mail_item{id=ID, inst_id=InstID}) || 
	#pack_grid{item_data=#item{instance_id=InstID}} <- Items, InstID /= 0],
    State.

process_notify_add_mail(#notify_add_mail{mail_data=MailData}, Socket, _List, State) ->
    io:format("有人发送邮件过来 ~p \n", [MailData]),
    io:format("删除刚发过的这封邮件\n"),
    #mail_info{id=ID, title=Title, sender_account=Sender} = MailData,
    send_data(Socket, #req_delete_mail{id=ID}),

    case Title == "交友信" of
	true ->
	    io:format("标题为交友信，就回一封邮件给对方\n"),
	    send_data(Socket, #req_send_mail{recver_account=Sender, title="please speak.", content="as title", items=[]});
	false -> ok
    end,

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

