%%%-------------------------------------------------------------------
%%% @author
%%% @copyright (C) 2010, 
%%% @doc
%%%  ����ģ������Npc����
%%% @end
%%% Created :  29 Apr 2010 by  <>
%%% before_command()��������ֵ˵��(�������false, ��ִ��Command()����)
%%% 1. ��������ȷ�󷵻� : true
%%% 2. ����������󷵻� : false
%%% command()��������ֵ˵��:(ѡ�������Լ���ʾ����:��д�ͻ���ģ����ģ��Id)
%%% 1. �򿪴���(����ѡ������): {open_dialog_by_message, �ͻ�����ʾ���ݵ�ģ��Id, ҳ��ΨһId, ҳ������, ѡ������}
%%% 2. �򿪴���(��ѡ������)  : {open_dialog_by_option, �ͻ�����ʾ���ݵ�ģ��Id, [{ѡ��1����, ҳ��ΨһId, ҳ������, ѡ������}, {ѡ��2����, ҳ��ΨһId, ҳ������, ѡ������}]}
%%% 3. �򿪴���(����Ʒ����)  : {open_dialog_by_item, �ͻ�����ʾ���ݵ�ģ��Id, [{��ƷId, ҳ��ΨһId, ҳ������, ѡ������}, {��ƷId, ҳ��ΨһId, ҳ������, ѡ������}]}
%%% 4. �رմ���              : {close_dialog}
%%%-------------------------------------------------------------------
-module(npc_2_12).

%% API 
-export([before_command/3, command/3]).

before_command(0, 0, _State) ->
    true;
before_command(1, 1, _State) ->
    true;
before_command(1, 2, _State) ->
    true.

command(0, 0, State) -> 
    script:open_float_bottle_pool_ui(State),
    {close_dialog, State}.
    
