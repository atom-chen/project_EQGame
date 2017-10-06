%%%-------------------------------------------------------------------
%%% @author
%%% @copyright (C) 2010, 
%%% @doc
%%%  根据模板生成Npc内容
%%% @end
%%% Created :  29 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(npc_1_9).

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
%%%-新手部分的服装店对话选项-选项的判定----打开商店
    List1 = [{46, 1, 1}],
%%%-新手部分的服装店对话选项-选项的判定----有无兑换券任务
    List2 = case script:get_sex(State) of
		1 ->
		    case script:task_is_exist(3, State) of
			true ->
			    [{30, 2, 2}];
			false ->
			    []
		    end;
		2 ->
		    case script:task_is_exist(5, State) of
			true ->
			    [{30, 3, 3}];
			false ->
			    []
		    end
	    end,  	
%%%-新手部分的服装店对话选项-选项的判定----说byebye
    List3 = [{35, 4, 4}],	
%%%-新手部分的服装店对话选项-选项的判定----生成选项
    List4 = List1 ++ List2 ++ List3,
    {open_dialog_by_option, 34, List4, State};
%%%-新手部分的服装店对话选项-选项的执行----打开服装店
command(1, 1, State) ->
    script:open_npc_shop_ui(State),
    {close_dialog, State};
%%%-新手部分的服装店对话选项-选项的执行----完成新手任务
command(2, 2, State) ->	
    {open_dialog_by_option, 31, [{44, 21, 21}, {23, 3, 3}], State};
command(21, 21, State) ->			
    {_Reply, NState} = script:complete_task(3, 0, State),
    NNState = script:add_game_coin(500, NState),
    {close_dialog, NNState};
command(3, 3, State) ->	
    {open_dialog_by_option, 31, [{44, 31, 31}, {23, 3, 3}], State};
command(31, 31, State) ->			
    {_Reply, NState} = script:complete_task(5, 0, State),
    NNState = script:add_game_coin(500, NState),
    {close_dialog, NNState};
%%%-新手部分的服装店对话选项-选项三，结束对话--
command(4, 4, State) ->
    {close_dialog, State}.

