%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%% ���ݵؿ�
%%% @end
%%% Created :  3 Mar 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(house_plot).

-export([init/1, add/1, get/1, add/2, get_empty_plot/1, replace/3, get_house/2]).
-export([get_plots/1, set_plots/2, get_plot_id/1, get_house_data/1, get_house_plot_id/2]).

-include("common_def.hrl").

-record(house_plot,         % ������ؿ��ϵ��
	{account,           % ����ʺ�
	 plots              % �ؿ鼯��
	}).

-record(plot,               % �ؿ�
	{id,                % �ؿ��ʶ
	 house_data         % ��������
	}).
%%%===================================================================
%%% API
%%%===================================================================
%% ��ʼ�����ݵؿ�
-spec init(atom()) -> tuple().
init(Account) ->
    Plots = [#plot{id=guid:make(?st_house_plot), house_data=[]}
	    ],
    #house_plot{account=Account, plots=Plots}.

get(Account) ->
    db_common:select_one(house_plot, Account).

%% ���ӵؿ�
add(HousePlot) ->
    add(HousePlot, 1).
%% ���ӵؿ�
add(HousePlot, Number) ->
    Plots = get_plots(HousePlot),
    CreatePlots = create_plot(Number, []),
    NPlots = lists:append(Plots, CreatePlots),
    set_plots(HousePlot, NPlots).

%% ��ȡ�յĵؿ�,���û�пյؿ��򷵻�0
-spec get_empty_plot(tuple()) -> integer().
get_empty_plot(HousePlot) ->
    Plots = get_plots(HousePlot),
    do_get_empty_plot(Plots).

%% �滻�ؿ��ϵķ���
-spec replace(tuple(), integer(), tuple()) -> tuple().
replace(HousePlot, Id, HouseData) when Id > 0 ->
    Plots = get_plots(HousePlot),
    Plot = lists:keyfind(Id, 2, Plots),
    NPlot = set_house_data(Plot, HouseData),
    NPlots = lists:keyreplace(Id, 2, Plots, NPlot),
    set_plots(HousePlot, NPlots).    

%% ��ȡ�ؿ��ϵķ���
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
%% ��ȡ�ؿ鼯��
get_plots(#house_plot{plots=Plots}) ->
    Plots.
%% ���õؿ鼯��
set_plots(HousePlot, Plots) ->
    HousePlot#house_plot{plots=Plots}.

%% ��ȡ��������
get_house_data(#plot{house_data=HouseData}) ->
    HouseData.
%% ���÷�������
set_house_data(Plot, HouseData) ->
    Plot#plot{house_data=HouseData}.

%% ��ȡ�ؿ�Id
get_plot_id(#plot{id=Id}) ->
    Id.

%% ��ȡ�յĵؿ�Id
do_get_empty_plot([]) ->
    0;
do_get_empty_plot([#plot{id=Id, house_data=HouseData}|Plots]) ->
    case HouseData == [] of
	true ->
	    Id;
	false ->
	    do_get_empty_plot(Plots)
    end.

%% �����ؿ�
create_plot(0, Plots) ->
    Plots;
create_plot(Number, Plots) ->
    Plot = #plot{id=guid:make(?st_house_plot), house_data=[]},
    create_plot(Number - 1, [Plot|Plots]).
