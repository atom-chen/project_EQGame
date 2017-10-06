%%%-------------------------------------------------------------------
%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  飘流池
%%%     注：因数据库随机访问某条记录比较低效，所以才会有这个缓存单元
%%% @end
%%% Created : 27 May 2011 by hongjx <hongjx@35info.cn>
%%%-------------------------------------------------------------------
-module(float_bottle_pool).

-behaviour(gen_server).

%% API
-export([start_link/0]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-export([add_bottle/5, rand_pick_bottle/2]).
-export([get_gm_bottle_count/0, add_gm_bottle/4, clear_all_gm_bottle/0]).

-include("packet_def.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").

-define(SERVER, ?MODULE). 

-record(state, {id_list,  %  
		size,     % 长度
		max_id    % 最大id
	       }).

-record(float_bottle, {id, 
		       datetime, 
		       type, 
		       sex, 
		       account, 
		       account_visible, 
		       msg, 
		       is_gm = false
		      }).

%%%===================================================================
%%% API
%%%===================================================================
%% 根据条件随机取飘流瓶
rand_pick_bottle(Picker, PickerSex) ->
    gen_server:call(?SERVER, {rand_pick_bottle, Picker, PickerSex}).


%% 增加飘流瓶
add_bottle(Msg, Type, Sex, Account, Visible) when is_list(Msg),
						  is_integer(Type),
						  is_integer(Sex),
						  is_atom(Account),
						  is_integer(Visible) ->
    gen_server:call(?SERVER, {add_bottle, {Msg, Type, Sex, Account, Visible}}).


%% 投放官方瓶
add_gm_bottle(BottleCount, GMName, Msg, RewardID) when is_integer(BottleCount),
						       is_list(GMName),
						       is_list(Msg),
						       is_integer(RewardID) ->
    gen_server:call(?SERVER, {add_gm_bottle, {BottleCount, GMName, Msg, RewardID}}).


%% 取官方瓶子数
get_gm_bottle_count() ->
    gen_server:call(?SERVER, get_gm_bottle_count).

%% 删除所有官方瓶
clear_all_gm_bottle() ->
    gen_server:call(?SERVER, clear_all_gm_bottle).


%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start_link() ->
    gen_server:start_link({local, ?SERVER}, ?MODULE, [], []).

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
    InitState = load_from_db(),
    {ok, InitState}.

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
handle_call(clear_all_gm_bottle, _From, 
	    #state{id_list=IDList}=State) ->
    F = fun(ID, Acc) -> 
		R = db_common:select_one(float_bottle, ID),
		case is_gm_bottle(R) of
		    true -> 
			db_common:delete(float_bottle, ID),			
			Acc;
		    _ -> [ID | Acc]
		end		
	end,
    NewIDList = lists:foldl(F, [], IDList),    
    NIDList = lists:reverse(NewIDList), %% 保持有序
    {reply, ok, State#state{id_list=NIDList, size=length(NIDList)}};   
handle_call(get_gm_bottle_count, _From, 
	    #state{size=Size, id_list=IDList}=State) ->
    F = fun(ID, Acc) -> 
		R = db_common:select_one(float_bottle, ID),
		case is_gm_bottle(R) of
		    true -> Acc + 1;
		    _ -> Acc
		end		
	end,
    GMCount = lists:foldl(F, 0, IDList),  
    {reply, {GMCount, Size - GMCount}, State};   
handle_call({add_gm_bottle, {BottleCount, GMName, Msg, _RewardID}}, _From, 
	    State) ->
    %% 类型暂时为普通瓶
    Type = ?bt_common,
    Sex = undefined,
    Visible = undefined,      

    F = fun(_X, Acc) ->
		{_ID, NewAcc} = do_add_bottle(Msg, Type, Sex, GMName, Visible, true, Acc),
		NewAcc
	end,

    NewState = lists:foldl(F, State, lists:seq(1, BottleCount)),
    
    {reply, ok, NewState};   
handle_call({rand_pick_bottle, Picker, PickerSex}, _From, 
	    #state{size=Size, id_list=IDList}=State) ->
    case Size > 0 of
	true ->
	    %% 随机取一条
	    N = rand:uniform(Size),
	    ID = lists:nth(N, IDList),
	    FloatBottle = db_common:select_one(float_bottle, ID),
	    #float_bottle{id=ID, datetime=Time, msg=Msg, type=Type, is_gm=IsGm,
			  account=Account, account_visible=Visible} = FloatBottle,    
	    
	    case can_pick(FloatBottle, Picker, PickerSex) of
		true ->
		    db_common:delete(float_bottle, ID),
		    NewState = State#state{id_list=lists:delete(ID, IDList),
					   size=Size - 1},
		    
		    AccountStr = get_account_str(IsGm, Account, Visible),
		    
		    Reply = #bottle_info{id=ID, type=Type, msg=Msg, time=Time, account=AccountStr},	    
		    {reply, Reply, NewState};
		_ ->
		    {reply, can_not_pick, State}
	    end;
	_  ->
	    {reply, no_record, State}		
    end;    

handle_call({add_bottle, {Msg, Type, Sex, Account, Visible}}, _From, 
	    State) ->

    {ID, NewState}= do_add_bottle(Msg, Type, Sex, Account, Visible, false, State), 

    {reply, ID, NewState};

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
terminate(_Reason, _State) ->
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

load_from_db() ->
    RawList = db_common:all_keys(float_bottle),
    IDList = lists:sort(RawList),
    Size = length(IDList),    
    MaxID = case Size of 
		0 ->
		    0;
		_ ->
		    lists:last(IDList)
	    end,

    #state{id_list=IDList, size=Size, max_id=MaxID}.


%% 判断能否拾取
can_pick(#float_bottle{sex=Sex, type=Type,
		       account=Account}, Picker, PickerSex) ->
    %% 不能捡到自已的, 
    case Account =:= Picker of
	true -> false;
	_ ->	
	    %% 交往瓶不能捡到性别不一样的
	    case (Type =:= ?bt_make_friend) of
		true -> (Sex =:= PickerSex);
		_ -> true
	    end
    end.

is_gm_bottle(#float_bottle{is_gm=IsGm}) ->
    IsGm.

get_account_str(IsGm, Account, Visible) ->
    %% 是GM时，Account表示字串, 否则Account是一个原子
    case IsGm of 
	true -> Account;
	_ ->
	    case Visible of 
		?TRUE -> atom_to_list(Account);
		_ -> ""
	    end
    end.
    

do_add_bottle(Msg, Type, Sex, Account, Visible, IsGm, 
	      #state{size=Size, id_list=IDList, max_id=MaxID}=State) ->
    %% 写入数据库
    NewMaxID = MaxID + 1,
    db_common:write(#float_bottle{id=NewMaxID,
				  datetime=datetime:localtime(), 
				  type=Type, 
				  sex=Sex, 
				  account=Account,
				  account_visible=Visible,
				  msg=Msg,
				  is_gm=IsGm}),


    Limit = common_def:get_val(game_float_bottle_count_limit),   %% 飘流瓶总数限制
    case (Size >= Limit) and (Limit > 0) of
	true ->	 
	    [MinID, Tail] = IDList, 

	    %% 删除最小ID, 也就是最早飘流瓶
	    db_common:delete(float_bottle, MinID),

	    %% 最大的放在尾部，保持有序
	    NIDList = Tail ++ [NewMaxID],
	    NState = State#state{id_list=NIDList,
			   size=Size,
			   max_id=NewMaxID
			  },
	    {NewMaxID, NState};
	_ ->
	    NewState = State#state{id_list=IDList ++ [NewMaxID],
			   size=Size + 1,
			   max_id=NewMaxID
			  },

	    {NewMaxID, NewState}
    end.
