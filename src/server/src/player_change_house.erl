%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%% 玩家切换房屋
%%% @end
%%% Created : 22 Mar 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(player_change_house).

-include("packet_def.hrl").
-include("sys_msg.hrl").
-include("mutex_actions.hrl").

%% API
-export([start/1]).
-export([handle_cast/2]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).

-export([test/0]).


%%%===================================================================
%%% API
%%%===================================================================
start(Account) ->
    [
     router:make_event_source(?msg_req_house_list, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_change_house, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(_Account, _Pid) ->
    [].

can_start_action(_Param, _State) ->
    true.

do_start_action({InstId, Type}, State) ->
    Account = player_data:get_account(State),
    SceneName = scene_manager:get_scene_name(Account),
    player:kick_guest(Account, SceneName, ?msg_change_house),
    NewState = 
	case router:send(SceneName, change_house, {Account, InstId, Type}) of
	    {Players, HouseData} ->
		{Pos, _BornID} = house:get_born_pos(HouseData),
		NState = player_data:set_pos(Pos, State),
		NNState = player_data:set_dest_pos(Pos, NState),
		do_notify_change_house(Account, Players, NNState),
		NNState;	
	    _ ->
		house:db_change_house(Account, InstId, Type),
		State
	end,

    net_helper:send2client(Account, #notify_change_house{inst_id=InstId}),
    NewState.

%% 结束动作
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action({auto_stop, _Reason}, State) ->
    do_stop_action(stop, State);

%% 结束动作
do_stop_action(stop, State) ->
    State.

%%%===================================================================
%%% Handle cast
%%%===================================================================
%% 请求获取房屋列表
handle_cast({_Msg, #req_house_list{account=Account}}, State) ->
    Master = player_data:get_account(State),
    AccountAtom = list_to_atom(Account),
    HousePlot = house_plot:get(AccountAtom),
    Plots = house_plot:get_plots(HousePlot),
    HouseVec = do_house_vec(Plots, []),
    CurrHouse = do_curr_house(AccountAtom),
    NHouseVec = [CurrHouse | HouseVec],
    notify_house_vec(Master, AccountAtom, NHouseVec),
    {noreply, State};
%% 处理玩家进入家装模式请求
handle_cast({_Msg, #req_change_house{inst_id=InstId, type=Type}}, State) ->
    NState = mutex_actions:try_start_action(?action_type_change_house, {InstId, Type}, State),
    {noreply, NState}.

do_notify_change_house(Account, Players, State) when is_list(Players) ->
    case lists:member(Account, Players) of
	true -> 
	    player_change_scene:enter_player_scene(Account, State);
	_ -> 
	    ok
    end.    
%%%===================================================================
%%% Internal functions
%%%===================================================================
notify_house_vec(Master, Account, HouseVec) ->
    NHouseVec = lists:reverse(lists:keysort(2, HouseVec)),
    net_helper:send2client(Master, #notify_house_list{account=Account, house_vec=NHouseVec}).

%% 生成玩家拥有的房屋
do_house_vec([], HouseVec) ->
    HouseVec;
do_house_vec([Plot|Plots], HouseVec) ->
    PlotId = house_plot:get_plot_id(Plot),
    HouseData = house_plot:get_house_data(Plot),
    House = do_house(HouseData, PlotId),
    do_house_vec(Plots, [House|HouseVec]). 

do_house([], PlotId) ->
    #house_plot_info{plot_id=PlotId, 
		     buy_date = datetime:empty(),
		     instance_id=0, 
		     template_id=0};
do_house(HouseData, PlotId) ->
    InstId = house_data:get_instance_id(HouseData),
    TempId = house_data:get_template_id(HouseData),
    BuyDate = house_data:get_buy_date(HouseData),
    #house_plot_info{plot_id=PlotId, 
		     buy_date = BuyDate,
		     instance_id=InstId, 
		     template_id=TempId}.

do_curr_house(Account) ->
    SceneName = scene_manager:get_scene_name(Account),
    CustomData = router:send(SceneName, get_custom_data),
    HouseData = house:get_house_data(CustomData),
    do_house(HouseData, 0).

%%%===================================================================
%%% Test
%%%==================================================================
test() ->
    test_req_house_list().

test_req_house_list() ->
    router:cast(test1, ?msg_req_house_list, #req_house_list{}).
