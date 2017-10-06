%%%-------------------------------------------------------------------
%%% @author
%%% @copyright (C) 2010, 
%%% @doc
%%%  根据模板生成Npc内容
%%% @end
%%% Created :  29 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(npc_12_1).

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
%%%-新手部分的家具店对话选项-选项的判定----打开商店
%%    List1 = [{46, 1, 1}],
%%%-新手部分的家具店对话选项-选项的判定----有无兑换券任务
%%%    List2 = case script:task_is_exist(4, State) of
%%%		true ->
%%%		    [{30, 2, 2}];
%%%		false ->
%%%		    []
%%%	    end, 
%%%新手部分的家具店对话选项-新手任务是否存在判定
	List2 = case script:task_is_exist(1107, State) of
	true ->
		[{56, 7, 7}];
	false ->
		[]
  end,
%%%-新手部分的家具店对话选项-查看地图-----------------	
    List3 = [{18, 5, 5}],	
%%%-新手部分的家具店对话选项-查看摆家具---------------	
    List4 = [{47, 6, 6}],	
%%%-新手部分的家具店对话选项-选项的判定----说byebye
    List7 = [{35, 3, 3}],	
%%%-新手部分的家具店对话选项-选项的判定----生成选项
    List6 = List2 ++ List3 ++ List4 ++ List7,
    {open_dialog_by_option, 36, List6, State};
%%%-新手部分的家具店对话选项-选项的执行----打开家具店
%%%command(1, 1, State) ->
%%%    script:open_shop(2,State),
%%%    {close_dialog, State};
%%%-新手部分的家具店对话选项-选项的执行----完成新手任务
%%%command(2, 2, State) ->	
%%%    {open_dialog_by_option, 33, [{44, 21, 21}, {23, 3, 3}], State};
%%%command(21, 21, State) ->			
%%%    {_Reply, NState} = script:complete_task(4, 0, State),
%%%    {close_dialog, NState};
%%%-新手部分的家具店对话选项-选项三，结束对话--
command(3, 3, State) ->
    {close_dialog, State};
command(4, 4, State) ->
    {close_dialog, State};
%%%-新手部分的道具师对话选项-show地图--
command(5, 5, State) ->
    script:show_picture(9, State),
    {do_nothing, State};
%%%-选择任务选项后
command(7, 7, State) ->
	{open_dialog_by_option, 24, [{25, 71, 71}], State};
%%%-选择“看到任务我就来了”后
command(71, 71, State) ->
	{open_dialog_by_option, 28, [{27, 72, 72}], State};
%%%-选择“好的，谢谢”后
command(72, 72, State) ->
    {_Reply, NState} = script:complete_task(1107, 0, State),
    {close_dialog, NState};
%%%-新手部分的道具师对话选项-show摆放家具--
command(6, 6, State) ->
    script:show_picture(7, State),
    script:show_picture(8, State),
    {do_nothing, State}.
