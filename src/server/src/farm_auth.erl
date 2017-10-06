%% Author: NoteBook
%% Created: 2009-9-12
%% Description: Add description to tcp_listen_player
-module(farm_auth).

%%
%% Include files
%%
-include("packet_def.hrl").
-include("enum_def.hrl").
-include("tcp_mm_data.hrl").
-include("table_info.hrl").

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
call(Socket, #tcp_mm_data{eq_pid=EQPid, addr=ClientIP}=State, TimeOut, 
     {?msg_req_login, #req_login{account=RawAccount, password=RawPassword}}) ->
    Account = util:simple_xor(RawAccount),
    Password = util:simple_xor(RawPassword),
    
    AccountAtom = list_to_atom(Account),
    LoginRet = 
	case common_def:get_val(use_35e_account) of
	    true -> 
		eq_manager:set_account(EQPid, Account), %% 很多模块调用eq_manager, 所以手动设入用户信息
		e35_login_center:login(Account, Password, ClientIP); 
	    _ -> eq_manager:login(farm, EQPid, Account, Password)
	end,

    case LoginRet of
	{false, err_pwd} ->
	    eq_manager:logout(EQPid),
	    net_helper:send(Socket, #notify_login_result{result=?lr_fail_pwd}),
	    {next_state, 'WAIT_FOR_AUTH', State#tcp_mm_data{account=AccountAtom}, TimeOut};
	{false, err_account} ->
	    eq_manager:logout(EQPid),
	    net_helper:send(Socket, #notify_login_result{result=?lr_fail_account}),
	    {next_state, 'WAIT_FOR_AUTH', State#tcp_mm_data{account=AccountAtom}, TimeOut};	
	{false, err_other} ->
	    eq_manager:logout(EQPid),
	    net_helper:send(Socket, #notify_login_result{result=?lr_fail_other}),
	    {next_state, 'WAIT_FOR_AUTH', State#tcp_mm_data{account=AccountAtom}, TimeOut};	
	{false, ErrorReason} ->
	    %% 记录错误
	    logger:log(error, "login error:~p~n", [ErrorReason]),

	    eq_manager:logout(EQPid),
	    net_helper:send(Socket, #notify_login_result{result=?lr_fail}),
	    {next_state, 'WAIT_FOR_AUTH', State#tcp_mm_data{account=AccountAtom}, TimeOut};
	_ ->
	    %% 是否防沉迷账号
	    IsWallowAccount = case LoginRet of
				  {true, is_wallow_account} -> true;
				  _ -> false
			      end,
	    NState = State#tcp_mm_data{is_wallow_account=IsWallowAccount, account=AccountAtom},

	    FarmName = farm:get_name(AccountAtom),
	    IsOnline = util:is_process_alive(FarmName),
	    case IsOnline of
		true ->
		    case farm:logout(FarmName) of
		    	true ->
			    eq_manager:req_vcard(EQPid, "get_vcard"),
			    net_helper:send(Socket, #notify_sys_time{sys_time=datetime:localtime()}),
			    NNState = do_start_farm(Socket, NState),
			    {next_state, 'WAIT_FOR_DATA', NNState, TimeOut};
		    	_ ->
		    	    eq_manager:logout(EQPid),
		    	    net_helper:send(Socket, #notify_login_result{result=?lr_fail}),
			    {next_state, 'WAIT_FOR_AUTH', NState, TimeOut}
			end;
		false ->
		    eq_manager:req_vcard(EQPid, "get_vcard"),
		    net_helper:send(Socket, #notify_sys_time{sys_time=datetime:localtime()}),
		    NNState = do_start_farm(Socket, NState),
		    {next_state, 'WAIT_FOR_DATA', NNState, TimeOut}
	    end	  
    end;
call(_Socket, #tcp_mm_data{eq_pid=EQPid}=State, _TimeOut, {get_vcard, Packet}) ->
    eq_manager:received_vcard(EQPid, Packet),
    State.


do_start_farm(Socket, #tcp_mm_data{account=Account, eq_pid=EQPid, 
				   is_wallow_account=IsWallowAccount}=State) ->
    Pid = start_farm(Account, self(), EQPid, IsWallowAccount),
    net_helper:send(Socket, #notify_login_result{result=?lr_ok}),
    State#tcp_mm_data{pid=Pid}.


%% 启动一个农场进程, 并且监控该进程
%% Account:玩家帐号
start_farm(Account, MMPid, EQPid, IsWallowAccount) ->
    case player_farm_sup:start(Account, MMPid, EQPid, IsWallowAccount) of
	{ok, Pid2} -> Pid2
    end.
