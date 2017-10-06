%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@china-channel.com>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% 玩家
%%% @end 
%%% Created :  6 Aug 2010 by linyibin <linyb@china-channel.com>
%%%-------------------------------------------------------------------
-module(player).

%% API
-export([search/3, add_item/4, add_coin/4, add_property/8, add_farm_exp/4, edit_gm_level/3]).

%%%===================================================================
%%% API
%%%===================================================================
%% 查询玩家
search(Account, Req, HostName) ->
    AccountAtom = list_to_atom(Account),
    SearchResult = rpc:call(HostName, gm_tool, search_player, [AccountAtom]),
    case SearchResult of
	{SAccount, SUserName, SGameCoin, SEQCoin, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, HouseClean} ->
	    IsOnline = rpc:call(HostName, gm_tool, is_online, [AccountAtom]),
	    Req:respond({200, [{"Content-Type", "text/html; charset=utf-8"}], 
			 [create(HostName, SAccount, SUserName, SGameCoin, SEQCoin, IsOnline, trunc(PlayerClean), trunc(PlayerHealth), trunc(PlayerCharm), trunc(ActiveValue), trunc(HouseClean))]});
	[] ->
	    Req:respond({200, [], ["fail"]})
    end.

%% 增加玩家物品
add_item(Account, Req, HostName, ItemId) ->
    InstId = guid:make(2),
    Result = rpc:call(HostName, gm_tool, add_item, [Account, ItemId, InstId]),
    case Result of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["doesn't exist node!"]});
	{badrpc, Reason} ->
	    Req:respond({200, [], [Reason]});
	{fail, Reason} ->
	    Req:respond({200, [], [Reason]});
	_ ->
	    Req:respond({200, [], ["ok"]})
    end.

%% 增加玩家金币
add_coin(Account, Req, HostName, Coin) ->
    Result = rpc:call(HostName, gm_tool, add_coin, [Account, Coin]),
    case Result of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["doesn't exist node!"]});
	{badrpc, Reason} ->
	    Req:respond({200, [], [Reason]});
	_ ->
	    Req:respond({200, [], ["ok"]})
    end.

add_property(Req, HostName, Account, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, HouseClean) ->
    Result = rpc:call(HostName, gm_tool, add_property, [Account, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, HouseClean]),
    case Result of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["doesn't exist node!"]});
	{badrpc, Reason} ->
	    Req:respond({200, [], [Reason]});
	_ ->
	    Req:respond({200, [], ["ok"]})
    end.

%% 增加玩家属性
add_farm_exp(Account, Req, HostName, Exp) ->
    Result = rpc:call(HostName, gm_tool, add_farm_exp, [Account, Exp]),
    case Result of
	{badrpc, nodedown} ->
	    Req:respond({200, [], ["doesn't exist node!"]});
	{badrpc, Reason} ->
	    Req:respond({200, [], [Reason]});
	_ ->
	    Req:respond({200, [], ["ok"]})
    end.


%% 设置GM权限
edit_gm_level(HostName, Account, Level)->
    rpc:call(HostName, gm_tool, edit_gm_level, [Account, Level]).

%%%===================================================================
%%% Internal functions
%%%===================================================================
create(HostName, Account, UserName, GameCoin, EQCoin, IsOnline, PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, HouseClean) ->
    "<tr id=\"player_row\" style=\"background-color:#E7E8EB\">" ++ 
	"<td>" ++ atom_to_list(Account) ++ "</td>" ++
	"<td>" ++ UserName ++ "</td>" ++
	"<td>" ++ integer_to_list(GameCoin) ++ "</td>" ++
	"<td>" ++ integer_to_list(EQCoin) ++ "</td>" ++
	"<td>" ++ atom_to_list(IsOnline) ++ "</td>" ++
	"<td>" ++ 
	    "人物清洁度：" ++ integer_to_list(PlayerClean) ++ "</br>" ++
            "人物健康值：" ++ integer_to_list(PlayerHealth) ++ "</br>" ++
            "人物魅力值：" ++ integer_to_list(PlayerCharm) ++ "</br>" ++
            "活　力　值：" ++ integer_to_list(ActiveValue) ++ "</br>" ++
            "房屋清洁度：" ++ integer_to_list(HouseClean) ++ "</br>" ++
	"</td>" ++
	create_1(IsOnline, HostName, Account) ++
    "</tr>".

create_1(IsOnline, HostName, Account) ->
    case IsOnline of
	true ->
	    "<td>" ++ 
		"<a href=\"#\" disabled=\"disabled\">增加物品</a></br>" ++
		"<a href=\"#\" disabled=\"disabled\">增加游戏币</a></br>" ++
		"<a href=\"#\" disabled=\"disabled\">增加属性值</a></br>" ++
		"<a href=\"#\" disabled=\"disabled\">设置GM命令权限</a></br>" ++	
	    "</td>";
	_ ->
	    "<td>" ++ 
		"<a href=\"#\" onclick=\"show_add_item('"++ atom_to_list(HostName) ++"', '"++ atom_to_list(Account) ++"')\">增加物品</a></br>" ++
		"<a href=\"#\" onclick=\"show_add_coin('"++ atom_to_list(HostName) ++"', '"++ atom_to_list(Account) ++"')\">增加游戏币</a></br>" ++
		"<a href=\"#\" onclick=\"show_add_property('"++ atom_to_list(HostName) ++"', '"++ atom_to_list(Account) ++"')\">增加属性值</a></br>" ++
		"<a href=\"#\" onclick=\"show_edit_gm_level('"++ atom_to_list(HostName) ++"', '"++ atom_to_list(Account) ++"')\">设置GM命令权限</a></br>" ++
		"</td>"	
    end.
