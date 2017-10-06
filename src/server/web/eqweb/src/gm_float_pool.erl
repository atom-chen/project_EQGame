%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%   系统公告
%%% @end
%%% Created : 28 Jun 2011 by hongjx <hongjx@35info.cn>

-module(gm_float_pool).
-export([get_gm_bottle_count/2, add_gm_bottle/6, clear_all_gm_bottle/2]).




%% 投放官方瓶
add_gm_bottle(Req, HostName, BottleCount, GMName, Msg, RewardID) when is_integer(BottleCount),
						       is_list(GMName),
						       is_list(Msg),
						       is_integer(RewardID) ->
    Content = java_escape_to_utf8(Msg),
    GM = java_escape_to_utf8(GMName),
    case rpc:call(HostName, float_bottle_pool, add_gm_bottle, [BottleCount, GM, Content, RewardID]) of
	{badrpc, _Reason} ->
	    Req:respond({200, [], ["Error"]});
	_ ->
	    Req:respond({200, [{"Content-Type", "text/html; charset=gb2312"}], ["ok"]})
    end.

%% 取官方瓶子数
get_gm_bottle_count(Req, HostName) ->
    case rpc:call(HostName, float_bottle_pool, get_gm_bottle_count, []) of
	{badrpc, _Reason} ->
	    Req:respond({200, [], ["Error"]});
	{GMCount, PlayerCount} ->
	    Result = integer_to_list(GMCount) ++ "," ++ integer_to_list(PlayerCount),
	    Req:respond({200, [{"Content-Type", "text/html; charset=gb2312"}], [Result]})
    end.


%% 删除所有官方瓶
clear_all_gm_bottle(Req, HostName) ->
    case rpc:call(HostName, float_bottle_pool, clear_all_gm_bottle, []) of
	{badrpc, _Reason} ->
	    Req:respond({200, [], ["Error"]});
	_ ->
	    Req:respond({200, [{"Content-Type", "text/html; charset=gb2312"}], ["ok"]})
    end.



java_escape_to_utf8(Str) ->
    unicode_to_utf8(java_escape_to_unicode(Str)).

%% unicode字串-> utf8
unicode_to_utf8(Str) ->
    Bin = unicode:characters_to_binary(Str),
    binary_to_list(Bin).    

%% javaScipt escape字串 -> unicode字串
java_escape_to_unicode(Str) when is_list(Str) ->
    tokenize_string(Str, []).

tokenize_string([], Acc) ->
    lists:reverse(Acc);
tokenize_string([$%, $u, C3, C2, C1, C0 | Rest], Acc) ->
    C = dehex(C0) bor
        (dehex(C1) bsl 4) bor
        (dehex(C2) bsl 8) bor 
        (dehex(C3) bsl 12),
    tokenize_string(Rest, [C | Acc]);
tokenize_string([C | Rest], Acc) ->
    tokenize_string(Rest, [C | Acc]).

dehex(C) when C >= $0, C =< $9 ->
    C - $0;
dehex(C) when C >= $a, C =< $f ->
    C - $a + 10;
dehex(C) when C >= $A, C =< $F ->
    C - $A + 10.



