%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  
%%% @end
%%% Created : 27 Jul 2011 by hongjx <hongjx@35info.cn>

-module(db_update_item_money_log).

-include("common_def.hrl").

-export([convert_bag_to_set/0, get_records/0]).

%% �ɱ�ṹ
%% -record(old_item_money_log,
%% 	{account,    %% ���
%% 	 datetime,   %% ʱ��
%% 	 action,     %% ���� buy_item | buy_house | trade_give | trade_get  
%% 	 item_info,  %% ��Ʒ��Ϣ {ItemTempID, Count, InstanceID} | [{ItemTempID, Count, InstanceID}, ...]
%% 	 money_info, %% ��Ǯ��Ϣ {GameMoney, EQMoney}
%% 	 target,     %% Ŀ����һ�npc 
%% 	 tag_id      %% ����������ͬ��¼ 
%% 	}
%%   ).

%% �±�ṹ
%% -record(item_money_log,
%% 	{
%% 	  id,
%% 	  account,    %% ���
%% 	  datetime,   %% ʱ��
%% 	  action,     %% ���� buy_item | buy_house | trade_give | trade_get  
%% 	  item_info,  %% ��Ʒ��Ϣ {ItemTempID, Count, InstanceID} | [{ItemTempID, Count, InstanceID}, ...]
%% 	  money_info, %% ��Ǯ��Ϣ {GameMoney, EQMoney}
%% 	  target      %% Ŀ����һ�npc 
%% 	}
%%   ).


%% �����Ϊset����(bag���ͱ����bug: ����mnesia:all_keysʱ���ܵ��µ���)
convert_bag_to_set() ->
    %% ��ȡ�ɱ�����
    Records = get_records(),
    %% ɾ�����
    mnesia:delete_table(item_money_log),
    %% �ؽ����
    {atomic, ok} = mnesia:create_table(item_money_log, 
			[{attributes, [id, account, datetime, action, item_info, money_info, target]},
			 {disc_only_copies, [node()]}]),
    case length(Records) > 0 of
	true ->
	    %% ת����¼
	    NewRecords = [begin
			      List = tuple_to_list(X),
			      %% ȥ��tag_id �� ͷ��
			      SubList = lists:sublist(List, 2, length(List) - 2),
			      ID = guid:make(?st_item_money_log),
			      %% ����id ��ͷ��
			      NewList = [item_money_log, ID | SubList],
			      list_to_tuple(NewList)
			  end || X <- Records],
	    %% д���±�    
	    db_common:write(NewRecords);
	_ -> 
	    ok
    end,

    {atomic, ok}.

get_records() ->
    db_common:transaction(
      fun() -> 
	      mnesia:foldl(fun(R, Acc) -> [R | Acc] end, [], item_money_log) 
      end).
    
