%%% @author hongjx <hongjx@china-channel.com>
%%% @copyright (C) 2010, hongjx
%%% @doc
%%%   美容
%%% @end
%%% Created : 17 Aug 2010 by hongjx <hongjx@china-channel.com>

-module(player_change_looks).


-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("router.hrl").
-include("enum_def.hrl").

-behaviour(player_action).


-export([handle_cast/2, start/1, start_change_looks/2]).
-export([do_start_action/2, do_stop_action/2, 
	 can_start_action/2, get_action_register_msgs/2]).


start(Account) ->
    [
     router:make_event_source(start_change_looks, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_start_change_looks, Account, {self(), ?MODULE})
    ].

-spec start_change_looks(integer(), tuple()) -> any().
start_change_looks(Type, State) ->
    Owner = player_data:get_account(State),
    router:cast(Owner, start_change_looks, Type).
    
%%%===================================================================
%%% 动作相关回调函数
%%%===================================================================
%% 取动作进行中的消息列表, 动作开始时被注册, 结束时被取消 
get_action_register_msgs(Account, PID) ->
    [
     router:make_event_source(?msg_req_end_change_looks, Account, {PID, ?MODULE}),
     router:make_event_source(?msg_req_cancel_change_looks, Account, {PID, ?MODULE})
    ].

%% 判断当前状态能否开始动作
%% 返回true | {false, Error}
can_start_action(_Param,  _State) ->
    true.

