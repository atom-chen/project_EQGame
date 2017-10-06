%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 房屋设置
%%% @end
%%% Created :  4 Aug 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(player_house).

-include("router.hrl").
-include("enum_def.hrl").
-include("packet_def.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").

%% API
-export([start/1]).
-export([handle_cast/2]).
-export([test/0]).

%%%===================================================================
%%% API
%%%===================================================================
start(Account) ->
    [router:make_event_source(?msg_req_house_basic_data, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_set_house_welcome_words, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_set_house_permission, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_clear_house_log, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% Handle cast
%%%===================================================================
%% 请求房屋基本数据
handle_cast({_Msg, #req_house_basic_data{}}, State) ->
    Account = player_data:get_account(State),
    HouseData = db_common:select_one(house_data, Account),
    NHouseData = player_house_log:reset(HouseData),
    net_helper:send2client(Account, #notify_house_basic_data{data=NHouseData#house_data{
								    furniture_vec = [],
								    room_tex_vec = [],
								    component_tex_vec = [],
								    component_mesh_vec = []
								   }}),
    {noreply, State};
%% 请求设置房屋欢迎词
handle_cast({_Msg, #req_set_house_welcome_words{words=Words}}, State) ->
    Account = player_data:get_account(State),
    HouseData = db_common:select_one(house_data, Account),
    case can_setting(Words, 128) of
	true ->
	    NHouseData = HouseData#house_data{welcome_words = Words},
	    db_common:write(NHouseData),
	    update_house_data(Account, NHouseData),
	    net_helper:send2client(Account, #notify_set_house_welcome_words{result=?shwwr_ok});
	{false, err_words} ->
	    sys_msg:send(Account, ?err_welcome_words)
    end,
    {noreply, State};
%% 请求设置房屋权限
handle_cast({_Msg, #req_set_house_permission{house_permission=HousePermission, 
					     furniture_permission=FurniPermission}}, 
	    State) ->
    Account = player_data:get_account(State),
    HouseData = db_common:select_one(house_data, Account),
    data_helper:format("HousePermission:~p, none:~p, friend:~p, FurniPermission:~p, yes:~p, no:~p~n", [HousePermission, ?hpt_none, ?hpt_friend, FurniPermission, ?fpt_yes, ?fpt_no]),
    NHouseData = HouseData#house_data{house_permission=HousePermission, furniture_permission=FurniPermission},
    db_common:write(NHouseData),
    update_house_data(Account, NHouseData),
    net_helper:send2client(Account, #notify_set_house_permission{result=?shpr_ok}),
    {noreply, State};
%% 请求清空房屋日志
handle_cast({_Msg, #req_clear_house_log{}}, State) ->
    Account = player_data:get_account(State),
    HouseData = db_common:select_one(house_data, Account),
    NHouseData = player_house_log:clear(HouseData),
    db_common:write(NHouseData),
    update_house_data(Account, NHouseData),
    net_helper:send2client(Account, #notify_clear_house_log{result=?chlr_ok}),
    {noreply, State}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 能否设置
-spec can_setting(list(), integer()) -> tuple() | atom().
can_setting(Words, MaxLength)  
  when is_list(Words), is_integer(MaxLength)->
    Func = fun(Content) ->
		   util:len(Content) =< MaxLength
	   end,
    action:can_do([{Func, Words, err_words}]).

%% 更新房屋数据
-spec update_house_data(atom(), tuple()) -> atom().
update_house_data(Account, HouseData) 
  when is_atom(Account), is_tuple(HouseData)->
    HouseName = house:get_house_name(Account),
    case util:is_process_alive(HouseName) of
	false ->
	    ok;
	_ ->
	    router:send(HouseName, update_house_data, HouseData)
    end.

%%%===================================================================
%%% Test case
%%%===================================================================
test() ->
    router:cast(test1, ?msg_req_house_basic_data, #req_house_basic_data{}),
    router:cast(test1, ?msg_req_set_house_welcome_words, #req_set_house_welcome_words{words="1234567890"}),
    router:cast(test1, ?msg_req_set_house_permission, #req_set_house_permission{house_permission=?hpt_friend,
										furniture_permission=?fpt_no}),
    router:cast(test1, ?msg_req_clear_house_log, #req_clear_house_log{}).


-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

can_setting_test() ->
    ?assertEqual(true, can_setting("1111111111", 128)),
    ?assertEqual({false, err_words}, can_setting("12345", 4)),
    ?assertEqual(true, can_setting("1234", 4)),
    ?assertEqual(true, can_setting("123", 4)).

-endif.
