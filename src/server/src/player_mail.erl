%%% @author hongjx <hongjx@china-channel.com>
%%% @copyright (C) 2010, hongjx
%%% @doc
%%%   邮件
%%%      注: 1. 服务端信任客户端的邮件状态
%%%          2. 邮件自动删除时机: 新增邮件时, 请求邮件列表时 
%%%          3. player_data#mail_count记录客户端邮件数(是个参考值, 只关心删除)
%%% @end
%%% Created :  2 Jul 2010 by hongjx <hongjx@china-channel.com>

-module(player_mail).


-include("packet_def.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").
-include("tplt_def.hrl").


%% API
-export([start/1, notify_login/1]).
-export([send_sys_mail/4, send_gm_mail/4, send_gm_mail/5]).

%% gen_server callbacks
-export([handle_call/3, handle_cast/2]).

-define(table_name, player_mail).

-record(player_mail, {
	  id, 
	  reciever,       %% 收信人账号
	  send_time,      %% 发信时间
	  sender,         %% 发信人账号
	  type,           %% 类型
	  title,          %% 标题
	  content,        %% 内容
	  items,          %% 物品
	  read_status,    %% 状态（1.未读，2.看过标题，3.看过内容)
	  recver_name,
	  sender_name
	 }
       ).


start(Account) -> 
    [
     router:make_event_source(dec_mail_count, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_mail_list, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_send_mail, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_get_mail_item, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_mail_content, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_reject_mail, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_know_new_mail_title, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_delete_mail, Account, {self(), ?MODULE})
    ].


