%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%% 房屋地块
%%% @end
%%% Created :  3 Mar 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(house_plot).

-export([init/1, add/1, get/1, add/2, get_empty_plot/1, replace/3, get_house/2]).
-export([get_plots/1, set_plots/2, get_plot_id/1, get_house_data/1, get_house_plot_id/2]).

-include("common_def.hrl").

-record(house_plot,         % 房屋与地块关系表
	{account,           % 玩家帐号
	 plots              % 地块集合
	}).

-record(plot,               % 地块
	{id,                % 地块标识
	 house_data         % 房屋数据
	}).
%%%===================================================================
%%% API
%%%===================================================================
%% 初始化房屋地块
-spec init(atom()) -> tuple().
init(Account) ->
    Plots = [#plot{id=guid:make(?st_house_plot), house_data=[]}
	    ],
    #house_plot{account=Account, plots=Plots}.

get(Account) ->
    db_common:select_one(house_plot, Account).

%% 增加地块
add(HousePlot) ->
    add(HousePlot, 1).
%% 增加地块
add(HousePlot, Number) ->
    Plots = get_plots(HousePlot),
    CreatePlots = create_plot(Number, []),
    NPlots = lists:append(Plots, CreatePlots),
    set_plots(HousePlot, NPlots).

%% 获取空的地块,如果没有空地块则返回0
-spec get_empty_plot(tuple()) -> integer().
get_empty_plot(HousePlot) ->
    Plots = get_plots(HousePlot),
    do_get_empty_plot(Plots).

%% 替换地块上的房屋
-spec replace(tuple(), integer(), tuple()) -> tuple().
replace(HousePlot, Id, HouseData) when Id > 0 ->
    Plots = get_plots(HousePlot),
    Plot = lists:keyfind(Id, 2, Plots),
    NPlot = set_house_data(Plot, HouseData),
    NPlots = lists:keyreplace(Id, 2, Plots, NPlot),
    set_plots(HousePlot, NPlots).    

%% 获取地块上的房屋
-spec get_house(tuple(), integer()) -> tuple().
get_house(HousePlot, PlotId) ->
    Plots = get_plots(HousePlot),
    Plot = lists:keyfind(PlotId, 2, Plots),
    get_house_data(Plot).

get_house_plot_id(HousePlot, InstId) ->
    Plots = get_plots(HousePlot),
    do_get_house_plot_id(Plots, InstId).

do_get_house_plot_id([], _InstId) ->
    0;
do_get_house_plot_id([Plot|Plots], InstId) ->
    HouseData = get_house_data(Plot),
    CurrInstId = house_data:get_instance_id(HouseData),
    case CurrInstId == InstId of
	true ->
	    get_plot_id(Plot);
	false ->
	    do_get_house_plot_id(Plots, InstId)	    
    end.

    
%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 获取地块集合
get_plots(#house_plot{plots=Plots}) ->
    Plots.
%% 设置地块集合
set_plots(HousePlot, Plots) ->
    HousePlot#house_plot{plots=Plots}.

%% 获取房屋数据
get_house_data(#plot{house_data=HouseData}) ->
    HouseData.
%% 设置房屋数据
set_house_data(Plot, HouseData) ->
    Plot#plot{house_data=HouseData}.

%% 获取地块Id
get_plot_id(#plot{id=Id}) ->
    Id.

%% 获取空的地块Id
do_get_empty_plot([]) ->
    0;
do_get_empty_plot([#plot{id=Id, house_data=HouseData}|Plots]) ->
    case HouseData == [] of
	true ->
	    Id;
	false ->
	    do_get_empty_plot(Plots)
    end.

%% 创建地块
create_plot(0, Plots) ->
    Plots;
create_plot(Number, Plots) ->
    Plot = #plot{id=guid:make(?st_house_plot), house_data=[]},
    create_plot(Number - 1, [Plot|Plots]).
