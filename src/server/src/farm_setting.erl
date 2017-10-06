%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 农场设置
%%% @end
%%% Created : 22 Jun 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(farm_setting).

-include("packet_def.hrl").
-include("player_data.hrl").
-include("tplt_def.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
%% API
-export([start/1, handle_cast/2]).
-export([new/1, get/2, prompt/3]).

-define(MAXLENGTH, 12).              %% 内容最大长度

start(Account) ->
    [router:make_event_source(?msg_req_farm_setting, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_set_farm_setting, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_reset_farm_setting, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% Handle cast
%%%===================================================================
handle_cast({_Msg, #req_farm_setting{}}, State)->
    Account = farm:get_account(State),
    FarmSetting = db_common:select_one(farm_setting, Account),
    net_helper:farm2client(Account, #notify_farm_setting{setting=FarmSetting}),
    {noreply, State};
%% 修改农场设置的内容
handle_cast({_Msg, #req_set_farm_setting{setting=Setting}}, State)->
    Account = farm:get_account(State),
    SettingTplt = get_setting_tplt(),
    Water = filter(Setting#farm_setting.water, SettingTplt#farm_setting_tplt.water),
    Weed = filter(Setting#farm_setting.weed, SettingTplt#farm_setting_tplt.weed),
    Worm = filter(Setting#farm_setting.worm, SettingTplt#farm_setting_tplt.worm),
    PutGrass = filter(Setting#farm_setting.put_grass, SettingTplt#farm_setting_tplt.put_grass),
    PutPest = filter(Setting#farm_setting.put_pest, SettingTplt#farm_setting_tplt.put_pest),
    case can_setting(Water, Weed, Worm, PutGrass, PutPest) of
	true ->
	    FarmSetting = db_common:select_one(farm_setting, Account),
    	    NFarmSetting = FarmSetting#farm_setting{water=Water, 
						    weed=Weed,
						    worm=Worm,
						    put_grass=PutGrass,
						    put_pest=PutPest},
    	    db_common:write(NFarmSetting),
	    farm_sys_msg:send(Account, ?msg_farm_setting_success);
	_ ->
	    farm_sys_msg:send(Account, ?msg_farm_setting_fail)
    end,
    {noreply, State};
%% 重置农场设置的内容
handle_cast({_Msg, #req_reset_farm_setting{}}, State)->
    Account = farm:get_account(State),
    FarmSetting = db_common:select_one(farm_setting, Account),
    SettingTplt = get_setting_tplt(),
    NFarmSetting = FarmSetting#farm_setting{water = translate(SettingTplt#farm_setting_tplt.water),
					    weed = translate(SettingTplt#farm_setting_tplt.weed),
					    worm = translate(SettingTplt#farm_setting_tplt.worm),
					    put_grass = translate(SettingTplt#farm_setting_tplt.put_grass),
					    put_pest = translate(SettingTplt#farm_setting_tplt.put_pest)
					   },
    db_common:write(NFarmSetting),
    net_helper:farm2client(Account, #notify_reset_farm_setting{setting=NFarmSetting}),
    farm_sys_msg:send(Account, ?msg_farm_reset_setting_success),
    {noreply, State}.
%%%===================================================================
%%% API
%%%===================================================================
%% 初始化农场设置
-spec new(atom()) -> tuple().
new(Account) ->
    SettingTplt = get_setting_tplt(),
    #farm_setting{account=Account,
		  water=SettingTplt#farm_setting_tplt.water,
		  weed=SettingTplt#farm_setting_tplt.weed,
		  worm=SettingTplt#farm_setting_tplt.worm,
		  put_grass=SettingTplt#farm_setting_tplt.put_grass,
		  put_pest=SettingTplt#farm_setting_tplt.put_pest
		 }.

translate(Content) ->
    binary_to_list(Content).

%% 获取农场设置内容
-spec get(atom(), atom()) -> list().
get(water, Account) when is_atom(Account) ->
    FarmSetting = db_common:select_one(farm_setting, Account),
    FarmSetting#farm_setting.water;
get(weed, Account) when is_atom(Account) ->
    FarmSetting = db_common:select_one(farm_setting, Account),
    FarmSetting#farm_setting.weed;
get(worm, Account) when is_atom(Account) ->
    FarmSetting = db_common:select_one(farm_setting, Account),
    FarmSetting#farm_setting.worm;
get(put_grass, Account) when is_atom(Account) ->
    FarmSetting = db_common:select_one(farm_setting, Account),
    FarmSetting#farm_setting.put_grass;
get(put_pest, Account) when is_atom(Account) ->
    FarmSetting = db_common:select_one(farm_setting, Account),
    FarmSetting#farm_setting.put_pest.

%% 提示农场设置
-spec prompt(atom(), atom(), atom()) -> any().
prompt(Type, Account, Owner) ->
    Prompt = get(Type, Owner),
    case Account =:= Owner of
	true ->
	    ok;
	false ->
	    farm_sys_msg:send(Account, ?msg_farm_setting_prompt, [Prompt])
    end.

%% 获取农场设置模板
-spec get_setting_tplt() -> tuple().
get_setting_tplt() ->
    [SettingTplt|_SettingList] = tplt:get_all_data(farm_setting_tplt),
    SettingTplt.

%%%===================================================================
%%% Internal functions
%%%===================================================================
filter(Content, Default) ->
    case is_empty(Content) of
	true ->
	    binary_to_list(Default);
	_ ->
	    Content
    end.

%% 是否为空
-spec is_empty(list()) -> atom().
is_empty(Content) when is_list(Content) ->
    length(string:strip(Content, both, $ )) =< 0.

%% 能否设置
-spec can_setting(list(), list(), list(), list(), list()) -> any().
can_setting(Water, Weed, Worm, PutGrass, PutPest) 
  when is_list(Water), is_list(Weed), is_list(Worm), is_list(PutGrass), is_list(PutPest)->
    Func = fun({Content}) ->
		   util:len(Content) =< ?MAXLENGTH
	   end,
    action:can_do([{Func, {Water}, err_water},
		   {Func, {Weed}, err_weed},
		   {Func, {Worm}, err_worm},
		   {Func, {PutGrass}, err_put_grass},
		   {Func, {PutPest}, err_put_pest}
		  ]).
%%%===================================================================
%%% Test cast
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

is_empty_test() ->
    ?assertEqual(true, is_empty("")),
    ?assertEqual(false, is_empty("  1  ")),
    ?assertEqual(true, is_empty("        ")).

filter_test() ->
    ?assertEqual("test1", filter("test1", <<"test2">>)),
    ?assertEqual("test2", filter("     ", <<"test2">>)),
    ?assertEqual("  1  ", filter("  1  ", <<"test2">>)).

can_setting_test() ->
    ?assertEqual(true, can_setting("111111111111", "111111111111", "111111111111", "111111111111", "111111111111")),
    ?assertEqual({false, err_put_pest}, can_setting("111111111111", "111111111111", "111111111111", "111111111111", "1111111111111")),
    ?assertEqual({false, err_water}, can_setting("1111111111111", "111111111111", "111111111111", "111111111111", "111111111111")).

-endif.
    
