%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   ��Ҵ��·�
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
%%% ������ػص�����
%%%===================================================================
%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
get_action_register_msgs(_Account, _PID) ->
    [
    ].

%% �жϵ�ǰ״̬�ܷ�ʼ����
%% ����true | {false, Error}
can_start_action(_Param, _State) ->
    true.

%% ��ʼ
do_start_action({Type1, Index1, Type2, Index2},  PlayerData) ->
    player_pack:move_item(Type1, Index1, Type2, Index2, PlayerData);
do_start_action({Account, Index, Grids},  PlayerData) ->
    player_pack:do_use_item(Account, Index, Grids, PlayerData).


%% ����
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action(_Param, State) ->
    State.

