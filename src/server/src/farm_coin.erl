%%% @author lyj <lyj@35info.cn>
%%% @copyright (C) 2010, lyj
%%% @doc
%%% 农场金钱
%%% @end
%%% Created : 15 Nov 2010 by lyj <lyj@35info.cn>

-module(farm_coin).

-include("packet_def.hrl").
-include("common_def.hrl").


-export([add_coin/2, dec_coin/2, diff_coin/2, get_coin/1, set_coin/2]).
-export([send_coin/2, send_coin/3, send_eq_coin/1]).

%% 增加货币, 不发送给客户端, 不保存数据库
add_coin(AddMoney, TotalMoney) when is_integer(TotalMoney) ->
    util:middle(TotalMoney + AddMoney, 0, ?MAX_GAMECOIN);
add_coin(AddMoney, Farm) when is_tuple(Farm) ->
    NCoin = add_coin(AddMoney, player_farm:get_coin(Farm)),
    player_farm:set_coin(NCoin, Farm).

%% 减少货币, 不发送给客户端, 也不保存数据库
dec_coin(DecMoney, TotalMoney) when is_integer(TotalMoney) ->
    erlang:max(0, TotalMoney - DecMoney);
dec_coin(DecMoney, Farm) when is_tuple(Farm) ->
    NCoin = dec_coin(DecMoney, player_farm:get_coin(Farm)),
    player_farm:set_coin(NCoin, Farm).
    

send_coin(Account, Coin) when is_integer(Coin) ->
    send_coin(Account, 0, Coin);
send_coin(Account, Farm) when is_tuple(Farm)->
    send_coin(Account, 0, player_farm:get_coin(Farm)).

send_coin(Account, AddMoney, Money) ->
    net_helper:farm2client(Account, #notify_farm_money{account = Account, 
						       add_money = AddMoney,
						       money = Money}).

send_eq_coin(Account) when is_atom(Account) ->
    EQCoin = player_coin:get_coin(?EQCOIN, Account),
    net_helper:farm2client(Account, #notify_farm_eq_coin{eq_coin=EQCoin}).

%% 获得两个PlayerData中指定金币的差值
diff_coin(Farm1, Farm2) ->
    erlang:abs(player_farm:get_coin(Farm1) - player_farm:get_coin(Farm2)).

get_coin(Farm) when is_tuple(Farm) ->
    player_farm:get_coin(Farm).

set_coin(Coin, Farm) when is_tuple(Farm) ->
    player_farm:set_coin(Coin, Farm).
    

