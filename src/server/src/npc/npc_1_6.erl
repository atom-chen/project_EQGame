%%%-------------------------------------------------------------------
%%% @author
%%% @copyright (C) 2010, 
%%% @doc
%%%  根据模板生成Npc内容
%%% @end
%%% Created :  29 Apr 2010 by  <>
%%% before_command()方法返回值说明(如果返回false, 则不执行Command()方法)
%%% 1. 当条件正确后返回 : true
%%% 2. 当条件错误后返回 : false
%%% command()方法返回值说明:(选项名称以及显示内容:填写客户端模板表的模板Id)
%%% 1. 打开窗口(不带选项内容): {open_dialog_by_message, 客户端显示内容的模板Id, 页面唯一Id, 页面索引, 选项索引}
%%% 2. 打开窗口(带选项内容)  : {open_dialog_by_option, 客户端显示内容的模板Id, [{选项1名称, 页面唯一Id, 页面索引, 选项索引}, {选项2名称, 页面唯一Id, 页面索引, 选项索引}]}
%%% 3. 打开窗口(带物品内容)  : {open_dialog_by_item, 客户端显示内容的模板Id, [{物品Id, 页面唯一Id, 页面索引, 选项索引}, {物品Id, 页面唯一Id, 页面索引, 选项索引}]}
%%% 4. 关闭窗口              : {close_dialog}
%%%-------------------------------------------------------------------
-module(npc_1_6).

%% API 
-export([before_command/3, command/3]).

before_command(0, 0, _State) ->
    true;
before_command(1, 1, _State) ->
    true;
before_command(1, 2, _State) ->
    true.

command(0, 0, State) -> 
    {open_dialog_by_option, 38, [{39, 1, 1}, {40, 1, 2}], State};
command(1, 1, State) -> 
    script:open_eq_coin_looks_ui(State),
    {close_dialog, State};
command(1, 2, State) -> 
    {close_dialog, State}.
