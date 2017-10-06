%%% @author lyj <lyj@35info.cn>
%%% @copyright (C) 2010, lyj
%%% @doc
%%%  农场锄地
%%% @end
%%% Created :  1 Dec 2010 by lyj <lyj@35info.cn>

-module(farm_hoeing).

-include("sys_msg.hrl").
-include("tplt_def.hrl").

-export([can_hoeing/1, do_hoeing/4]).

%% 是否能能锄地
can_hoeing(Crop) ->
    case farm_crop:is_crop(Crop) of 
	true -> true;
	false -> {false, ?err_plot_empty}
    end.
    
%% 铲除作物
do_hoeing(Account, FarmLvl, Crop, State)->
    case farm_crop:is_crop_dead(Crop) of
	true ->
	    Tplt = tplt:get_data2(farmhoeing_awards_tplt, FarmLvl),
	    case ((not wallow:in_bad_time(State)) and (random:uniform(100) < Tplt#farmhoeing_awards_tplt.ratio)) of
		true ->
		    ItemID = get_awards(Tplt#farmhoeing_awards_tplt.item_id),
		    Count = wallow:calc_income(Tplt#farmhoeing_awards_tplt.count, State),
		    Depot = farm_depot:read(Account),
		    NDepot = farm_depot:add_item(Depot, ItemID, Count),
		    farm_depot:save(NDepot),
		    NCrop = farm_crop:new_crop(0, [], Crop),
		    {NCrop, ItemID, Count};
		false ->
		    NCrop = farm_crop:new_crop(0, [], Crop),
		    {NCrop, 0, 0}
	    end;
	false ->
	    NCrop = farm_crop:new_crop(0, [], Crop),
	    {NCrop, 0, 0}
    end.


get_awards(ItemList)->
    L = length(ItemList),
    R = rand:uniform(L),
    lists:nth(R, ItemList).

-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

get_awards_test() ->
    Items = [1, 2, 3 ,4, 5],
    erlymock:start(),
    erlymock:stub(rand, uniform, [length(Items)], [{return, 2}]),
    erlymock:replay(),


    ?assertEqual(2, get_awards(Items)),


    erlymock:verify(),
    ok.

-endif.
