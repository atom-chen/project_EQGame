%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   好友相关
%%% @end
%%% Created : 10 May 2010 by  <hjx>
-module(player_friends).

-behaviour(gen_msgproc).
-include("router.hrl").


-include("packet_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").
-include("enum_def.hrl").

-record(state, {owner, 
		eq_pid,
		friend_list=[]    %% 好友列表 
	       }).

%% API
-export([start_link/2, broadcast_friends/2]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3, register/1]).


-define(SERVER, ?MODULE). 

%%%===================================================================
%%% API
%%%===================================================================


%% 监听指定的消息
register([Owner, _EQPid])-> 
    [
     router:make_event_target(is_friend, Owner, self()),
     router:make_event_target(make_friend, Owner, self()),
     router:make_event_target(get_friends, Owner, self()),
     router:make_event_source(sync_friend_list_add, Owner, self()),
     router:make_event_source(sync_friend_list_del, Owner, self()),
     router:make_event_source(?msg_req_eq_users_info, Owner, self()),
     router:make_event_source(?msg_req_add_friend, Owner, self()),
     router:make_event_source(?msg_req_friend_setting_black_someone, Owner, self()),
     router:make_event_source(?msg_req_friend_setting_accept_all, Owner, self()),
     router:make_event_source(?msg_req_friend_setting, Owner, self()),
     router:make_event_source(received_eq_packet, Owner, self()),
     router:make_event_source(req_eq_vcard, Owner, self()),
     router:make_event_source(?msg_req_remove_friend, Owner, self()),
     router:make_event_source(?msg_req_friend_list, Owner, self())
    ].


broadcast_friends(Account, Packet) when is_atom(Account) ->
    FriendList = case router:send2(Account, get_friends) of
		     List when is_list(List) ->
			 List;
		     _ ->
			 db_friends:get_friends(Account)
		 end,
    F = fun(FriendAccount) ->
		case player_is_active(FriendAccount) of
		    false ->
			ok;
		    _Pid ->
			net_helper:send2client(FriendAccount, Packet)			    
		end		
	end,
    [F(P) || P <- FriendList].



%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link(Owner, EQPid) when is_atom(Owner) ->
    gen_msgproc:start_link(?MODULE, [Owner, EQPid], []).

%%%===================================================================
%%% gen_server callbacks
%%%===================================================================

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Initializes the server
%%
%% @spec init(Args) -> {ok, State} |
%%                     {ok, State, Timeout} |
%%                     ignore |
%%                     {stop, Reason}
%% @end
%%--------------------------------------------------------------------

init([Owner, EQPid]) when is_atom(Owner) ->
    GMFriends = gm_friend:get_gm_friends(),
    FriendList = 
	case lists:member(Owner, GMFriends) of
	    false ->
		merage_friend_list(GMFriends, db_friends:get_friends(Owner));
	    _ -> [] %% GM 账号上线后看不到好友
	end,
    
    %% 通知朋友们，自已上线了
    broadcast_online(FriendList, Owner, get_user_name(Owner)),
    
    {ok, #state{owner=Owner, 
		friend_list=FriendList, 
		eq_pid=EQPid
	       }}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling call messages
