%%% @author hongjx <hongjx@china-channel.com>
%%% @copyright (C) 2010, hongjx
%%% @doc
%%%   �ʼ�
%%%      ע: 1. ��������οͻ��˵��ʼ�״̬
%%%          2. �ʼ��Զ�ɾ��ʱ��: �����ʼ�ʱ, �����ʼ��б�ʱ 
%%%          3. player_data#mail_count��¼�ͻ����ʼ���(�Ǹ��ο�ֵ, ֻ����ɾ��)
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
	  reciever,       %% �������˺�
	  send_time,      %% ����ʱ��
	  sender,         %% �������˺�
	  type,           %% ����
	  title,          %% ����
	  content,        %% ����
	  items,          %% ��Ʒ
	  read_status,    %% ״̬��1.δ����2.�������⣬3.��������)
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
%% ���ߴ���
notify_login(State) ->
    Account = player_data:get_account(State),
    Mails = db_mail:get_player_mails(Account),
    F = fun(#player_mail{read_status=Status}) -> 
		   Status =:= ?mrs_not_read 
	end,
    case lists:any(F, Mails) of
	true ->
	    %% ֪ͨ�ͻ���, ��δ���ʼ�
	    net_helper:send2client(Account, #notify_mail_not_read{});
	_ -> ok
    end.
    
%% ����ϵͳ�ʼ�
%% @MailItems = [#pack_grid{}]
send_sys_mail(RecverList, Title, Content, MailItems) when is_list(RecverList), 
							  is_list(MailItems) ->
    send_sys_mail_2(RecverList, Title, Content, MailItems, ?mt_system_mail, "");
send_sys_mail(Recver, Title, Content, MailItems) when is_atom(Recver),
						      is_list(MailItems) ->
    send_sys_mail_1(Recver, Title, Content, MailItems, ?mt_system_mail, "").

%% ����gm�ʼ�
send_gm_mail(RecverList, Title, Content, MailItems) when is_list(RecverList) ->
    [send_gm_mail(Recver, Title, Content, MailItems) || Recver <- RecverList];
send_gm_mail(Recver, Title, Content, MailItems) ->
    send_sys_mail_1(Recver, Title, Content, MailItems, ?mt_gm_mail, "").

%% ����gm�ʼ�
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
%% �����ʼ�
handle_cast({#msg{event=dec_mail_count}, NDel}, State) ->
    OldMailCount = player_data:get_mail_count(State),
    NewMailCount = OldMailCount - NDel,
    MaxCanSeeMail = get_max_can_see_mail(),
    ClientMailCount = 
     	case (NewMailCount < MaxCanSeeMail) and (MaxCanSeeMail =< OldMailCount) of
     	    false -> 	  
     		NewMailCount;
     	    _ -> %% �ɼ��ʼ������������·����ʼ��б�
		Account = player_data:get_account(State),
      		N = do_notify_mail_list(Account),		
     		N
     	end,

    NewState = player_data:set_mail_count(ClientMailCount, State),
    {noreply, NewState};



%% �����ʼ��б�
handle_cast({_Msg, #req_mail_list{}}, State) ->    
    Account = player_data:get_account(State),
    {_DelIDList, RejectMails} = db_update_mail(Account),
    
    %% ֪ͨ�ͻ��� ���ʼ�(����)
    [send2other(Recver, #notify_add_mail{mail_data=get_mail_info(M)}) 
     || #player_mail{reciever=Recver}=M <- RejectMails],

    MailCount = do_notify_mail_list(Account),    
    NewState = player_data:set_mail_count(MailCount, State),
    {noreply, NewState};
%% ɾ���ʼ�
handle_cast({_Msg, 
	     #req_delete_mail{id=ID}}, State) ->
    Account = player_data:get_account(State),

    %% �ж��ʼ��Ƿ�����ҵ�
    CanDelete = exists_mail(Account, ID),
    case CanDelete of
	false -> 
	    sys_msg:send(Account, ?err_mail_not_exists),
	    {noreply, State};
	true ->
	    NewState = do_delete_mail(ID, Account, State),
	    {noreply, NewState}		    
    end;
%% ���ʼ�����
handle_cast({_Msg, 
	     #req_mail_content{id=ID}}, State) ->
    Account = player_data:get_account(State),
    case get_mail(Account, ID) of
	[] -> sys_msg:send(Account, ?err_mail_not_exists);
	Mail ->
	    %% ȡ�ʼ�����
	    #player_mail{content=Content} = Mail,

	    %% ���ݿ�����������Ѷ�״̬
	    set_read_status(?mrs_read_content, [Mail]), 
	    %% ֪ͨ�ͻ���
	    net_helper:send2client(Account, #notify_mail_content{id=ID, content=Content})	    
    end,
    {noreply, State};
%% �ͻ���֪ͨ����˿����ʼ�����
handle_cast({_Msg, 
	     #req_know_new_mail_title{id_list=IDList}}, State) -> 
    Account = player_data:get_account(State),
    %% ȡ������ʼ�
    RawMails = [get_mail(Account, ID) || ID <- IDList],
    Mails = [X || X <- RawMails, X /= []], 
    %% ���ݿ����, �����Ѷ�״̬
    set_read_status(?mrs_read_title, Mails),
    {noreply, State};
%% ����
handle_cast({_Msg, 
	     #req_reject_mail{id=ID}}, State) ->
    Account = player_data:get_account(State),  
    
    Mail = get_mail(Account, ID),
    case can_reject_mail(Mail) of
	{false, Reason} -> 
	    sys_msg:send(Account, Reason),
	    {noreply, State};
	_ ->
	    %% ȡ��ԭ�����ŷ�
	    #player_mail{sender=TargetAccount} = Mail,

	    %% ���ݿ����Ŵ���
	    NewMail = build_reject_mail(Mail),
	    db_mail:write(NewMail),	    
	    

	    %% ֪ͨ���ųɹ�
	    net_helper:send2client(Account, #notify_reject_mail_success{id=ID}),

	    NewState = after_delete_mail(Account, State), 

	    %% ֪ͨԭ�����ŷ�
	    send2other(TargetAccount, 
		       #notify_add_mail{mail_data=get_mail_info(NewMail)}),
	    
	   
	    %% �����ʼ�, �Զ�ɾ�������ʼ�
	    update_mail(TargetAccount),
	    {noreply, NewState}
    end;
%% ȡ��Ʒ
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
	    %% ���ӱ�����Ʒ 
	    NewPlayerPack = player_pack:make_package(Account, ?pt_bag, NewBag),
	    OpAddBagItem = {write, [NewPlayerPack]},
	    NewState = player_data:set_pack(?pt_bag, NewBag, State),		    
	    
	    %% ɾ���ʼ���Ʒ
	    NewMail = Mail#player_mail{items=NewMailItems},
	    OpDelMailItem = {write, [NewMail]},
	    
	    %% ���ݿ����: ɾ���ʼ���Ʒ, ���ӱ�����Ʒ
	    db_mail:change([OpAddBagItem, OpDelMailItem]),
	    
	    %% ֪ͨɾ���ʼ���Ʒ
	    net_helper:send2client(Account, #notify_delete_mail_item{id=ID, inst_id=InstID}),

	    %% ֪ͨ���ӱ�����Ʒ
	    notify_add_item(Account, NewState),
	    

	    %% ����ʼ�û���⣬û���ݣ�û��Ʒ����ɾ��֮
	    MailNotItems = (NewMailItems =:= []),
	    #player_mail{title=Title, content=Content, sender=Sender} = NewMail,
	    CanDelete = MailNotItems and (length(Title) =:= 0) and (length(Content) =:= 0), 

	    %% ��̨��Ǯ��Ʒ��¼
	    ItemInfo = {item:get_tempid(Item), ItemCount, item:get_instid(Item)},
	    item_money_log:log_item(Account, get_mail_item, ItemInfo, Sender),
	    
	    %% ��ʾ�����Ʒ
	    sys_msg:send(Account, ?msg_get_item, [item:get_item_name(Item), ItemCount]),

	    case CanDelete of
		true -> 
		    State1 = do_delete_mail(ID, Account, NewState),

		    {noreply, State1};
		false ->
		    {noreply, NewState}
	    end		       
    end;

%% �����ʼ�
handle_cast({_Msg, 
	     #req_send_mail{recver_name=RecverName, title=Title,
			    recver_account=RawAccount,
			    content=Content, items=ItemIDs}}, State) ->
    Account = player_data:get_account(State),
    BasicData = player_data:get_basic_data(State),
    SenderName = basic_data:get_data(username, BasicData), 

    %% ȡ�������˺�
    TargetAccount = get_recver_account(RawAccount, RecverName),

    Bag = player_data:get_pack(?pt_bag, State),
    
    %% ����ʵ��ID, ȡ����Ʒ����λ��
    IndexList = get_item_index_list(ItemIDs, Bag),

    case can_send_mail(Account, TargetAccount, Title, Content, 
		       ItemIDs, IndexList, Bag) of
	{false, Reason} -> 
 	    sys_msg:send(Account, Reason),
	    {noreply, State};
	true ->
	    %% �������ʼ�
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
			%% ɾ��������Ʒ
			FDelItem = fun(Idx, TmpBag) ->
					   pack:set_item(empty, Idx, TmpBag)			       
				   end,
			NewBag = lists:foldl(FDelItem, Bag, IndexList),
			State1 = player_data:set_pack(?pt_bag, NewBag, State),
			
			NewPlayerPack = player_pack:make_package(Account, ?pt_bag, NewBag),
			%% ���ݿ����
			OpDelItems = {write, [NewPlayerPack]},
			OpAddMail = {write, [Mail]},
			db_mail:change([OpDelItems, OpAddMail]),
			
			%% ֪ͨɾ����Ʒ
			notify_del_items(Account, State1),

			%% ��̨��Ǯ��Ʒ��¼
			ItemInfo = [begin
					DelItem = pack:get_item(Idx, Bag),
					{item:get_tempid(DelItem),
					-pack:get_item_count(Idx, Bag),
					item:get_instid(DelItem)}
				    end || Idx <- IndexList],
			item_money_log:log_item(Account, send_mail, ItemInfo, TargetAccount),

			State1;
		    false -> 
			%% ���ݿ����
			db_mail:write(Mail),		    
			State
		end,
	    

	    %% ֪ͨ���ʼ�
	    send2other(TargetAccount, 
				   #notify_add_mail{mail_data=get_mail_info(Mail)}),	    

	    %% ֪ͨ���ʼ��ɹ�
	    net_helper:send2client(Account, #notify_send_mail_success{}),

	    %% �����ʼ�, �Զ�ɾ�������ʼ�
	    update_mail(TargetAccount),
	    {noreply, NewState}
    end;
handle_cast(_Msg, State) ->
    {noreply, State}.


%%%===================================================================
%%% �ڲ�����
%%%===================================================================
%% �жϿɷ����ʼ�
can_send_mail(Account, TargetAccount, Title, Content, 
	      ItemIDs, IndexList, Bag) when is_atom(Account),
					    is_atom(TargetAccount) ->

    case TargetAccount =:= undefined of
	true -> {false, ?err_mail_reciever_not_exists}; %% �Ҳ����ռ���
	false ->	    
	case Account =:= TargetAccount of
	    true -> {false, ?err_can_not_mail_to_self}; %% ���ܼĸ�����
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


%% �ж��ʼ������Ƿ�Ƿ�
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
	    true -> {false, ?err_mail_item_full};   %% �ʼ���Ʒ��������
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



%% ȡ�ʼ���Ϣ
get_mail_info(#player_mail{id=ID, type=Type, read_status=ReadStatus, 
			   send_time=SendTime, sender_name=SenderName, 
			   sender=SenderAccount, title=Title, items=Items}) ->
    %% ��Ч��
    EffectSeconds = datetime:datetime_to_gregorian_seconds(SendTime) + get_max_mail_second(),
    EffectTime = datetime:gregorian_seconds_to_datetime(EffectSeconds),

    MailItems = Items,
    Now = datetime:local_time(),
    {D, {H, M, S}} = datetime:time_difference(Now, EffectTime),
    %% ��ʣ��ʱ��(��)
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


%% �����ʼ�״̬��0.δ����1.�������⣬2.��������)
-spec set_read_status(integer(), list()) -> any().
set_read_status(NewStatus, Mails) when is_integer(NewStatus),
				       is_list(Mails) ->     
    NewMails = [Mail#player_mail{read_status=NewStatus} || 
		 #player_mail{read_status=OldStatus}=Mail <- Mails, 
		 OldStatus < NewStatus],
    OpList = [{write, NewMails}],
    db_mail:change(OpList).    

%% �Զ��������ʼ�id
fetch_new_mail_id() ->
    guid:make(?st_mail).

%% ȡĳ���ʼ� 
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

%% �ж��ʼ��Ƿ����
-spec exists_mail(atom(), integer()) -> boolean().
exists_mail(Account, ID) when is_atom(Account),
			      is_integer(ID) ->
    get_mail(Account, ID) /= [].


%% �����ʼ�����ɾ��ɾ�����˵���
update_mail(Account) when is_atom(Account) ->
    {DelIDList, RejectMails} = db_update_mail(Account),
    
    %% ֪ͨ�ͻ��� ���ʼ�(����)
    [send2other(Recver, #notify_add_mail{mail_data=get_mail_info(M)}) 
     || #player_mail{reciever=Recver}=M <- RejectMails],

    case util:is_process_alive(Account) of
	true ->
	    %% ֪ͨ�ͻ��� ɾ�ʼ�
	    [net_helper:send2client(Account, #notify_delete_mail{id=ID}) 
	     || {_, ID} <- DelIDList],

	    NDel = length(DelIDList),
	    case NDel > 0 of
		true ->
		    %% ֪ͨ�ʼ�����������
		    router:cast(Account, dec_mail_count, NDel);
		_ ->
		    ok
	    end;
	_ ->
	    ok
    end.


%% �����ʼ�����ɾ��ɾ�����˵���
-spec db_update_mail(atom()) -> any().			  
db_update_mail(Account) when is_atom(Account) ->
    %% ȡɾ���б�    
    Mails = get_overtime_mails(Account),
    DelIDList = [{?table_name, ID} || #player_mail{id=ID} <- Mails],
    DeleteOp = {delete, DelIDList},

    %% ȡ�����б�
    RejectMails = extract_reject_mails(Mails),      
    RejectOp = {write, RejectMails},
    
    %% ���ݿ����
    OpList = [DeleteOp, RejectOp],
    db_mail:change(OpList),
    {DelIDList, RejectMails}.




%% ȡ�����ʼ�
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


%% ��ȡ����(��Ը����ʼ�)
-spec extract_reject_mails(list()) -> list().
extract_reject_mails(Mails) when is_list(Mails) ->
    [build_reject_mail(M) || #player_mail{type=Type, read_status=ReadStatus}=M <- Mails, 
	Type =:= ?mt_person_mail, ReadStatus =:= ?mrs_not_read ].


%% �����շ���, �ʼ����͸�Ϊ����, ״̬��Ϊδ��
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


%% ȡ����Ʒ�������б�
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

%% �жϿɷ�����
can_reject_mail(Mail) ->
    case Mail =:= [] of
	true -> {false, ?err_mail_not_exists};
	false ->
	    %% ȡ��ԭ�����ŷ�
	    #player_mail{sender=SenderAccount, type=Type} = Mail,
	    case Type of
		?mt_gm_mail -> {false, ?err_sys_mail_can_not_reject};
		?mt_system_mail -> {false, ?err_sys_mail_can_not_reject};  %% ϵͳ�ʼ���������
		?mt_reject_mail -> {false, ?err_mail_can_not_reject_again};  %% �������ʼ�, ��������  
		?mt_person_mail -> 
		    case SenderAccount =:= undefined of
			true -> {false, ?err_mail_sender_undefined}; %% �Ҳ���������
			false -> true
		    end
	    end
    end.

%% ����ȡ�ʼ���Ʒ
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


%% ֪ͨɾ����Ʒ
notify_del_items(Account, State) ->
    player_pack:send(Account, State).

%% ֪ͨ������Ʒ
notify_add_item(Account, State) ->
    player_pack:send(Account, State).



send_sys_mail_1(Recver, Title, Content, MailItems, Type, SenderName) 
  when is_atom(Recver) ->
    Mail = build_sys_mail(Recver, Title, Content, MailItems, Type, SenderName),

    %% ���ݿ����
    db_mail:write(Mail),		    
    
    %% ֪ͨ���ʼ�
    send2other(Recver, #notify_add_mail{mail_data=get_mail_info(Mail)}),	    
    
    %% �����ʼ�, �Զ�ɾ�������ʼ�
    update_mail(Recver),
    ok.
send_sys_mail_2(RecverList, Title, Content, MailItems, Type, SenderName) 
  when is_list(RecverList),
       is_list(MailItems) ->
    Mails = [build_sys_mail(Recver, Title, Content, MailItems, Type, SenderName) 
	     || Recver <- RecverList],
    
    %% ���ݿ����
    db_mail:change([{write, Mails}]),		    
    
    %% ֪ͨ���ʼ�
    [send2other(Mail#player_mail.reciever, #notify_add_mail{mail_data=get_mail_info(Mail)}) 
     ||	Mail <- Mails], 	    
    
    %% �����ʼ�, �Զ�ɾ�������ʼ�
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


%% ȡ��Ч��Ʒ
-spec get_effect_items(list()) -> list().			       
get_effect_items(MailItems) ->
    Items = [case X of
		 {ItemTptID, ItemCount} -> 
		     #pack_grid{count=ItemCount, item_data=item:make_item(ItemTptID)};
		 ItemTptID ->
		     #pack_grid{count=1, item_data=item:make_item(ItemTptID)}
	     end || X <- MailItems],
    Items.

%% ȡ�ռ����˺�
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

%% ����ʼ�����, ���˻��Զ�ɾ�ʼ�
-spec get_max_mail_second() -> integer().   
get_max_mail_second() ->
    common_def:get_val(max_mail_second).

%% ���ɼ��ʼ�����
get_max_can_see_mail() ->
    common_def:get_val(max_can_see_mail).

%% ȡ�ɼ��ʼ��б�
get_can_see_mail_list(Account) when is_atom(Account) ->
    Mails = db_mail:get_player_mails(Account),
    MailCount = get_max_can_see_mail() + common_def:get_val(mail_cache_count),
    case length(Mails) =< MailCount of
	true ->
	    Mails;
	_ ->
	    %%����, ������
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
    
    %% ֪ͨ�ͻ���
    net_helper:send2client(Account, #notify_mail_list{mails=MailInfoList}),
    length(MailInfoList).

do_delete_mail(ID, Account, State) ->
    %% ���ݿ�ɾ��
    db_mail:delete_mail(ID),
    %% ֪ͨ�ͻ���
    net_helper:send2client(Account, #notify_delete_mail{id=ID}),

    NewState = after_delete_mail(Account, State),
    NewState.
    
after_delete_mail(Account, State) ->
    OldMailCount = player_data:get_mail_count(State),
    NewMailCount = 
	case OldMailCount =:= get_max_can_see_mail() of %% ע: �˴� �����ж�"С��" 
	    false ->
		OldMailCount - 1;
	    true ->
		%% �ɼ��ʼ������������·����ʼ��б�
		N = do_notify_mail_list(Account),		
		N
	end,
    player_data:set_mail_count(NewMailCount, State).


