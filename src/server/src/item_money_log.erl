%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  ��Ʒ,��Ǯ��¼
%%% @end
%%% Created : 17 Jan 2011 by hongjx <hongjx@35info.cn>
-module(item_money_log).

-export([build_record/3, build_record/4, build_record/5, build_record/6, get_money_info/2]). 
-export([log/4, log/5, log/6, log_money/3, log_item/3, log_money/4, log_item/4]).

%% ͳ���������
-export([gather/0]).

-record(item_money_log,
	{id, 
	 account,    %% ���
	 datetime,   %% ʱ��
	 action,     %% ���� buy_item | buy_house | trade_give | trade_get  
	 item_info,  %% ��Ʒ��Ϣ {ItemTempID, Count, InstanceID} | [{ItemTempID, Count, InstanceID}, ...]
	 money_info, %% ��Ǯ��Ϣ {GameMoney, EQMoney}
	 target     %% Ŀ����һ�npc 
	}
  ).


-record(sum_record, {income = money0(),      % ����
		     consume = money0(),     % ֧��
		     work_income = money0(), % ������
		     farm_income = money0(),  % ũ������
		     farm_consume = money0(), % ũ��֧��  
		     furniture_consume = money0(), % �Ҿ�֧��
		     house_consume = money0(),     % ������
		     dress_consume = money0(),     % ��װ֧��
		     script_consume = money0(),    % �ű�֧��(������)
		     dress_dict = dict:new(),      % �����װͳ�ƣ���������
		     furniture_dict = dict:new(),  % ����Ҿ�ͳ�ƣ���������
		     farm_item_dict = dict:new()   % ����ũ����Ʒͳ��, ��������
		    }).

-include("common_def.hrl").


%% д��Ǯʹ�ü�¼
log_money(Account, Action, MoneyInfo) -> 
    Target = undefined,
    log_money(Account, Action, MoneyInfo, Target).

log_money(Account, Action, MoneyInfo, Target) -> 
    ItemInfo = {0, 0, 0},
    log(Account, Action, ItemInfo, MoneyInfo, Target).


%% д��Ʒʹ�ü�¼
log_item(Account, Action, ItemInfo) ->
    Target = undefined,
    log_item(Account, Action, ItemInfo, Target).    

log_item(Account, Action, ItemInfo, Target) ->
    MoneyInfo = {0, 0},
    log(Account, Action, ItemInfo, MoneyInfo, Target).


%% д��Ʒ�ͽ�Ǯʹ�ü�¼
log(Account, Action, ItemInfo, MoneyInfo) ->
    Target = undefined,
    log(Account, Action, ItemInfo, MoneyInfo, Target). 
 
log(Account, Action, ItemInfo, MoneyInfo, Target) ->
    DateTime = datetime:local_time(),
    log(Account, Action, ItemInfo, MoneyInfo, Target, DateTime). 

log(Account, Action, ItemInfo, MoneyInfo, Target, DateTime) ->
    Record = build_record(Account, Action, ItemInfo, MoneyInfo, Target, DateTime), 
    
    db_common:write(Record).
 
%% ���ɼ�¼
build_record(Account, Action, ItemInfo) ->
    MoneyInfo = {0, 0},
    build_record(Account, Action, ItemInfo, MoneyInfo).

build_record(Account, Action, ItemInfo, MoneyInfo) ->
    Target = undefined,
    build_record(Account, Action, ItemInfo, MoneyInfo, Target).

build_record(Account, Action, ItemInfo, MoneyInfo, Target) ->
    DateTime = datetime:local_time(),
    build_record(Account, Action, ItemInfo, MoneyInfo, Target, DateTime).

build_record(Account, Action, ItemInfo, MoneyInfo, Target, DateTime) 
  when is_atom(Account),
       is_atom(Action) ->
    #item_money_log{id=guid:make(?st_item_money_log),
		    datetime=DateTime, 
		    account=Account,
		    action=Action,
		    item_info=ItemInfo,
		    money_info=MoneyInfo,
		    target=Target
		   }.


get_money_info(?GAMECOIN, N) ->
    {N, 0};
get_money_info(?EQCOIN, N) ->
    {0, N}.

money0() ->
    {0, 0}.



%%%===================================================================
%%% ͳ�ƺ���
%%%===================================================================

%% �ռ����� 
%% ���� {PlayerCount, {TopInfo, IncomeInfo, AvgIncome, ConsumeInfo, AvgConsume}} 
%%      or {0, undefined}
gather() -> 
    PlayerCount = get_player_count(),
    sum_item_money_log(PlayerCount).

