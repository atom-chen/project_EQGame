%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  系统消息的提示模块
%%% @end
%%% Created : 25 Mar 2010 by  <>
%%%-------------------------------------------------------------------
-module(sys_msg).

-include("packet_def.hrl").

%% API
-export([send/2, send/3, sendall/0, save_offline_msgs/2, load_offline_sys_msg/1]).

-record(sys_msg, {account,         %% 账号
		  msg_list=[]      %% 消息列表
		 }).

%%%===================================================================
%%% API
%%%===================================================================
%% 发送系统消息给客户端
%% Player: 玩家帐号
%% SysType: 系统类型, 参见common_def.hrl定义
%% MsgCode: 消息代码, 参见sys_msg.hrl定义
send(Player, MsgCode)->
    send(Player, MsgCode, []).

%% Params: [Param | ...]
%% Param: int, float, atom, string
send(Player, MsgCode, Params)->
    NParams = [translate(P) || P <- Params],
    net_helper:send2client(Player, #notify_sys_msg{code=MsgCode, params=NParams}).

translate(Data) when is_integer(Data) ->
    integer_to_list(Data);
translate(Data) when is_float(Data) ->
    [Str] = io_lib:format("~p", [Data]),
    Str;
translate(Data) when is_atom(Data) ->
    atom_to_list(Data);
translate(Data) when is_binary(Data) ->
    binary_to_list(Data);
translate(Data) when is_list(Data) ->
    Data;
translate(Data) ->
    erlang:error({badtype, Data}).


%% TODO: 暂时未开发
sendall()->
    ok.


%% 取离线消息
get_offline_msgs(Account) when is_atom(Account) ->
    case db_common:select_one(sys_msg, Account) of
	[] ->
	    [];
	#sys_msg{msg_list=Msgs} -> 
	    Msgs
    end.    

%% 发送离线消息, 玩家上线会收到通知
save_offline_msgs(Account, MsgList) when is_atom(Account), is_list(MsgList) ->    
    F = fun(#sys_msg{msg_list=OldMsgs}=R) -> 
		R#sys_msg{msg_list=MsgList ++ OldMsgs}
	end,
    DefRecord = #sys_msg{account=Account},
    db_common:modify(sys_msg, Account, F, DefRecord).	    


%% 玩家不在线时，保存的一些系统消息, 玩家上线后，应通知玩家
load_offline_sys_msg(Account) ->
    Msgs = get_offline_msgs(Account),
    [sys_msg:send(Account, MsgID, Params) || {MsgID, Params} <- Msgs],
    db_common:delete(sys_msg, Account),
    ok.


