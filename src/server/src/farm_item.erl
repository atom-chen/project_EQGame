%%%-------------------------------------------------------------------
%%% @author linyijie <linyijie@china-channel.com>
%%% @copyright (C) 2010, linyijie
%%% @doc
%%%  农场物品
%%% @end
%%% Created : 12 Jul 2010 by linyijie <linyijie@china-channel.com>
%%%-------------------------------------------------------------------
-module(farm_item).

-include("packet_def.hrl").
-include("router.hrl").
-include("tplt_def.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").

-export([start/1, use/3, use/4, handle_cast/2]).



start(Account) ->
    [router:make_event_source(?msg_req_use_farm_item, Account, {self(), ?MODULE})
    ].

handle_cast({#msg{src=Account}, 
	     #req_use_farm_item{item_inst_id=InstID, plot_index=PlotIndex}}, State) ->
    use(Account, InstID, PlotIndex),
    {noreply, State}.

use(Account, InstId, PlotIndex) when is_integer(PlotIndex) ->
    Depot = farm_depot:read(Account),
    case farm_depot:get_index(InstId, Depot) of
	not_found -> 
	    {false, err_item};
	Index -> 
	    Item = farm_depot:get_item(Index, Depot),
	    Farm = player_farm:get_farm_data(Account),
	    %% 根据模板表中物品类型的定义执行对应的函数
	    TpltData = tplt:get_data2(farm_prop_tplt, item:get_tempid(Item)),
	    M = get_proc_fun(TpltData#farm_prop_tplt.type),
	    case do_use(M, Account, Index, TpltData, Farm, PlotIndex, Item, Depot) of
		{Depot1, NFarm, Reason} ->
		    NDepot = farm_depot:compress(Depot1),
		    farm_depot:save(NDepot),
		    farm_depot:send_depot(Account, NDepot),
		    player_farm:notify_farm_data(Account, Account, NFarm),
		    db_farm:write(NFarm),
		    M:after_use(Account, Reason);
		_ ->
		    ok
	    end
    end.

use(Account, TempId, Depot, Count) ->
    case farm_depot:get_index_by_tempid(TempId, Depot) of
	not_found -> {false, err_item};
	Index -> 
	    Item = farm_depot:get_item(Index, Depot),
	    Farm = player_farm:get_farm_data(Account),
	    %% 根据模板表中物品类型的定义执行对应的函数
	    TpltData = tplt:get_data2(farm_prop_tplt, item:get_tempid(Item)),
	    M = get_proc_fun(TpltData#farm_prop_tplt.type),
	    use(Account, TempId, Depot, Count, M, [], Farm, Index, Item, TpltData)
    end.

use(Account, _TempId, Depot, 0, M, Reason, Farm, _Index, _Item, _TpltData) ->
    farm_depot:save(Depot),
    farm_depot:send_depot(Account, Depot),
    player_farm:notify_farm_data(Account, Account, Farm),
    db_farm:write(Farm),
    M:after_use(Account, Reason);
use(Account, TempId, Depot, Count, M, _Reason, Farm, Index, Item, TpltData) ->
    case do_use(M, Account, Index, TpltData, Farm, 0, Item, Depot) of
	{NDepot, NFarm, NReason} ->
	    use(Account, TempId, NDepot, Count-1, M, NReason, NFarm, Index, Item, TpltData);
	_ ->
	    ok
    end.

do_use(M, Account, Index, TpltData, Farm, PlotIndex, Item, Depot) ->
    case M:can_use(Farm, PlotIndex, Item) of
	true ->
	    {NFarm, Reason} = M:do_use(Account, Farm, PlotIndex, TpltData, Item),
	    %% 删除该物品
	    NDepot = farm_depot:del_item(Index, 1, Depot),
	    {NDepot, NFarm, Reason};
	{false, Reason} ->
	    farm_sys_msg:send(Account, Reason)
    end.

get_proc_fun(1) ->
    farm_fertilizer; %% 化肥
get_proc_fun(7) ->
    farm_pet_food;
get_proc_fun(9) ->
    farm_aberrance. %% 变异道具

    