get_player_count() ->
    db_common:transaction(fun() ->  mnesia:table_info(player_basic_data, size) end).

sum_item_money_log(0) -> 
    {0, undefined};
sum_item_money_log(PlayerCount) -> 
    Result = db_common:transaction(
	       fun() -> 
		       mnesia:foldl(fun process_record/2, #sum_record{}, item_money_log) 
	       end),

    #sum_record{income=Income,
		work_income=WorkIncome,
		farm_income=FarmIncome,
		consume=Consume,
		farm_consume=FarmConsume,
		furniture_consume=FurConsume,
		house_consume=HouseConsume,
		dress_consume=DressConsume,
		script_consume=ScriptConsume
	       } = Result,

    %% ����
    TopInfo = get_top(10, Result),
    %% ����
    IncomeInfo = {Income, WorkIncome, FarmIncome},
    AvgIncome = avg_money(IncomeInfo, PlayerCount),
    %% ֧��
    ConsumeInfo = {negative_money(Consume), 
		   negative_money(FarmConsume), 
		   negative_money(FurConsume),
		   negative_money(HouseConsume),
		   negative_money(DressConsume),
		   negative_money(ScriptConsume)
		   },
    AvgConsume = avg_money(ConsumeInfo, PlayerCount),
    
    {PlayerCount, {TopInfo, IncomeInfo, AvgIncome, ConsumeInfo, AvgConsume}}.


avg_money(Tuple, N) ->
    List = tuple_to_list(Tuple),
    NList = [{X div N, Y div N} || {X, Y} <- List],
    list_to_tuple(NList).

%% ͳ�Ƽ���

