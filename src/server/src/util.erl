%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  一些杂项函数放在这个模块中
%%% @end
%%% Created : 24 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(util).

-include("packet_def.hrl").

-export([middle/3, replace/3, replace_str/3, array_index_of/2, array_count_if/2]).
-export([binary_to_integer/1, len/1, char_count/1, is_process_alive/1, is_farm_app/0]).
-export([rate_select_one/2, rate_select_one/3]).
-export([filter_user_name/1, simple_xor/1]).


%% 返回>= Min, <= Max的值
-spec middle(number(), number(), number()) -> number().
middle(Value, Min, _Max) when Value < Min ->
    Min;
middle(Value, _Min, Max) when Value > Max ->
    Max;
middle(Value, _Min, _Max) ->
    Value.



%% 替换数组里指定索引里的值
replace(L, N, New) ->
    replace(L, N, New, []).

replace([], _N, _New, NewList) ->
    lists:reverse(NewList);
replace([_|Rest], 1, New, NewList) ->
    replace(Rest, 0, New, [New | NewList]);
replace([H|Rest], N, New, NewList) ->
    replace(Rest, N-1, New, [H | NewList]).


%% 替换字符串(只替换一次)
replace_str(String, OldStr, NewStr) ->
    StartIdx = string:str(String, OldStr),
    case StartIdx > 0 of
	true ->
	    MidIdx = StartIdx + string:len(OldStr),
	    A = string:substr(String, 1, StartIdx - 1),
	    B = string:substr(String, MidIdx),
	    A ++ NewStr ++ B;
	_ ->
	    String
    end.
    

%% 查找符合条件的第一个元素的Index
%% 没找到返回-1
%% Fun(X) -> true | false.
-spec array_index_of(any(), any()) -> integer().
array_index_of(Fun, GridVec) ->
    array_index_of_1(Fun, GridVec, array:size(GridVec), 0).

%% 没找到返回-1
array_index_of_1(_Fun, _GridVec, ArraySize, ArraySize) ->
    -1;
array_index_of_1(Fun, GridVec, ArraySize, Index) ->
    Item = array:get(Index, GridVec),
    case Fun(Item) of
	true ->
	    Index;
	false ->
	    array_index_of_1(Fun, GridVec, ArraySize, Index + 1)
    end.
 
%% 统计符合条件的元素个数
%% Fun(X) -> true | false.
-spec array_count_if(any(), any()) -> integer().
array_count_if(Fun, GridVec) ->
    Count = array:size(GridVec),
    N = 0,
    array_count_if_1(Fun, GridVec, Count, N).

array_count_if_1(_Fun, _GridVec, 0, N) ->
    N;
array_count_if_1(Fun, GridVec, Count, N) ->
    Index = Count - 1,
    Item = array:get(Index, GridVec),
    case Fun(Item) of
	true ->
	    array_count_if_1(Fun, GridVec, Index, N + 1);
	false ->
	    array_count_if_1(Fun, GridVec, Index, N)
    end.

binary_to_integer(Content) ->
    list_to_integer(binary_to_list(Content)).

%% 计算字符串长度
-spec len(list()) -> integer().
len(Content) ->
    Binary = iolist_to_binary(Content),
    List = unicode:characters_to_list(Binary),
    erlang:length(List).


filter_user_name(UserName) when is_list(UserName) ->
    %% 取前8个汉字, 16个字符
    get_n_char(UserName, 16).


%% 取前面n个字符
%%  Content = utf8, 返回值也是utf8 
get_n_char(Content, N) when is_list(Content) ->
    case length(Content) > N of 
	true -> 
	    Binary = iolist_to_binary(Content),
	    %% 转成Unicode
	    List = unicode:characters_to_list(Binary),
	    
	    %% 取前面N个
	    F = fun(X, {Count, Acc}) -> 
			NewCount = 
			    case X > 255 of 
				true -> Count + 2;
				_ -> Count + 1
			    end,
			case NewCount > N of
			    true -> {Count, Acc};
			    _ -> {NewCount, [X | Acc]}
			end
		end,
	    {_Count, Str} = lists:foldl(F, {0, []}, List),
	    Bin = unicode:characters_to_binary(lists:reverse(Str), utf8),
	    binary_to_list(Bin);
	_ -> %% N个utf8包含的字符数，一定小于GBK 和 Unicode
	    Content
    end.


%% 计算字符个数(注:汉字占两个字符，字母占一个字符)
%%  Content = utf8 
-spec char_count(list()) -> integer().
char_count(Content) when is_list(Content) ->
    Binary = iolist_to_binary(Content),
    %% 转成Unicode
    List = unicode:characters_to_list(Binary),
    %% 数量加总
    F = fun(X, Acc) -> 
		case X > 255 of 
		    true -> Acc + 2;
		    _ -> Acc + 1
		end
	end,
    lists:foldl(F, 0, List).

is_process_alive(Name) ->
    case whereis(Name) of
	undefined ->
	    false;
	    %% {ResL, _BadNodes} = rpc:multicall(nodes(), erlang, whereis, [Name]),
	    %% is_process_alive_1(ResL);
	_ ->
	    true
    end.

%% 判断当前节点是否农场服务器
is_farm_app() ->
    case whereis(farmsvr_app) of
	undefined -> false;
	_ -> true
    end.

%% is_process_alive_1([]) ->
%%     false;
%% is_process_alive_1([Res|ResL]) ->
%%     case Res =:= undefined of
%% 	true ->
%% 	    is_process_alive_1(ResL);
%% 	_ ->
%% 	    true
%%     end.


%% 按机率多选一
rate_select_one(Hit, EventRates, Events) when is_integer(Hit),
					 is_list(EventRates), 
					 is_list(Events),
					 length(Events) =:= length(EventRates) ->
    
    F = fun(E, Acc) ->
		case Acc of
		    {not_find, N, Idx} ->
			case Hit =< N + E of
			    true -> Idx;
			    false -> {not_find, N + E, Idx + 1}
			end;
		    Idx ->
			Idx
		end
	end,

    Idx = lists:foldl(F, {not_find, 0, 1}, EventRates),
    case is_integer(Idx) of
	true ->
	    EventID = lists:nth(Idx, Events),
	    EventID;
	false ->
	    0
    end.

rate_select_one(EventRates, Events) ->
    TotalRate = 100,
    %% 命中点
    Hit = rand:uniform(TotalRate),
    rate_select_one(Hit, EventRates, Events).

simple_xor(Data) when is_list(Data) ->
    Key = "X_+#@!GRa^|E%D", %% 此密钥要与客户端保持一致
    AdjustKey = string:left(Key, length(Data), $ ),
    F = fun(X, Y) ->
		X bxor Y
	end,
    lists:zipwith(F, Data, AdjustKey).
    %%binary_to_list(crypto:exor(list_to_binary(Data), list_to_binary(AdjustKey))).


