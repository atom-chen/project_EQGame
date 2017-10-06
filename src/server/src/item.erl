%%%-------------------------------------------------------------------
%%% @author  <>
%%% @copyright (C) 2010, 
%%% @doc
%%%  物品一些通用的操作
%%% @end
%%% Created :  7 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(item).

-include("common_def.hrl").
-include("packet_def.hrl").
-include("tplt_def.hrl").
-include("sys_msg.hrl").
%% API
-export([make_item/1, make_item/2, new/2, new/3, get_tempid/1, get_instid/1]).
-export([get_empty/0, is_empty/1, set_item/2, get_item_name/1, can_trade/1]).
-export([set_del_time/2, has_del_time/1, cut_trade_time/1, get_del_time/1]).
-export([set_decoration_val/2, get_decoration_val/1]).

%%%===================================================================
%%% API
%%%===================================================================
%% 根据物品的模板ID生成一个物品
-spec make_item(pos_integer()) -> tuple().
make_item(ItemTempID) when is_integer(ItemTempID), ItemTempID > 0->
    new(ItemTempID, guid:make(?st_item)).
make_item(ItemTempID, DelTime) when is_integer(ItemTempID), ItemTempID > 0->
    new(ItemTempID, guid:make(?st_item), DelTime).

new(TemplateID, InstID)->
    new(TemplateID, InstID, init_del_time(TemplateID)).
new(TemplateID, InstID, DelTime)->
    #item{template_id = TemplateID, instance_id=InstID, 
	  del_time=DelTime, decoration_val=init_decoration_val(TemplateID)}.

