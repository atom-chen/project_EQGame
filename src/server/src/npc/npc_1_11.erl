%%%-------------------------------------------------------------------
%%% @author
%%% @copyright (C) 2010, 
%%% @doc
%%%  根据模板生成Npc内容
%%% @end
%%% Created :  29 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(npc_1_11).

%% API 
-export([before_command/3, command/3]).

%%% before_command()方法返回值说明(如果返回false, 则不执行Command()方法)
%%% 1. 当条件正确后返回 : true
%%% 2. 当条件错误后返回 : {false, Reason}
before_command(_, _, _State) ->
    true.

%%% command()方法返回值说明:(选项名称以及显示内容:填写客户端模板表的模板Id)
%%% 1. 打开窗口(不带选项内容): {open_dialog_by_message, 客户端显示内容的模板Id, 页面唯一Id, 选项索引, State}
%%% 2. 打开窗口(带选项内容)  : {open_dialog_by_option, 客户端显示内容的模板Id, [{选项1名称, 页面唯一Id, 选项索引}, {选项2名称, 页面唯一Id, 选项索引}], State}
%%% 3. 打开窗口(带物品内容)  : {open_dialog_by_item, 客户端显示内容的模板Id, [{物品Id, 页面唯一Id, 选项索引}, {物品Id, 页面唯一Id, 选项索引}], State}
%%% 4. 关闭窗口              : {close_dialog, State}
%%% 5. 不做任何事情          : {do_nothing, State}

command(0, 0, State) ->
%%%-页面一：选项一------领取奖励，选项二------离开。若无任务则选项一不显示
    List1 = [{51,1,1}],
%%%-选项二----离开
    List2 = [{35,3,3}],	
%%%-----生成选项
    List3 = 
	case script:task_is_exist(10001, State) of
	    true ->
    		List1 ++ List2;
	    false ->
    		List2
    	end,
    {open_dialog_by_option, 50, List3, State};
    
%%%-选择领取奖励后，判断是否完成过任务：完成---NPC说“你领到奖励了”；未完成---NPC说“活动奖励为25000EQ币和250EQ点。”，并可选择领取
command(1, 1, State) ->
    case script:task_is_complete(10001,State) of
	true ->
	    {open_dialog_by_option,54, [{35,3,3}], State};
	false ->
	    {open_dialog_by_option,52, [{53, 2, 2}, {35,3,3}], State}
    end;
	    
%%%-选择奖励给我吧，完成任务。NPC提示“你领到奖励了”
command(2, 2, State) ->			
    {_Reply, NState} = script:complete_task(10001, 0, State),
   {open_dialog_by_option,54, [{35,3,3}], NState};
   
%%%-选择离开选项后，结束对话--
command(3, 3, State) ->
    {close_dialog, State}.
