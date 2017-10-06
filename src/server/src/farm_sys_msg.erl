%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% ũ��ϵͳ��Ϣ
%%% @end
%%% Created : 22 Nov 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm_sys_msg).

%% API
-export([send/2, send/3]).


%%%===================================================================
%%% API
%%%===================================================================

%% ����ϵͳ��Ϣ���ͻ���
%% Player: ����ʺ�
%% SysType: ϵͳ����, �μ�common_def.hrl����
%% MsgCode: ��Ϣ����, �μ�sys_msg.hrl����
send(Player, MsgCode)->
    Account = farm:get_name(Player),
    sys_msg:send(Account, MsgCode).

%% Params: [Param | ...]
%% Param: int, float, atom, string
send(Player, MsgCode, Params)->
    Account = farm:get_name(Player),
    sys_msg:send(Account, MsgCode, Params).

%%%===================================================================
%%% Internal functions
%%%===================================================================
