%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%
%%% @end
%%% Created : 21 Jun 2011 by hongjx <hongjx@35info.cn>

-module(db_wallow).

-export([new/1, save/2,
	 on_login/2, on_logout/3]).

-record(player_wallow, {account,         %% �˺�
			start_time,      %% ��ʼʱ��
			last_online_time,%% ��ʼʱ�䵱ʱ������ʱ��
			login_time,      %% ����ʱ��
			ref_list=[]      %% ����Щϵͳ����(eq �� farm)
		       }).

%%%===================================================================
%%% Api
%%%===================================================================

new(Account) ->
    Now = datetime:localtime(),
    #player_wallow{account=Account,   %% �˺�
		   start_time=Now,    %% ��ʼʱ��
		   login_time=Now,    %%
		   ref_list=[],
		   last_online_time=0}.


on_logout(Account, SystemFlag, {LastOnlineTime, LoginTime}) ->
    Table = player_wallow, 
    Ref = {SystemFlag, node()},
    F = fun() ->
		case mnesia:read(Table, Account) of		    
		    [#player_wallow{ref_list=[Ref]}=Record] -> %% ֻ�е�ǰ������ʹ�ó��Լ�¼
			%% ��������ʱ�䣬���������
			DiffTime = datetime:diff_time(LoginTime, datetime:localtime()),
			NRecord = Record#player_wallow{ref_list=[], 
					     last_online_time=LastOnlineTime + DiffTime},
			mnesia:write(NRecord);			
		    [#player_wallow{ref_list=RefList}=Record] -> %% ���������ʹ�ó��Լ�¼
			%% �������
			NList = lists:delete(Ref, RefList),
			mnesia:write(Record#player_wallow{ref_list=NList});	
		    _ ->
			ok
		end
        end,
    mnesia:activity(transaction, F, [], mnesia_frag).


save(Account, {LastOnlineTime, LoginTime}) ->
    Table = player_wallow, 
    F = fun() ->
		case mnesia:read(Table, Account) of		    
		    [#player_wallow{}=Record] -> 
			%% ��������ʱ��
			Now = datetime:localtime(),
			DiffTime = datetime:diff_time(LoginTime, Now),
			NRecord = Record#player_wallow{last_online_time=LastOnlineTime + DiffTime,
						      login_time=Now},
			%%data_helper:format("save wallow online time ~p~n~n", [NRecord]),
			mnesia:write(NRecord)			
		end
        end,
    mnesia:activity(transaction, F, [], mnesia_frag).


on_login(Account, SystemFlag) ->
    Ref = {SystemFlag, node()},
    F = fun() ->
		R = get_record(Account, Ref),
		
		List = R#player_wallow.ref_list,
		%% ��������			
		NList = case lists:member(Ref, List) of
			    true -> List;
			    _ -> [Ref | List] 
			end,

		NRecord = R#player_wallow{ref_list=NList},
		mnesia:write(NRecord),

		%% ����WallowInfo
		#player_wallow{last_online_time=LastOnlineTime, login_time=LoginTime} = NRecord,
		WallowInfo = {LastOnlineTime, LoginTime},
		WallowInfo
        end,
    mnesia:activity(transaction, F, [], mnesia_frag).
    
%%%===================================================================
%%% �ڲ�����
%%%===================================================================
get_record(Account, Ref) ->
    Table = player_wallow, 
    case mnesia:read(Table, Account) of		    
	[#player_wallow{ref_list=OldRefList, 
			start_time=StartTime, 
			last_online_time=OnlineTime}=Record] -> 
	    %% ȥ����������
	    RefList = drop_bad_Ref(OldRefList, Ref, Account),
	    case RefList of
		[] -> 
		    Now = datetime:localtime(),

		    %% ȡ��ʱ��
		    AllTime = datetime:diff_time(StartTime, Now),
		    
		    %% ȡ�ۼ�����ʱ��
		    OfflineTime = AllTime - OnlineTime,
		    %%data_helper:format("offline time ~p~n~n", [{OnlineTime, OfflineTime}]),

		    %% �������ʱ�� > ����ʱ��, ���¼�ʱ
		    %% ����ۼ�����ʱ�� > 5Сʱ, Ҳ���¼�ʱ
		    case %%(OfflineTime > OnlineTime) or 
			(OfflineTime > 5 * common_def:get_val(wallow_seconds_per_hour)) of
			true ->
			    new(Account);					    
			_ ->
			    %% ��������ʱ��
			    Record#player_wallow{login_time=Now, ref_list=RefList}
		    end;
		_ -> %% ���������ʹ�ó��Լ�¼
		    Record#player_wallow{ref_list=RefList}
	    end;
	_ -> %% ���ݿ�û��¼
	    new(Account)
    end.

%% ȥ����������
drop_bad_Ref(RefList, Ref, Account) ->
    %% �жϽ����Ƿ����, �����������ȥ������
    F = fun({SystemFlag, Node}=X, Acc) ->
		ServerAtom = 
		    case SystemFlag of
			farm -> farm:get_name(Account);
			_ -> Account
		    end,
		case rpc:call(Node, erlang, whereis, [ServerAtom]) of
		    Pid when is_pid(Pid) ->
			[X | Acc];
		    _ -> Acc
		end
	end,

    lists:foldl(F, [], lists:delete(Ref, RefList)).

    

