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
%%%-新手部分的道具师对话选项-查看帮助内容--
    List1 = [{17, 1, 1}],
%%%-新手部分的道具师对话选项-新手任务是否存在判定--
    List2 = case script:task_is_exist(2, State) of
		true ->
		    [{22, 2, 2}];
		false ->
		    []
	    end,
%%%-新手部分的道具师对话选项-结束对话--
    List3 = [{23, 3, 3}],
%%%-新手部分的道具师对话选项-完成新手任务后增加两个选项--
    List5 = case script:get_sex(State) of
		1 ->
		    case script:task_is_exist(3, State) of
			true ->
			    [];
			false ->
			    case script:task_is_complete(3, State) of
				true ->
				    [];
				false ->
				    case script:task_is_complete(2, State) of
					true ->
					    [{25, 22, 22}];
					false ->
					    []
				    end
			    end
		    end;
		2 ->
		    case script:task_is_exist(5, State) of
			true ->
			    [];
			false ->
			    case script:task_is_complete(5, State) of
				true ->
				    [];
				false ->
				    case script:task_is_complete(2, State) of
					true ->
					    [{25, 22, 22}];
					false ->
					    []
				    end
			    end
		    end
	    end,  
    List6 = case script:task_is_exist(4, State) of
		true ->
		    [];
		false ->
		    case script:task_is_complete(4, State) of
			true ->
			    [];
			false ->
			    case script:task_is_complete(2, State) of
				true ->
				    [{26, 23, 23}];
				false ->
				    []
			    end
		    end
	    end,   
%%%-新手部分的道具师对话选项-生成正式对话--
    List4 = List1 ++ List2 ++ List3 ++ List5 ++ List6,
    {open_dialog_by_option, 16, List4, State};
%%%-新手部分的道具师对话选项-选项一查看帮助内容--
command(1, 1, State) ->
    {open_dialog_by_option, 16, [{18, 11, 11}, {19, 12, 12}, {20, 13, 13}, {21, 14, 14}, {23, 3, 3}], State};
%%%-新手部分的道具师对话选项-show地图--
command(11, 11, State) ->
    script:show_picture(1, State),
    {do_nothing, State};
%%%-新手部分的道具师对话选项-show主界面--
command(12, 12, State) ->
    script:show_picture(2, State),
    script:show_picture(3, State),
    {do_nothing, State};
%%%-新手部分的道具师对话选项-show鼠标--
command(13, 13, State) ->
    script:show_picture(1, State),
    {do_nothing, State};
%%%-新手部分的道具师对话选项-show换衣服--
command(14, 14, State) ->
    script:show_picture(6, State),
    {do_nothing, State};
%%%-新手部分的道具师对话选项-选项二，完成新手任务的对话--
command(2, 2, State) ->
    {open_dialog_by_option, 24, [{44, 21, 21}, {23, 3, 3}], State};
%%%-新手部分的道具师对话选项-选项二，完成新手任务--
command(21, 21, State) ->		
    {_Reply, NState} = script:complete_task(2, 0, State),
    NNState = script:add_game_coin(500, NState),
%%%-新手部分的道具师对话，完成新手任务后给新手券--
    {open_dialog_by_option, 45, [{25, 22, 22}, {26, 23, 23}, {23, 3, 3}], NNState};
%%%-新手部分的道具师对话选项-选项一服装店奖励--加入男女的判定----
command(22, 22, State) ->
    case script:get_sex(State) of
	1 ->
	    {_Reply, NState}=script:give_task(3, State),
	    NNState = script:add_item(40001, NState),
	    {close_dialog, NNState};
	2 ->
	    {_Reply, NState}=script:give_task(5, State),
	    NNState = script:add_item(40001, NState),
	    {close_dialog, NNState}
    end;
%%%-新手部分的道具师对话选项-选项一服装店奖励--
command(23, 23, State) ->
    {_Reply, NState}=script:give_task(4, State),
    NNState = script:add_item(40002, NState),
    {close_dialog, NNState};	
%%%-新手部分的道具师对话选项-选项三，结束对话--
command(3, 3, State) ->
    {close_dialog, State}.


