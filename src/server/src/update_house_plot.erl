%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%% 升级房屋地块信息
%%% @end
%%% Created : 16 Mar 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(update_house_plot).


%% API
-export([update/0, add_fields/1]).

%%%===================================================================
%%% API
%%%===================================================================
update() ->
    Keys = get_table_keys(house_data),
    HousePlots = update_house_plot(Keys, []),
    case HousePlots == [] of
	true ->
	    ok;
	false ->
	    db_common:write(HousePlots)
    end,
    {atomic, ok}. 

update_house_plot([], HousePlots) ->
    HousePlots;
update_house_plot([Key|Keys], HousePlots) ->
    HousePlot = house_plot:init(Key),
    update_house_plot(Keys, [HousePlot|HousePlots]).


%%%===================================================================
%%% Internal functions
%%%===================================================================
get_table_keys(Tab) ->
    F = fun() ->
		mnesia:all_keys(Tab)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).



add_fields(DefValList) when is_list(DefValList) ->
    PlotKeys = get_table_keys(house_plot),
    [add_house_plot_field(Key, DefValList) || Key <- PlotKeys],    
    {atomic, ok}.


add_house_plot_field(Key, DefValList) ->
    {TabName, Account, Plots} = db_common:select_one(house_plot, Key),
    NewPlots = [{plot, ID, do_add_field(HouseData, DefValList)} 
		|| {plot, ID, HouseData} <- Plots],

    db_common:write({TabName, Account, NewPlots}).

do_add_field([], _DefValList) ->
    [];
do_add_field(HouseData, DefValList) ->
    [erlang:append_element(HouseData, Val) || Val <- DefValList].
