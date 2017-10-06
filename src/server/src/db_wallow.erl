%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%
%%% @end
%%% Created : 21 Jun 2011 by hongjx <hongjx@35info.cn>

-module(db_wallow).

-export([new/1, save/2,
	 on_login/2, on_logout/3]).

-record(player_wallow, {account,         %% 账号
			start_time,      %% 开始时间
			last_online_time,%% 开始时间当时的上线时间
			login_time,      %% 登入时间
			ref_list=[]      %% 被哪些系统引用(eq 或 farm)
		       }).

%%%===================================================================
%%% Api
%%%===================================================================

new(Account) ->
    Now = datetime:localtime(),
    #player_wallow{account=Account,   %% 账号
		   start_time=Now,    %% 开始时间
		   login_time=Now,    %%
		   ref_list=[],
		   last_online_time=0}.


on_logout(Account, SystemFlag, {LastOnlineTime, LoginTime}) ->
    Table = player_wallow, 
    Ref = {SystemFlag, node()},
    F = fun() ->
		case mnesia:read(Table, Account) of		    
		    [#player_wallow{ref_list=[Ref]}=Record] -> %% 只有当前进程在使用沉迷记录
			%% 更新上线时间，并解除引用
			DiffTime = datetime:diff_time(LoginTime, datetime:localtime()),
			NRecord = Record#player_wallow{ref_list=[], 
					     last_online_time=LastOnlineTime + DiffTime},
			mnesia:write(NRecord);			
		    [#player_wallow{ref_list=RefList}=Record] -> %% 多个进程在使用沉迷记录
			%% 解除引用
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
			%% 更新上线时间
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
		%% 增加引用			
		NList = case lists:member(Ref, List) of
			    true -> List;
			    _ -> [Ref | List] 
			end,

		NRecord = R#player_wallow{ref_list=NList},
		mnesia:write(NRecord),

		%% 返回WallowInfo
		#player_wallow{last_online_time=LastOnlineTime, login_time=LoginTime} = NRecord,
		WallowInfo = {LastOnlineTime, LoginTime},
		WallowInfo
        end,
    mnesia:activity(transaction, F, [], mnesia_frag).
    
%%%===================================================================
%%% 内部函数
%%%===================================================================
get_record(Account, Ref) ->
    Table = player_wallow, 
    case mnesia:read(Table, Account) of		    
	[#player_wallow{ref_list=OldRefList, 
			start_time=StartTime, 
			last_online_time=OnlineTime}=Record] -> 
	    %% 去除错误引用
	    RefList = drop_bad_Ref(OldRefList, Ref, Account),
	    case RefList of
		[] -> 
		    Now = datetime:localtime(),

		    %% 取总时间
		    AllTime = datetime:diff_time(StartTime, Now),
		    
		    %% 取累计下线时间
		    OfflineTime = AllTime - OnlineTime,
		    %%data_helper:format("offline time ~p~n~n", [{OnlineTime, OfflineTime}]),

		    %% 如果下线时间 > 上线时间, 重新计时
		    %% 如果累计下线时间 > 5小时, 也重新计时
		    case %%(OfflineTime > OnlineTime) or 
			(OfflineTime > 5 * common_def:get_val(wallow_seconds_per_hour)) of
			true ->
			    new(Account);					    
			_ ->
			    %% 设置上线时间
			    Record#player_wallow{login_time=Now, ref_list=RefList}
		    end;
		_ -> %% 多个进程在使用沉迷记录
		    Record#player_wallow{ref_list=RefList}
	    end;
	_ -> %% 数据库没记录
	    new(Account)
    end.

%% 去除错误引用
drop_bad_Ref(RefList, Ref, Account) ->
    %% 判断进程是否存在, 如果不存在则去除引用
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

    

