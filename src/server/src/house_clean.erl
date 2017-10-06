%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%% 房屋清洁度
%%% @end
%%% Created : 15 Apr 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(house_clean).

-include("router.hrl").
-include("gen_scene.hrl").

-define(max_house_clean, 100).

%% API
-export([add/3, dec/2, dec/3]).
-export([start/1, handle_call/3]).

start(SceneName)->
    [router:make_event_target(calc_add_house_clean, SceneName, {self(), ?MODULE}),
     router:make_event_target(house_clean_is_full, SceneName, {self(), ?MODULE}),
     router:make_event_target(add_house_clean, SceneName, {self(), ?MODULE})
    ].

%%%===================================================================
%%% 处理handle_call
%%%===================================================================
%% 计算可增加的房屋清洁度
handle_call({#msg{event=calc_add_house_clean}, AddHouseClean}, _From, #scene_data{custom_data=SceneData}=State) -> 
    HouseData = house:get_house_data(SceneData),
    NHouseClean = calc_add_house_clean(HouseData, AddHouseClean),
    {reply, NHouseClean, State};
handle_call(#msg{event=house_clean_is_full}, _From, #scene_data{custom_data=SceneData}=State) -> 
    HouseData = house:get_house_data(SceneData),
    HouseClean = house_data:get_house_clean(HouseData),
    IsFull = (?max_house_clean - HouseClean) =< 0,
    {reply, IsFull, State};
handle_call({#msg{event=add_house_clean}, AddHouseClean}, _From, #scene_data{custom_data=SceneData}=State) ->
    HouseData = house:get_house_data(SceneData),
    Owner = house_data:get_owner(HouseData),
    {CanAddHouseClean, NHouseData} = adjust_house_clean(HouseData, AddHouseClean),
    NSceneData = house:set_house_data(SceneData, NHouseData),
    net_helper:send2client(Owner, #notify_house_clean{house_clean=house_data:get_house_clean(NHouseData)}),
    {reply, {CanAddHouseClean, NHouseData}, State#scene_data{custom_data=NSceneData}}.
%%%===================================================================
%%% API
%%%===================================================================
%% 增加房屋清洁度，并通知客户端
add(Account, HouseData, HouseClean) ->
    HouseClean1 = house_data:get_house_clean(HouseData),
    NHouseClean = add(HouseClean1, HouseClean),
    net_helper:send2client(Account, #notify_house_clean{house_clean=NHouseClean}),
    house_data:set_house_clean(HouseData, NHouseClean).

dec(Account, HouseData, HouseClean) ->
    HouseClean1 = house_data:get_house_clean(HouseData),
    NHouseClean = dec(HouseClean1, HouseClean),
    net_helper:send2client(Account, #notify_house_clean{house_clean=NHouseClean}),
    house_data:set_house_clean(HouseData, NHouseClean). 

dec(HouseData, HouseClean) when is_record(HouseData, house_data) ->
    HouseClean1 = house_data:get_house_clean(HouseData),
    NHouseClean = dec(HouseClean1, HouseClean),
    house_data:set_house_clean(HouseData, NHouseClean);
dec(HouseClean1, HouseClean2) ->
    NHouseClean = HouseClean1 - HouseClean2,
    case NHouseClean >= 0 of
	true ->
	    NHouseClean;
	false ->
	    0
    end.
%%%===================================================================
%%% Internal functions
%%%===================================================================
calc_add_house_clean(HouseData, AddHouseClean) when AddHouseClean >= 0 ->
    HouseClean = house_data:get_house_clean(HouseData),
    CanAddHouseClean = ?max_house_clean - HouseClean,
    case CanAddHouseClean >= AddHouseClean of
	true ->
	    AddHouseClean;
	false ->
	    CanAddHouseClean
    end;
calc_add_house_clean(HouseData, AddHouseClean) when AddHouseClean < 0 ->
    HouseClean = house_data:get_house_clean(HouseData),
    case HouseClean + AddHouseClean >= 0 of
	true ->
	    AddHouseClean;
	false ->
	    -HouseClean
    end.

add(HouseData, HouseClean) when is_record(HouseData, house_data) ->
    HouseClean1 = house_data:get_house_clean(HouseData),
    NHouseClean = add(HouseClean1, HouseClean),
    house_data:set_house_clean(HouseData, NHouseClean);
add(HouseClean1, HouseClean2) ->
    NHouseClean = HouseClean1 + HouseClean2,
    case NHouseClean > ?max_house_clean of
	true ->
	    ?max_house_clean;
	false ->
	    NHouseClean
    end.

adjust_house_clean(HouseData, AddHouseClean) ->
    CanAddHouseClean = calc_add_house_clean(HouseData, AddHouseClean),
    NHouseData = add(HouseData, CanAddHouseClean),
    {CanAddHouseClean, NHouseData}.

