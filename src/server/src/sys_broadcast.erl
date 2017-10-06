%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2010, hongjx
%%% @doc
%%%   系统广播
%%% @end
%%% Created :  9 Dec 2010 by hongjx <hongjx@35info.cn>

-module(sys_broadcast).
-include("packet_def.hrl").
-include("tplt_def.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").
-include_lib("stdlib/include/qlc.hrl").


-export([start_link/0]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-export([send/2, add/5, modify/6, gm_change_broadcast/2, req_fixed_broadcast/2]).
-export([get_type/1]).

-behaviour(gen_server).

-define(SERVER, ?MODULE). 
-define(table_name, sys_broadcast).

-record(state, {
	  is_farm_app,        %% 是否农场
	  timer_list = [],    %% 定时器列表
	  fix_list = []       %% 固定公告，虽然发送过了，但玩家上线后仍需要知道的公告
	 }
       ).

-record(sys_broadcast, {
	  id, 
	  type,         %% 类型
	  content,      %% 内容
	  start_time,   %% 开始时间
	  play_times,   %% 播放次数
	  priority      %% 优先级
	 }
       ).


%%%===================================================================
%%% API
%%%===================================================================
get_type(#sys_broadcast{type=Type}) ->
    Type.

%% GM修改公告通知
gm_change_broadcast(ID, Type) when is_integer(ID) ->
    gen_server:cast(?SERVER, {gm_change_broadcast, {ID, Type}}).

add(Type, Content, StartTime, PlayTimes, Priority) ->
    Id = guid:make(?st_sys_boardcast),
    SysBroadcast = #sys_broadcast{id=Id,
				  type=Type,
				  content=Content,
				  start_time=StartTime,
				  play_times=PlayTimes,
				  priority=Priority
				 },
    db_common:write(SysBroadcast),
    Id.

modify(Id, Type, Content, StartTime, PlayTimes, Priority) ->
    SysBroadcast = #sys_broadcast{id=Id,
				  type=Type,
				  content=Content,
				  start_time=StartTime,
				  play_times=PlayTimes,
				  priority=Priority
				 },
    db_common:write(SysBroadcast).
%% 发送公告
send(ID, Type, Content, PlayTimes, Priority, ShowSeconds, StartTime, IsFarmApp) ->
    Packet = #notify_sys_broadcast{id=ID, type=Type, content=Content, 
				   play_times=PlayTimes, 
				   priority=Priority,
				   start_time=datetime:make_time(StartTime),
				   show_seconds=ShowSeconds},
    send_to_all_players(Packet, IsFarmApp).

send_to_all_players(Packet, IsFarmApp) ->
    %%data_helper:format("send broadcast ~p~n~n", [Packet]),
    case IsFarmApp of
	true ->
	    Childs = supervisor:which_children(tcp_server_app),
	    FarmPlayers = [Id || {Id, _Child, _Type, Modules} <- Childs, Modules =:= [farm]],	    
	    %% 通知公告消息
	    TailLen = length("_sup"),
	    [begin
		 SupName = atom_to_list(Acc),
		 FarmName = list_to_atom(lists:sublist(SupName, length(SupName) - TailLen)),
		 
		 net_helper:send2client(FarmName, Packet) 
	     end|| Acc <- FarmPlayers];		
	_ ->
	    Players = db_player:get_online_players_key(node()),
	    %% 通知公告消息
	    [net_helper:send2client(Acc, Packet) || Acc <- Players]		
    end,
    ok.
    

send(0=_BroadcastID, _Params) ->
    ok;
send(BroadcastID, Params) when is_integer(BroadcastID),
			       is_list(Params) ->
    %% 取对应公告内容
    #sys_broadcast_tplt{content=Content, show_seconds=ShowSeconds} =
	tplt:get_data(sys_broadcast_tplt, BroadcastID),

    F = fun(X, Acc) -> util:replace_str(Acc, "%s", X) end,
    NewContent = lists:foldl(F, binary_to_list(Content), Params),
    Type = ?sbt_player_behavior, %% 炫耀个人信息(购买商品之类的)
    PlayTimes = 0,
    Priority = 0,
    send(0, Type, NewContent, PlayTimes, Priority, ShowSeconds, datetime:local_time(), false).

%% 请求固定公告
req_fixed_broadcast(Socket, {req_fixed_broadcast, Type}) 
  when is_port(Socket) ->
    gen_server:cast(?SERVER, {req_fixed_broadcast, Type, Socket}).

%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link() ->
    {ok, Pid} = gen_server:start_link({local, ?SERVER}, ?MODULE, [], []),
    gen_server:cast(Pid, load_from_db),
    {ok, Pid}.

