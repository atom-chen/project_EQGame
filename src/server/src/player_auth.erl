%% Author: NoteBook
%% Created: 2009-9-12
%% Description: Add description to tcp_listen_player
-module(player_auth).

%%
%% Include files
%%
-include("packet_def.hrl").
-include("enum_def.hrl").
-include("tcp_mm_data.hrl").
-include("table_info.hrl").
-include("sys_msg.hrl").

-define(NODE, 'game@wxp-linyj.CHINA-CHANNEL.COM').

%%
%% Exported Functions
%%
-export([call/4]).
%%
%% API Functions
%%

%% 请求版本验证
call(Socket, State, TimeOut, {?msg_req_ver_validate, #req_ver_validate{ver=Ver}}) ->
    data_helper:format("req ver validate, server version:~p~n", [?proto_ver]),
    NetData = case Ver of
        ?proto_ver ->
            #notify_ver_validate{result=?ver_ok};
        _ ->
            #notify_ver_validate{result=?ver_fail}
    end,
    net_helper:send(Socket, NetData),
    {next_state, 'WAIT_FOR_AUTH', State, TimeOut};

%% 请求登录
call(Socket, #tcp_mm_data{eq_pid=EQPid, addr=ClientIP}=State, 
     TimeOut, {?msg_req_login, #req_login{account=RawAccount, password=RawPassword}}) ->
    Account = util:simple_xor(RawAccount),
    Password = util:simple_xor(RawPassword),
    AccountAtom = list_to_atom(string:to_lower(Account)),
    LoginRet = 
	case common_def:get_val(use_35e_account) of
	    true -> 
		eq_manager:set_account(EQPid, Account), %% 很多模块调用eq_manager, 所以手动设入用户信息
		e35_login_center:login(Account, Password, ClientIP);
	    _ -> 
		eq_manager:login(eq, EQPid, Account, Password)
	end,

    case LoginRet of
	{false, err_pwd} ->
	    net_helper:send(Socket, #notify_login_result{result=?lr_fail_pwd}),
	    {next_state, 'WAIT_FOR_AUTH', State#tcp_mm_data{account=AccountAtom}, TimeOut};
	{false, err_account} ->
	    net_helper:send(Socket, #notify_login_result{result=?lr_fail_account}),
	    {next_state, 'WAIT_FOR_AUTH', State#tcp_mm_data{account=AccountAtom}, TimeOut};
	{false, err_other} ->
	    net_helper:send(Socket, #notify_login_result{result=?lr_fail_other}),
	    {next_state, 'WAIT_FOR_AUTH', State#tcp_mm_data{account=AccountAtom}, TimeOut};
	{false, ErrorReason} ->
	    %% 记录错误
	    logger:log(error, "login error:~p~n", [ErrorReason]),

	    net_helper:send(Socket, #notify_login_result{result=?lr_fail}),
	    {next_state, 'WAIT_FOR_AUTH', State#tcp_mm_data{account=AccountAtom}, TimeOut};
	_ ->
	    %% 是否防沉迷账号
	    IsWallowAccount = case LoginRet of
				  {true, is_wallow_account} -> true;
				  _ -> false
			      end,
	    NState = State#tcp_mm_data{is_wallow_account=IsWallowAccount},
	    IsOnline = util:is_process_alive(AccountAtom),
	    case IsOnline of
		true ->
		    case router:send(AccountAtom, req_logout) of
			true ->
			    db_player:insert_online_player(AccountAtom),
			    net_helper:send(Socket, #notify_login_result{result=?lr_ok});
			_ ->
			    eq_manager:logout(EQPid),
			    net_helper:send(Socket, #notify_login_result{result=?lr_fail})
		    end,
		    {next_state, 'WAIT_FOR_AUTH', NState#tcp_mm_data{account=AccountAtom}, TimeOut};
		false ->
		    db_player:insert_online_player(AccountAtom),
		    net_helper:send(Socket, #notify_login_result{result=?lr_ok}),
		    {next_state, 'WAIT_FOR_AUTH', NState#tcp_mm_data{account=AccountAtom}, TimeOut}
	    end	    
    end;

%% 客户端请求获得角色
call(Socket, #tcp_mm_data{account=AccountAtom, eq_pid=EQPid}=State, TimeOut, 
     {?msg_req_get_roles, #req_get_roles{}}) ->
    case eq_manager:is_login(EQPid) of
    	false ->
    	    PlayerInfo = 
    		case db_player:select(once, AccountAtom) of
    		    [] -> [];
    		    PlayerData ->
    			set_player_data(PlayerData, [])
    		end,
	    NickName = case PlayerInfo of
			   [] -> [];
			   _ -> AccountAtom
		       end,
    	    Data = #notify_get_roles_result{player=PlayerInfo, nick_name=NickName},
    	    net_helper:send(Socket, Data);
    	true ->
	    eq_manager:req_vcard(EQPid, "get_vcard")
    end,
    {next_state, 'WAIT_FOR_AUTH', State, TimeOut};

%% 获取用户详细信息
call(Socket, #tcp_mm_data{account=AccountAtom, eq_pid=EQPid}=State, TimeOut, {get_vcard, Packet}) ->
    eq_manager:received_vcard(EQPid, Packet),
    Data = 
    case db_player:select(once, AccountAtom) of
	[] -> 
	    %%UserName = eq_manager:get_user_name(EQPid),
	    #notify_get_roles_result{player=[], nick_name=[]};%%filter_user_name(AccountAtom, UserName)};
	PlayerData ->
	    %% UserName = eq_manager:get_user_name(EQPid),	
	    %% case atom_to_list(AccountAtom) of
	    %% 	UserName -> ok;
	    %% 	[] -> ok;
	    %% 	_ ->
	    %% 	    db_player:update(AccountAtom, util:filter_user_name(UserName), PlayerData)
	    %% end,
	    PlayerInfo = set_player_data(PlayerData, []),
	    #notify_get_roles_result{player=PlayerInfo}
    end,
    net_helper:send(Socket, Data),
    {next_state, 'WAIT_FOR_AUTH', State, TimeOut};
%% 处理创建角色的请求
call(Socket, #tcp_mm_data{account=AccountAtom}=State, TimeOut, 
     {?msg_req_create_role, #req_create_role{basic_data=BasicData,equips=Equips}}) ->
    IsExist = db_player:is_exist(AccountAtom),
    NewState = State#tcp_mm_data{is_first_time_login=not IsExist},
    UserName = basic_data:get_data(username, BasicData),
    case can_create_role(UserName) of
	true ->
	    case IsExist of
		true -> 
		    net_helper:send(Socket, #notify_create_role_result{result=?rr_fail}),
		    {next_state, 'WAIT_FOR_AUTH', NewState, TimeOut};
		false ->% 玩家不存在，创建玩家的数据	    
		    %% player_init:init(?NODE, AccountAtom, BasicData, Equips),
		    player_init:init(AccountAtom, BasicData, Equips),
		    net_helper:send(Socket, #notify_create_role_result{result=?rr_ok}),
		    {next_state, 'WAIT_FOR_AUTH', NewState, TimeOut}
	    end;
	{false, Err} ->	    
	    net_helper:send(Socket, #notify_sys_msg{code=Err}),
	    {next_state, 'WAIT_FOR_AUTH', NewState, TimeOut}	    
    end;

%% 处理进入场景的请求
call(Socket, #tcp_mm_data{account=AccountAtom, eq_pid=EQPid, 
			  is_first_time_login=IsFirstTimeLogin, 
			  is_wallow_account=IsWallowAccount}=State, 
     TimeOut, 
     {?msg_req_enter_scene, #req_enter_scene{}}) ->
    net_helper:send(Socket, #notify_enter_scene_result{result=?sr_ok}),
    net_helper:send(Socket, #notify_sys_time{sys_time=datetime:localtime()}),
    BasicData = db_player:select(once, AccountAtom),
    PlayerSupPid = start_player(self(), AccountAtom, BasicData, EQPid, 
				IsFirstTimeLogin, IsWallowAccount),
    net_helper:send(Socket, #notify_player_data{basic_data=basic_data:convert(BasicData)}),
    %% player:enter_my_home(AccountAtom),
    {next_state, 'WAIT_FOR_DATA', 
     State#tcp_mm_data{player_sup=PlayerSupPid}, TimeOut}.

set_player_data([], PlayerInfo) ->
    PlayerInfo;
set_player_data(PlayerBasicData, PlayerInfo) ->
    %%AccountStr = atom_to_list(basic_data:get_data(account, PlayerBasicData)),
    %%UserName = basic_data:get_data(username, PlayerBasicData),
    %% NUserName = case UserName of
    %% 	 	    AccountStr -> [];
    %% 	 	    _ -> UserName
    %% 	 	end,

    %% NBasicData = basic_data:set_data(username, NUserName, PlayerBasicData),
    NBasicData = basic_data:set_data(username, [], PlayerBasicData),

    NewPlayerInfo= #player_info{basic_data=NBasicData},    
    set_player_data([], [NewPlayerInfo | PlayerInfo]).

%% 启动一个玩家进程, 并且监控该进程
%% MMPid:中间人Pid, Account:玩家帐号
start_player(MMPid, Account, BasicData, EQPid, IsFirstTimeLogin, IsWallowAccount) 
  when is_record(BasicData, player_basic_data)->
    %% {ok, Pid} = player_sup:start(?NODE, MMPid, Account, BasicData),
    case player_sup:start(MMPid, Account, BasicData, EQPid, 
			  IsFirstTimeLogin, IsWallowAccount) of
	{error,{already_started, Pid1}} -> 
	    player_sup:stop(node(Pid1), Account),
	    {ok, Pid3} = player_sup:start(MMPid, Account, BasicData, EQPid, 
					  IsFirstTimeLogin, IsWallowAccount),
	    Pid3;
	{error,already_present} ->
	    player_sup:stop(player_sup:get_sup_name(Account), Account),
	    {ok, Pid4} = player_sup:start(MMPid, Account, BasicData, EQPid, 
					  IsFirstTimeLogin, IsWallowAccount),
	    Pid4;
	{ok, Pid2} -> Pid2
    end.


can_create_role([]) ->
    {false, ?err_player_name_empty};
can_create_role(UserName) when is_list(UserName) ->  
    MaxLen = common_def:get_val(max_player_name_len),
    case util:char_count(UserName) > MaxLen of
	true ->
	    {false, ?err_player_name_too_long};
	_ ->
	    case db_player:name_to_account(UserName) of
		undefined ->
		    true;
		_ ->
		    {false, ?err_player_name_exists}
	    end		    
    end.


    
%% filter_user_name(AccountAtom, UserName) when is_atom(AccountAtom), 
%% 					     is_list(UserName) ->
%%     AccountStr = atom_to_list(AccountAtom),
%%     filter_user_name(AccountStr, UserName);
%% filter_user_name(AccountStr, UserName) when is_list(AccountStr), 
%% 					     is_list(UserName) ->
%%     NUserName = string:strip(UserName),
%%     case AccountStr =:= NUserName of
%% 	true -> [];
%% 	_ ->
%% 	    util:filter_user_name(NUserName)
%%     end.
