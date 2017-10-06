%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%
%%% @end
%%% Created : 28 Apr 2011 by hongjx <hongjx@35info.cn>

-module(player_preview_house).

-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("router.hrl").

-behaviour(player_action).


-export([handle_cast/2, start/1]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).


start(Account) ->
    [
     router:make_event_source(?msg_req_preview_shop_house, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_preview_house, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_end_preview_house, Account, {self(), ?MODULE})
    ].
    
%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(_Account, _PID) ->
    [
    ].

%% 判断当前状态能否开始动作
%% 返回true | {false, Error}
can_start_action(_Param,  _State) ->
    true.

%% 开始动作
do_start_action({AccountAtom, InstId}, State) ->
    Master = player_data:get_account(State),
    HousePlot = house_plot:get(AccountAtom),
    PlotId = house_plot:get_house_plot_id(HousePlot, InstId),
    HouseData = house_plot:get_house(HousePlot, PlotId),
    net_helper:send2client(Master, #notify_preview_house{house=HouseData}),
    net_helper:send2client(Master, #notify_finish_enter_scene{}),       
    State;
do_start_action(HouseId, State) when is_integer(HouseId) ->
    Account = player_data:get_account(State),
    HouseData = house:get_house_data(Account, HouseId),
    net_helper:send2client(Account, #notify_house_data{data=HouseData}),
    net_helper:send2client(Account, #notify_finish_enter_scene{}),    
    State.

%% 结束动作
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action(_Param, State) ->
    Account = player_data:get_account(State),
    SceneName = player_data:get_scene_name(State),
    Pos = player_data:get_pos(State),
    net_helper:send2client(Account, #notify_end_preview_house{}),
    router:send(SceneName, resee_scene, {Account, Pos}),
    State.


%%%===================================================================
%%% 处理handle_cast
%%%===================================================================
%% 玩家房屋
handle_cast({_Msg, #req_preview_house{account=Account, inst_id=InstId}}, State) ->
    AccountAtom = list_to_atom(Account),
    NewState = mutex_actions:try_start_action(?action_type_preview_house, {AccountAtom, InstId}, State),
    {noreply, NewState};
%% 售楼处的房屋
handle_cast({_Msg, #req_preview_shop_house{house_id=HouseId}}, State) ->
    NewState = mutex_actions:try_start_action(?action_type_preview_house, HouseId, State),
    {noreply, NewState};
%% 处理玩家重新看到场景
handle_cast({_Msg, 
	     #req_end_preview_house{}}, State) ->
    NewState = mutex_actions:call_stop_action(?action_type_preview_house, undefined, State),
    {noreply, NewState}.	




