%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 农场系统消息
%%% @end
%%% Created : 22 Nov 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm_sys_msg).

%% API
-export([send/2, send/3]).


%%%===================================================================
%%% API
%%%===================================================================

%% 发送系统消息给客户端
%% Player: 玩家帐号
%% SysType: 系统类型, 参见common_def.hrl定义
%% MsgCode: 消息代码, 参见sys_msg.hrl定义
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
