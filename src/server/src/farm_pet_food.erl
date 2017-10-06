%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 宠物食品(狗粮)
%%% @end
%%% Created : 11 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_pet_food).

-define(QUANTITY, 200).

-include("packet_def.hrl").
-include("sys_msg.hrl").

%% API
-export([is_exist/1]).
-export([can_use/3, do_use/5, after_use/2]).


get_period_date(#farm_pet_food{period_date=PeriodDate}) ->
    PeriodDate.

set_period_date(PetFood, PeriodDate) ->
    PetFood#farm_pet_food{period_date=PeriodDate}.
%%%===================================================================
%%% API
%%%===================================================================
can_use(Farm, _PlotIndex, _Item) ->
    PetFood = player_farm:get_pet_food(Farm),
    Decoration = player_farm:get_decoration(Farm),
    case farm_decoration:is_exist_dog(Decoration) of
	true ->
	    can_add(PetFood);
	false ->
	    {false, ?msg_farm_does_not_have_dog}
    end.

do_use(_Account, Farm, _PlotIndex, _Tplt, Item)->
    TempId = item:get_tempid(Item),
    DelItemTime = item:get_del_time(TempId),
    PetFood = player_farm:get_pet_food(Farm),
    Now = datetime:localtime(),
    Second = datetime:diff_time(Now, DelItemTime),
    {Reply, NPetFood} = add(Second, Now, PetFood),
    NFarm = player_farm:set_pet_food(Farm, NPetFood),

    Reason = 
    case Reply == normal of
	true ->
	    ?msg_pet_food_used;
	false ->
	    ?msg_farm_pet_food_waste
    end,
    {NFarm, Reason}.

after_use(Account, Reason) ->
    io:format("Reason:~p~n", [Reason]),
    farm_sys_msg:send(Account, Reason).


%% 是否存在食物
-spec is_exist(tuple()) -> atom().
is_exist(PetFood) ->
    PeriodDate = get_period_date(PetFood),
    Now = datetime:localtime(),
    DiffTime = datetime:diff_time(Now, PeriodDate),
    DiffTime > 0.

%% 是否能够增加狗粮
-spec can_add(tuple()) -> atom() | tuple().
can_add(PetFood) ->
    PeriodDate = get_period_date(PetFood),
    Now = datetime:localtime(),
    case datetime:diff_time(PeriodDate, Now) > 0 of
	true -> %% 如果当前没有狗粮
	    true;
	false -> %% 如果当前存在狗粮
	    case Now == PeriodDate of
		true ->
		    {false, ?msg_pet_food_is_full};
		false ->
		    true
	    end
    end.

%% 增加狗粮
-spec add(integer(), tuple(), tuple()) -> tuple().
add(Second, Now, PetFood) ->
    MaxDate = calc_max_date(Now),
    CurrDate = get_period_date(PetFood),
    {Reply, NPeriodDate} = calc_date(Second, CurrDate, Now, MaxDate),
    data_helper:format("Second:~p, Now:~p, MaxDate:~p, NPeriodDate:~p~n", [Second, Now, MaxDate, NPeriodDate]),
    NPetFood = set_period_date(PetFood, NPeriodDate),
    {Reply, NPetFood}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
calc_date(Second, CurrDate, Now, MaxDate) ->
    NCurrDate = get_curr_date(CurrDate, Now),
    Date = datetime:add_time(NCurrDate, Second),
    case datetime:diff_time(Date, MaxDate) > 0 of
	true ->
	    {normal, Date};
	false ->
	    {waste, MaxDate}
    end.

get_curr_date(CurrDate, Now) ->
    case datetime:diff_time(CurrDate, Now) > 0 of
	true ->
	    Now;
	false ->
	    CurrDate
    end.

calc_max_date(Date) ->
    Second = 24000 * 24 * 60 * 60,
    datetime:add_time(Date, Second).
