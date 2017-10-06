%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   ��װֵ
%%%     ע: ��Ҫ�õ����ֵ�ĵط��Ƚ��٣�����ֱ�������ݿ��ȡ
%%% @end
%%% Created : 20 Sep 2011 by hongjx <hongjx@35info.cn>

-module(player_decoration).

-export([get_decoration/1, add_decoration/2, notify_decoration/2, notify_decoration/1]).

-include("packet_def.hrl").

-record(player_decoration, {
	  account,
	  decoration = 0
	  }).

notify_decoration(Account) ->
    notify_decoration(Account, Account).
notify_decoration(Account, HouseOwner) ->
    Val = get_decoration(HouseOwner),
    %% ֪ͨ�ͻ���
    net_helper:send2client(Account, 
			   #notify_house_decoration{
			     account=atom_to_list(HouseOwner),
			     decoration=Val}).
    

get_decoration(Account) ->
    case db_common:select_one(player_decoration, Account) of
	#player_decoration{decoration=Val} ->
	    Val;
	_ -> 0
    end.

add_decoration(_Account, 0) ->
    false;
add_decoration(Account, N) when N > 0 ->
    %% ��������
    F = fun(R) -> 
		OldVal = R#player_decoration.decoration,
		R#player_decoration{decoration=OldVal + N}
	end,

    Key = Account,
    case db_common:modify(player_decoration, Key, F, 
			  #player_decoration{account=Key}) of
	{ok, #player_decoration{decoration=NewCount}} ->
	    %% ֪ͨ�ͻ���
	    net_helper:send2client(Account, 
				   #notify_house_decoration{
				     account=atom_to_list(Account),
				     decoration=NewCount})
    end,

    true.
    



