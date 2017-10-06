%%%-------------------------------------------------------------------
%%% @author
%%% @copyright (C) 2010, 
%%% @doc
%%%  根据模板生成Npc内容
%%% @end
%%% Created :  29 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(npc_1_8).

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
command(0, 0, State) ->
%%%新手部分的道具师对话选项-新手任务是否存在判定--男
	List1 = case script:task_is_exist(1000, State) of
	true ->
		[{22, 2, 2}];
	false ->
		[]
  end,
%%%新手部分的道具师对话选项-新手任务是否存在判定--女
	List2 = case script:task_is_exist(1001, State) of
	true ->
		[{22, 2, 2}];
	false ->
		[]
	end, 
%%%道具师对话选项-结束对话--
	List3 = [{23, 5, 5}],  
%%%新手部分的道具师对话选项-生成正式对话--
	List4 = List1 ++ List2 ++ List3,
	{open_dialog_by_option, 16, List4, State};
%%%-新手部分的道具师--完成新手任务-	
command(2, 2, State) ->
	{open_dialog_by_option, 24, [{25, 3, 3}, {26, 3, 3}], State};
command(3, 3, State) ->
	{open_dialog_by_option, 28, [{27, 4, 4}], State};
command(4, 4, State) ->
	case script:task_is_exist(1000, State) of
	true ->
		{_Reply, NState} = script:complete_task(1000, 0, State),
		NNState =script:send_express_gift("妙丽", 25071, "感谢你的参与，这是送给你的额外奖励", NState),
%%%		State = script:send_self_sys_mail(newer_task_tile_1, newer_task_mail_content_1, [117, 118], NState),
		{close_dialog, NNState};
	false ->
		{_Reply, NState} = script:complete_task(1001, 0, State),		
		NNState =script:send_express_gift("妙丽", 25071, "感谢你的参与，这是送给你的额外奖励", NState),
%%%		State = script:send_self_sys_mail(newer_task_tile_1, newer_task_mail_content_1, [117, 118], NState),
		{close_dialog, NNState}
  end;
%%%-新手部分的道具师对话选项-选项三，结束对话--
command(5, 5, State) ->
	{close_dialog, State}.

