%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   玩家穿衣服
%%% @end
%%% Created : 29 Mar 2011 by hongjx <hongjx@35info.cn>

-module(player_wear).


-behaviour(player_action).

-include("packet_def.hrl").
-include("sys_msg.hrl").
-include("tplt_def.hrl").
-include("common_def.hrl").
-include("enum_def.hrl").
-include("mutex_actions.hrl").


%%-export([handle_cast/2, start/1]).


-export([do_start_action/2, do_stop_action/2,
	 can_start_action/2, get_action_register_msgs/2]).



%% start(_Account) ->
%%     [
%% %%     router:make_event_source(?msg_notify_server_empty_box_positions, Account, {self(), ?MODULE}),
%% %%     router:make_event_source(?msg_req_pick_magic_box, Account, {self(), ?MODULE})
%%     ].


%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(_Account, _PID) ->
    [
    ].

%% 判断当前状态能否开始动作
%% 返回true | {false, Error}
can_start_action(_Param, _State) ->
    true.

%% 开始
do_start_action({Type1, Index1, Type2, Index2},  PlayerData) ->
    player_pack:move_item(Type1, Index1, Type2, Index2, PlayerData);
do_start_action({Account, Index, Grids},  PlayerData) ->
    player_pack:do_use_item(Account, Index, Grids, PlayerData).


%% 结束
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action(_Param, State) ->
    State.

