%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 农场留言
%%% @end
%%% Created : 18 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_comment).

-include("packet_def.hrl").
-include("router.hrl").
-include("player_data.hrl").
-include("common_def.hrl").
-include("enum_def.hrl").

-define(MAXCOMMENT, 50).
-define(MAXCOMMENTQTY, 200).

%% API
-export([new/1, start/1, handle_cast/2]).
-export([test/0]).

new(Account) ->
    #player_farm_comment{account=Account,
			 comment=[]}.

start(Account) ->
    [router:make_event_source(?msg_req_farm_comment_list, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_farm_comment, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_reply_farm_comment, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_clear_farm_comment, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% Handle cast
%%%===================================================================
%% 请求获取日志列表
handle_cast({_Msg, #req_farm_comment_list{account=Owner1}}, State)->
    Account = farm:get_account(State),
    Owner = list_to_atom(Owner1),
    PlayerFarmComment = db_common:select_one(player_farm_comment, Owner),
    net_helper:farm2client(Account, #notify_farm_comment_list{farm_comment=PlayerFarmComment}),
    {noreply, State};
%% 请求发布留言
handle_cast({_Msg, #req_farm_comment{account=Owner1, comment=Comment}}, State) ->
    data_helper:format("State:~p~n", [State]),
    Account = farm:get_account(State),
    EQPid = farm:get_eq_pid(State),
    Owner = list_to_atom(Owner1),
    case util:len(Comment) - 1 =< ?MAXCOMMENT of %% 判断留言长度
	true ->
	    PlayerFarmComment = db_common:select_one(player_farm_comment, Owner),
	    UserName = eq_manager:get_user_name(EQPid),
	    {NPlayerFarmComment, FarmComment} = 
		case Account =:= Owner of %% 判断是否是主人自己留言
		    true ->
			write(?fct_owner, PlayerFarmComment, Owner, UserName, Comment);
		    _ ->
			write(?fct_guest, PlayerFarmComment, Account, UserName, Comment)
		end,
	    %% 保存数据库
	    db_common:write(NPlayerFarmComment),
	    net_helper:farm2client(Account, #notify_farm_comment{comment=FarmComment});
	_ ->
	    erlang:error(badarg, Comment)
    end,
    {noreply, State};
%% 请求回复留言
handle_cast({_Msg, #req_reply_farm_comment{account=Guest1, comment=Comment}}, State) ->
    Account = farm:get_account(State),
    Guest = list_to_atom(Guest1),
    case util:len(Comment) - 1 =< ?MAXCOMMENT of
	true ->
	    PlayerFarmComment = db_common:select_one(player_farm_comment, Account),
	    {NPlayerFarmComment, FarmComment} = 
		write(?fct_reply, PlayerFarmComment, Guest, db_player:get_user_name(Guest), Comment),
	    db_common:write(NPlayerFarmComment),
	    net_helper:farm2client(Account, #notify_reply_farm_comment{comment=FarmComment});
	_ ->
	    erlang:error(badarg, Comment)
    end,
    {noreply, State};
%% 请求清空日志列表
handle_cast({_Msg, #req_clear_farm_comment{}}, State) ->
    Account = farm:get_account(State),
    PlayerFarmComment = db_common:select_one(player_farm_comment, Account),
    NPlayerFarmComment = clear(PlayerFarmComment),
    db_common:write(NPlayerFarmComment),
    net_helper:farm2client(Account, #notify_farm_comment_list{farm_comment=NPlayerFarmComment}),
    {noreply, State}.

%%%===================================================================
%%% API
%%%===================================================================


%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 清空日志
-spec clear(tuple()) -> tuple().
clear(PlayerFarmComment) when is_tuple(PlayerFarmComment) ->
    PlayerFarmComment#player_farm_comment{comment=[]}.

%% 保存日志
write(Type, PlayerFarmComment, Account, UserName, Comment) ->
    FarmComment = #farm_comment{type=Type,
				account=Account,
				comment_date=datetime:localtime(),
				username=UserName,
				content=Comment},
    NFarmComment = [FarmComment | PlayerFarmComment#player_farm_comment.comment],
    NFarmComment1 = lists:sublist(NFarmComment, ?MAXCOMMENTQTY),
    {PlayerFarmComment#player_farm_comment{comment=NFarmComment1}, FarmComment}.
%%%===================================================================
%%% Test case
%%%===================================================================
test() ->
    router:cast(test1, ?msg_req_clear_farm_comment, #req_clear_farm_comment{}),
    router:cast(test1, ?msg_req_farm_comment_list, #req_farm_comment_list{account="test1"}),
    router:cast(test1, ?msg_req_farm_comment, #req_farm_comment{account="test1", comment="123"}),
    router:cast(test2, ?msg_req_farm_comment, #req_farm_comment{account="test1", comment="dm"}),
    router:cast(test1, ?msg_req_reply_farm_comment, #req_reply_farm_comment{account="test1", comment="222"}),
    test_write().

test_write() ->
    PlayerFarmComment = #player_farm_comment{
      comment=[#farm_comment{type=1, account=2, comment_date=datetime:localtime(), username=test, content="test"},
	       #farm_comment{type=1, account=2, comment_date=datetime:localtime(), username=test, content="test"},
	       #farm_comment{type=1, account=2, comment_date=datetime:localtime(), username=test, content="test"}]
     },
    write(1, PlayerFarmComment, 1, 11, "23").
