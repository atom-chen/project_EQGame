%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 处理npc脚本
%%% @end
%%% Created : 20 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(player_npc).

-include("router.hrl").
-include("sys_msg.hrl").
-include("packet_def.hrl"). 

%% API
-export([start/1, handle_call/3]).


%%%===================================================================
%%% API
%%%===================================================================
start(Account) ->
    [router:make_event_target(exec_npc_event, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% Handle call
%%%===================================================================
handle_call({#msg{event=exec_npc_event}, {ReqNpcCommand, Mod}}, _From, State) ->
    #req_npc_command{npc_key=_NpcKey, 
		     unique_id=UniqueId, 
		     page_index=_PageIndex, 
		     command_index=CommandIndex} = ReqNpcCommand,
    Account = player_data:get_account(State),
    NNState = 
    case Mod:before_command(UniqueId, CommandIndex, State) of
	true ->
	    case Mod:command(UniqueId, CommandIndex, State) of
		{open_dialog_by_option, TemplateId, CommandList, NState} -> %% 打开一个新的对话框
		    NewCommandList = trans_command_list(CommandList, []),
		    net_helper:send2client(Account, 
					   #notify_npc_show_dialog_by_option{template_id=TemplateId, 
									     command_list=NewCommandList}),
		    NState;
		{open_dialog_by_message, TemplateId, NewUniqueId, NewCommandIndex, NState} ->
		    net_helper:send2client(Account, 
					   #notify_npc_show_dialog_by_message{template_id=TemplateId, 
									      unique_id=NewUniqueId, 
									      page_index=0, 
									      command_index=NewCommandIndex}),
		    NState;
		{open_dialog_by_item, TemplateId, ItemList, NState} ->
		    NewItemList = trans_command_list(ItemList, []),
		    net_helper:send2client(Account, 
					   #notify_npc_show_dialog_by_item{template_id=TemplateId, 
									   item_list=NewItemList}),
		    NState;
		{close_dialog, NState} ->
		    net_helper:send2client(Account, #notify_npc_close_dialog{}),
		    NState;
		{do_nothing, NState} ->
		    NState;
		_ ->
		    State
	    end;
	{false, Reason} ->
	    sys_msg:send(Account, ?msg_npc_script, Reason),
	    State;
	_ ->
	    erlang:error("error return value before command")
    end,
    {reply, ok, NNState}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
trans_command_list([], NewCommandList) ->
    lists:reverse(NewCommandList);
trans_command_list([{TemplateId, UniqueId, CommandIndex} | CommandList], NewCommandList) ->
    Command = #npc_command{template_id=TemplateId, unique_id=UniqueId, page_index=0, command_index=CommandIndex},
    trans_command_list(CommandList, [Command|NewCommandList]).
