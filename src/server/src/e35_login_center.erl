%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   E号通登陆模块
%%% @end
%%% Created :  9 Sep 2011 by hongjx <hongjx@35info.cn>

-module(e35_login_center).

-export([login/3, login/9]).


%%%===================================================================
%%% 接口
%%%===================================================================
login(User, Password, ClientIP) ->
    GetTicketUrl = common_def:get_val(get_ticket_url),
    ValidTicketUrl = common_def:get_val(valid_ticket_url), 	    
    TicketSPID = common_def:get_val(server_ticket_spid),
    TicketKey = common_def:get_val(server_ticket_key),
    AreaID = common_def:get_val(server_area_id),
    ServerID = common_def:get_val(server_id),
    login(User, Password, ClientIP, 
			   GetTicketUrl, ValidTicketUrl,
			   TicketSPID, TicketKey, AreaID, ServerID).


login(User, Password, ClientIP, GetTicketUrl, ValidTicketUrl, 
      TicketSPID, TicketKey, AreaID, ServerID) 
  when is_list(User), is_list(Password) ->
    inets:start(),

    case Password of
	"" ->
	    {false, err_pwd};
	_ ->
	    %% 取网卡地址
	    SEV_MAC = get_server_mac(),
	    
	    %% 取票
	    case get_ticket(User, Password, SEV_MAC, GetTicketUrl, TicketSPID, TicketKey) of
		{error, Reason} ->
		    {false, Reason};
		Ticket ->
	    %% 验票
		    verify_ticket(Ticket, User, SEV_MAC, ClientIP, ValidTicketUrl, AreaID, ServerID)
	    end
    end.

%%%===================================================================
%%% 内部函数
%%%===================================================================
%% 取得正文
get_body({ok, {{_Version, 200, _ReasonPhrase}, _Headers, Body}}) ->
    Body;
get_body(Error) ->
    {error, Error}.

%% 取票
get_ticket(User, Password, SEV_MAC, Url, TicketSPID, TicketKey) ->
    SPID = TicketSPID,
    RawKey = TicketKey,
    Key = lists:sublist(RawKey, 1, 8), %% des 加密，只用前8位
    DES_PWD = binary_to_list(base64:encode(des_encode(Key, Password))),
    GL_TIME = get_gl_time(),
    Md5 = integer_to_list(SPID) ++ User ++ DES_PWD ++ 
			   SEV_MAC ++ integer_to_list(GL_TIME) ++ RawKey,

    SIGN = get_md5_sign(Md5),

    
    Args = [
	    {"SPID", SPID},
	    {"ACCOUNT", User},
	    {"SIGN", SIGN},
	    {"DES_PWD", DES_PWD},
	    {"GL_TIME", GL_TIME},
	    {"SEV_MAC", SEV_MAC}
	   ],


    Json = json_encode(Args),

    Base64 = binary_to_list(base64:encode(Json)),

    Req = Url ++ "?b64js=" ++ Base64,
    
    ReqResult = httpc:request(Req),

    Ticket = get_body(ReqResult),

    Ticket.

%% 验票
verify_ticket(Ticket, User, SEV_MAC, {IPA, IPB, IPC, IPD}, Url, AreaID, ServerID) ->

    CL_IP = integer_to_list(IPA) ++ "." ++
	integer_to_list(IPB) ++ "." ++
	integer_to_list(IPC) ++ "." ++
	integer_to_list(IPD), 

    Args = [
	    {"TICKET", Ticket},
	    {"SEV_MAC", SEV_MAC},
	    {"CL_IP", CL_IP},
	    {"AREA_ID", AreaID},
	    {"SERVER_ID", ServerID}
	   ],


    Json = json_encode(Args),

    Base64 = binary_to_list(base64:encode(Json)),

    Req = Url ++ "?b64js=" ++ Base64,
    
    ReqResult = httpc:request(Req),


    case get_body(ReqResult) of
	{error, Reason} ->
	    {false, Reason};			
	VerifyResult ->
	    KVList = get_key_values(VerifyResult),
	    %%io:format("KVList: ~p~n~n~n", [KVList]),
	    %% 转换返回值
	    convert_result(KVList, User)
    end.

    

%%从0001-01-01
%% 00:00:00起到当前格林标准时间相隔的100纳秒数
get_gl_time() ->
    K366 = 366 * 24 * 3600,
    TotalSeconds = calendar:datetime_to_gregorian_seconds(calendar:universal_time()) - K366,    
    MMS = 1000 * 1000 * 10,
    TotalSeconds * MMS + element(3, now()) * 10.

