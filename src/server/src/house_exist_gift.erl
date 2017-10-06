%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 房屋礼物动作
%%% @end
%%% Created :  2 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(house_exist_gift).

-export([start/1, handle_cast/2]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).

-include("mutex_actions.hrl").
-include("router.hrl").

%% API
-spec start(atom()) -> list().
start(Account) ->
    [
     router:make_event_source(start_house_gift, Account, {self(), ?MODULE}),
     router:make_event_source(end_house_gift, Account, {self(), ?MODULE}),
     router:make_event_source(restart_house_gift, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(_Account, _Pid) ->
    [].

can_start_action(_Param, _State) ->
    true.

do_start_action(_Param, State) ->
    State.

%% 结束动作
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action({auto_stop, _Reason}, State) ->
    do_stop_action(stop, State);

%% 结束动作
do_stop_action(stop, State) ->
    State.

%%%===================================================================
%%% API
%%%===================================================================

   
%%%===================================================================
%%% Handle cast
%%%===================================================================
%% 开始房屋礼物动作
handle_cast({#msg{event=start_house_gift}, Data}, State) ->    
    NState = start_house_gift(Data, State),
    {noreply, NState};

%% 重新计算房屋存在礼物互斥
handle_cast({#msg{event=restart_house_gift}, Data}, State) ->
    NState = stop_house_gift(Data, State),
    NNState = start_house_gift(Data, NState),
    {noreply, NNState};

%% 请求
handle_cast({#msg{event=end_house_gift}, Data}, State) ->
    NState = stop_house_gift(Data, State),
    {noreply, NState}.
%%%===================================================================
%%% Internal functions
%%%===================================================================
start_house_gift(Data, State) ->
    IsContainsAction = mutex_actions:contains_action(?action_type_house_exist_gift, State),
    case (IsContainsAction == false) and (length(Data) > 0) of %% 如果房屋存在礼物，开始互斥系统
	true ->
	    mutex_actions:try_start_action(?action_type_house_exist_gift, undefined, State);
	false ->
	    State
    end.

stop_house_gift(Data, State) ->
    IsContainsAction = mutex_actions:contains_action(?action_type_house_exist_gift, State),
    case (IsContainsAction == true) and (length(Data) == 0) of %% 如果房屋存在礼物，开始互斥系统
	true ->
	    mutex_actions:call_stop_action(?action_type_house_exist_gift, stop, State);
	false ->
	    State
    end.    