%% 获得物品的ID
-spec get_tempid(tuple()) -> non_neg_integer().
get_tempid(#item{template_id=TempID}) ->
    TempID.

%% 获取物品实例Id
-spec get_instid(tuple()) -> non_neg_integer().
get_instid(#item{instance_id=InstID}) ->
    InstID.

%% 设置物品
-spec set_item(integer(), integer()) -> tuple().
set_item(InstId, TempId) when is_integer(InstId), is_integer(TempId) ->
    #item{template_id=TempId, instance_id=InstId}.
%%    new(TempId, InstId).

get_decoration_val(#item{decoration_val=Val}) ->
    Val.
set_decoration_val(Item, Val) ->
    Item#item{decoration_val=Val}.


init_decoration_val(TemplateID) ->
    #item_tplt{decoration=Decoration} = tplt:get_data2(item_tplt, TemplateID),
    Decoration.

%% 设置到期时间
set_del_time(Item, DelTime) ->
    Item#item{del_time=DelTime}.
%% 取到期时间
get_del_time(#item{del_time=DelTime}) ->
    DelTime;
get_del_time(ItemTemplateID) when is_integer(ItemTemplateID) ->
    TemplateID = ItemTemplateID,
    #item_tplt{effect_time_id=EffectTimeID} = tplt:get_data2(item_tplt, TemplateID),
    case EffectTimeID =/= 0 of
	true ->
	    #item_effect_time_tplt{type=Type,
			      effect_time=NTime,
			      del_time=YYYYMMDD}
		= tplt:get_data2(item_effect_time_tplt, EffectTimeID),
	    case Type of
		1 -> %% 1表示倒计时时效(单位:天)
		    NDay = NTime,
		    Now = datetime:local_time(),

		    %% 规定多少秒代表一天
		    %%ItemEffectTimeUnit = 24 * 60 * 60,
		    ItemEffectTimeUnit = common_def:get_val(seconds_of_a_day),
		    {{Y, M, D}, {H, N, S}} = datetime:add_time(Now, NDay * ItemEffectTimeUnit),
		    datetime:make_time(Y, M, D, H, N, S);
		2 -> %% 2表示时间段
		    Day = YYYYMMDD rem 100,
		    YYYYMM = YYYYMMDD div 100,
		    Month = YYYYMM rem 100,
		    Year = YYYYMM div 100,
		    datetime:make_time(Year, Month, Day, 0, 0, 0);
		3 -> %% 3表示倒计时时效(单位:分钟)
		    Now = datetime:local_time(),
		    NMinute = NTime,
		    {{Y, M, D}, {H, N, S}} = datetime:add_time(Now, NMinute * 60),
		    datetime:make_time(Y, M, D, H, N, S)
	    end;	
	_ ->
	    datetime:make_time(0, 0, 0, 0, 0, 0)
    end.

%% 获得空的物品
-spec get_empty() -> tuple().
get_empty() ->
    #item{template_id=0, instance_id=0}.

%% 物品是否为空
-spec is_empty(tuple()) -> true | false.
is_empty(#item{instance_id=0, template_id=0})->
    true;
is_empty(Item) when is_record(Item, item)->
    false;
is_empty(Item) ->
    erlang:error({badarg, Item}).

%% 初始化物品过期时间, 返回#stime
init_del_time(TemplateID) ->
    get_del_time(TemplateID).

%% 扣除转手时间(交易时，邮寄时)
cut_trade_time(Item) ->
    #item{template_id=TemplateID, 
	  del_time=DelTime} = Item,
    NewDelTime = do_cut_time(TemplateID, DelTime),
    Item#item{del_time=NewDelTime}.


%% 判断物品是否有时效
has_del_time(#item{del_time=DelTime}) ->
    has_del_time(DelTime);
has_del_time(#stime{year=0}) ->
    false;
has_del_time(#stime{}) ->
    true.

%% 取物品名称
get_item_name(#item{template_id=TpltID}) -> 
    get_item_name(TpltID);
get_item_name(ItemTpltID) when is_integer(ItemTpltID) -> 
    #item_tplt{name=ItemName} = tplt:get_data(item_tplt, ItemTpltID),
    binary_to_list(ItemName).

do_cut_time(TemplateID, DelTime) ->    
    #item_tplt{effect_time_id=EffectTimeID} = 
	tplt:get_data(item_tplt, TemplateID),

    case EffectTimeID =/= 0 of
	true ->
	    #item_effect_time_tplt{type=Type,
			      trade_cut_time=CutN}
		= tplt:get_data(item_effect_time_tplt, EffectTimeID),
	    case Type of
		1 -> %% 1表示倒计时时效(单位:天)
		    CutDay = CutN,
		    %%ItemEffectTimeUnit = 24 * 60 * 60,
		    ItemEffectTimeUnit = common_def:get_val(seconds_of_a_day),
		    datetime:add_time(DelTime, -CutDay * ItemEffectTimeUnit);
		2 -> %% 2表示时间段
		    DelTime;
		3 -> %% 3表示倒计时时效(单位：分钟)
		    CutMinute = CutN,
		    datetime:add_time(DelTime, -CutMinute * 60)
	    end;	
	_ ->
	    DelTime
    end.

%% 是否可交易
can_trade(Item) ->
    #item_tplt{bind=Bind} = tplt:get_data(item_tplt, item:get_tempid(Item)),	    
    case Bind =:= ?ITEM_CAN_TRADE of
	false -> {false, ?err_item_bind_can_not_trade}; %% 物品绑定不能交易 
	_ ->
	    case trade_cut_time_enough(Item) of
		false ->
		    {false, ?err_trade_cut_time_not_enough};
		_ ->
		    true
	    end
    end.



%% 是否足够扣除交易时间
trade_cut_time_enough(Item) ->
    %%ItemMinTradeSecond = ?ITEM_MIN_TRADE_SECOND,
    ItemMinTradeSecond = common_def:get_val(item_min_trade_second),
    %% 取物品交易有效期
    EffectTime = datetime:add_time(datetime:local_time(), 
				   ItemMinTradeSecond),
    
    case item:has_del_time(Item) of
	true -> %% 如果物品有时效
	    #stime{year=Y, month=M, day=D, 
		   hour=H, minute=N, second=S} = Item#item.del_time, 
	    DelTime = {{Y, M, D}, {H, N, S}},
	    DelTime > EffectTime;
	_ ->
	    true
    end.
