-module(cupid_robot).

-include("enum_def.hrl").
-include("net_type.hrl").
-include("packet_def.hrl").  

-export([start/4, start/3, start/1]).
-export([connect/2, send_data/2]).

-define(boy, [[#item{template_id=117}, #item{template_id=576}, #item{template_id=510}, 
	      %% #item{template_id=275}, 
#item{}, #item{}, #item{}, #item{}],
	      [#item{template_id=310}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	      [#item{template_id=311}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	      [#item{template_id=312}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	      [#item{template_id=313}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	      [#item{template_id=318}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	      [#item{template_id=319}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	      [#item{template_id=320}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	      [#item{template_id=321}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}]
	     ]).
-define(girl, [[#item{template_id=913}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       [#item{template_id=914}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       %%[#item{template_id=915}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       [#item{template_id=916}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       [#item{template_id=917}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       [#item{template_id=918}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       [#item{template_id=919}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       [#item{template_id=920}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       [#item{template_id=921}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       [#item{template_id=922}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       [#item{template_id=923}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}],
	       [#item{template_id=924}, #item{template_id=1189}, #item{}, #item{}, #item{}, #item{}, #item{}, #item{}]
	      ]).

init() ->
    [{?msg_notify_login_result, fun process_notify_login_result/6},
     {?msg_notify_create_role_result, fun process_notify_create_role_result/6},
     {?msg_notify_cupid_participate, fun process_notify_cupid_participate/6},
     {?msg_notify_cupid_girl_match_result, fun process_notify_cupid_girl_match_result/6},
     {?msg_notify_ver_validate, fun process_notify_ver_validate/6},
     {?msg_notify_get_roles_result, fun process_notify_get_roles_result/6},
     {?msg_notify_enter_scene_result, fun process_notify_enter_scene_result/6},
     {?msg_notify_cupid_boy_match_result, fun process_notify_cupid_boy_match_result/6},
     {?msg_notify_house_data, fun common_proc/6}
     %% {?msg_notify_package, fun process_notify_player_pack/6},
     %% {?msg_notify_npc_list, fun process_notify_npc_list/6},
     %% {?msg_notify_use_furniture_result, fun process_notify_use_furniture_result/6},
     %% {?msg_notify_task_list, fun process_notify_task_list/6},
     %% {?msg_notify_stop_walk, fun process_notify_stop_walk/6},
     %% {?msg_notify_chat_around, fun process_notify_chat_around/6}
    ].
start([Start, End, IP, Port])->
    start(list_to_integer(Start), list_to_integer(End), IP, list_to_integer(Port)).

start(Start, End, IP, Port)->
    tplt:start_link(),
    start1(Start, End, IP, Port).    

start1(Start, End, _IP, _Port)
  when Start > End ->
    ok;
start1(Start, End, IP, Port)
  when Start =< End ->
    spawn(?MODULE, start, [Start, IP, Port]),
    timer:sleep(1000),
    start1(Start+1, End, IP, Port).

start(N,IP,Port)->
    Socket = connect(IP, Port),
    Data = #req_ver_validate{ver=?proto_ver},
    send_data(Socket, Data),
    loop(Socket, init(), string:to_lower(util:simple_xor("cupid" ++ integer_to_list(N))), N, 2).

connect(Ip, Port) -> 
    {ok, Socket} = gen_tcp:connect(Ip, Port, [{packet, 4}]),
    Socket.

send_data(Socket, Data) ->
    {Type, Binary} = protocal:encode(Data),
    Msg = net_helper:make_net_binary(Type, Binary),
    gen_tcp:send(Socket, Msg),
    ok.

get_reg(Account, Sex, Tags)->
    #req_cupid_participate{account=Account, sex=Sex, tags=Tags}.

get_tags()->
    Tags = tplt:get_all_data(cupid_tag),
    random:seed(now()),
    Count = random:uniform(8),
    T = get_tags2(Count, lists:sort(fun(_A, _B)-> random:uniform()>=0.5 end, Tags)),
    %% io:format("Count:~p  Tags:~p~n", [Count, T]),
    T.

get_tags2(0, _Tags)->
    [];    
get_tags2(_Count, [])->
    [];
get_tags2(Count, [{cupid_tag, Tag}|Others])->
    [Tag|get_tags2(Count-1, Others)].

loop(Socket, List, RoleName, Num, ModNum)->
    receive
	{tcp, Socket, Bin} ->
	    Binary = list_to_binary(Bin),
	    {MsgType, MsgData}= net_helper:get_data(Binary),
	    io:format("Type:~p Data:~p~n", [MsgType, MsgData]),
	    Processor = find_proc(MsgType, List),
	    Processor(MsgData, Socket, List, RoleName, Num, ModNum),
	    loop(Socket, List, RoleName, Num, ModNum);
	{tcp_closed, Socket} ->
	    io:format("User:~p~n", [RoleName]),
	    printf("tcp close~n")
    end.

find_proc(MsgType, List) ->
    case lists:keyfind(MsgType, 1, List) of
	{_, Proc} ->
	    Proc;
	_ ->
	    fun common_proc/6
    end.

common_proc(_Data, _Socket, _List, _RoleName, _Num, _ModNum) ->
    ok.

process_notify_ver_validate({notify_ver_validate, ?ver_ok}, Socket, _List, RoleName, _Num, _ModNum) ->
    Data = #req_login{account=RoleName},
    send_data(Socket, Data);

process_notify_ver_validate({notify_ver_validate, ?ver_fail}, _Socket, _List, _RoleName, _Num, _ModNum) ->
    printf("Version Error~n").

process_notify_login_result({notify_login_result, ?lr_ok}, Socket, _List, _RoleName, _Num, _ModNum) ->
    send_data(Socket, #req_get_roles{});

process_notify_login_result({notify_login_result, ?lr_fail}, _Socket, _List, _RoleName, _Num, _ModNum) ->
    printf("Account login fail~n").

process_notify_get_roles_result({notify_get_roles_result, Roles, _NickName}, Socket, _List, RoleName, Num, _ModNum) ->
    %% io:format("get roles result~n"),
    case Roles of
        [] ->
	    Sex = (Num rem 2) + 1,
	    %% io:format("Sex:~p~n", [Sex]),
            PlayerBasicData = #player_basic_data{username=string:to_lower(util:simple_xor(RoleName)), 
						 sex=Sex,
						 skin_color=1, 
						 hair=1, 
						 face=1, 
						 beard=1},
	    Equips = 
		case Sex == 1 of
		    true ->
			Number = rand:uniform(length(?boy)),
			lists:nth(Number, ?boy);
		    _ ->
			Number = rand:uniform(length(?girl)),
			lists:nth(Number, ?girl)
		end,
            Data = #req_create_role{basic_data=PlayerBasicData,equips=Equips},
            send_data(Socket, Data);
        _ ->
            Data = #req_enter_scene{},
            send_data(Socket, Data)
    end.

process_notify_create_role_result({notify_create_role_result, Result}, Socket, _List, _RoleName, _Num, _ModNum) ->
    case Result of
        ?rr_ok ->
            Data = #req_enter_scene{},
            send_data(Socket, Data);
        ?rr_fail ->
            %% printf("create role fail~n")
	    ok
    end.

process_notify_enter_scene_result({notify_enter_scene_result, Result}, Socket, _List, RoleName, Num, _ModNum) ->
    case Result of
        ?sr_ok ->
            %% io:format("~p enter scene success~n", [RoleName]),
	    Sex = case Num rem 2 of 
		      0-> ?st_boy;
		      _ ->?st_girl
		  end,
	    %%×¢²á
	    send_data(Socket, get_reg(list_to_atom(string:to_lower(util:simple_xor(RoleName))), Sex, get_tags()));
        ?sr_fail ->
            printf("enter scene fail~n")
    end.

process_notify_cupid_participate({notify_cupid_participate, Result}, Socket, _List, RoleName, Num, _ModNum)->
    case Result of 
	1->
	    io:format("~p reg success~n", [RoleName]),
	    case Num rem 8 of 
		0->
		    %%È¡Ïû
		    send_data(Socket,#req_cupid_cancel_participate{account=string:to_lower(util:simple_xor(RoleName))}),
		    ok;
		_->
		    ok
	    end;
	_->
	    io:format("~p repeate reg~n", [RoleName])
    end.

process_notify_cupid_girl_match_result({notify_cupid_girl_match_result, Result}, Socket, _List, RoleName, Num, _ModNum)
->
    io:format("Girl:~p receive ~p~n", [RoleName, Result]),
    Answer =  Num rem 3,
    send_data(Socket, #req_cupid_girl_confirm{account=string:to_lower(util:simple_xor(RoleName)), result=Answer}).

process_notify_cupid_boy_match_result({notify_cupid_boy_match_result, Result}, Socket, _List, RoleName, Num, _ModNum)
->
    io:format("Boy:~p receive ~p~n", [RoleName, Result]),
    #notify_cupid_boy_match_result{girl_result=GirlResult}=Result,
    Answer =  Num rem 2,
    send_data(Socket, #req_cupid_boy_confirm{account=string:to_lower(util:simple_xor(RoleName)), result=Answer, girl_result=GirlResult}).

printf(Str) ->
    data_helper:format(Str),
    ok.