%%%===================================================================
%%% API
%%%===================================================================
%% 上线处理
notify_login(State) ->
    Account = player_data:get_account(State),
    Mails = db_mail:get_player_mails(Account),
    F = fun(#player_mail{read_status=Status}) -> 
		   Status =:= ?mrs_not_read 
	end,
    case lists:any(F, Mails) of
	true ->
	    %% 通知客户端, 有未读邮件
	    net_helper:send2client(Account, #notify_mail_not_read{});
	_ -> ok
    end.
    
%% 发送系统邮件
%% @MailItems = [#pack_grid{}]
send_sys_mail(RecverList, Title, Content, MailItems) when is_list(RecverList), 
							  is_list(MailItems) ->
    send_sys_mail_2(RecverList, Title, Content, MailItems, ?mt_system_mail, "");
send_sys_mail(Recver, Title, Content, MailItems) when is_atom(Recver),
						      is_list(MailItems) ->
    send_sys_mail_1(Recver, Title, Content, MailItems, ?mt_system_mail, "").

%% 发送gm邮件
send_gm_mail(RecverList, Title, Content, MailItems) when is_list(RecverList) ->
    [send_gm_mail(Recver, Title, Content, MailItems) || Recver <- RecverList];
send_gm_mail(Recver, Title, Content, MailItems) ->
    send_sys_mail_1(Recver, Title, Content, MailItems, ?mt_gm_mail, "").

%% 发送gm邮件
send_gm_mail(Recver, Title, Content, MailItems, GMAccount) when is_list(GMAccount) ->
    send_sys_mail_1(Recver, Title, Content, MailItems, ?mt_gm_mail, GMAccount).




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
%% 更新邮件
handle_cast({#msg{event=dec_mail_count}, NDel}, State) ->
    OldMailCount = player_data:get_mail_count(State),
    NewMailCount = OldMailCount - NDel,
    MaxCanSeeMail = get_max_can_see_mail(),
    ClientMailCount = 
     	case (NewMailCount < MaxCanSeeMail) and (MaxCanSeeMail =< OldMailCount) of
     	    false -> 	  
     		NewMailCount;
     	    _ -> %% 可见邮件数不够，重新发送邮件列表
		Account = player_data:get_account(State),
      		N = do_notify_mail_list(Account),		
     		N
     	end,

    NewState = player_data:set_mail_count(ClientMailCount, State),
    {noreply, NewState};



%% 请求邮件列表
handle_cast({_Msg, #req_mail_list{}}, State) ->    
    Account = player_data:get_account(State),
    {_DelIDList, RejectMails} = db_update_mail(Account),
    
    %% 通知客户端 新邮件(退信)
    [send2other(Recver, #notify_add_mail{mail_data=get_mail_info(M)}) 
     || #player_mail{reciever=Recver}=M <- RejectMails],

    MailCount = do_notify_mail_list(Account),    
    NewState = player_data:set_mail_count(MailCount, State),
    {noreply, NewState};
%% 删除邮件
handle_cast({_Msg, 
	     #req_delete_mail{id=ID}}, State) ->
    Account = player_data:get_account(State),

    %% 判断邮件是否是玩家的
    CanDelete = exists_mail(Account, ID),
    case CanDelete of
	false -> 
	    sys_msg:send(Account, ?err_mail_not_exists),
	    {noreply, State};
	true ->
	    NewState = do_delete_mail(ID, Account, State),
	    {noreply, NewState}		    
    end;
%% 读邮件内容
handle_cast({_Msg, 
	     #req_mail_content{id=ID}}, State) ->
    Account = player_data:get_account(State),
    case get_mail(Account, ID) of
	[] -> sys_msg:send(Account, ?err_mail_not_exists);
	Mail ->
	    %% 取邮件内容
	    #player_mail{content=Content} = Mail,

	    %% 数据库操作，标上已读状态
	    set_read_status(?mrs_read_content, [Mail]), 
	    %% 通知客户端
	    net_helper:send2client(Account, #notify_mail_content{id=ID, content=Content})	    
    end,
    {noreply, State};
%% 客户端通知服务端看过邮件标题
handle_cast({_Msg, 
	     #req_know_new_mail_title{id_list=IDList}}, State) -> 
    Account = player_data:get_account(State),
    %% 取得相关邮件
    RawMails = [get_mail(Account, ID) || ID <- IDList],
    Mails = [X || X <- RawMails, X /= []], 
    %% 数据库操作, 标上已读状态
    set_read_status(?mrs_read_title, Mails),
    {noreply, State};
%% 退信
handle_cast({_Msg, 
	     #req_reject_mail{id=ID}}, State) ->
    Account = player_data:get_account(State),  
    
    Mail = get_mail(Account, ID),
    case can_reject_mail(Mail) of
	{false, Reason} -> 
	    sys_msg:send(Account, Reason),
	    {noreply, State};
	_ ->
	    %% 取得原来发信方
	    #player_mail{sender=TargetAccount} = Mail,

	    %% 数据库退信处理
	    NewMail = build_reject_mail(Mail),
	    db_mail:write(NewMail),	    
	    

	    %% 通知退信成功
	    net_helper:send2client(Account, #notify_reject_mail_success{id=ID}),

	    NewState = after_delete_mail(Account, State), 

	    %% 通知原来发信方
	    send2other(TargetAccount, 
		       #notify_add_mail{mail_data=get_mail_info(NewMail)}),
	    
	   
	    %% 更新邮件, 自动删除过期邮件
	    update_mail(TargetAccount),
	    {noreply, NewState}
    end;
%% 取物品
handle_cast({_Msg, 
	     #req_get_mail_item{id=ID, inst_id=InstID}}, State) ->
    Account = player_data:get_account(State),    
    Mail = get_mail(Account, ID),
    Bag = player_data:get_pack(?pt_bag, State),
    
    case try_get_mail_item(Mail, InstID, Bag) of
	{false, Reason} -> 
	    sys_msg:send(Account, Reason),
	    {noreply, State};	    
	{NewBag, NewMailItems, Item, ItemCount} ->
	    %% 增加背包物品 
	    NewPlayerPack = player_pack:make_package(Account, ?pt_bag, NewBag),
	    OpAddBagItem = {write, [NewPlayerPack]},
	    NewState = player_data:set_pack(?pt_bag, NewBag, State),		    
	    
	    %% 删除邮件物品
	    NewMail = Mail#player_mail{items=NewMailItems},
	    OpDelMailItem = {write, [NewMail]},
	    
	    %% 数据库操作: 删除邮件物品, 增加背包物品
	    db_mail:change([OpAddBagItem, OpDelMailItem]),
	    
	    %% 通知删除邮件物品
	    net_helper:send2client(Account, #notify_delete_mail_item{id=ID, inst_id=InstID}),

	    %% 通知增加背包物品
	    notify_add_item(Account, NewState),
	    

	    %% 如果邮件没标题，没内容，没物品，则删除之
	    MailNotItems = (NewMailItems =:= []),
	    #player_mail{title=Title, content=Content, sender=Sender} = NewMail,
	    CanDelete = MailNotItems and (length(Title) =:= 0) and (length(Content) =:= 0), 

	    %% 后台金钱物品记录
	    ItemInfo = {item:get_tempid(Item), ItemCount, item:get_instid(Item)},
	    item_money_log:log_item(Account, get_mail_item, ItemInfo, Sender),
	    
	    %% 提示获得物品
	    sys_msg:send(Account, ?msg_get_item, [item:get_item_name(Item), ItemCount]),

	    case CanDelete of
		true -> 
		    State1 = do_delete_mail(ID, Account, NewState),

		    {noreply, State1};
		false ->
		    {noreply, NewState}
	    end		       
    end;

%% 发送邮件
handle_cast({_Msg, 
	     #req_send_mail{recver_name=RecverName, title=Title,
			    recver_account=RawAccount,
			    content=Content, items=ItemIDs}}, State) ->
    Account = player_data:get_account(State),
    BasicData = player_data:get_basic_data(State),
    SenderName = basic_data:get_data(username, BasicData), 

    %% 取收信人账号
    TargetAccount = get_recver_account(RawAccount, RecverName),

    Bag = player_data:get_pack(?pt_bag, State),
    
    %% 根据实例ID, 取得物品所在位置
    IndexList = get_item_index_list(ItemIDs, Bag),

    case can_send_mail(Account, TargetAccount, Title, Content, 
		       ItemIDs, IndexList, Bag) of
	{false, Reason} -> 
 	    sys_msg:send(Account, Reason),
	    {noreply, State};
	true ->
	    %% 增加新邮件
	    FAddItem = fun(Idx, AList) ->
			       Item = pack:get_item(Idx, Bag),
			       NewItem = item:cut_trade_time(Item), 
			       ItemCount = pack:get_item_count(Idx, Bag),
			       Grid = #pack_grid{count=ItemCount, item_data=NewItem},
			       [Grid | AList]
		       end,
	    MailItems = lists:foldl(FAddItem, [], IndexList),

	    Mail = #player_mail{id=fetch_new_mail_id(),
				sender_name=SenderName,
				send_time=datetime:local_time(),
				sender=Account,
				recver_name=RecverName,
				reciever=TargetAccount,
				title=Title,
				content=Content,
				items=MailItems,
				read_status=?mrs_not_read,
				type=?mt_person_mail			
			       },

	    NewState = 
		case length(IndexList) > 0 of
		    true ->
			%% 删除背包物品
			FDelItem = fun(Idx, TmpBag) ->
					   pack:set_item(empty, Idx, TmpBag)			       
				   end,
			NewBag = lists:foldl(FDelItem, Bag, IndexList),
			State1 = player_data:set_pack(?pt_bag, NewBag, State),
			
			NewPlayerPack = player_pack:make_package(Account, ?pt_bag, NewBag),
			%% 数据库操作
			OpDelItems = {write, [NewPlayerPack]},
			OpAddMail = {write, [Mail]},
			db_mail:change([OpDelItems, OpAddMail]),
			
			%% 通知删除物品
			notify_del_items(Account, State1),

			%% 后台金钱物品记录
			ItemInfo = [begin
					DelItem = pack:get_item(Idx, Bag),
					{item:get_tempid(DelItem),
					-pack:get_item_count(Idx, Bag),
					item:get_instid(DelItem)}
				    end || Idx <- IndexList],
			item_money_log:log_item(Account, send_mail, ItemInfo, TargetAccount),

			State1;
		    false -> 
			%% 数据库操作
			db_mail:write(Mail),		    
			State
		end,
	    

	    %% 通知新邮件
	    send2other(TargetAccount, 
				   #notify_add_mail{mail_data=get_mail_info(Mail)}),	    

	    %% 通知发邮件成功
	    net_helper:send2client(Account, #notify_send_mail_success{}),

	    %% 更新邮件, 自动删除过期邮件
	    update_mail(TargetAccount),
	    {noreply, NewState}
    end;
handle_cast(_Msg, State) ->
    {noreply, State}.


%%%===================================================================
%%% 内部函数
%%%===================================================================
%% 判断可否发送邮件
can_send_mail(Account, TargetAccount, Title, Content, 
	      ItemIDs, IndexList, Bag) when is_atom(Account),
					    is_atom(TargetAccount) ->

    case TargetAccount =:= undefined of
	true -> {false, ?err_mail_reciever_not_exists}; %% 找不到收件人
	false ->	    
	case Account =:= TargetAccount of
	    true -> {false, ?err_can_not_mail_to_self}; %% 不能寄给自已
	    false -> 	    
	    case util:char_count(get_effect_title(Title)) > common_def:get_val(max_mail_title_length) of
	    	true -> {false, ?err_mail_title_too_long};
	    	false ->
                case util:char_count(Content) > common_def:get_val(max_mail_content_length) of
	    	    true -> {false, ?err_mail_content_too_long};
	    	    false ->
	    		can_send_mail_1(ItemIDs, IndexList, Bag)
	    	end
	    end
	end
    end.


%% 判断邮件标题是否非法
get_effect_title(Title) ->
    Head = "re:",
    HeadSize = length(Head),
    case string:str(Title, Head) =:= 1 of
	true -> get_effect_title(string:sub_string(Title, HeadSize + 1));
	false -> Title
    end.


can_send_mail_1(ItemIDs, IndexList, Bag) when is_list(ItemIDs),
					      is_list(IndexList) ->
    case length(ItemIDs) /= length(IndexList) of
	true -> {false, ?err_mail_item_not_exists};
	false -> 
	case length(IndexList) > common_def:get_val(max_mail_item_count) of
	    true -> {false, ?err_mail_item_full};   %% 邮寄物品个数超限
	    false ->
		F = fun(Idx) ->
			    can_trade(Idx, Bag) =/= true				    
		    end,
		case lists:any(F, IndexList) of
		    true -> {false, ?err_item_bind_can_not_trade};
		    false -> true
		end
	end
    end.



%% 取邮件信息
get_mail_info(#player_mail{id=ID, type=Type, read_status=ReadStatus, 
			   send_time=SendTime, sender_name=SenderName, 
			   sender=SenderAccount, title=Title, items=Items}) ->
    %% 有效期
    EffectSeconds = datetime:datetime_to_gregorian_seconds(SendTime) + get_max_mail_second(),
    EffectTime = datetime:gregorian_seconds_to_datetime(EffectSeconds),

    MailItems = Items,
    Now = datetime:local_time(),
    {D, {H, M, S}} = datetime:time_difference(Now, EffectTime),
    %% 算剩余时间(秒)
    RemainTime0 = (D * 24 + H) * 3600 +  M * 60 +  S,
    RemainTime = 
	case RemainTime0 < 0 of 
	    true -> 0; 
	    _ -> RemainTime0 
	end,

    SenderAccountStr = case SenderAccount of
			   undefined -> "";
			   _ -> atom_to_list(SenderAccount)
		       end,
    #mail_info{id=ID, 
	       type=Type, 
	       read_status=ReadStatus, 
	       remain_time=RemainTime, 
	       sender_name=SenderName, 
	       sender_account=SenderAccountStr, 
	       items=MailItems,
	       title=Title}.


%% 设置邮件状态（0.未读，1.看过标题，2.看过内容)
-spec set_read_status(integer(), list()) -> any().
set_read_status(NewStatus, Mails) when is_integer(NewStatus),
				       is_list(Mails) ->     
    NewMails = [Mail#player_mail{read_status=NewStatus} || 
		 #player_mail{read_status=OldStatus}=Mail <- Mails, 
		 OldStatus < NewStatus],
    OpList = [{write, NewMails}],
    db_mail:change(OpList).    

%% 自动生成新邮件id
fetch_new_mail_id() ->
    guid:make(?st_mail).

%% 取某封邮件 
-spec get_mail(atom(), integer()) -> any().
get_mail(Account, ID) when is_atom(Account),
			   is_integer(ID) ->
    case db_mail:select_mail(ID) of
	#player_mail{reciever=RecvAccount}=Mail -> 
	    case RecvAccount =:= Account of
		true -> Mail;
		false -> []
	    end;
	_Other -> 
	    []
    end.

%% 判断邮件是否存在
-spec exists_mail(atom(), integer()) -> boolean().
exists_mail(Account, ID) when is_atom(Account),
			      is_integer(ID) ->
    get_mail(Account, ID) /= [].


%% 更新邮件，该删的删，该退的退
update_mail(Account) when is_atom(Account) ->
    {DelIDList, RejectMails} = db_update_mail(Account),
    
    %% 通知客户端 新邮件(退信)
    [send2other(Recver, #notify_add_mail{mail_data=get_mail_info(M)}) 
     || #player_mail{reciever=Recver}=M <- RejectMails],

    case util:is_process_alive(Account) of
	true ->
	    %% 通知客户端 删邮件
	    [net_helper:send2client(Account, #notify_delete_mail{id=ID}) 
	     || {_, ID} <- DelIDList],

	    NDel = length(DelIDList),
	    case NDel > 0 of
		true ->
		    %% 通知邮件数量减少了
		    router:cast(Account, dec_mail_count, NDel);
		_ ->
		    ok
	    end;
	_ ->
	    ok
    end.


%% 更新邮件，该删的删，该退的退
-spec db_update_mail(atom()) -> any().			  
db_update_mail(Account) when is_atom(Account) ->
    %% 取删信列表    
    Mails = get_overtime_mails(Account),
    DelIDList = [{?table_name, ID} || #player_mail{id=ID} <- Mails],
    DeleteOp = {delete, DelIDList},

    %% 取退信列表
    RejectMails = extract_reject_mails(Mails),      
    RejectOp = {write, RejectMails},
    
    %% 数据库操作
    OpList = [DeleteOp, RejectOp],
    db_mail:change(OpList),
    {DelIDList, RejectMails}.




%% 取过期邮件
get_overtime_mails(Account) ->
    Mails = db_mail:get_player_mails(Account),
    Now = datetime:local_time(),
    
    F = fun(X, Sum) -> 
		#player_mail{send_time=SendTime}=X,
		{Days, {Hour, Min, Sec}} = datetime:time_difference(SendTime, Now), 
		case datetime:time_to_seconds({Days * 24 + Hour, Min, Sec}) >= get_max_mail_second() of
		    true -> [X | Sum];
		    false -> Sum
		end
	end,
    MailList = lists:foldr(F, [], Mails),
    MailList.    


%% 提取退信(针对个人邮件)
-spec extract_reject_mails(list()) -> list().
extract_reject_mails(Mails) when is_list(Mails) ->
    [build_reject_mail(M) || #player_mail{type=Type, read_status=ReadStatus}=M <- Mails, 
	Type =:= ?mt_person_mail, ReadStatus =:= ?mrs_not_read ].


%% 交换收发人, 邮件类型改为退信, 状态改为未读
build_reject_mail(#player_mail{sender=SenderAccount, reciever=RecverAccount,
			      sender_name=SenderName,
			      recver_name=RecverName}=Mail) ->
    Mail#player_mail{sender_name=RecverName,
		     sender=RecverAccount,
		     recver_name=SenderName, 
		     reciever=SenderAccount,
		     send_time=datetime:local_time(),
		     type=?mt_reject_mail, 
		     read_status=?mrs_not_read}.


%% 取该物品的索引列表
get_item_index_list(InstanceList, Bag) when is_list(InstanceList) ->
    FGetIndex = fun(InstanceID, List) ->
			ItemIndex = pack:get_index(InstanceID, Bag),
			case ItemIndex of
			    not_found -> List;
			    _ -> [ItemIndex | List]
			end
		end,

    IndexList = lists:foldl(FGetIndex, [], InstanceList),
    IndexList.

%% 判断可否退信
can_reject_mail(Mail) ->
    case Mail =:= [] of
	true -> {false, ?err_mail_not_exists};
	false ->
	    %% 取得原来发信方
	    #player_mail{sender=SenderAccount, type=Type} = Mail,
	    case Type of
		?mt_gm_mail -> {false, ?err_sys_mail_can_not_reject};
		?mt_system_mail -> {false, ?err_sys_mail_can_not_reject};  %% 系统邮件不能退信
		?mt_reject_mail -> {false, ?err_mail_can_not_reject_again};  %% 已退信邮件, 不能再退  
		?mt_person_mail -> 
		    case SenderAccount =:= undefined of
			true -> {false, ?err_mail_sender_undefined}; %% 找不到发信人
			false -> true
		    end
	    end
    end.

%% 尝试取邮件物品
try_get_mail_item(Mail, InstID, Bag) ->
    case Mail of
	[] -> {false, ?err_mail_not_exists};
	#player_mail{items=Items} ->
	    F = fun(#pack_grid{item_data=ItemData}) ->
			ItemData#item.instance_id =:= InstID 
		end,
	    case lists:partition(F, Items) of
		{[], _NewMailItems1} -> 
		    {false, ?err_mail_item_not_exists};
		{[PackGrid], NewMailItems} ->
		    #pack_grid{count=Count, item_data=Item} = PackGrid,
		    case pack:add_item(Item, Count, Bag) of
			full -> {false, ?err_bag_full};
			NewBag -> 
			    {NewBag, NewMailItems, Item, Count}   
		    end
	    end
    end.

can_trade(Idx, Bag) ->
    BagItem = pack:get_item(Idx, Bag),
    item:can_trade(BagItem).  


%% 通知删除物品
notify_del_items(Account, State) ->
    player_pack:send(Account, State).

%% 通知增加物品
notify_add_item(Account, State) ->
    player_pack:send(Account, State).



send_sys_mail_1(Recver, Title, Content, MailItems, Type, SenderName) 
  when is_atom(Recver) ->
    Mail = build_sys_mail(Recver, Title, Content, MailItems, Type, SenderName),

    %% 数据库操作
    db_mail:write(Mail),		    
    
    %% 通知新邮件
    send2other(Recver, #notify_add_mail{mail_data=get_mail_info(Mail)}),	    
    
    %% 更新邮件, 自动删除过期邮件
    update_mail(Recver),
    ok.
send_sys_mail_2(RecverList, Title, Content, MailItems, Type, SenderName) 
  when is_list(RecverList),
       is_list(MailItems) ->
    Mails = [build_sys_mail(Recver, Title, Content, MailItems, Type, SenderName) 
	     || Recver <- RecverList],
    
    %% 数据库操作
    db_mail:change([{write, Mails}]),		    
    
    %% 通知新邮件
    [send2other(Mail#player_mail.reciever, #notify_add_mail{mail_data=get_mail_info(Mail)}) 
     ||	Mail <- Mails], 	    
    
    %% 更新邮件, 自动删除过期邮件
    [update_mail(Recver) || Recver <- RecverList],
    ok.


build_sys_mail(Recver, Title, Content, MailItems, Type, SenderName) 
  when is_list(MailItems),
       is_atom(Recver) ->
    
    Items = get_effect_items(MailItems),

    RecverName = db_player:get_user_name(Recver),
    Mail = #player_mail{id=fetch_new_mail_id(),
			sender_name=SenderName,
			send_time=datetime:local_time(),
			sender=undefined,
			recver_name=RecverName,
			reciever=Recver,
			title=Title,
			content=Content,
			items=Items,
			read_status=?mrs_not_read,
			type=Type			
		       },
    Mail.
    
    
send2other(TargetAccount, Msg) ->
    case util:is_process_alive(TargetAccount) of
	false -> ok;
	_Pid -> net_helper:send2client(TargetAccount, Msg)	    
    end.


%% 取有效物品
-spec get_effect_items(list()) -> list().			       
get_effect_items(MailItems) ->
    Items = [case X of
		 {ItemTptID, ItemCount} -> 
		     #pack_grid{count=ItemCount, item_data=item:make_item(ItemTptID)};
		 ItemTptID ->
		     #pack_grid{count=1, item_data=item:make_item(ItemTptID)}
	     end || X <- MailItems],
    Items.

%% 取收件人账号
-spec get_recver_account(list(), list()) -> atom().				
get_recver_account(RawAccount, _RecverName) when is_list(RawAccount), is_list(_RecverName)-> 
    case RawAccount of
	[] -> undefined;
	_ ->
	    TargetAccount = list_to_atom(RawAccount),
	    case db_player:is_exist(TargetAccount) of
	    	true ->
	    	    TargetAccount;
	    	false ->
	    	    undefined
	    end
    end.

%% 最大邮件秒数, 到了会自动删邮件
-spec get_max_mail_second() -> integer().   
get_max_mail_second() ->
    common_def:get_val(max_mail_second).

%% 最大可见邮件数量
get_max_can_see_mail() ->
    common_def:get_val(max_can_see_mail).

%% 取可见邮件列表
get_can_see_mail_list(Account) when is_atom(Account) ->
    Mails = db_mail:get_player_mails(Account),
    MailCount = get_max_can_see_mail() + common_def:get_val(mail_cache_count),
    case length(Mails) =< MailCount of
	true ->
	    Mails;
	_ ->
	    %%排序, 并返回
	    NewMails = sort_mails(Mails),
	    {List1, _List2} = lists:split(MailCount, NewMails),	  
  	    List1
    end.
    
-spec sort_mails(list()) -> list().
sort_mails(Mails) when is_list(Mails) ->
    F = fun(#player_mail{send_time=SendTimeA}, #player_mail{send_time=SendTimeB}) -> 
		SendTimeB < SendTimeA
	end,

    lists:sort(F, Mails).

do_notify_mail_list(Account) when is_atom(Account)  ->    
    MailInfoList = [get_mail_info(Mail) || Mail <- get_can_see_mail_list(Account)],
    
    %% 通知客户端
    net_helper:send2client(Account, #notify_mail_list{mails=MailInfoList}),
    length(MailInfoList).

do_delete_mail(ID, Account, State) ->
    %% 数据库删信
    db_mail:delete_mail(ID),
    %% 通知客户端
    net_helper:send2client(Account, #notify_delete_mail{id=ID}),

    NewState = after_delete_mail(Account, State),
    NewState.
    
after_delete_mail(Account, State) ->
    OldMailCount = player_data:get_mail_count(State),
    NewMailCount = 
	case OldMailCount =:= get_max_can_see_mail() of %% 注: 此处 不能判断"小于" 
	    false ->
		OldMailCount - 1;
	    true ->
		%% 可见邮件数不够，重新发送邮件列表
		N = do_notify_mail_list(Account),		
		N
	end,
    player_data:set_mail_count(NewMailCount, State).