%%%===================================================================
%%% gen_server callbacks
%%%===================================================================

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Initializes the server
%%
%% @spec init(Args) -> {ok, State} |
%%                     {ok, State, Timeout} |
%%                     ignore |
%%                     {stop, Reason}
%% @end
%%--------------------------------------------------------------------
init([]) ->
    process_flag(trap_exit, true),
    new_day_reset(),
    {ok, #state{is_farm_app=util:is_farm_app()}}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling call messages
%%
%% @spec handle_call(Request, From, State) ->
%%                                   {reply, Reply, State} |
%%                                   {reply, Reply, State, Timeout} |
%%                                   {noreply, State} |
%%                                   {noreply, State, Timeout} |
%%                                   {stop, Reason, Reply, State} |
%%                                   {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
handle_call(_Request, _From, State) ->
    Reply = ok,
    {reply, Reply, State}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling cast messages
%%
%% @spec handle_cast(Msg, State) -> {noreply, State} |
%%                                  {noreply, State, Timeout} |
%%                                  {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
%% 请求固定公告
handle_cast({req_fixed_broadcast, Type, Socket}, 
	    #state{fix_list=FixList}=State) ->
    [begin
	case X of
	    #sys_broadcast{id=ID,
			   type=Type, 
			   content=Content,
			   start_time=StartTime,
			  play_times=PlayTimes,
			  priority=Priority} ->

		ShowSeconds = 0,
		Packet = #notify_sys_broadcast{id=ID,
					       type=Type, 
					       content=Content, 
					       play_times=PlayTimes, 
					       priority=Priority,
					       start_time=datetime:make_time(StartTime),
					       show_seconds=ShowSeconds},		
		net_helper:send(Socket, Packet);
	    _ ->
		ok
	end
     end || X <- FixList],
    {noreply, State};   
%% GM修改公告通知
handle_cast({gm_change_broadcast, {ID, Type}}, #state{is_farm_app=IsFarmApp}=State) ->
    data_helper:format("gm change broadcast ~n~n"),
    case select_record(ID) of
	[] -> %% 找不到ID说明是删除操作, 要通知所有的客户端
	    Packet = #notify_del_broadcast{id=ID, type=Type}, 	    
	    send_to_all_players(Packet, IsFarmApp);
	_Record ->
	    ok
    end,
    %% 重新加载
    NewState = load_from_db(State),
    {noreply, NewState};  
    
%% 加载公告列表
handle_cast(load_from_db, State) ->
    NewState = load_from_db(State),
    {noreply, NewState};
handle_cast(_Msg, State) ->
    {noreply, State}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling all non call/cast messages
%%
%% @spec handle_info(Info, State) -> {noreply, State} |
%%                                   {noreply, State, Timeout} |
%%                                   {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
%% 发送公告
handle_info({time_to_send, #sys_broadcast{id=ID,
					  type=Type,
					  content=Content,
					  play_times=PlayTimes,
					  start_time=StartTime,
					  priority=Priority}=SysBroadcast}, 
	    #state{fix_list=FixList, 
		   timer_list=TimerList,
		   is_farm_app=IsFarmApp}=State) ->
    %%data_helper:format("time to send ~p~n", [Content]),
    %% 删除对应定时器
    NewTimerList = lists:keydelete(ID, 1, TimerList),

    ShowSeconds = 0,
    SendType = case Type of
		   ?sbt_per_day -> ?sbt_at_time; %% 每天公告其实就是定点公告
		   _ -> Type
	       end,
    send(ID, SendType, Content, PlayTimes, Priority, ShowSeconds, StartTime, IsFarmApp),
    %% 如果是固定公告需要放到列表中
    case Type of
	?sbt_sys_show ->
	    NewFixList = [SysBroadcast | FixList],
	    {noreply, State#state{timer_list=NewTimerList, fix_list=NewFixList}};
	?sbt_farm ->
	    NewFixList = [SysBroadcast | FixList],
	    {noreply, State#state{timer_list=NewTimerList, fix_list=NewFixList}};
	?sbt_per_day ->
	    TimerRef = next_every_day_timer(SysBroadcast, get_one_day_seconds()),
	    
	    TimerList2 = [{ID, TimerRef} | NewTimerList],
	    {noreply, State#state{timer_list=TimerList2}};
	_ ->
	    {noreply, State#state{timer_list=NewTimerList}}
    end;
%% 通知新的一天重置(鲜花记录清空，鸡蛋记录清空, 飘流瓶记录清空)
handle_info({time_to_send, new_day_reset},
	    #state{is_farm_app=IsFarmApp}=State) ->

    %% 通知每日重置
    send_to_all_players(#notify_new_day_reset{}, IsFarmApp),

    new_day_reset(),

    {noreply, State};

handle_info(_Info, State) ->
    {noreply, State}.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% This function is called by a gen_server when it is about to
%% terminate. It should be the opposite of Module:init/1 and do any
%% necessary cleaning up. When it returns, the gen_server terminates
%% with Reason. The return value is ignored.
%%
%% @spec terminate(Reason, State) -> void()
%% @end
%%--------------------------------------------------------------------
terminate(_Reason, State) ->
    clear_timers(State),
    ok.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Convert process state when code is changed
%%
%% @spec code_change(OldVsn, State, Extra) -> {ok, NewState}
%% @end
%%--------------------------------------------------------------------
code_change(_OldVsn, State, _Extra) ->
    {ok, State}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 过滤记录
%% 取未到时公告 或 过时,但必需显示的
filter_record(#sys_broadcast{start_time=StartTime, type=Type}, 
	      Now) ->
    (Type =:= ?sbt_sys_show) or
    (Type =:= ?sbt_farm) or
    (Type =:= ?sbt_per_day) or
    (StartTime > Now).


%% 取得公告列表
get_broadcast_list(Now) ->
    Q = qlc:q([E || E <- mnesia:table(sys_broadcast), filter_record(E, Now)]),
    PrepareList = db_common:do_qlc(Q),
    PrepareList.

%% 根据公告ID取记录
select_record(ID) ->    
    db_common:select_one(?table_name, ID).


%% 从数据库加载
%% 返回NewState
load_from_db(State) ->   
    %%mnesia:wait_for_tables([sys_broadcast], 100000),
    clear_timers(State),

    Now = datetime:local_time(),
    BroadcastList = get_broadcast_list(Now),
    
    %% 分离每天公告    
    FEveryDay = fun(#sys_broadcast{type=Type}) -> Type =:= ?sbt_per_day end,
    {EveryDayList, OtherList} = lists:partition(FEveryDay, BroadcastList),

    F = fun(#sys_broadcast{start_time=StartTime}) -> StartTime > Now end,
    %% 分离固定公告
    {PrepareList, FixList} = lists:partition(F, OtherList),
    %%data_helper:format("FixList: ~p~n", [FixList]),
			 
    TimerList = 
	[begin 
	     #sys_broadcast{id=ID, start_time=StartTime} = X,
	     %% 算出多少秒后应发送公告
	     Seconds = datetime:diff_time(Now, StartTime),
	     
	     %% 启动定时器
	     {ok, TimerRef} = timer:send_after(Seconds * 1000, {time_to_send, X}),
	     %%data_helper:format("start a timer: ~p~n~p~n", [Seconds, X]),
	     {ID, TimerRef}	 
	 end  || X <- PrepareList],

	
    TimerEveryDay = 
	[begin 
	     #sys_broadcast{id=ID} = X,
	     
	     TimerRef = start_every_day_timer(X),
	     {ID, TimerRef}	 
	 end  || X <- EveryDayList],
    

    State#state{timer_list=(TimerList ++ TimerEveryDay), fix_list=FixList}.

%% 清除定时器    
clear_timers(#state{timer_list=TimerList}) ->
    %% 取消未执行的定时器
    [timer:cancel(TimerRef) || {_ID, TimerRef} <- TimerList],
    ok.

get_time(#stime{hour=H, minute=Min, second=S})->
    {H, Min, S};
get_time({_Date, Time})->
    Time.

get_seconds({Hour, Minute, Seconds}) ->
     Hour * 60 * 60 + Minute * 60 + Seconds.

get_one_day_seconds() ->
    %% 24 * 60 * 60.
    common_def:get_val(seconds_of_a_day).

start_every_day_timer(SysBroadCast) ->
    ADaySeconds = 24 * 60 * 60,
    #sys_broadcast{start_time=StartTime} = SysBroadCast,

    %% 算出多少秒后应发送公告	     
    Diff = get_seconds(get_time(StartTime)) - get_seconds(datetime:time()),
    Seconds = 
	case Diff > 0 of
	    true -> Diff;
	    _ -> Diff + ADaySeconds
	end,
    
    %% 启动定时器
    {ok, TimerRef} = timer:send_after(Seconds * 1000, {time_to_send, SysBroadCast}),
    TimerRef.

next_every_day_timer(SysBroadCast, ADaySeconds) ->
    Seconds = ADaySeconds,
    %% 启动定时器
    {ok, TimerRef} = timer:send_after(Seconds * 1000, {time_to_send, SysBroadCast}),
    TimerRef.

new_day_reset() ->
    ADaySeconds = 24 * 60 * 60,
    StartTime = {{0, 0, 0}, common_def:get_val(every_day_reset_time)},

    %% 算出多少秒后应发送公告	     
    Diff = get_seconds(get_time(StartTime)) - get_seconds(datetime:time()),
    Seconds = 
	case Diff > 0 of
	    true -> Diff;
	    _ -> Diff + ADaySeconds
	end,
    
    %% 启动定时器
    {ok, _TimerRef} = timer:send_after(Seconds * 1000, {time_to_send, new_day_reset}).
