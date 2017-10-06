%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  ϵͳ��Ϣ����ʾģ��
%%% @end
%%% Created : 25 Mar 2010 by  <>
%%%-------------------------------------------------------------------
-module(sys_msg).

-include("packet_def.hrl").

%% API
-export([send/2, send/3, sendall/0, save_offline_msgs/2, load_offline_sys_msg/1]).

-record(sys_msg, {account,         %% �˺�
		  msg_list=[]      %% ��Ϣ�б�
		 }).

%%%===================================================================
%%% API
%%%===================================================================
%% ����ϵͳ��Ϣ���ͻ���
%% Player: ����ʺ�
%% SysType: ϵͳ����, �μ�common_def.hrl����
%% MsgCode: ��Ϣ����, �μ�sys_msg.hrl����
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


%% TODO: ��ʱδ����
sendall()->
    ok.


%% ȡ������Ϣ
get_offline_msgs(Account) when is_atom(Account) ->
    case db_common:select_one(sys_msg, Account) of
	[] ->
	    [];
	#sys_msg{msg_list=Msgs} -> 
	    Msgs
    end.    

%% ����������Ϣ, ������߻��յ�֪ͨ
save_offline_msgs(Account, MsgList) when is_atom(Account), is_list(MsgList) ->    
    F = fun(#sys_msg{msg_list=OldMsgs}=R) -> 
		R#sys_msg{msg_list=MsgList ++ OldMsgs}
	end,
    DefRecord = #sys_msg{account=Account},
    db_common:modify(sys_msg, Account, F, DefRecord).	    


%% ��Ҳ�����ʱ�������һЩϵͳ��Ϣ, ������ߺ�Ӧ֪ͨ���
load_offline_sys_msg(Account) ->
    Msgs = get_offline_msgs(Account),
    [sys_msg:send(Account, MsgID, Params) || {MsgID, Params} <- Msgs],
    db_common:delete(sys_msg, Account),
    ok.


