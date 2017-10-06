%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% �������ﶯ��
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
%%% ������ػص�����
%%%===================================================================
%% ȡ���������е���Ϣ�б�, ������ʼʱ��ע��, ����ʱ��ȡ�� 
get_action_register_msgs(_Account, _Pid) ->
    [].

can_start_action(_Param, _State) ->
    true.

do_start_action(_Param, State) ->
    State.

%% ��������
%% �Զ�����ʱ��Param ��ʽΪ {auto_stop, Reason}, �������Ϊcall_stop_action�����Param
%% �Զ���������ʱ��: ��ǰ�������¶���ֹͣʱ�����˳���Ϸʱ
do_stop_action({auto_stop, _Reason}, State) ->
    do_stop_action(stop, State);

%% ��������
do_stop_action(stop, State) ->
    State.

%%%===================================================================
%%% API
%%%===================================================================

   
%%%===================================================================
%%% Handle cast
%%%===================================================================
%% ��ʼ�������ﶯ��
handle_cast({#msg{event=start_house_gift}, Data}, State) ->    
    NState = start_house_gift(Data, State),
    {noreply, NState};

%% ���¼��㷿�ݴ������ﻥ��
handle_cast({#msg{event=restart_house_gift}, Data}, State) ->
    NState = stop_house_gift(Data, State),
    NNState = start_house_gift(Data, NState),
    {noreply, NNState};

%% ����
handle_cast({#msg{event=end_house_gift}, Data}, State) ->
    NState = stop_house_gift(Data, State),
    {noreply, NState}.
%%%===================================================================
%%% Internal functions
%%%===================================================================
start_house_gift(Data, State) ->
    IsContainsAction = mutex_actions:contains_action(?action_type_house_exist_gift, State),
    case (IsContainsAction == false) and (length(Data) > 0) of %% ������ݴ��������ʼ����ϵͳ
	true ->
	    mutex_actions:try_start_action(?action_type_house_exist_gift, undefined, State);
	false ->
	    State
    end.

stop_house_gift(Data, State) ->
    IsContainsAction = mutex_actions:contains_action(?action_type_house_exist_gift, State),
    case (IsContainsAction == true) and (length(Data) == 0) of %% ������ݴ��������ʼ����ϵͳ
	true ->
	    mutex_actions:call_stop_action(?action_type_house_exist_gift, stop, State);
	false ->
	    State
    end.    
