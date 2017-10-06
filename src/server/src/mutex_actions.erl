%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%     动作互斥基础单元
%%% @end
%%% Created : 20 May 2010 by  <hjx>

-module(mutex_actions).


-include("packet_def.hrl").
-include("mutex_actions.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").


-export([start/1, handle_call/3, handle_cast/2]).

%% 动作互斥相关
-export([try_start_action/3, 
	 try_start_two_action/6, 
	 try_start_same_action/5,
	 call_stop_action/2, 
	 call_stop_action/3, 
	 contains_action/2
	]).

-export([clear_actions/2]).

-define(action_id_index, 1).


start(Account) ->
    [
     router:make_event_target(you_try_start_two_action, Account, {self(), ?MODULE}),
     router:make_event_target(you_try_start_same_action, Account, {self(), ?MODULE}),
     router:make_event_source(cast_stop_action, Account, {self(), ?MODULE}),
     router:make_event_target(stop_action, Account, {self(), ?MODULE})
    ].


%%%===================================================================
%%% 定义每个动作对应的模块
%%%===================================================================
-spec get_action_mod(integer()) -> atom().
get_action_mod(?action_type_edit_house) -> 
    player_edit_house;
get_action_mod(?action_type_invite) -> 
    player_invite;
get_action_mod(?action_type_be_invite)-> 
    player_be_invite;
get_action_mod(?action_type_walk) -> 
    player_walk;
get_action_mod(?action_type_change_scene) -> 
    player_change_scene;
get_action_mod(?action_type_trade) -> 
    player_trade;
get_action_mod(?action_type_use_furniture) -> 
    player_use_furniture;
get_action_mod(?action_type_work) -> 
    player_work;
get_action_mod(?action_type_domestic) -> 
    player_domestic;
get_action_mod(?action_type_change_looks) -> 
    player_change_looks;
get_action_mod(?action_type_farm) -> 
    player_farm;
get_action_mod(?action_type_body_action) -> 
    player_body_action;
get_action_mod(?action_type_gift) ->
    player_gift_box;
get_action_mod(?action_type_house_exist_gift) ->
    house_exist_gift;
get_action_mod(?action_type_buy_house) ->
    house_transaction;
get_action_mod(?action_type_be_kick) ->
    player_be_kick;
get_action_mod(?action_type_pick_magic_box) ->
    player_magic_box;
get_action_mod(?action_type_change_house) ->
    player_change_house;
get_action_mod(?action_type_preview_house) ->
    player_preview_house;
get_action_mod(?action_type_wear) ->
    player_wear;
get_action_mod(ActionID) when is_integer(ActionID) ->
    erlang:error({action_id_module_undefined, ActionID}).

%%%===================================================================
%%% 是否为瞬时动作, 如果是, 不记录状态，不添加到动作列表
%%%===================================================================
-spec is_immediate_action(integer()) -> boolean().
is_immediate_action(?action_type_invite) ->        %% 邀请
    true;
is_immediate_action(?action_type_be_kick) ->       %% 被踢
    true;
is_immediate_action(?action_type_change_house) ->  %% 切换房屋
    true;
is_immediate_action(?action_type_pick_magic_box) ->%% 捡垃圾，宝箱
    true;
is_immediate_action(?action_type_wear) ->          %% 穿衣服
    true;
is_immediate_action(ActionID) when is_integer(ActionID) -> %% 默认不是瞬时动作
    false.

