%%%-------------------------------------------------------------------
%%% File    : basic_data.erl
%%% Author  :  <>
%%% Description : 
%%%
%%% Created :  5 Nov 2009 by  <>
%%%-------------------------------------------------------------------
-module(basic_data).

-include("packet_def.hrl").

%% API
-compile(export_all).
%%====================================================================
%% API
%%====================================================================
get_data(account, #player_basic_data{account=Account})->
    Account;
get_data(username, #player_basic_data{username=UserName})->
    UserName;
get_data(health_status, #player_basic_data{health_status=HealthStatus}) ->
    HealthStatus;
get_data(sex, #player_basic_data{sex=Sex})->
    Sex;
get_data(hair, #player_basic_data{hair=Hair})->
    Hair;
get_data(face, #player_basic_data{face=Face})->
    Face;
get_data(beard, #player_basic_data{beard=Beard})->
    Beard;
get_data(hair_color, #player_basic_data{hair_color=HairColor})->
    HairColor;
get_data(skin_color, #player_basic_data{skin_color=SkinColor})->
    SkinColor;

get_data(online_time, #player_basic_data{online_time=OnlineTime}) ->
    OnlineTime;
get_data(logout_time, #player_basic_data{logout_time=LogoutTime}) ->
    LogoutTime;
get_data(Type, Data) ->
    erlang:error({badarg, Type, Data}).

get_datas(Types, BasicData) ->
    [get_data(Type, BasicData) || Type <- Types].

%% Datas=[Data]
%% Data ={tuple(), Value}
modify_datas([], BasicData)->
    BasicData;
modify_datas([{Type, Value} | Rest], BasicData) ->
    NewBasicData = set_data(Type, Value, BasicData),
    modify_datas(Rest, NewBasicData).

set_data(account, Account, BasicData)->
    data_helper:format("Account:~p, BasicData:~p~n", [Account, BasicData]),
    BasicData#player_basic_data{account=Account};
set_data(username, Username, BasicData)->
    BasicData#player_basic_data{username=Username};
set_data(sex, Sex, BasicData)->
    BasicData#player_basic_data{sex=Sex};
set_data(hair, Hair, BasicData)->
    BasicData#player_basic_data{hair=Hair};
set_data(face, Face, BasicData)->
    BasicData#player_basic_data{face=Face};
set_data(skin_color, SkinColor, BasicData)->
    BasicData#player_basic_data{skin_color=SkinColor};
set_data(hair_color, HairColor, BasicData)->
    BasicData#player_basic_data{hair_color=HairColor};
set_data(last_login_time, LastLoginTime, BasicData) ->
    BasicData#player_basic_data{last_login_time=LastLoginTime};
set_data(beard, Beard, BasicData)->
    BasicData#player_basic_data{beard=Beard}.
    
convert(#player_basic_data{account=Account, username=Username}=BasicData) ->
    NUsername =
	case Username of
	    [] -> atom_to_list(Account);
	    _ -> Username
	end,
	    
    BasicData#player_basic_data{username=NUsername}.
%% -------------------------------test--------------------------------
-ifdef(TEST).
-include_lib("eunit/include/eunit.hrl"). 
test()->
    ok.
-endif.