%% 取服务器MAC
get_server_mac() ->
    {ok, B} =  inet:getifaddrs(),
    C = lists:nth(1, B),
    D = element(2, C),
    case lists:keyfind(hwaddr, 1, D) of
	{hwaddr, RawMac} ->
	    List = list_to_hex(RawMac),
	    F = fun(X, Acc) -> [X, $- | Acc] end,
	    K = lists:flatten(lists:foldr(F, [], List)),    
	    lists:sublist(K, 1, length(K) - 1);
	_ ->
	    ""
    end.

%% 转换返回值
convert_result(KVList, _User) ->
    Authenticated = "Authenticated",
    case lists:keyfind(Authenticated, 1, KVList) of
	{_Key, "0"} -> %% 未通过验证
	    FailedCode = "FailedCode",
	    case lists:keyfind(FailedCode, 1, KVList) of
		{_Key1, "1"} -> %% 用户不存在
		    {false, err_account};
		{_Key1, "4"} -> %% 普通密码不对
		    {false, err_pwd};
		_ ->
		    {false, err_other}
	    end;
	{_Key, "1"} -> %% 通过验证
	    %% 核对用户名是否一致, 不一样就报错
	    %%{_, User} = 
	    lists:keyfind("AccountName", 1, KVList),

	    IsRestricted = "IsRestricted",
	    case lists:keyfind(IsRestricted, 1, KVList) of
		{_Key2, "1"} -> %% 该用户未通过实名认证
		    {true, is_wallow_account};
		_ -> 
		    true
	    end
    end.

%% 提取字典
get_key_values(VerifyResult) when is_list(VerifyResult) ->
    List = string:tokens(VerifyResult, "|"),
    F = fun(X, Acc) -> 
		[A, B] = string:tokens(X, ":"),
		[{A, B} | Acc]
	end,
    lists:foldl(F, [], List).


%% 生成json格式文本
json_encode(List) when is_list(List) ->
    F = fun({Key, Val}, Acc) ->
		Acc ++ encode_val(Key) ++ ":" ++ encode_val(Val) ++ "," 		 
	end,
    SMid = lists:foldl(F, [], List),
    S = case length(SMid) > 0 of
	    true -> lists:sublist(SMid, 1, length(SMid) - 1);
	     _ -> SMid
	end,
    "{" ++ S ++ "}".

encode_val(Val) when is_list(Val) ->
    "\"" ++ Val ++ "\"";
encode_val(Val) when is_integer(Val) ->
    integer_to_list(Val).


%%%===================================================================
%%% md5
%%%===================================================================
%% 取md5字串
get_md5_sign(S) ->            
  Md5_bin =  erlang:md5(S),   
  Md5_list = binary_to_list(Md5_bin),
  lists:flatten(list_to_hex(Md5_list)).  

list_to_hex(L) ->  
  lists:map(fun(X) -> int_to_hex(X) end, L).  

int_to_hex(N) when N < 256 ->     
  [hex(N div 16), hex(N rem 16)].
 
hex(N) when N < 10 ->     
  $0+N; 
hex(N) when N >= 10, N < 16 -> 
   $a + (N-10). 


%%%===================================================================
%%% des
%%%===================================================================
des_encode(Key, Text) ->
  des_encode(fun crypto:des_ecb_encrypt/2, Key, Text).
%% des_decode(Key, Text) ->
%%   des_decode(fun crypto:des_ecb_decrypt/2, Key, Text).

des_encode(F, Key, Text) ->
  des_encode(F, Key, Text, []).
%% des_decode(F, Key, Text) ->
%%   Str = des_encode(F, Key, Text),
%%   remove_zero_tail(Str).

des_encode(_F, _Key, [], Result) ->
  Result;
des_encode(F, Key, Text, Result) when length(Text) >= 8 ->
  {Byte8, Tail} = lists:split(8, Text),
  OutStr = binary_to_list(F(Key, Byte8)),
  des_encode(F, Key, Tail, lists:append(Result, OutStr));
des_encode(F, Key, Text, Result) ->
  Tail = [0 || _X <- lists:seq(1, 8 - length(Text))], 
  Byte8 = lists:append(Text, Tail),
 	OutStr = binary_to_list(F(Key, Byte8)),
  lists:append(Result, OutStr).

%% %% 去掉0结尾符
%% remove_zero_tail(Str) ->
%%   case lists:last(Str) =:= 0 of
%%   	true -> remove_zero_tail(lists:sublist(Str, 1, length(Str) - 1));
%%   	_ -> Str
%%   end.
