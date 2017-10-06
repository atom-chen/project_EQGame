%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% EQ管理器
%%% @end
%%% Created : 23 Nov 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(eq_manager).

-behaviour(gen_server).

%% API
-export([start_link/0]).
-export([stop/1, login/4, logout/1, is_login/1]).
-export([req_vcard/2, req_vcard/3, received_vcard/2, 
	 req_friend_list/1, received_friend_list/2, get_friend_list/1,
	 req_add_friend/3, req_del_friend/2, set_account/2,
	 broadcast_signature/2, received_eq_presence/2,
	 get_user_info/1, extract_user_info/1,
	 get_user_name/1, get_nick_name/1]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-export([get_packet_type/1, get_raw_packet/1, get_raw_packet_id/1, get_raw_packet_type/1]).

-include("packet_def.hrl"). 
-include("enum_def.hrl").
-include("../lib/exmpp/include/exmpp_client.hrl").
-include("../lib/exmpp/include/exmpp_nss.hrl").
-include("../lib/exmpp/include/exmpp_xml.hrl").

-define(SERVER, ?MODULE). 

-record(state, {is_login = false,
		account,
		session = [],     %% 会话
		user_info = [],   %% 个人详细信息
		friend_list = []
	       }).

%%%===================================================================
%%% API
%%%===================================================================
stop(EQPid) ->
    gen_server:call(EQPid, stop).

login(farm, EQPid, Account, Password) ->
    gen_server:call(EQPid, {login_farm, Account, Password}, infinity);
login(eq, EQPid, Account, Password) ->
    gen_server:call(EQPid, {login_eq, Account, Password}, infinity).

logout(EQPid) ->
    gen_server:call(EQPid, logout).

is_login(EQPid) ->
    gen_server:call(EQPid, is_login).

get_packet_type(#received_packet{packet_type=PacketType}) ->
    PacketType.

get_raw_packet(#received_packet{raw_packet=RawPacket}) ->
    RawPacket.

get_raw_packet_id(RawPacket) ->
    RawPacketId = exmpp_xml:get_attribute(RawPacket, id, []),
    case RawPacketId == [] of
	true ->
	    "";
	false ->
	    binary_to_list(RawPacketId)
    end.

get_raw_packet_type(RawPacket) ->
    RawPacketType = exmpp_xml:get_attribute(RawPacket, type, []),
    case RawPacketType == [] of
	true ->
	    "";
	false ->
	    binary_to_list(RawPacketType)
    end.

%% 获取用户信息(自己)
req_vcard(EQPid, Id) ->
    gen_server:call(EQPid, {req_vcard, Id}).

%% 获取用户信息(他人)
req_vcard(EQPid, Id, OtherPlayer) when is_list(OtherPlayer) ->
    gen_server:call(EQPid, {req_vcard, Id, OtherPlayer}).

%% 发布个性签名
broadcast_signature(EQPid, Signature) when is_list(Signature) ->
    gen_server:cast(EQPid, {broadcast_signature, Signature}).

req_friend_list(EQPid) ->
    gen_server:cast(EQPid, req_friend_list).

req_add_friend(EQPid, FriendStr, FriendName) ->
    gen_server:cast(EQPid, {req_add_friend, FriendStr, FriendName}).

req_del_friend(EQPid, FriendStr) ->
    gen_server:cast(EQPid, {req_del_friend, FriendStr}).

get_friend_list(EQPid) ->
    gen_server:call(EQPid, get_friend_list).

received_friend_list(EQPid, Packet) ->
    gen_server:call(EQPid, {received_friend_list, Packet}).

received_eq_presence(EQPid, Packet) ->
    gen_server:cast(EQPid, {received_eq_presence, Packet}).

received_vcard(EQPid, Packet) ->
    gen_server:call(EQPid, {received_vcard, Packet}).

get_user_info(EQPid) ->
    gen_server:call(EQPid, get_user_info).

set_account(EQPid, Account) ->
    gen_server:call(EQPid, {set_account, Account}).
    
get_user_name(EQPid) ->
    gen_server:call(EQPid, get_user_name).

get_nick_name(EQPid) ->
    gen_server:call(EQPid, get_nick_name).
%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link() ->
    Session = exmpp_session:start(),
    gen_server:start_link(?MODULE, [Session], []).

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
init([Session]) ->
    {ok, #state{session=Session}}.

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
handle_call(stop, _From, #state{session=Session}=State) ->
    Reply = exmpp_session:stop(Session),
    {reply, Reply, State};
handle_call({start, Session}, _From, State) ->
    {reply, ok, State#state{session=Session, user_info=[]}};
handle_call({login_farm, Account, Password}, _From, #state{session=Session}=State) ->
    Reply =
	case db_player:is_exist(list_to_atom(Account)) of 
	    true ->
		case Password == [] of
		    true ->
			NeedPassword = common_def:get_val(is_account_login),
			case NeedPassword =:= 1 of
			    true -> {false, err_pwd};
			    _ -> {true, not_login_eq}
			end;
		    false ->
			login(Session, Account, Password)
		end;
	    _->
		{false, err_other}
	end,
    IsLogin = is_login_success(Reply),
    {reply, Reply, State#state{is_login=IsLogin, account=Account}};
handle_call({login_eq, Account, Password}, _From, #state{session=Session}=State) ->
    Reply =
	case Password == [] of
	    true ->
		NeedPassword = common_def:get_val(is_account_login),
		case NeedPassword =:= 1 of
		    true -> {false, err_pwd};
		    _ -> {true, not_login_eq}
		end;
	    false ->
		login(Session, Account, Password)
	end,
    IsLogin = is_login_success(Reply),
    {reply, Reply, State#state{is_login=IsLogin, account=Account}};
handle_call(logout, _From, State) ->
    {reply, ok, State#state{is_login=false}};
handle_call(is_login, _From, #state{is_login=IsLogin}=State) ->
    {reply, IsLogin, State};
handle_call({req_vcard, Id}, _From, #state{session=Session, is_login=IsLogin}=State) ->
    case IsLogin of
	true ->
	    Query = #xmlel{ns = ?NS_VCARD, name = 'vCard'},
	    Iq = exmpp_xml:set_attributes(
		   #xmlel{ns = ?NS_JABBER_CLIENT, name = 'iq'},
		   [{'type', "get"}, {'id', Id}]),
	    Packet = exmpp_xml:append_child(Iq, Query),
	    exmpp_session:send_packet(Session, Packet);
	_ -> ok
    end,
    {reply, ok, State};
handle_call({req_vcard, Id, OtherPlayer}, _From, #state{session=Session, is_login=IsLogin}=State) ->
    case IsLogin of
	true ->
	    Query = #xmlel{ns = ?NS_VCARD, name = 'vCard'},
	    Iq = exmpp_xml:set_attributes(
		   #xmlel{ns = ?NS_JABBER_CLIENT, name = 'iq'},
		   [{'type', "get"}, {'id', Id}, {'to', OtherPlayer}]),
	    Packet = exmpp_xml:append_child(Iq, Query),
	    exmpp_session:send_packet(Session, Packet);
	_ -> ok
    end,
    {reply, ok, State};


handle_call({received_vcard, #received_packet{raw_packet=RawPacket}=_Packet}, _From, State) ->
    Child1 = RawPacket#xmlel.children,
    UserInfo = 
	case length(Child1) > 0 of
	    true ->
		HD = hd(Child1),
		Childs = HD#xmlel.children,
		extract_user_info(Childs, #eq_user_info{});
	    _ ->
		[]
	end,
    {reply, ok, State#state{user_info=UserInfo}};
handle_call({received_friend_list, #received_packet{raw_packet=RawPacket}=_Packet}, _From, State) ->
    Child1 = RawPacket#xmlel.children,
    FriendList = 
	case length(Child1) > 0 of
	    true ->
		HD = hd(Child1),
		List = HD#xmlel.children,
		[{list_to_atom(binary_to_list(exmpp_xml:get_attribute(X, jid, <<>>))),
		  string:strip(binary_to_list(exmpp_xml:get_attribute(X, name, <<>>)))}
		 || X <- List];
	    false ->
		[]
	end,
    {reply, ok, State#state{friend_list=FriendList}};
handle_call(get_friend_list, _From, #state{friend_list=FriendList}=State) ->
    {reply, FriendList, State};
handle_call(get_user_info, _From, #state{user_info=UserInfo}=State) ->
    NUserInfo =
	case UserInfo == [] of
	    true ->
		#eq_user_info{};
	    false ->
		UserInfo
	end,
    {reply, NUserInfo, State};
handle_call(get_user_name, _From, #state{user_info=UserInfo, account=Account}=State) ->
    UserName = 
	case UserInfo == [] of
	    true ->
		Account;
	    false ->
		#eq_user_info{full_name=FullName, nick_name=NickName} = UserInfo,
		case NickName == [] of
		    true ->
			case FullName == [] of
			    true ->
				Account;
			    false ->
				FullName
			end;
		    false ->
			NickName
		end
	end,
    {reply, UserName, State};
handle_call(get_nick_name, _From, #state{user_info=UserInfo, account=Account}=State) ->
    NickName = 
	case UserInfo == [] of
	    true ->
		Account;
	    false ->
		UserInfo#eq_user_info.nick_name
	end,
    {reply, NickName, State};
handle_call({set_account, Account}, _From, State) ->
    {reply, ok, State#state{account=Account}};    
handle_call(_Request, _From, State) ->
    Reply = ok,
    {reply, Reply, State}.

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
handle_cast({received_eq_presence, #received_packet{raw_packet=RawPacket}}, 
	    #state{account=Account}=State) ->
    notify_eq_presence(RawPacket, list_to_atom(Account)),
    {noreply, State};
handle_cast(req_friend_list, #state{session=Session, is_login=IsLogin}=State) ->
    case IsLogin of
	true ->
	    Id = "list_friend",
	    Iq = exmpp_xml:set_attributes(
		   #xmlel{ns = ?NS_JABBER_CLIENT, name = 'iq'},
		   [{'type', "get"}, {'id', Id}]),
	    
	    Query = #xmlel{ns = ?NS_ROSTER, name = 'query'},
	    
	    Req = exmpp_xml:append_child(Iq, Query),
	    exmpp_session:send_packet(Session, Req);
	_ -> ok
    end,
    {noreply, State};
handle_cast({req_add_friend, FriendStr, FriendName}, 
	    #state{session=Session, is_login=IsLogin}=State) ->
    case IsLogin of
	true ->
	    Id = "add_" ++ FriendStr,
	    Iq = exmpp_xml:set_attributes(
		   #xmlel{ns = ?NS_JABBER_CLIENT, name = 'iq'},
		   [{'type', "set"}, {'id', Id}]),
	    
	    
	    Query = #xmlel{ns = ?NS_ROSTER, name = 'query'},

	    Item = exmpp_xml:set_attributes(#xmlel{name = 'item'},
					    [{'jid', FriendStr}, {'name', FriendName}]),
	    
	    Query2 = exmpp_xml:append_child(Query, Item),
	    
	    Req = exmpp_xml:append_child(Iq, Query2),
	    exmpp_session:send_packet(Session, Req);
	_ -> ok
    end,
    {noreply, State};
handle_cast({req_del_friend, FriendStr}, #state{session=Session, is_login=IsLogin}=State) ->
    case IsLogin of
	true ->
	    Id = "del_" ++ FriendStr,
	    Iq = exmpp_xml:set_attributes(
		   #xmlel{ns = ?NS_JABBER_CLIENT, name = 'iq'},
		   [{'type', "set"}, {'id', Id}]),
	    Query = #xmlel{ns = ?NS_ROSTER, name = 'query'},
	    Item = exmpp_xml:set_attributes(#xmlel{name = 'item'},
					    [{'jid', FriendStr}, 
					     {'subscription', "remove"}]),
	    Query2 = exmpp_xml:append_child(Query, Item),
	    Req = exmpp_xml:append_child(Iq, Query2),
	    exmpp_session:send_packet(Session, Req);
	 _ -> ok
    end,
    {noreply, State};
handle_cast({broadcast_signature, Signature}, #state{session=Session, is_login=IsLogin}=State)->
    case IsLogin of
	true ->
	    Presence = #xmlel{ns = ?NS_JABBER_CLIENT, name = 'presence'},
	    Status = exmpp_xml:set_cdata(#xmlel{name = 'status'},
					 Signature),
	    
	    Packet = exmpp_xml:append_child(Presence, Status),
	    
	    exmpp_session:send_packet(Session, Packet);
	_ -> ok
    end,
    {noreply, State};
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
terminate(_Reason, _State) ->
    data_helper:format("eq manager terminate~n"),
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
is_login_success(Reply) ->
    case Reply of
	{false, _Reason} ->
	    false;
	{true, not_login_eq} ->
	    false;
	_ ->
	    true
    end.

%% 登录
-spec login(pid(), list(), list()) -> tuple().
login(Session, Account, Password) ->
    AccountList = string:tokens(Account, "@"),
    data_helper:format("Accountlist:~p~n", [AccountList]),
    case length(AccountList) > 1 of
	true ->
	    NAccount = lists:nth(1, AccountList),
	    Diamon = lists:nth(2, AccountList),
	    login(Session, NAccount, Password, Diamon, 5222);
	false ->
	    {false, err_account}
    end.

login(Session, Account, Password, Diamon, Port) when Diamon == "35info.cn" ->
    login(Session, Account, Password, Diamon, "mail.35info.cn", Port);
login(Session, Account, Password, Diamon, Port) when Diamon == "35.cn" ->
    login(Session, Account, Password, Diamon, "218.85.134.240", Port);
login(_Session, _Account, _Password, _Diamon, _Port) ->
    {false, err_account}.

-spec login(pid(), list(), list(), list(), list(), integer()) -> tuple().
login(Session, Account, Password, Diamon, IP, Port) ->
    try 
    	Jid = exmpp_jid:make(Account, Diamon, random),
    	exmpp_session:auth_basic_digest(Session, Jid, Password),
    	exmpp_session:connect_TCP(Session, IP, Port),
    	exmpp_session:login(Session)
    catch
	throw:{auth_error, 'not-authorized'} ->
	    exmpp_session:stop(Session),
	    {false, err_other};
    	_:_ ->
	    exmpp_session:stop(Session),
    	    {false, err_login}
    end.


%% 取EQ用户信息
extract_user_info(#received_packet{raw_packet=RawPacket}) ->
    Child1 = RawPacket#xmlel.children,
    UserInfo = 
	case length(Child1) > 0 of
	    true ->
		HD = hd(Child1),
		Childs = HD#xmlel.children,
		extract_user_info(Childs, #eq_user_info{});
	    _ ->
		[]
	end,
    UserInfo.


%% 取EQ用户信息
extract_user_info([], UserInfo) ->
    UserInfo;
%% 全名
extract_user_info([#xmlel{name='FN', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{full_name=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 昵称
extract_user_info([#xmlel{name='NICKNAME', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{nick_name=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 生日
extract_user_info([#xmlel{name='BDAY', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{birthday=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 职位
extract_user_info([#xmlel{name='TITLE', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{title=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 角色
extract_user_info([#xmlel{name='ROLE', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{role=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 个人主页
extract_user_info([#xmlel{name='URL', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{url=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 个人简介
extract_user_info([#xmlel{name='DESC', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{desc=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 国家
extract_user_info([#xmlel{name='CTRY', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{country=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 省
extract_user_info([#xmlel{name='REGION', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{region=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 市
extract_user_info([#xmlel{name='LOCALITY', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{city=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 街道
extract_user_info([#xmlel{name='STREET', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{street=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 邮箱
extract_user_info([#xmlel{name='USERID', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{email=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 手机
extract_user_info([#xmlel{name='NUMBER', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{phone=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 公司
extract_user_info([#xmlel{name='ORGNAME', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{org_name=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 部门
extract_user_info([#xmlel{name='ORGUNIT', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{org_unit=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 图片数据格式
extract_user_info([#xmlel{name='TYPE', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = UserInfo#eq_user_info{photo_type=xmlcdata_to_string(Children)},
    extract_user_info(Rest, NewUserInfo);
%% 图片数据
extract_user_info([#xmlel{name='BINVAL', children=Children} | Rest], UserInfo) ->    
    %% BinList = [Bin || {xmlcdata, Bin} <- Children],
    %% InputData = list_to_binary(BinList),
    %% Pic = base64:decode(InputData),
    %% PhotoData = binary_to_list(Pic),

    %% base64解码改为在客户端处理
    PhotoData = xmlcdata_to_string(Children), 
    NewUserInfo = UserInfo#eq_user_info{photo_data=PhotoData},
    extract_user_info(Rest, NewUserInfo);
%% 地址相关
extract_user_info([#xmlel{name='ADR', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = extract_user_info(Children, UserInfo),
    extract_user_info(Rest, NewUserInfo);
%% 电话相关
extract_user_info([#xmlel{name='TEL', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = extract_user_info(Children, UserInfo),
    extract_user_info(Rest, NewUserInfo);
%% 公司相关
extract_user_info([#xmlel{name='ORG', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = extract_user_info(Children, UserInfo),
    extract_user_info(Rest, NewUserInfo);
%% 邮箱相关
extract_user_info([#xmlel{name='EMAIL', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = extract_user_info(Children, UserInfo),
    extract_user_info(Rest, NewUserInfo);
%% 头像图片相关
extract_user_info([#xmlel{name='PHOTO', children=Children} | Rest], UserInfo) ->    
    NewUserInfo = extract_user_info(Children, UserInfo),
    extract_user_info(Rest, NewUserInfo);
%% 其它无用数据
extract_user_info([_OtherData | Rest], UserInfo) ->    
    extract_user_info(Rest, UserInfo).

%% 提取字符串
xmlcdata_to_string(List) when is_list(List) ->
    F = fun({xmlcdata, Bin}, Str) ->
		Str ++ binary_to_list(Bin)
	end,
    Result = lists:foldl(F, [], List),
    string:strip(Result).

%% 取签名信息
get_signature(RawPacket) ->
    case exmpp_xml:get_element(RawPacket, status) of
	#xmlel{name=status, children=Children} ->
	    RawWho = binary_to_list(exmpp_xml:get_attribute(RawPacket, from, <<>>)),
    	    {Who, _PCInfo} = lists:splitwith(fun(A) -> A =/= $/ end, RawWho),

	    Signature = xmlcdata_to_string(Children),
	    {Who, Signature};
	_ -> %% 没找到签名信息
	    undefined
    end.
    
%% 通知个性签名
notify_eq_presence(RawPacket, Account) when is_atom(Account) ->
    case get_signature(RawPacket) of
	{Who, Signature} ->
	    NotifyPacket = #notify_eq_user_presence{account=Who, presence=Signature},
	    net_helper:send2client(Account, NotifyPacket);
	_ ->
	    ok
    end.
