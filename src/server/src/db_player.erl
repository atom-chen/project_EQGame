%% Author: NoteBook
%% Created: 2009-9-12
%% Description: Add description to db_player
-module(db_player).

%%
%% Include files
%%
-include("table_info.hrl").
-include("packet_def.hrl").
-include_lib("stdlib/include/qlc.hrl").
%%
%% Exported Functions
%%
-export([has_same_name/1, is_exist/1, is_online/1, get_online_players/0]).
-export([insert/1, create/1, update/3, update/5, update/7, select/2, 
	 select/1, insert_online_player/1, delete_online_player/1]).
-export([get_user_name/1, name_to_account/1]).
-export([get_online_players_count/1, get_online_players_key/1]).

%%
%% API Functions
%%


%%
%% 是否存在相同的名称
%% 结果: true | false
%%
-spec has_same_name(string()) -> atom().
has_same_name(UserName) ->
    F = fun() ->
                mnesia:select(player_basic_data, [#player_basic_data{username = UserName}])
        end,
    Record = mnesia:activity(transaction, F, [], mnesia_frag),
    has_record(Record).
   
%%
%% 是否存在EQ帐号
%% 结果: true | false
%%
-spec is_exist(atom()) -> atom().
is_exist(Account) ->
    Record = select(once, Account),
    has_record(Record).

has_record(Record) ->
    case Record of
        [] -> false;
        _ -> true
    end.

get_user_name(Account) when is_list(Account) ->
    get_user_name(list_to_atom(Account));
get_user_name(Account) ->
    case db_common:select_one(player_basic_data, Account) of
	#player_basic_data{username=UserName} -> UserName;
	_ -> []
    end.
    
name_to_account(UserName) when is_list(UserName) ->
    case db_common:index_select(player_basic_data, UserName, #player_basic_data.username) of
	[] -> undefined;
	[#player_basic_data{account=Account} | _Rest] -> Account
    end.

%%------------------------------player_base_info--------------------------------------
%%
%% 保存玩家数据
%%
-spec insert(tuple()) -> tuple().
insert(PlayerBasicData) ->
    F = fun() -> 
		mnesia:write(PlayerBasicData)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

%% 玩家创建时,需要初始化一些系统中的数据
create(DataList) ->
    F = fun() -> 
		[ok = mnesia:write(Data) || Data <- DataList],
		ok
	end,
    ok = mnesia:activity(transaction, F, [], mnesia_frag).
%%
%% 更新玩家数据
%%
-spec update(atom(), string(), integer(), tuple(), integer()) -> tuple().
update(Account, UserName, Sex, PlayerProperty, HealthStatus) ->
    F = fun() -> 
		[OldBasicData] = mnesia:read({player_basic_data, Account}),
		NewPlayerData = OldBasicData#player_basic_data{username=UserName, 
							       sex=Sex,
							       health_status=HealthStatus},
		mnesia:write(NewPlayerData),
		mnesia:write(PlayerProperty)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

-spec update(atom(), string(), integer(), integer(), tuple(), integer(), tuple()) -> tuple().
update(Account, UserName, Sex, Time, PlayerProperty, HealthStatus, LogoutTime) ->
    F = fun() -> 
		[OldBasicData] = mnesia:read({player_basic_data, Account}),
		OnlineTime = OldBasicData#player_basic_data.online_time + Time,
		NewPlayerData = OldBasicData#player_basic_data{username=UserName, 
							       sex=Sex, 
							       online_time=OnlineTime,
							       logout_time=LogoutTime,
							       health_status=HealthStatus},
		mnesia:write(NewPlayerData),
		mnesia:write(PlayerProperty)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

update(Account, UserName, PlayerData) ->
    NPlayerData = PlayerData#player_basic_data{username=UserName},
    db_common:write(NPlayerData),
    put(Account, NPlayerData).


%%
%% 获取玩家数据
%% 格式：{#player_basic_data}
%% 每次都从数据库读取数据
%%
-spec select(atom()) -> list() | tuple().
select(Account) ->
    F = fun() -> 
		mnesia:read({player_basic_data, Account})
	end,
    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    [];
	[BasicData] ->
	    BasicData
    end.
%%
%% 读取一次数据库数据，读取完后将数据放入进程字典中,
%% 第二次将从数据字典中获取数据
%%
select(once, Account) ->
    case get(Account) of
	undefined ->
	    BasicData = select(Account),
	    case BasicData of
		[] ->
		    [];
		_ ->
		    case put(Account, BasicData) of
			undefined ->
			    BasicData;
			ExistData ->
			    erlang:error({'duplicate player data', Account, ExistData})
		    end
	    end;
        PlayerBasicData ->
	    PlayerBasicData
    end;
select(basic_data, Account) ->
    F = fun() -> 
		Q = qlc:q([[PlayerBasicData, PlayerProperty] 
			   || PlayerBasicData <- mnesia:table(player_basic_data),
			      PlayerBasicData#player_basic_data.account =:= Account,
			      PlayerProperty <- mnesia:table(player_property),
			      PlayerBasicData#player_basic_data.account =:= PlayerProperty#player_property.account]),
		qlc:e(Q)
	end,
    case mnesia:activity(transaction, F, [], mnesia_frag) of
	[] ->
	    [];
	[BasicData] ->
	    BasicData
    end.
%%-----------------------------------------------------------------------------------

%%------------------------------online_player----------------------------------------

%%
%% 玩家是否在线
%%
-spec is_online(atom()) -> atom().
is_online(Account) ->
    F = fun() -> 
		mnesia:read({online_player, Account}) 
	end,
    Record = mnesia:activity(transaction, F, [], mnesia_frag),
    has_record(Record).

%%
%% 加入在线玩家
%%
-spec insert_online_player(atom()) -> tuple().
insert_online_player(Account) ->
    F = fun() -> 
		case mnesia:read({player_basic_data, Account}) of
		    [] ->
			[];
		    [BasicData] ->
			NBasicData = BasicData#player_basic_data{last_login_time=datetime:localtime()},
			mnesia:write(NBasicData)
		end,
		mnesia:write({online_player, Account, date(), node()})
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).

%%
%% 删除在线玩家
%%
-spec delete_online_player(atom()) -> tuple().
delete_online_player(Account) ->
    F = fun() -> mnesia:delete({online_player, Account}) end,
    mnesia:activity(transaction, F, [], mnesia_frag).

%%
%% 取所有在线玩家
%%
-spec get_online_players() -> list().
get_online_players() ->
    %% 直接用dirty方法，比较快速
    mnesia:dirty_all_keys(online_player).

get_online_players_count(Node) when is_list(Node) ->
    get_online_players_count(list_to_atom(Node));
get_online_players_count(Node) ->
    F = fun() -> 
		Q = qlc:q([OnlinePlayer || OnlinePlayer <- mnesia:table(online_player), 
					     OnlinePlayer#online_player.login_node =:= Node]),
		R = qlc:e(Q),
		length(R)
	end,

    mnesia:activity(transaction, F, [], mnesia_frag).

get_online_players_key(Node) ->
    F = fun() -> 
		Q = qlc:q([OnlinePlayer#online_player.account || OnlinePlayer <- mnesia:table(online_player), 
								   OnlinePlayer#online_player.login_node =:= Node]),
		qlc:e(Q)
	end,

    mnesia:activity(transaction, F, [], mnesia_frag).

%%-----------------------------------------------------------------------------------