%% ��ͳ����ҳ�ʼ��Ʒ����Ǯ
process_record(#item_money_log{action=player_init_money}, Sum) ->
    Sum;
process_record(#item_money_log{action=player_init_items}, Sum) ->
    Sum;
process_record(#item_money_log{action=player_init_furnitures},Sum) ->
    Sum;
process_record(#item_money_log{action=farm_init}, Sum) ->
    Sum;
%% ��ͳ�ƽ���
process_record(#item_money_log{action=player_trade}, Sum) ->
    Sum;

process_record(#item_money_log{money_info={GameMoney, EQMoney}}=R, Sum) ->
    A = process_sum_game_money(GameMoney, Sum),
    B = process_sum_eq_money(EQMoney, A),
    C = process_work_income(R, B),
    D = process_buy_consume(R, C),
    E = process_farm_income(R, D),
    F = process_top(R, E),
    process_script_consume(R, F).


%% �������
process_top(#item_money_log{action=farm_buy_item,
				     item_info=ItemInfo}, 
		    #sum_record{farm_item_dict=Dict}=Sum) ->
    NDict = process_top_item(ItemInfo, Dict),
    %% ũ����Ʒͳ��
    Sum#sum_record{farm_item_dict=NDict};
process_top(#item_money_log{action=buy_item,
				     item_info=ItemInfo}, 
		    #sum_record{dress_dict=DressDict,
			       furniture_dict=FurDict}=Sum) ->
    {ItemID, _ItemCount, _InstID} = ItemInfo,

    case get_item_type(ItemID) of
	dress ->  %% ��װ
	    NDressDict = process_top_item(ItemInfo, DressDict),
	    Sum#sum_record{dress_dict=NDressDict};
	furniture -> %% �Ҿ�
	    NFurDict = process_top_item(ItemInfo, FurDict),
	    Sum#sum_record{furniture_dict=NFurDict};
	_ ->
	    Sum
    end;
process_top(#item_money_log{action=buy_furnitures,
				     item_info=ItemInfo}, 
		    #sum_record{furniture_dict=Dict}=Sum) ->

    NDict = process_top_item(ItemInfo, Dict),
    %% ��װ
    Sum#sum_record{furniture_dict=NDict};
process_top(_Log, Sum) ->
    Sum.

process_top_item([], Dict) ->
    Dict;
process_top_item([ItemInfo | Rest], Dict) ->
    NDict = process_top_item(ItemInfo, Dict),
    process_top_item(Rest, NDict);
process_top_item({ItemID, ItemCount, _InstID}, Dict) ->
    inc_dict_val(ItemID, ItemCount, Dict).

inc_dict_val(Key, N, Dict) ->
    NewVal = 
	case dict:find(Key, Dict) of
	    {ok, Val} -> Val + N;
	    _ -> N
	end,
    dict:store(Key, NewVal, Dict).	    


process_sum_game_money(GameMoney, #sum_record{income=Income, consume=Consume}=Sum) ->
    case GameMoney > 0 of
	true ->
	    NIncome = add_money(Income, {GameMoney, 0}),
	    Sum#sum_record{income=NIncome};
	_ ->
	    NConsume = add_money(Consume, {GameMoney, 0}),
	    Sum#sum_record{consume=NConsume}
    end.

process_sum_eq_money(EQMoney, #sum_record{income=Income, consume=Consume}=Sum) ->    
    case EQMoney > 0 of
	true ->
	    NIncome = add_money(Income, {0, EQMoney}),
	    Sum#sum_record{income=NIncome};
	_ ->
	    NConsume = add_money(Consume, {0, EQMoney}),
	    Sum#sum_record{consume=NConsume}
    end.

%% ͳ�ƴ�����
process_work_income(#item_money_log{action=work_gain, 
				    money_info=MoneyInfo}, 
	       #sum_record{work_income=WorkIncome}=Sum) ->
    Sum#sum_record{work_income=add_money(WorkIncome, MoneyInfo)};
process_work_income(_Log, Sum) ->
    Sum.

%% ͳ��ũ������
process_farm_income(#item_money_log{action=farm_sell_item, 
				    money_info=MoneyInfo}, 
	       #sum_record{farm_income=FarmIncome}=Sum) ->
    Sum#sum_record{farm_income=add_money(FarmIncome, MoneyInfo)};
process_farm_income(#item_money_log{action=farm_sell_items, 
				    money_info=MoneyInfo}, 
	       #sum_record{farm_income=FarmIncome}=Sum) ->
    Sum#sum_record{farm_income=add_money(FarmIncome, MoneyInfo)};
process_farm_income(_Log, Sum) ->
    Sum.

%% ͳ������
process_buy_consume(#item_money_log{action=farm_buy_item,
				     money_info=MoneyInfo}, 
		    #sum_record{farm_consume=FarmConsume}=Sum) ->
    %% ����ũ����Ʒ
    Sum#sum_record{farm_consume=add_money(FarmConsume, MoneyInfo)};
process_buy_consume(#item_money_log{action=buy_house,
				     money_info=MoneyInfo}, 
		    #sum_record{house_consume=HouseConsume}=Sum) ->
    %% ������
    Sum#sum_record{house_consume=add_money(HouseConsume, MoneyInfo)};
process_buy_consume(#item_money_log{action=buy_item,
				     item_info=ItemInfo,
				     money_info=MoneyInfo}, 
		    #sum_record{dress_consume=DressConsume, 
				furniture_consume=FurConsume}=Sum) ->
    ItemID = erlang:element(1, ItemInfo),
    case get_item_type(ItemID) of
	dress ->  %% ��װ����
	    Sum#sum_record{dress_consume=add_money(DressConsume, MoneyInfo)};
	furniture -> %% �Ҿ�����
	    Sum#sum_record{furniture_consume=add_money(FurConsume, MoneyInfo)};
	_ ->
	    Sum
    end;
process_buy_consume(#item_money_log{action=buy_furnitures,
				     money_info=MoneyInfo}, 
		    #sum_record{furniture_consume=FurConsume}=Sum) ->
    %% �Ҿ�����
    Sum#sum_record{furniture_consume=add_money(FurConsume, MoneyInfo)};
process_buy_consume(_Log, Sum) ->
    Sum.

%% �ű�֧��(������)
process_script_consume(#item_money_log{action=script_dec_money,
				     money_info=MoneyInfo}, 
		    #sum_record{script_consume=ScriptConsume}=Sum) ->
    Sum#sum_record{script_consume=add_money(ScriptConsume, MoneyInfo)};
process_script_consume(_Log, Sum) ->
    Sum.




add_money({A1, A2}, {B1, B2}) ->
    {A1 + B1, A2 + B2}.

negative_money({GameMoney, EQMoney}) ->
    {-GameMoney, -EQMoney}.
     

get_item_type(ItemID) when is_integer(ItemID) ->
    case ItemID < 10000 of
	true -> dress;
	_ ->
	    case (20000 =< ItemID) and (ItemID < 40000) of
		true -> furniture;
		_ ->
		    case (80000 =< ItemID) and (ItemID < 90000) of
			true -> farm_item;
			_ ->
		   	    logger:log(error, "unknow item type id:~p~n", [ItemID]), 
			    unknow
		    end
	    end
    end.

%% ���ط�װ���Ҿߣ�ũ����Ʒ
get_top(N, #sum_record{dress_dict=DressDict, 
		       furniture_dict=FurDict, 
		       farm_item_dict=FarmDict}) -> 	    
    TopDress = get_top_n(N, dict:to_list(DressDict)),
    TopFur = get_top_n(N, dict:to_list(FurDict)),
    TopFarm = get_top_n(N, dict:to_list(FarmDict)),

    {TopDress, TopFur, TopFarm}.
		
get_top_n(N, List) when is_list(List) ->
    NList = lists:keysort(2, List),
    Len = length(List),
    Ret = 
	case Len > N of
	    true ->
		{_, B} = lists:split(Len - N, NList),
		B;
	    _ ->
		NList
	end,
    lists:reverse(Ret).

	      
%%%===================================================================
%%% ���Ժ���
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

get_top_n_test() ->
    A = get_top_n(5, [{a, 3}, {b, 1}, {c, 5}]),
    ?assertEqual([{c, 5}, {a, 3}, {b, 1}], A),

    B = get_top_n(2, [{a, 3}, {b, 1}, {c, 5}]),
    ?assertEqual([{c, 5}, {a, 3}], B),


    C = get_top_n(2, []),
    ?assertEqual([], C),

    D = get_top_n(2, [{a, 3}, {b, 1}, {c, 5}, {d, 2}]),
    ?assertEqual([{c, 5}, {a, 3}], D),
   
    ok.

get_item_type_test() ->
    ?assertEqual(dress, get_item_type(123)),
    ?assertEqual(furniture, get_item_type(20000)),
    ?assertEqual(furniture, get_item_type(20010)),
    ?assertEqual(farm_item, get_item_type(80000)),
    ?assertEqual(farm_item, get_item_type(80010)),    
    ok.

avg_test() ->
    ?assertEqual({{1, 0}, {2, 0}, {0, 3}}, avg_money({{2, 0}, {4, 0}, {1, 6}}, 2)),
    ?assertEqual({{0, 1}, {2, 0}}, avg_money({{1, 3}, {4, 0}}, 2)),
    ok.

inc_dict_val1_test() ->
    Key = a,
    Val = 5,
    Dict = dict:store(a, 3, dict:new()),
    NDict = inc_dict_val(Key, Val, Dict),
    ?assertEqual(8, dict:fetch(a, NDict)),
    ok.

inc_dict_val2_test() ->
    Key = a,
    NDict = inc_dict_val(Key, 3, dict:new()),
    ?assertEqual(3, dict:fetch(a, NDict)),
    ok.


process_record1_test() ->
    Logs = [
	    build_record(hjx, buy_item, {15, 1, 0}, {-20, 0}),	    
	    build_record(abc, work_gain, {0, 0, 0}, {50, 0}),	    
	    build_record(abc, farm_sell_item, {1, -1, 0}, {150, 0}),	    
	    build_record(abc, farm_buy_item, {80002, 1, 0}, {-30, 0}),	    
	    build_record(hjx, buy_item, {20001, 2, 0}, {-300, 0}),	    
	    build_record(hjx, buy_furnitures, [{20002, 2, 0}], {-100, 0}),	    
	    build_record(hjx, buy_item, {13, 1, 0}, {-25, 0})	    
	   ],
    FSum = fun(R, Acc) -> process_record(R, Acc) end,
    A = lists:foldl(FSum, #sum_record{}, Logs),
    #sum_record{income=Income, 
		work_income=WorkIncome,
		consume=Consume,
		farm_income=FarmIncome,
		dress_consume=DressConsume,
		furniture_consume=FurConsume,
		dress_dict=DressDict,
		furniture_dict=FurDict,
		farm_consume=FarmConsume,
		farm_item_dict=FarmDict
	       } = A, 
    
    ?assertEqual({200, 0}, Income),
    ?assertEqual({-475, 0}, Consume),
    ?assertEqual({-45, 0}, DressConsume),
    ?assertEqual({-400, 0}, FurConsume),

    ?assertEqual({50, 0}, WorkIncome),
    ?assertEqual({150, 0}, FarmIncome),
    ?assertEqual({-30, 0}, FarmConsume),
    ?assertEqual(2, dict:size(DressDict)),
    ?assertEqual(1, dict:size(FarmDict)),
    ?assertEqual(2, dict:size(FurDict)),
    
    ok.



-endif.
    
