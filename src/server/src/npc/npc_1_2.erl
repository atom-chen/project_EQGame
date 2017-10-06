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
-module(npc_1_2).

-include("common_def.hrl").

%% API 
-export([before_command/3, command/3]).

before_command(0, 0, _State) ->
    true;
before_command(1, 1, _State) ->
    true;
before_command(2, 2, _State) ->
    true;
before_command(3, 3, _State) ->
    true;
before_command(4, 1, _State) ->
    true;
before_command(5, 2, _State) ->
    true;
before_command(6, 1, _State) ->
    true.

command(0, 0, State) -> 
    {open_dialog_by_option, 1, [{2, 1, 1}, {3, 2, 2}, {4, 3, 3}], State};
command(1, 1, State) -> 
    {open_dialog_by_option, 5, [{6, 4, 1}, {7, 5, 2}], State};
command(2, 2, State) -> 
    {open_dialog_by_option, 8, [{9, 6, 1}, State]};
command(3, 3, State) -> 
    {close_dialog, State};
command(4, 1, State) -> 
    HaveDisease = script:whether_have_disease(State),
    State1 = 
	case HaveDisease of
	    true -> %% 如果有疾病
		TreatMoney = script:get_treat_money(State),
		GameCoin = script:get_game_coin(State),
		case GameCoin >= TreatMoney of
		    true -> %% 如果有足够的金币治疗
			NState = script:dec_game_coin(TreatMoney, State),
			NNState = script:update_player_property(0, 100, 0, 0, NState),
			script:treat_disease(NNState);
		    false ->
			script:promot(["您的金钱不足"], State),
			State
		end;
	    false -> %% 如果没有疾病则提示
		script:promot(["你很健康，不需要治疗"], State),
		State
	end,
    {close_dialog, State1};
command(5, 2, State) -> 
    {close_dialog, State};
command(6, 1, State) -> 
    {close_dialog, State}.