%%
%% @spec handle_call(Request, From, State) ->
%%                                   {reply, Reply, State} |
%%                                   {reply, Reply, State, Timeout} |
%%                                   {noreply, State} |
%%                                   {noreply, State, Timeout} |
%%                                   {stop, Reason, Reply, State} |
%%                                   {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
%% 判断是否好友
handle_call({#msg{event=is_friend}, He}, _From, 	    
	    #state{friend_list=Friends, owner=Owner}=State)->  
    IsFriend =
	case gm_friend:is_friend(Owner, He) of 
	    true -> true;
	    _ ->
		lists:member(He, Friends)
	end,
    {reply, IsFriend, State};

handle_call(#msg{event=get_friends}, _From, 	    
	    #state{friend_list=Friends}=State)->  
    {reply, Friends, State};
 
handle_call({#msg{event=make_friend}, FriendAccount}, _From, 	    
	    #state{owner=Owner, eq_pid=EQPid}= State)->  
    NewState = do_add_eq_friend(Owner, atom_to_list(FriendAccount), State), 

    case eq_manager:is_login(EQPid) of
	true ->
	    FriendName = db_player:get_user_name(FriendAccount),
	    eq_manager:req_add_friend(EQPid, atom_to_list(FriendAccount), FriendName);
	_ ->
	    ok
    end,

    {reply, ok, NewState};

handle_call(_Request, _From, State) ->
    {reply, ok, State}.
%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling cast messages
%%
%% @spec handle_cast(Msg, State) -> {noreply, State} |
%%                                  {noreply, State, Timeout} |
%%                                  {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------

%% 接受邀请设定
handle_cast({_Msg, #req_friend_setting_accept_all{accept_all=AcceptAll}},
	     #state{owner=Owner}=State) ->    
    db_friends:set_accept_invite(Owner, AcceptAll),
    Setting = #friend_setting_record{accept_all_invite=AcceptAll},    

    NotifyPacket = #notify_friend_setting{setting=Setting},
    net_helper:send2client(Owner, NotifyPacket),
    
    {noreply, State};

%% 设定不接收某人的请求
handle_cast({_Msg, #req_friend_setting_black_someone{who=Who}},
	     #state{owner=Owner}=State) ->    
    db_friends:black_someone(Owner, Who),
    {noreply, State};

%% 好友设定
handle_cast({_Msg, #req_friend_setting{}},
	     #state{owner=Owner}=State) ->    

    AcceptInvite = db_friends:get_accept_invite(Owner),
    Setting = #friend_setting_record{accept_all_invite=AcceptInvite},    

    NotifyPacket = #notify_friend_setting{setting=Setting},
    net_helper:send2client(Owner, NotifyPacket),
	    
    {noreply, State};
%% 增加好友
handle_cast({_Msg, #req_add_friend{friend_account=Friend}},
	     #state{owner=Owner, friend_list=OldFriends}=State) ->
    FriendAccount = list_to_atom(Friend),

    %% 判断好友是否存在
    AddSelf = (FriendAccount =:= Owner),
    AlreadyExists = lists:member(FriendAccount, OldFriends),
    case can_add_friend(AddSelf, AlreadyExists) of
	true ->
	    %% 判断对方加好友设置
	    Setting = db_friends:get_setting(FriendAccount), 
	    AcceptInvite = db_friends:get_accept_invite(Setting),
	    case AcceptInvite of
		?accept_invite_yes ->
		    case player_is_active(FriendAccount) of
			true -> %% 走在线邀请流程
			    router:cast(Owner, req_invite_someone, {?ivt_make_friend, FriendAccount});
			_ -> %% 走离线邀请流程
			    %% 判断离线加好友设置
			    BlackList = db_friends:get_black_list(Setting),		    
			    case lists:member(Owner, BlackList) of
				false ->
				    db_friends:add_offline_invite(FriendAccount, Owner),
				    %% 提示请求发送成功
				    sys_msg:send(Owner, ?msg_send_make_friend_request);
				_ ->
				    sys_msg:send(Owner, ?err_other_reject_accept_friend)
			    end
		    end;
		_ ->
		    sys_msg:send(Owner, ?err_other_reject_accept_friend)
	    end;	    
	{false, Error} -> 
	    sys_msg:send(Owner, Error)
    end,
	    
    {noreply, State};
  
%% 删除好友
handle_cast({_Msg, #req_remove_friend{friend_account=Friend}},
	    #state{eq_pid=EQPid, owner=Account,
		   friend_list=OldFriends} = State) ->
    FriendAccount = list_to_atom(Friend),
    
    NewState = 
    case gm_friend:is_gm_account(FriendAccount) of
	true ->
	    sys_msg:send(Account, ?err_gm_friend_can_not_remove),
	    State;
	_ ->
	    case lists:member(FriendAccount, OldFriends) of
		false -> %% 不存在好友
		    sys_msg:send(Account, ?err_not_exists_friend),
		    State;
		true ->
		    State1 = do_del_eq_friend(Account, Friend, State),
		    case eq_manager:is_login(EQPid) of
			true ->
			    eq_manager:req_del_friend(EQPid, Friend);
			_ ->
			    ok
		    end,		    
		    State1
	    end
    end,

    {noreply, NewState};

%% 请求好友列表
handle_cast({_Msg, #req_friend_list{}}, #state{eq_pid=EQPid, 
					       owner=Account, 
					       friend_list=Friends} = State) ->    
    notify_friend_list(Account, Friends),
    case eq_manager:is_login(EQPid) of
	true ->	    
	    %% 请求35EQ好友
	    eq_manager:req_friend_list(EQPid),
	    %% 发布个性签名
	    %% (目前自已的个性签名为空)
	    Signature = "",
	    eq_manager:broadcast_signature(EQPid, Signature);
	_ ->
	    ok
    end,

    {noreply, State};

%% 请求EQ用户信息(批量请求)
handle_cast({_Msg, #req_eq_users_info{account_list=AccountList}}, 
	    #state{eq_pid=EQPid}=State) ->

    case eq_manager:is_login(EQPid) of
	true ->	    
	    %% 请求35EQ用户信息
	    [eq_manager:req_vcard(EQPid, "vcard_" ++ Who, Who) || Who <- AccountList];
	_ -> 
	    ok
    end,

    {noreply, State};

%% 请求EQ用户信息
handle_cast({#msg{event=req_eq_vcard}, Who}, #state{owner=Owner, eq_pid=EQPid}=State) 
  when is_list(Who) ->
    case eq_manager:is_login(EQPid) of
	true ->
	    case list_to_atom(Who) == Owner of
		true ->
		    UserInfo = eq_manager:get_user_info(EQPid),
		    NotifyPacket = #notify_eq_user_info{account=Who, info=UserInfo},
		    net_helper:send2client(Owner, NotifyPacket);
		false ->
		    %% 请求35EQ用户信息
		    eq_manager:req_vcard(EQPid, "vcard_" ++ Who, Who)
	    end;
	_ ->
	    ok
    end,
    {noreply, State};

%% 处理与EQ通讯获得的数据
handle_cast({#msg{event=received_eq_packet}, Packet}, 
	    State) ->
    RawPacket = eq_manager:get_raw_packet(Packet),
    %% RawPacketId的格式 add_xxx, del_xxx, list_xxx 
    RawPacketId = eq_manager:get_raw_packet_id(RawPacket),
    %% 分解RawPacketId, 使用分割符 _
    {Prefix, RawWho} = lists:splitwith(fun(A) -> A =/= $_ end, RawPacketId),
    
    NState = process_eq_message(Prefix, RawWho, Packet, State),
    {noreply, NState};

%% 同步好友列表
handle_cast({#msg{event=sync_friend_list_add}, FriendAccount}, 
	    #state{friend_list=OldFriends}=State) ->
    NewFriends = [FriendAccount | OldFriends],
    {noreply, State#state{friend_list=NewFriends}};

%% 同步好友列表
handle_cast({#msg{event=sync_friend_list_del}, FriendAccount}, 
	    #state{friend_list=OldFriends}=State) ->
    NewFriends = lists:delete(FriendAccount, OldFriends),    
    {noreply, State#state{friend_list=NewFriends}};


handle_cast(_Msg, State) ->
    {noreply, State}.
%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling all non call/cast messages
%%
%% @spec handle_info(Info, State) -> {noreply, State} |
%%                                   {noreply, State, Timeout} |
%%                                   {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
handle_info(_Info, State) ->
    {noreply, State}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% This function is called by a gen_server when it is about to
%% terminate. It should be the opposite of Module:init/1 and do any
%% necessary cleaning up. When it returns, the gen_server terminates
%% with Reason. The return value is ignored.
%%
%% @spec terminate(Reason, State) -> void()
%% @end
%%--------------------------------------------------------------------
terminate(_Reason, #state{owner=Owner, friend_list=FriendList}) ->
    %% 通知朋友们，自已下线了
    broadcast_offline(FriendList, Owner, get_user_name(Owner)),
    ok.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Convert process state when code is changed
%%
%% @spec code_change(OldVsn, State, Extra) -> {ok, NewState}
%% @end
%%--------------------------------------------------------------------
code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 取得朋友的信息
-spec get_friend_info(atom(), list()) -> tuple().
get_friend_info(FriendAccount, FriendName) when is_atom(FriendAccount) ->
    BasicData = db_player:select(once, FriendAccount),
    case BasicData of
	[] -> %% 有些人可能没开通EQ家园，所以数据为空
	    Online1 = false,
	    DefName = atom_to_list(FriendAccount),
	    NewFriendName = case FriendName of
				[] -> DefName;
				_ -> FriendName
			    end,
	    BasicData1 = #player_basic_data{account=DefName, username=util:filter_user_name(NewFriendName)},
	    #friend_info{basic_data=BasicData1, is_online=get_online(Online1)};
	 #player_basic_data{username=UserName} ->
	    NewFriendName = case FriendName of
				[] ->
				    case UserName of
					[] ->
					    atom_to_list(FriendAccount);
					_ ->
					    UserName
				    end;
				_ -> FriendName
			    end,

	    BasicData1 = BasicData#player_basic_data{username=util:filter_user_name(NewFriendName)},
	    Online = get_online(db_player:is_online(FriendAccount)),
	    #friend_info{basic_data=BasicData1, is_online=Online}
    end.


%% 1 表示在线，2 表示不在线
get_online(true) ->
    1;
get_online(false) ->
    2.

%% 玩家进程是否存在
-spec player_is_active(atom()) -> any().			    
player_is_active(Account) when is_atom(Account) ->
    util:is_process_alive(Account).

%% 对朋友列表进行广播
broadcast_online(FriendList, Account, UserName) when is_list(FriendList) ->
    Packet = #notify_friend_online{friend_account=atom_to_list(Account)},
    F = fun(FriendAccount) ->
		case player_is_active(FriendAccount) of
		    false ->
			ok;
		    _Pid ->
			sys_msg:send(FriendAccount, ?friend_online_notify, [UserName]),
			net_helper:send2client(FriendAccount, Packet)			    
		end		
	end,
    [F(P) || P <- FriendList].


%% 对朋友列表进行广播
broadcast_offline(FriendList, Account, UserName) when is_list(FriendList) ->
    Packet = #notify_friend_offline{friend_account=atom_to_list(Account)},

    F = fun(FriendAccount) ->
		case player_is_active(FriendAccount) of
		    false ->
			ok;
		    _Pid ->
			sys_msg:send(FriendAccount, ?friend_offline_notify, [UserName]),
			net_helper:send2client(FriendAccount, Packet)
		end		
	end,
    [F(P) || P <- FriendList].
    

%% 判断可否加为好友(非自身，没有重复加好友)
%% 返回值 true | {false, Error}
can_add_friend(AddSelf, AlreadyExists) ->
    case AddSelf of
	true -> {false, ?err_add_self_as_friend};
	false ->
	    case AlreadyExists of
		true -> {false, ?err_already_exists_friend};
		false ->
		    true
	    end
    end.

merage_friend_list(OldFriends, EQFriends) ->
    F = fun(X, Acc) -> 
		case lists:member(X, OldFriends) of
		    true ->
			Acc;
		    _ ->
			[X | Acc]
		end
	end,
    lists:foldl(F, OldFriends, EQFriends).

%% 返回好友列表(35EQ回复)
do_sync_eq_friend_list(Account, EQFriendList,
		       #state{friend_list=OldFriends}=State)
  when is_atom(Account) ->    
    EQFriends = [User || {User, _Name} <- EQFriendList],
    NewFriends = merage_friend_list(OldFriends, EQFriends),  
    db_friends:sync_friend_list(Account, NewFriends),
    case length(NewFriends) =/= length(OldFriends) of
	true ->  %% 如果好友列表有改变，才通知
	    notify_friend_list(Account, NewFriends);
	_ ->
	    ok
    end,

    State#state{friend_list=NewFriends}.

notify_friend_list(Account, FriendList) ->    
    FriendVec = [get_friend_info(X, []) || X <- FriendList],
    Packet = #notify_friend_list{friend_vec=FriendVec},
    net_helper:send2client(Account, Packet),
    %% 发送垃圾信息
    [house_magic_box:notify_friend_garbage_info(Account, X) || X <- FriendList].

%% 加好友(35EQ回复)
do_add_eq_friend(Account, Friend, 
		 #state{friend_list=OldFriends}=State) when is_atom(Account), is_list(Friend) ->
    FriendAccount = list_to_atom(Friend),
    
    %% 增加好友
    db_friends:add_friend(Account, FriendAccount),
    NewFriends = [FriendAccount | OldFriends],

    %% 通知加好友
    do_notify_add_friend(Account, FriendAccount, Friend),

    %% 通知加好友
    case player_is_active(FriendAccount) of
	true ->
	    do_notify_add_friend(FriendAccount, Account, atom_to_list(Account)),
	    router:cast(FriendAccount, sync_friend_list_add, Account);
	_ ->
	    ok
    end,

    State#state{friend_list=NewFriends}.

%% 发送好友个人信息
do_notify_add_friend(Account, FriendAccount, Friend) 
  when is_atom(Account), is_atom(FriendAccount) ->
    FriendInfo = get_friend_info(FriendAccount, []),
    Packet = #notify_add_friend{friend_account=Friend, friend_infos=FriendInfo}, 	    
    net_helper:send2client(Account, Packet),

    %% 发送垃圾信息
    house_magic_box:notify_friend_garbage_info(Account, FriendAccount),

    sys_msg:send(Account, ?msg_add_friend_success).


%% 删好友(35EQ回复)
do_del_eq_friend(Account, Friend, 
		 #state{friend_list=OldFriends}=State) when is_list(Friend) ->
    FriendAccount = list_to_atom(Friend),
    db_friends:remove_friend(Account, FriendAccount), 
    NewFriends = lists:delete(FriendAccount, OldFriends),

    %% 通知删除好友
    do_notify_remove_friend(Account, Friend),

    %% 通知删除好友
    case player_is_active(FriendAccount) of
	true ->
	    do_notify_remove_friend(FriendAccount, atom_to_list(Account)),
	    router:cast(FriendAccount, sync_friend_list_del, Account);
	_ ->
	    ok
    end,

    State#state{friend_list=NewFriends}.

%% 通知删除好友
do_notify_remove_friend(Account, Friend) when is_atom(Account), is_list(Friend) ->
    Packet = #notify_remove_friend{friend_account=Friend},    
    net_helper:send2client(Account, Packet).
    

%% 处理35EQ回复
%% 返回NewState
process_eq_message("list", _RawWho, _Packet, #state{owner=Account, eq_pid=EQPid}=State) ->
    RawFriendList = eq_manager:get_friend_list(EQPid),      

    EQFriendList = [{A, N} || {A, N} <- RawFriendList, db_player:is_exist(A)],
    case length(EQFriendList) > 0 of
	true ->
	    do_sync_eq_friend_list(Account, EQFriendList, State);
	false ->
	    error_logger:error_msg("~p has not friend list\n", [Account]),
	    State
    end;
process_eq_message("vcard", [$_ | Who], Packet, #state{owner=Account}=State) ->
    UserInfo = eq_manager:extract_user_info(Packet),
    case UserInfo of
	#eq_user_info{} ->
	    NotifyPacket = #notify_eq_user_info{account=Who, info=UserInfo},
	    net_helper:send2client(Account, NotifyPacket);
	_ ->
	    ok
    end,
    State;
process_eq_message(_Msg, _Who, _Packet, State) ->
    State.

get_user_name(Owner) ->
    case db_player:get_user_name(Owner) of
	[] -> atom_to_list(Owner);
	Name ->
	    Name
    end.


%%-----------------------------测试代码---------------------------------
-include_lib("eunit/include/eunit.hrl").

