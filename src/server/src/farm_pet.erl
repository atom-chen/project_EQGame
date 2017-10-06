%%%-------------------------------------------------------------------
%%% @author  linyb
%%% @copyright (C) 2010, 
%%% @doc
%%%  ũ������
%%% @end
%%% Created : 11 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_pet).

-include("packet_def.hrl").
-include("router.hrl").
-include("tplt_def.hrl").
-include("common_def.hrl").
-include("sys_msg.hrl").

%% API
-export([can_bite/2, do_pet_bite/6, has_pet_and_food/1, is_bite/1]).
%%%===================================================================
%%% API
%%%===================================================================
can_bite(GameCoin, PlayerFarm) ->
    Decoration = player_farm:get_decoration(PlayerFarm),
    case farm_decoration:is_exist_dog(Decoration) of
	true ->
	    Dog = farm_decoration:get_dog(Decoration),
	    TempId = item:get_tempid(Dog),
	    PetTplt = get_pet_tplt(TempId),
	    CanBiteByRate = get_rate(PetTplt#farm_pet_tplt.rate),
	    Decoration = player_farm:get_decoration(PlayerFarm),
	    PetFood = player_farm:get_pet_food(PlayerFarm),
	    ((GameCoin == 0) or (CanBiteByRate and (GameCoin > 0))) and 
		farm_pet_food:is_exist(PetFood);
	false -> false
    end.
    

%% ���������˺�
-spec do_pet_bite(tuple(), tuple(), atom(), atom(), tuple(), tuple()) -> tuple().
do_pet_bite(PlayerFarm, LaborerFarm, Owner, Guest, UserName, State) when is_atom(Owner), is_atom(Guest) ->
    Decoration = player_farm:get_decoration(PlayerFarm),
    Dog = farm_decoration:get_dog(Decoration),
    TempId = item:get_tempid(Dog),
    Hurt = calc_pet_bite(farm_coin:get_coin(LaborerFarm), TempId),

    NHurt = wallow:calc_income(Hurt, State), %% ��������Ժ����۳���ֵ
    farm_log:write(bite, Owner, Guest, UserName, TempId, Hurt),
    NLaborerFarm = farm_coin:dec_coin(Hurt, LaborerFarm),
    NFarm = farm_coin:add_coin(NHurt, PlayerFarm),
    {NFarm, NLaborerFarm, NHurt}.

%% �Ƿ��г���
-spec has_pet_and_food(tuple()) -> boolean().
has_pet_and_food(Farm) ->
    Decoration = player_farm:get_decoration(Farm),
    PetFood = player_farm:get_pet_food(Farm),
    (farm_decoration:is_exist_dog(Decoration) and farm_pet_food:is_exist(PetFood)).

%% ���ݼ����ж��Ƿ�ҧ��
is_bite(Farm) ->
    Decoration = player_farm:get_decoration(Farm),
    Dog = farm_decoration:get_dog(Decoration),
    TempId = item:get_tempid(Dog),
    PetTplt = get_pet_tplt(TempId),
    get_rate(PetTplt#farm_pet_tplt.rate).
%%%===================================================================
%%% Internal functions
%%%===================================================================
%% ���������˺�ֵ
-spec calc_pet_bite(integer(), integer()) -> integer().
calc_pet_bite(LaborerCoin, PetId) ->
    PetTplt = get_pet_tplt(PetId),
    Seed = PetTplt#farm_pet_tplt.max_coin - PetTplt#farm_pet_tplt.min_coin,
    Coin = rand:uniform(Seed) + PetTplt#farm_pet_tplt.min_coin,
    erlang:min(LaborerCoin, Coin).

%% ��ȡ����ģ����Ϣ
-spec get_pet_tplt(integer()) -> tuple().
get_pet_tplt(TempId) ->
    tplt:get_data2(farm_pet_tplt, TempId).

%% �Ƿ������˺����
-spec get_rate(float()) -> atom().
get_rate(Rate) ->
    rand:uniform(100) =< Rate.


%%%===================================================================
%%% test
%%%===================================================================