%%%===================================================================
%%% 处理handle_call
%%%===================================================================
%% 结束动作
handle_call({#msg{event=stop_action}, {ActionID, Param}}, _From, State) ->  
    %%io:format("handle call stop action ~p ~n~n~n", [{Param}]),
    NewState = call_stop_action(ActionID, Param, State),
    {reply, ok, NewState};
handle_call({#msg{event=you_try_start_two_action}, 
	     {YourActionID, YourParam, YourState, YourPID, MyActionID, MyParam}}, 
	    _From, MyState) ->  
    Ret = you_try_start_two_action(YourActionID, YourParam, YourState, YourPID, MyActionID, MyParam, MyState),
    case Ret of
	{ok, YourNewState, MyNewState} ->
	    {reply, YourNewState, MyNewState};
	Error ->
	    {reply, Error, MyState}
    end;
handle_call({#msg{event=you_try_start_same_action}, 
	     {ActionID, YourParam, YourState, YourPID, MyParam}},
	    _From, MyState) ->  
    Ret = you_try_start_same_action(ActionID, YourParam, YourState, YourPID, MyParam, MyState),    
    case Ret of
	{ok, YourNewState, MyNewState} ->
	    {reply, YourNewState, MyNewState};
	Error ->
	    {reply, Error, MyState}
    end.

%% 结束动作
handle_cast({#msg{event=cast_stop_action}, {ActionID, Param}}, State) ->  
    %%io:format("handle cast stop action ~p ~n~n~n", [{Param}]),
    NewState = call_stop_action(ActionID, Param, State),
    {noreply, NewState}.

%%%===================================================================
%%% API
%%%===================================================================
-spec contains_action(integer(), tuple()) -> boolean().			      
contains_action(ActionID, State) ->
    Actions = player_data:get_actions(State),
    lists:keymember(ActionID, ?action_id_index, Actions).

can_start_new_action(OldActions, NewActionID, Param, State) ->
    %% data_helper:format("\naccount: ~w, oldAction: ~w, NewActionID: ~w\n",
    %% 	     [player_data:get_account(State), OldActions, NewActionID]),
    case inner_can_start_new_action(OldActions, NewActionID) of
	true ->
	    Mod = get_action_mod(NewActionID),
	    Mod:can_start_action(Param, State);	
	Other ->
	    Other
    end.

%% 开始某个动作
call_start_action(ActionID, Param, OldState) 
  when is_integer(ActionID) ->
    call_start_action(ActionID, Param, OldState, self()).
call_start_action(ActionID, Param, OldState, PID) 
  when is_integer(ActionID) ->
    OldActions = player_data:get_actions(OldState),
    CurState = before_start_action(ActionID, OldActions, OldState),
    start_action_proc(ActionID, Param, CurState, PID).

%% 结束某个动作
call_stop_action(ActionID, State) when is_integer(ActionID) ->
    call_stop_action(ActionID, no_param, State).
call_stop_action(ActionID, Param, State) when is_integer(ActionID) ->
    OldActions = player_data:get_actions(State),
    case lists:keyfind(ActionID, ?action_id_index, OldActions) of
	{ActionID, Msgs} ->
	    Mod = get_action_mod(ActionID),
	    NewState = Mod:do_stop_action(Param, State),
	    %% 注消消息
	    router:unregister_handler(Msgs),
	    %% 从列表中删除
	    %%
	    NewActions = lists:keydelete(ActionID, ?action_id_index, OldActions),	    
	    player_data:set_actions(NewActions, NewState);
	false ->
	    data_helper:format("stop action not find ~p~n", [{ActionID, Param}]),
	    State
    end.

%% 尝试开始某个动作
try_start_action(ActionID, Param, State) when is_integer(ActionID) ->    
    Account = player_data:get_account(State),
    OldActions = player_data:get_actions(State),
    data_helper:format("try start ActionId: ~w, OldActions: ~w\n", [ActionID, OldActions]), 
    %% 取出当前正在进行中的所有动作项[1,2,3,5,7], 判断可否开始动作 
    case can_start_new_action(OldActions, ActionID, Param, State) of
	{false, ?err_no_error} -> %% 不报告错误
	    State;
	{false, Error} ->
	    %% 如果不允许的话，且有!则只提示第一个!错误, 退出
	    sys_msg:send(Account, Error),
	    data_helper:format("can not start action, error: ~w\n", [Error]),
	    State;
	{false, Error, Params} ->
	    sys_msg:send(Account, Error, Params),
	    State;
	true ->
	    %% 启动动作
	    call_start_action(ActionID, Param, State);
	Any ->
	    erlang:error({can_start_new_action_return_val_error, Any, {ActionID, Param, State}})
    end;
try_start_action(ActionID, Param, State) ->     
    erlang:error({try_start_action_error, {ActionID, Param, State}}).
    


%% 清除所有动作
clear_actions(Reason, State) ->
    ActionList = player_data:get_actions(State),
    ActionIDs = [ID || {ID, _Msgs} <- ActionList],
    data_helper:format("\nclear actions ~w\n", [ActionIDs]),
    auto_stop_actions(ActionIDs, Reason, State).

%% 双方同时开始不同的动作
try_start_two_action(MyActionID, MyParam, YourActionID, YourParam, YourAccount, MyState) ->    
    MyAccount = player_data:get_account(MyState),
    
    Result = router:send(YourAccount, you_try_start_two_action, {MyActionID, MyParam, MyState, self(), YourActionID, YourParam}),
    
    case Result of
	undefined -> %%对方不在线
	    sys_msg:send(MyAccount,  ?err_player_offline),
	    MyState;
	{false, ?err_no_error} -> %% 不报告错误
	    MyState;
	{false, Err} ->
	    sys_msg:send(MyAccount,  Err),
	    MyState;
	{false, Error, Params} ->
	    sys_msg:send(MyAccount,  Error, Params),
	    MyState;
	NewState ->
	    NewState
    end.	    


%% 双方同时开始不同的动作
%% 返回{ok, YourNewState, MyNewState} || {false, Error} || {false, Error, Params}
you_try_start_two_action(YourActionID, YourParam, YourState, YourPID, MyActionID, MyParam, MyState) ->    
    MyActions = player_data:get_actions(MyState),
    YourActions = player_data:get_actions(YourState),

    %% 判断自已可否发起共同动作
    case can_start_new_action(MyActions, MyActionID, MyParam, MyState) of
	true ->
	    %% 判断对方可否发起共同动作
	    case can_start_new_action(YourActions, YourActionID, YourParam, YourState) of
		true ->	    
		    %% 自已开始动作
		    MyNewState = call_start_action(MyActionID, MyParam, MyState),
		    %% 对方也开始动作
		    YourNewState = call_start_action(YourActionID, YourParam, YourState, YourPID),
		    {ok, YourNewState, MyNewState};
		YourError -> 
		    YourError
	    end;
	{false, ?err_no_error} -> %% 不报告错误
	    {false, ?err_no_error};
	_MyError ->  %% 其它情况，提示对方正忙
	    {false, ?err_other_is_busy}	    
    end.


%% 双方同时开始同一个动作(只做单次判断)
try_start_same_action(ActionID, MyParam, YourParam, YourAccount,  
			MyState) ->    
    MyAccount = player_data:get_account(MyState),
    
    Result = router:send(YourAccount, you_try_start_same_action, {ActionID, MyParam, MyState, self(), YourParam}),

    case Result of
	undefined -> %%对方不在线
    	    sys_msg:send(MyAccount,  ?err_player_offline),
	    MyState;
	{false, ?err_no_error} -> %% 不报告错误
	    MyState;
	{false, Err} ->
	    sys_msg:send(MyAccount, Err),
	    %% 另一方也要收到提示
	    sys_msg:send(YourAccount, Err),
	    MyState;
	{false, Error, Params} ->
	    sys_msg:send(MyAccount, Error, Params),
	    MyState;
	NewState ->
	    NewState
    end.	    

%% 双方同时开始同一个动作(只做单次判断)
%% 返回{ok, YourNewState, MyNewState} || {false, Error} || {false, Error, Params}
you_try_start_same_action(ActionID, YourParam, YourState, YourPID, MyParam, MyState) ->    
    MyActions = player_data:get_actions(MyState),
    YourActions = player_data:get_actions(YourState),
    

    Mod = get_action_mod(ActionID),
    %% 公共判断
    case Mod:can_start_action(MyParam, MyState) of
	true ->    
	    %% 判断自已可否发起共同动作
	    case inner_can_start_new_action(MyActions, ActionID) of
		true ->
		    %% 判断对方可否发起共同动作
		    case inner_can_start_new_action(YourActions, ActionID) of
			true ->	    
			    %% 自已开始动作
			    MyNewState = call_start_action(ActionID, MyParam, MyState),
			    %% 对方也开始动作
			    YourNewState = call_start_action(ActionID, YourParam, YourState, YourPID),
			    {ok, YourNewState, MyNewState};
			YourError -> 
			    YourError
		    end;
		{false, ?err_no_error} -> %% 不报告错误
		    {false, ?err_no_error};
		_MyError ->  %% 其它情况，提示对方正忙
		    {false, ?err_other_is_busy}	    
	    end;
	CommError ->
	    CommError
    end.


%%%===================================================================
%%% 内部函数
%%%===================================================================

%% 开始某个动作前需要做的一些处理
%% 返回State
before_start_action(ActionID, OldActions, State) when is_integer(ActionID), is_list(OldActions) ->
    StopActionIDs = get_stop_actions(OldActions, ActionID),
    Reason = ActionID, %% 原因为开始 新动作
    auto_stop_actions(StopActionIDs, Reason, State).

%% 取得需要停止的动作列表
%% 返回 list() 
get_stop_actions(OldActions, NewActionID) 
  when is_list(OldActions), is_integer(NewActionID) ->
    F = fun({OldActionID, _Msgs}, RetList) -> 
		MutexList = get_action_mutex_list(OldActionID),
		case lists:nth(NewActionID, MutexList) of
		    ?action_stop_mark -> [OldActionID | RetList]; 
		    _  -> RetList
		end			
	end,
    
    lists:foldl(F, [], OldActions).


%% 判断能否开始某个动作
%% 遇到第一个不为true的就停止搜索
%% 返回true | {false, Reason}
inner_can_start_new_action([], _NewActionID) ->
    true;
inner_can_start_new_action([{OldActionID, _Msgs} | Rest], NewActionID) 
  when is_integer(NewActionID), is_integer(OldActionID) ->
    case inner_can_start_action(OldActionID, NewActionID, get_action_mutex_list(OldActionID)) of
	true -> %% 成功，则继续看下一当前动作是否也允许开始新动作
	    inner_can_start_new_action(Rest, NewActionID);	    
	Error -> 
	    Error
    end.

%% 取出一个动作的动作关系描述列表
get_action_mutex_list(ActionID) when is_integer(ActionID) ->
    {_File, ActionID, _Name, MutexDesc} = tplt:get_data(mutex_actions, ActionID),
    MutexDesc.


%% 自动停止一系列动作
%% 返回State
auto_stop_actions(ActionIDs, Reason, State) when is_list(ActionIDs) ->
    auto_stop_action_1(ActionIDs, Reason, State).

auto_stop_action_1([], _Reason, State) ->
    State;
auto_stop_action_1([ActionID | Rest], Reason, State) ->
    NewState = auto_stop_action(ActionID, Reason, State),
    auto_stop_action_1(Rest, Reason, NewState).

%% 判断可否开始某个动作
%% 传入新旧动作ID 以及它们的关系Mark
%% 返回true | {false, Reason}
can_start_action_result(OldActionID, NewActionID, Mark) 
  when is_integer(OldActionID), is_integer(NewActionID), is_integer(Mark) -> 
    case Mark of
	?action_allow_mark ->
	    true;
	?action_stop_mark ->
	    true;
	    %%{false, ?err_no_error};
	?action_error_mark ->
	    %% 错误号命名规则
	    ErrorNo = (100 + OldActionID) * 100 + NewActionID,
	    {false, ErrorNo}
    end.

inner_can_start_action(OldActionID, NewActionID, MutexList) 
  when is_integer(OldActionID), is_integer(NewActionID), is_list(MutexList) -> 
    case (1 =< NewActionID) and (NewActionID =< length(MutexList)) of
	true ->
	    Mark = lists:nth(NewActionID, MutexList),
	    can_start_action_result(OldActionID, NewActionID, Mark);
	false ->
	    erlang:error({"mutex action id unbound.", NewActionID, MutexList, OldActionID})
    end.

%% 自动停止某个动作
%% 返回State
auto_stop_action(ActionID, Reason, State) ->
    Param = {auto_stop, Reason},
    call_stop_action(ActionID, Param, State).


%% 开始某个动作
start_action_proc(ActionID, Param, State, PID) ->
    Mod = get_action_mod(ActionID),
    Account = player_data:get_account(State),
    
    NewState = Mod:do_start_action(Param, State),
    case is_immediate_action(ActionID) of
	true -> %% 表示瞬时动作，不加入动作列表	   
	    NewState;
	false ->
	    %% 注册消息
	    Msgs = Mod:get_action_register_msgs(Account, PID),
	    ok = router:register_handler(Msgs),

	    %% 加入到动作列表中
	    %%
	    CurActions = player_data:get_actions(NewState),
	    NewActions = [{ActionID, Msgs} | CurActions],
	    player_data:set_actions(NewActions, NewState)
    end.


%%%===================================================================
%%% 测试函数
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

%% 测试函数的入口

inner_can_start_action_test() ->
    MutexList = [?action_allow_mark, ?action_error_mark, ?action_stop_mark],
    ?assertEqual({false, 10102}, inner_can_start_action(1, 2, MutexList)),
    ?assertEqual(true, inner_can_start_action(2, 1, MutexList)),
    ?assertEqual(true, inner_can_start_action(1, 3, MutexList)),

    ok.

%% get_action_mutex_list_test() ->
%%     ActionID = 1,
%%     AList = get_action_mutex_list(ActionID),
%%     ?assertEqual(true, is_list(AList)),
    
%%     AllData = tplt:get_all_data(mutex_actions),
%%     Len = length(AList),
%%     ?assertEqual(Len, length(AllData)),
%%     ok.

-endif.
