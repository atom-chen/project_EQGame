%% Author: NoteBook
%% Created: 2009-9-11
%% Description: Add description to client
-module(test_edit_house1).

%%
%% Include files
%%
-include("net_type.hrl").
-include("packet_def.hrl").  
-include("enum_def.hrl").
-include("common_def.hrl").
-record(state, {bag, money,account,furniture_vec, enter_times=0}).

%%
%% Exported Functions
%%
-export([start/1, start/0]).   
%%
%% API Functions
%%
start() ->
    start(["127.0.0.1", "2222", "master"]).

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
     {?msg_notify_ver_validate, fun process_notify_ver_validate/4},
     {?msg_notify_get_roles_result, fun process_notify_get_roles_result/4},
     {?msg_notify_enter_scene_result, fun process_notify_enter_scene_result/4},
     {?msg_notify_player_enter_scene, fun process_notify_player_enter_scene/4},
     {?msg_notify_house_data, fun process_notify_house_data/4},
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

process_notify_enter_scene_result({notify_enter_scene_result, _Result}, _Socket, _List, State) ->
    State.


%%--------------------------------------------------------------------------------
process_notify_player_enter_scene(#notify_player_enter_scene{account=SomeOne}, Socket, _List, 
				  #state{account=Account,
					 furniture_vec=Vec,
					 enter_times=Times}=State) ->
    case (SomeOne /= Account) and (Times == 0) of
    	true ->
	    io:format(" 取第一个家具\n"),
	    [First | _] = Vec,
	    #house_furniture{instance_id=InstanceID, x=X, z=Z,
		      height=Height, floor=Floor, face=Face} = First,

	    io:format("%% 开始编辑房屋\n"),
    	    send_data(Socket, #req_start_edit_house{}),

	    io:format("%% 换墙纸\n"),
	    send_data(Socket, #req_gm_command{type=?gct_replace_tex, 
	     				      params=["1", "2", "mytex"]}),
	    io:format("%% 移动家具\n"),
	    timer:sleep(100),
	    send_data(Socket, #req_move_furniture{instance_id=InstanceID,x=X,z=Z,height=Height,
				 floor=Floor,face=Face}), 

	    timer:sleep(1000),
	    io:format("%% 结束编辑房屋\n"),
    	    send_data(Socket, #req_end_edit_house{}),

	    io:format("%% 移动家具\n"),
	    timer:sleep(100),
	    send_data(Socket, #req_move_furniture{instance_id=InstanceID,x=X,z=Z,height=Height,
	     			 floor=Floor,face=Face}), 

	    State#state{enter_times=Times + 1};
    	false ->
    	    State
    end.


process_notify_house_data(#notify_house_data{data=HouseData}, _Socket, _List, State) ->
    #house_data{furniture_vec=Vec} = HouseData,
    
    State#state{furniture_vec=Vec}.


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