%% 开始动作
do_start_action(Type, State) ->
    Account = player_data:get_account(State),
    net_helper:send2client(Account, #notify_start_change_looks{type=Type}),
    State.

%% 结束动作
%% 自动结束时，Param 格式为 {auto_stop, Reason}, 其它情况为call_stop_action传入的Param
%% 自动结束发生时机: 当前动作被新动作停止时，或退出游戏时
do_stop_action({auto_stop, _Reason}, State) ->
    do_stop_action(cancel, State);
do_stop_action(cancel, State) ->
    Account = player_data:get_account(State),
    %% 通知结束美容
    net_helper:send2client(Account, #notify_end_change_looks{}),
    State.


%%%===================================================================
%%% 处理handle_cast
%%%===================================================================
handle_cast({#msg{event=start_change_looks}, Type}, State) ->    
    NewState = mutex_actions:try_start_action(?action_type_change_looks, Type, State),
    {noreply, NewState};
%% 请求开始
handle_cast({_Msg, 
	     #req_start_change_looks{}}, 
	    State) -> 
    Type = ?GAMECOIN,
    NewState = mutex_actions:try_start_action(?action_type_change_looks, Type, State),
    {noreply, NewState};

%% 请求结束美容
handle_cast({_Msg, 
	     #req_end_change_looks{new_hair=NewHair, new_face=NewFace, new_beard=NewBeard, 
				   new_skin_color=NewSkinColor, new_hair_color=NewHairColor}}, 
	    State) -> 
    NewLooks = {NewHair, NewHairColor, NewFace, NewSkinColor, NewBeard},
    case do_change_looks(NewLooks, State) of
	{false, Reason} ->	    
	    Account = player_data:get_account(State),	    
	    sys_msg:send(Account, Reason),
	    {noreply, State};
	State1 ->
	    NewState = mutex_actions:call_stop_action(?action_type_change_looks, cancel, State1),
	    BasicData = player_data:get_basic_data(State),
	    Hair = basic_data:get_data(hair, BasicData),
	    case NewHair of
		Hair -> {noreply, NewState};
		_ -> %% 每日任务
		    {noreply, daily_task:inc_action_amount(?tat_change_hair, NewState)}
	    end
    end;

handle_cast({_Msg, 
	     #req_cancel_change_looks{}}, 
	    State) -> 
    NewState = mutex_actions:call_stop_action(?action_type_change_looks, cancel, State),
    {noreply, NewState}.


%%%===================================================================
%%% 内部函数
%%%===================================================================

%% 算出玩家美容后的新State
get_player_new_state({NewHair, NewHairColor, NewFace, NewSkinColor, NewBeard}, State) ->
    BasicData = player_data:get_basic_data(State),
    BasicData1 = inner_set_basic_data(hair, NewHair, BasicData), 
    BasicData2 = inner_set_basic_data(hair_color, NewHairColor, BasicData1), 
    BasicData3 = inner_set_basic_data(face, NewFace, BasicData2), 
    BasicData4 = inner_set_basic_data(skin_color, NewSkinColor, BasicData3), 
    NBasicData = inner_set_basic_data(beard, NewBeard, BasicData4), 

    player_data:set_basic_data(NBasicData, State).

%% 取旧脸型发型之类的
get_old_looks(State) ->    
    BasicData = player_data:get_basic_data(State),

    {
      basic_data:get_data(hair, BasicData),
      basic_data:get_data(hair_color, BasicData),
      basic_data:get_data(face, BasicData),
      basic_data:get_data(skin_color, BasicData),
      basic_data:get_data(beard, BasicData)
    }.

%% 
inner_set_basic_data(Type, NewVal, BasicData) ->
    case NewVal > 0 of
	true ->
	    basic_data:set_data(Type, NewVal, BasicData);
	false ->
	    BasicData    
    end.
	     
%% 通知玩家及他人, 形象改变了
notify_looks_changed(Account, {NewHair, NewHairColor, NewFace, NewSkinColor, NewBeard}, State) ->
    Packet = #notify_change_looks{account=atom_to_list(Account),
				  new_hair=NewHair,
				  new_hair_color=NewHairColor,
				  new_face=NewFace,
				  new_skin_color=NewSkinColor,
				  new_beard=NewBeard},

    %% 通知自已
    net_helper:send2client(Account, Packet),

    %% 通知周围的人
    %% TODO 这个可能会改
    SceneName = player_data:get_scene_name(State),
    router:cast(undefined, SceneName, broadcast_other_players, {Account, Packet}),    
    ok.


%% 判断可否进行美容
can_change_looks({GameCost, EQCost}, PlayerCoin) ->
    MyGameMoney = player_coin:get_coin(?GAMECOIN, PlayerCoin),
    MyEQMoney = player_coin:get_coin(?EQCOIN, PlayerCoin),
    case MyGameMoney < GameCost of
	true -> {false, ?err_not_enough_coin};
	false -> 
	    case MyEQMoney < EQCost of
		true -> {false, ?err_not_enough_coin};
		false -> true
	    end
    end.



%% 根据性别，改变类型Type，算出对应费用的ID
get_change_looks_id(Sex, Type, Val) ->
    Sex * 10000 + Type * 1000 + Val.

do_change_looks(ChangeRecord, State) 
  when is_tuple(ChangeRecord), size(ChangeRecord) =:= 5 ->
    Account = player_data:get_account(State),
    Sex = basic_data:get_data(sex, player_data:get_basic_data(State)),
    OldLooks = get_old_looks(State),
    %%data_helper:format("~n old looks: ~p~n", [OldLooks]),
    %%data_helper:format("~n new looks: ~p~n", [ChangeRecord]),
    
    %% 算费用
    F = fun(I, {Money1, Money2}) ->
		NewVal = element(I, ChangeRecord),    %% 取得改变后的新值
		OldVal = element(I, OldLooks),     %% 取得旧值
		case OldVal =/= NewVal of
		    true ->
			ID = get_change_looks_id(Sex, I, NewVal),
			case tplt:get_data2(change_looks_tplt, ID) of
			    #change_looks_tplt{money_type=MoneyType, money=Money} = _Record -> 
				%%data_helper:format("~nchange record:~p~n", [Record]),
				case MoneyType of
				    ?GAMECOIN -> {Money1 + Money, Money2};
				    ?EQCOIN -> {Money1, Money2 + Money}			    
				end
			end;
		    _ ->
			{Money1, Money2}
		end		    
	end,

    {GameCost, EQCost} = lists:foldl(F, {0, 0}, lists:seq(1, size(ChangeRecord))),
    %%data_helper:format("~n money{~p, ~p}~n", [GameCost, EQCost]),
    PlayerCoin = player_data:get_coin(State),
    %% 判断可否进行美容
    case can_change_looks({GameCost, EQCost}, PlayerCoin) of
	true ->
	    PlayerCoin1 = player_coin:dec_coin2(?GAMECOIN, GameCost, PlayerCoin),
	    NewCoin = player_coin:dec_coin2(?EQCOIN, EQCost, PlayerCoin1),
	    State1 = player_data:set_coin(NewCoin, State),    
	    NewState = get_player_new_state(ChangeRecord, State1),
	    
	    NewBasicData = player_data:get_player_basic_data(NewState),

	    %% 数据库操作
	    OpCoinAndBasicData = {write, [NewCoin, NewBasicData]},

	    db_common:change([OpCoinAndBasicData]),
	    
	    %% 通知结束美容
	    net_helper:send2client(Account, #notify_end_change_looks{}),
	    
	    %% 通知货币改变了
	    player_coin:send_coin(NewCoin),
	    
	    %% 通知自已及其他人, 形象改变了
	    notify_looks_changed(Account, ChangeRecord, NewState),    
	    
	    %% 后台金钱物品记录
	    MoneyInfo = {-GameCost, -EQCost},
	    item_money_log:log_money(Account, player_change_looks, MoneyInfo),

	    
	    NewState;
	Error ->	    	    
	    Error	        
    end.
