%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   好友数据库操作
%%% @end
%%% Created : 11 May 2010 by  <hjx>

-module(db_friends).

%% Include files
-include("packet_def.hrl").
-include("enum_def.hrl").

%% Exported Functions

-export([get_friends/1, sync_friend_list/2, 
	add_friend/2, remove_friend/2]).

-export([get_accept_invite/1, get_offline_invite_list/1, add_offline_invite/2,
	 del_offline_invite/3, get_setting/1,
	set_accept_invite/2, black_someone/2, get_black_list/1]).

-define(table_name, player_friends).

-define(invitor_index, 1).


-record(friend_setting, {account,
			 accept_invite=?accept_invite_yes, 			 
			 black_list=[]}).




%% 获取朋友列表
-spec get_friends(atom()) -> list().
get_friends(Account) when is_atom(Account) ->
    RawFriends = db_common:select_list(?table_name, Account),
    Friends = [Friend || {?table_name, _Account, Friend} <- RawFriends],
    Friends.



%% 加好友
-spec add_friend(atom(), atom()) -> any().
add_friend(Account, FriendAccount) 
  when is_atom(Account), is_atom(FriendAccount) ->
    OpAddFriends = {write, [{?table_name, Account, FriendAccount}, 
			   {?table_name, FriendAccount, Account}]},
    db_common:change([OpAddFriends]).



%% 删除好友
-spec remove_friend(atom(), atom()) -> any().
remove_friend(Account, FriendAccount) 
  when is_atom(Account), is_atom(FriendAccount) ->
    OpDeleteFriends = {delete_object, [{?table_name, Account, FriendAccount},
				      {?table_name, FriendAccount, Account}]},

    db_common:change([OpDeleteFriends]).


%% 同步好友
sync_friend_list(Account, FriendList) when is_list(FriendList) ->
    %% 2011.08.31 乐园的好友列表改为包含35EQ的好友列表
    %%OpDeleteFriends = {delete, [{?table_name, Account}]},  
    A = [{?table_name, Account, X} || X <- FriendList],
    B = [{?table_name, X, Account} || X <- FriendList],
    OpAddFriends = {write, A ++ B},

    db_common:change([%%OpDeleteFriends, 
		      OpAddFriends]).

% 取邀请选项
get_accept_invite(Owner) when is_atom(Owner) ->
    #friend_setting{accept_invite=Val} = get_setting(Owner),
    Val;
get_accept_invite(Setting) ->
    Setting#friend_setting.accept_invite.

% 设邀请选项
set_accept_invite(Owner, Val) ->
    Setting = get_setting(Owner),
    NewSetting = Setting#friend_setting{accept_invite=Val},
    db_common:write(NewSetting).    
    

get_black_list(Owner) when is_atom(Owner) ->
    Setting = get_setting(Owner),
    Setting#friend_setting.black_list;
get_black_list(Setting) ->
    Setting#friend_setting.black_list.
    

% 将某人加入黑名单
black_someone(Owner, BlackAccount) when is_atom(Owner), is_list(BlackAccount) ->
    Setting = get_setting(Owner),
    OldList = Setting#friend_setting.black_list,
    NewList = case lists:member(BlackAccount, OldList) of
		  true -> OldList;
		  _ -> [BlackAccount | OldList]
	      end,

    NewSetting = Setting#friend_setting{black_list=NewList},
    db_common:write(NewSetting).    

get_offline_invite_list(Owner) when is_atom(Owner) ->
    case db_common:select_one(offline_invite, Owner) of
	[] -> [];
	{_Tab, _Owner, List} -> List
    end.

add_offline_invite(Owner, Friend) when is_atom(Owner), is_atom(Friend) ->
    DateTime = datetime:localtime(),
    List = get_offline_invite_list(Owner),
    List1 = case lists:keymember(Friend, ?invitor_index, List) of
		  true -> 
		      lists:keydelete(Friend, ?invitor_index, List);
		  _ ->
		      List
	      end,
    NewList = [{Friend, ?ivt_make_friend, DateTime} | List1],
    %% 限制条数
    Max = common_def:get_val(max_offline_invite_count),
    NewList2 =
	case length(NewList) > Max of
	    true ->
		lists:sublist(NewList, Max);
	    _ -> NewList
	end,
    
    db_common:write({offline_invite, Owner, NewList2}).

del_offline_invite(Owner, Invitor, _Type) when is_atom(Invitor) ->
    List = get_offline_invite_list(Owner),
    case lists:keymember(Invitor, ?invitor_index, List) of
	true -> 
	    NewList = lists:keydelete(Invitor, ?invitor_index, List),
	    db_common:write({offline_invite, Owner, NewList});
	_ ->
	    ok
    end.
    
get_setting(Owner) when is_atom(Owner) ->
    case db_common:select_one(friend_setting, Owner) of
	[] ->
	    DefSetting = #friend_setting{account=Owner},
	    DefSetting;
	Data ->
	    Data
    end.

%%%===================================================================
%%% 内部函数
%%%===================================================================


%%%===================================================================
%%% 测试函数
%%%===================================================================

