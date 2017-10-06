%%%-------------------------------------------------------------------
%%% @author  <>
%%% @copyright (C) 2010, 
%%% @doc
%%%  ��ƷһЩͨ�õĲ���
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
%% ������Ʒ��ģ��ID����һ����Ʒ
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

%% �����Ʒ��ID
-spec get_tempid(tuple()) -> non_neg_integer().
get_tempid(#item{template_id=TempID}) ->
    TempID.

%% ��ȡ��Ʒʵ��Id
-spec get_instid(tuple()) -> non_neg_integer().
get_instid(#item{instance_id=InstID}) ->
    InstID.

%% ������Ʒ
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

%% ���õ���ʱ��
set_del_time(Item, DelTime) ->
    Item#item{del_time=DelTime}.
%% ȡ����ʱ��
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
		1 -> %% 1��ʾ����ʱʱЧ(��λ:��)
		    NDay = NTime,
		    Now = datetime:local_time(),

		    %% �涨���������һ��
		    %%ItemEffectTimeUnit = 24 * 60 * 60,
		    ItemEffectTimeUnit = common_def:get_val(seconds_of_a_day),
		    {{Y, M, D}, {H, N, S}} = datetime:add_time(Now, NDay * ItemEffectTimeUnit),
		    datetime:make_time(Y, M, D, H, N, S);
		2 -> %% 2��ʾʱ���
		    Day = YYYYMMDD rem 100,
		    YYYYMM = YYYYMMDD div 100,
		    Month = YYYYMM rem 100,
		    Year = YYYYMM div 100,
		    datetime:make_time(Year, Month, Day, 0, 0, 0);
		3 -> %% 3��ʾ����ʱʱЧ(��λ:����)
		    Now = datetime:local_time(),
		    NMinute = NTime,
		    {{Y, M, D}, {H, N, S}} = datetime:add_time(Now, NMinute * 60),
		    datetime:make_time(Y, M, D, H, N, S)
	    end;	
	_ ->
	    datetime:make_time(0, 0, 0, 0, 0, 0)
    end.

%% ��ÿյ���Ʒ
-spec get_empty() -> tuple().
get_empty() ->
    #item{template_id=0, instance_id=0}.

%% ��Ʒ�Ƿ�Ϊ��
-spec is_empty(tuple()) -> true | false.
is_empty(#item{instance_id=0, template_id=0})->
    true;
is_empty(Item) when is_record(Item, item)->
    false;
is_empty(Item) ->
    erlang:error({badarg, Item}).

%% ��ʼ����Ʒ����ʱ��, ����#stime
init_del_time(TemplateID) ->
    get_del_time(TemplateID).

%% �۳�ת��ʱ��(����ʱ���ʼ�ʱ)
cut_trade_time(Item) ->
    #item{template_id=TemplateID, 
	  del_time=DelTime} = Item,
    NewDelTime = do_cut_time(TemplateID, DelTime),
    Item#item{del_time=NewDelTime}.


%% �ж���Ʒ�Ƿ���ʱЧ
has_del_time(#item{del_time=DelTime}) ->
    has_del_time(DelTime);
has_del_time(#stime{year=0}) ->
    false;
has_del_time(#stime{}) ->
    true.

%% ȡ��Ʒ����
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
		1 -> %% 1��ʾ����ʱʱЧ(��λ:��)
		    CutDay = CutN,
		    %%ItemEffectTimeUnit = 24 * 60 * 60,
		    ItemEffectTimeUnit = common_def:get_val(seconds_of_a_day),
		    datetime:add_time(DelTime, -CutDay * ItemEffectTimeUnit);
		2 -> %% 2��ʾʱ���
		    DelTime;
		3 -> %% 3��ʾ����ʱʱЧ(��λ������)
		    CutMinute = CutN,
		    datetime:add_time(DelTime, -CutMinute * 60)
	    end;	
	_ ->
	    DelTime
    end.

%% �Ƿ�ɽ���
can_trade(Item) ->
    #item_tplt{bind=Bind} = tplt:get_data(item_tplt, item:get_tempid(Item)),	    
    case Bind =:= ?ITEM_CAN_TRADE of
	false -> {false, ?err_item_bind_can_not_trade}; %% ��Ʒ�󶨲��ܽ��� 
	_ ->
	    case trade_cut_time_enough(Item) of
		false ->
		    {false, ?err_trade_cut_time_not_enough};
		_ ->
		    true
	    end
    end.



%% �Ƿ��㹻�۳�����ʱ��
trade_cut_time_enough(Item) ->
    %%ItemMinTradeSecond = ?ITEM_MIN_TRADE_SECOND,
    ItemMinTradeSecond = common_def:get_val(item_min_trade_second),
    %% ȡ��Ʒ������Ч��
    EffectTime = datetime:add_time(datetime:local_time(), 
				   ItemMinTradeSecond),
    
    case item:has_del_time(Item) of
	true -> %% �����Ʒ��ʱЧ
	    #stime{year=Y, month=M, day=D, 
		   hour=H, minute=N, second=S} = Item#item.del_time, 
	    DelTime = {{Y, M, D}, {H, N, S}},
	    DelTime > EffectTime;
	_ ->
	    true
    end.
