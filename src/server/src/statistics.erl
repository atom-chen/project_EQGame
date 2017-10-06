%%% @author lyj <lyj@35info.cn>
%%% @copyright (C) 2010, lyj
%%% @doc
%%%  统计的功能模块
%%% @end
%%% Created : 16 Dec 2010 by lyj <lyj@35info.cn>
-module(statistics).

-behaviour(gen_server).

%% API
-export([start_link/0]).

%% gen_server callbacks
-export([init/1, handle_call/3, handle_cast/2, handle_info/2,
	 terminate/2, code_change/3]).

-export([buy_item/3, add_money/3]).

%% -record(stat,
%% 	{who,
%% 	 datas = gb_trees:empty()
%% 	}               
%%        ).  

-define(SERVER, ?MODULE). 

%%%===================================================================
%%% API
%%%===================================================================
%% 统计购买物品数量
-spec buy_item(atom(), pos_integer(), pos_integer()) -> ok.
buy_item(Account, ItemID, Count) ->
    gen_server:cast(?SERVER, {buy_item, Account, ItemID, Count}),
    ok.

-spec add_money(atom(), pos_integer(), pos_integer()) -> ok.
add_money(Account, Type, Count) ->
    gen_server:cast(?SERVER, {add_money, Account, Type, Count}),
    ok.

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
    %% TODO: 读取数据
    %% TODO: 定时写入数据
    {ok, gb_trees:empty()}.

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
handle_cast({buy_item, Account, ItemID, Count}, State) ->
    Datas = find_datas(Account, State),
    NDatas = change_datas(fun do_buy_item/2, buy_item, {ItemID, Count}, Datas),
    {noreply, update_state(Account, NDatas, State)};

handle_cast({add_money, Account, Type, Count}, State) ->
    Datas = find_datas(Account, State),
    NDatas = change_datas(fun do_add_money/2, add_money, {Type, Count}, Datas),
    {noreply, update_state(Account, NDatas, State)};
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
do_buy_item({ItemID, Count}=Data, ItemList) -> 
    case lists:keyfind(ItemID, 1, ItemList) of 
	false -> [Data | ItemList];
	{ID, C} -> lists:keyreplace(ID, 1, ItemList, {ID, Count + C})
    end.


do_add_money({Type, Count}=Data, AddList) -> 
    case lists:keyfind(Type, 1, AddList) of 
	false -> [Data | AddList];
	{_T, C} -> lists:keyreplace(Type, 1, AddList, {Type, Count + C})
    end.

change_datas(Fun, Type, Data, Datas) ->
    case lists:keyfind(Type, 1, Datas) of
	false ->
	    [{Type, [Data]} | Datas];
	{_Key, Val} ->
	    NVal = Fun(Data, Val),
	    lists:keyreplace(Type, 1, Datas, {Type, NVal})
    end.

find_datas(Who, State) ->
    case gb_trees:lookup(Who, State) of
	none ->
	    [];
	{value, Val} ->
	    Val
    end.

update_state(Who, Datas, State) ->
    gb_trees:enter(Who, Datas, State).


-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

do_buy_item_test() ->
    ?assertEqual([{1, 1}], do_buy_item({1, 1}, [])),
    ?assertEqual([{2, 2}, {1, 1}], do_buy_item({2, 2}, [{1, 1}])),
    ?assertEqual([{1, 3}], do_buy_item({1, 2}, [{1, 1}])),
    ok.

do_add_money_test() ->
    ?assertEqual([{work, 1}], do_add_money({work, 1}, [])),
    ?assertEqual([{farm, 2}, {work, 1}], do_buy_item({farm, 2}, [{work, 1}])),
    ?assertEqual([{work, 3}], do_buy_item({work, 2}, [{work, 1}])),
    ok.

-endif.  


