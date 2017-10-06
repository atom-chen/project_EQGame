%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  
%%% @end
%%% Created : 27 Jul 2011 by hongjx <hongjx@35info.cn>

-module(db_update_item_money_log).

-include("common_def.hrl").

-export([convert_bag_to_set/0, get_records/0]).

%% 旧表结构
%% -record(old_item_money_log,
%% 	{account,    %% 玩家
%% 	 datetime,   %% 时间
%% 	 action,     %% 动作 buy_item | buy_house | trade_give | trade_get  
%% 	 item_info,  %% 物品信息 {ItemTempID, Count, InstanceID} | [{ItemTempID, Count, InstanceID}, ...]
%% 	 money_info, %% 金钱信息 {GameMoney, EQMoney}
%% 	 target,     %% 目标玩家或npc 
%% 	 tag_id      %% 用于区分相同记录 
%% 	}
%%   ).

%% 新表结构
%% -record(item_money_log,
%% 	{
%% 	  id,
%% 	  account,    %% 玩家
%% 	  datetime,   %% 时间
%% 	  action,     %% 动作 buy_item | buy_house | trade_give | trade_get  
%% 	  item_info,  %% 物品信息 {ItemTempID, Count, InstanceID} | [{ItemTempID, Count, InstanceID}, ...]
%% 	  money_info, %% 金钱信息 {GameMoney, EQMoney}
%% 	  target      %% 目标玩家或npc 
%% 	}
%%   ).


%% 将表改为set类型(bag类型表存在bug: 调用mnesia:all_keys时可能导致当机)
convert_bag_to_set() ->
    %% 读取旧表数据
    Records = get_records(),
    %% 删除表格
    mnesia:delete_table(item_money_log),
    %% 重建表格
    {atomic, ok} = mnesia:create_table(item_money_log, 
			[{attributes, [id, account, datetime, action, item_info, money_info, target]},
			 {disc_only_copies, [node()]}]),
    case length(Records) > 0 of
	true ->
	    %% 转化记录
	    NewRecords = [begin
			      List = tuple_to_list(X),
			      %% 去掉tag_id 和 头部
			      SubList = lists:sublist(List, 2, length(List) - 2),
			      ID = guid:make(?st_item_money_log),
			      %% 加上id 和头部
			      NewList = [item_money_log, ID | SubList],
			      list_to_tuple(NewList)
			  end || X <- Records],
	    %% 写入新表    
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
    
