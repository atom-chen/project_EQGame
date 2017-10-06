%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 农场日志
%%% @end
%%% Created : 13 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_log).

-include("packet_def.hrl").
-include("common_def.hrl").
-include("player_data.hrl").
-include("router.hrl").
-include("enum_def.hrl").

%% API
-export([new/1, start/1, handle_cast/2]).
-export([write/4, write/6]).
-export([test/0]).

-define(MAXLOGQTY, 200). % 最大保留日志的数量
-define(MAXLOGDATE, 3 * 24 * 60 * 60).  % 最大保留日志的天数

new(Account) ->
    #player_farm_log{account=Account,
		     message_log=[],
		     gain_log=[],
		     consumption_log=[]}.

start(Account) ->
    [router:make_event_source(?msg_req_farm_log_list, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_clear_farm_log, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% Handle cast
%%%===================================================================
%% 请求获取日志列表
handle_cast({_Msg, #req_farm_log_list{type=?flt_help, account=Owner1}}, State)->
    Account = farm:get_account(State),
    Owner = list_to_atom(Owner1),
    PlayerFarmLog = select(Owner),
    NPlayerFarmLog = PlayerFarmLog#player_farm_log{gain_log=[], consumption_log=[]},
    net_helper:farm2client(Account, #notify_farm_log_list{log=NPlayerFarmLog}),
    {noreply, State};
%% 请求获取日志列表
handle_cast({_Msg, #req_farm_log_list{type=?flt_gain, account=Owner1}}, State)->
    Account = farm:get_account(State),
    Owner = list_to_atom(Owner1),
    PlayerFarmLog = select(Owner),
    NPlayerFarmLog = PlayerFarmLog#player_farm_log{message_log=[], consumption_log=[]},
    net_helper:farm2client(Account, #notify_farm_log_list{log=NPlayerFarmLog}),
    {noreply, State};
%% 请求获取日志列表
handle_cast({_Msg, #req_farm_log_list{type=?flt_consumption, account=Owner1}}, State)->
    Account = farm:get_account(State),
    Owner = list_to_atom(Owner1),
    case Account =:= Owner of
	true ->
	    PlayerFarmLog = select(Owner),
	    NPlayerFarmLog = PlayerFarmLog#player_farm_log{message_log=[], gain_log=[]},
	    net_helper:farm2client(Account, #notify_farm_log_list{log=NPlayerFarmLog});
	_ ->
	    ok
    end,
    {noreply, State};
%% 请求清空日志列表
handle_cast({_Msg, #req_clear_farm_log{}}, State) ->
    Account = farm:get_account(State),
    PlayerFarmLog = select(Account),
    NPlayerFarmLog = clear(PlayerFarmLog),
    db_common:write(NPlayerFarmLog),
    net_helper:farm2client(Account, #notify_farm_log_list{log=NPlayerFarmLog}),
    {noreply, State}.

%%%===================================================================
%%% API
%%%===================================================================

%% 农场帮助log
write(help, Owner, Guest, UserName) when is_atom(Owner), is_atom(Guest) ->
    case Owner =/= Guest of
	true ->
	    PlayerFarmLog = select(Owner),
	    MessageLog = PlayerFarmLog#player_farm_log.message_log,
	    NMessageLog = add_message_log(?flmt_help, Guest, UserName, MessageLog),
	    NNMessageLog = lists:sublist(NMessageLog, ?MAXLOGQTY),
	    NPlayerFarmLog = PlayerFarmLog#player_farm_log{message_log=NNMessageLog},
	    db_common:write(NPlayerFarmLog);
	_ ->
	    ok
    end;
%% 农场破坏log
write(demage, Owner, Guest, UserName) when is_atom(Owner), is_atom(Guest) ->
    case Owner =/= Guest of
	true ->
	    PlayerFarmLog = select(Owner),
	    MessageLog = PlayerFarmLog#player_farm_log.message_log,
	    NMessageLog = add_message_log(?flmt_demage, Guest, UserName, MessageLog),
	    NNMessageLog = lists:sublist(NMessageLog, ?MAXLOGQTY),
	    NPlayerFarmLog = PlayerFarmLog#player_farm_log{message_log=NNMessageLog},
	    db_common:write(NPlayerFarmLog);
	_ ->
	    ok
    end;
%% 农场收获log
write(gain, Guest, CropId, CropQuantity)
  when is_atom(Guest), is_integer(CropId), is_integer(CropQuantity) ->
    PlayerFarmLog = select(Guest),
    GainLog = PlayerFarmLog#player_farm_log.gain_log,
    NGainLog = 
	case lists:keyfind(CropId, 2, GainLog) of
	    false ->
		[#farm_log_gain{crop_id=CropId,
				crop_quantity=CropQuantity,
				gain_quantity=CropQuantity} | GainLog];
	    Data -> 
		NCropQuantity = CropQuantity + Data#farm_log_gain.crop_quantity,
		NGainQuantity = CropQuantity + Data#farm_log_gain.gain_quantity,
		NData = Data#farm_log_gain{crop_quantity=NCropQuantity,
					   gain_quantity=NGainQuantity},
		lists:keyreplace(CropId, 2, GainLog, NData)
	end,
    NPlayerFarmLog = PlayerFarmLog#player_farm_log{gain_log=NGainLog},
    db_common:write(NPlayerFarmLog);
%% 农场偷取Log, (保存在成果)
write(steal, Guest, CropId, CropQuantity) ->
    PlayerFarmLog = select(Guest),
    GainLog = PlayerFarmLog#player_farm_log.gain_log,
    NGainLog = 
	case lists:keyfind(CropId, 2, GainLog) of
	    false ->
		[#farm_log_gain{crop_id=CropId,
				crop_quantity=CropQuantity,
				steal_quantity=CropQuantity} | GainLog];
	    Data -> 
		NCropQuantity = CropQuantity + Data#farm_log_gain.crop_quantity,
		NStealQuantity = CropQuantity + Data#farm_log_gain.steal_quantity,
		NData = Data#farm_log_gain{crop_quantity=NCropQuantity,
					   steal_quantity=NStealQuantity},
		lists:keyreplace(CropId, 2, GainLog, NData)
	end,
    NPlayerFarmLog = PlayerFarmLog#player_farm_log{gain_log=NGainLog},
    db_common:write(NPlayerFarmLog).

%% 狗咬的log
write(bite, Owner, Guest, UserName, PetId, BiteCoin) ->
    PlayerFarmLog = select(Owner),
    MessageLog = PlayerFarmLog#player_farm_log.message_log,
    NMessageLog = add_message_log(?flmt_bite, Guest, BiteCoin, UserName, PetId, MessageLog),
    NNMessageLog = lists:sublist(NMessageLog, ?MAXLOGQTY),
    NPlayerFarmLog = PlayerFarmLog#player_farm_log{message_log=NNMessageLog},
    db_common:write(NPlayerFarmLog);
%% 农场摘取log
write(extract, Owner, Guest, CropId, CropQuantity, UserName)
  when is_atom(Owner), is_atom(Guest), is_integer(CropId), is_integer(CropQuantity) ->
    case Owner =/= Guest of
	true ->
	    write(steal, Owner, Guest, CropId, CropQuantity, UserName),
	    write(steal, Guest, CropId, CropQuantity);
	_ -> 
	    write(gain, Guest, CropId, CropQuantity)
    end;
%% 农场偷取log
write(steal, Owner, Guest, CropId, CropQuantity, UserName)
  when is_atom(Owner), is_atom(Guest), is_integer(CropId), is_integer(CropQuantity) ->
    PlayerFarmLog = select(Owner),
    MessageLog = PlayerFarmLog#player_farm_log.message_log,
    NMessageLog = add_message_log(?flmt_extract, Guest, UserName, CropId, CropQuantity, MessageLog),
    NNMessageLog = lists:sublist(NMessageLog, ?MAXLOGQTY),
    NPlayerFarmLog = PlayerFarmLog#player_farm_log{message_log=NNMessageLog},
    db_common:write(NPlayerFarmLog);
%% 农场消费log
%% 买物品
write(buy, Owner, CropId, CropQuantity, UseCoin, RestCoin)
  when is_atom(Owner), is_integer(CropId), is_integer(CropQuantity), is_integer(UseCoin), is_integer(RestCoin) ->
    consumption(?flct_buy, Owner, CropId, CropQuantity, UseCoin, RestCoin);
%% 卖物品
write(sell, Owner, CropId, CropQuantity, UseCoin, RestCoin)
  when is_atom(Owner), is_integer(CropId), is_integer(CropQuantity), is_integer(UseCoin), is_integer(RestCoin)->
    consumption(?flct_sell, Owner, CropId, CropQuantity, UseCoin, RestCoin).

%%%===================================================================
%%% Internal functions
%%%===================================================================

%% 增加消息日志
-spec add_message_log(integer(), atom(), atom(), list()) -> list().
add_message_log(Type, Account, UserName, MessageLog) ->
    Now = datetime:localtime(),
    case is_exist(Account, Type, Now, MessageLog) of
	true ->
	    MessageLog;
	false ->
	    [#farm_log_message{log_type=Type, account=Account, log_date=Now, user_name=UserName}|MessageLog]
    end.

%% 增加被狗咬的日志
add_message_log(?flmt_bite, Account, BiteCoin, UserName, PetId, MessageLog) ->
    Now = datetime:localtime(),
    case lists:keyfind(?flmt_bite, 2, MessageLog) of
	false ->
	    [#farm_log_message{log_type=?flmt_bite, account=Account, log_date=Now,
			       user_name=UserName, pet_id=PetId, bite_coin=BiteCoin}|MessageLog];
	Log ->
	    case datetime:is_equal(Now, Log#farm_log_message.log_date) and (Account == Log#farm_log_message.account) of
		true ->
		    OBiteCoin = Log#farm_log_message.bite_coin,
		    NBiteCoin = OBiteCoin + BiteCoin,
		    NLog = Log#farm_log_message{bite_coin=NBiteCoin},
		    lists:keyreplace(?flmt_bite, 2, MessageLog, NLog);
		false ->
		    [#farm_log_message{log_type=?flmt_bite, account=Account, log_date=Now, 
				       user_name=UserName, pet_id=PetId, bite_coin=BiteCoin}|MessageLog]
	    end
    end;
%% 增加消息日志
add_message_log(Type, Account, UserName, CropId, CropQuantity, MessageLog) ->
    Now = datetime:localtime(),
    add_message_log(Account, UserName, Type, Now, CropId, CropQuantity, MessageLog, []).
%% 增加消息日志
-spec add_message_log(atom(), atom(), integer(), tuple(), integer(), integer(), list(), list()) -> list().
add_message_log(Account, UserName, Type, Now, CropId, CropQuantity, [], []) ->
    [#farm_log_message{account=Account, log_type=Type, log_date=Now, 
		       user_name=UserName, crop_id=CropId, crop_quantity=CropQuantity}];
add_message_log(_Account, _UserName, _Type, _Now, _CropId, _CropQuantity, [], NMessageLog) ->
    NMessageLog;
add_message_log(Account, UserName, Type, Now, CropId, CropQuantity,
		[#farm_log_message{log_type=LogType, log_date=LogDate, account=LogAccount, crop_id=LogCropId}=Log],
		NMessageLog) ->
    IsConsistent = is_consistent(Account, LogAccount, Type, LogType, CropId, LogCropId, Now, LogDate),
    case IsConsistent of
	true ->
	    NNMessageLog = update_message_log(CropQuantity, Log, NMessageLog),
	    add_message_log(Account, UserName, Type, Now, CropId, CropQuantity, [], lists:reverse(NNMessageLog));
	false ->
	    NNMessageLog = lists:reverse([Log| NMessageLog]),
	    NNNMessageLog = [#farm_log_message{account=Account, log_type=Type, log_date=Now, 
					       user_name=UserName, crop_id=CropId, crop_quantity=CropQuantity}
			     |NNMessageLog],
	    add_message_log(Account, UserName, Type, Now, CropId, CropQuantity, [], NNNMessageLog)
    end;
add_message_log(Account, UserName, Type, Now, CropId, CropQuantity,
		     [#farm_log_message{log_type=LogType, log_date=LogDate, 
					account=LogAccount, crop_id=LogCropId}=Log|MessageLog], NMessageLog) ->
    IsConsistent = is_consistent(Account, LogAccount, Type, LogType, CropId, LogCropId, Now, LogDate),
    case IsConsistent of
	true ->
	    NNMessageLog = update_message_log(CropQuantity, Log, NMessageLog),
	    add_message_log(Account, UserName, Type, Now, CropId, CropQuantity, [], lists:reverse(lists:reverse(MessageLog) ++ NNMessageLog));
	false ->
	    add_message_log(Account, UserName, Type, Now, CropId, CropQuantity, MessageLog, [Log|NMessageLog])
    end.

%% 更新消息日志
-spec update_message_log(integer(), tuple(), list()) -> list().
update_message_log(CropQuantity, Log, NMessageLog) ->
    LogCropQuantity = Log#farm_log_message.crop_quantity,
    NCropQuantity = LogCropQuantity + CropQuantity,
    [Log#farm_log_message{crop_quantity=NCropQuantity}|NMessageLog].

%% 判断消息是否一致
-spec is_consistent(atom(), atom(), integer(), integer(), integer(), integer(), tuple(), tuple()) -> atom().
is_consistent(Account, LogAccount, Type, LogType, CropId, LogCropId, Now, LogDate) ->
    (Account == LogAccount) and (Type == LogType) and (CropId == LogCropId) and (datetime:is_equal(Now, LogDate)).

%% 是否存在消息日志
-spec is_exist(atom(), integer(), tuple(), list()) -> atom().
is_exist(_Account, _Type, _Now, []) ->
    false;
is_exist(Account, Type, Now, 
		     [#farm_log_message{log_type=LogType, log_date=LogDate, account=LogAccount}|MessageLog]) ->
    case (Account == LogAccount) and (Type == LogType) and (datetime:is_equal(Now, LogDate)) of
	true ->
	    true;
	false ->
	    is_exist(Account, Type, Now, MessageLog)
    end.

%% 农场消费
consumption(Type, Owner, CropId, CropQuantity, UseCoin, RestCoin)
  when is_integer(Type), is_atom(Owner), is_integer(CropId), is_integer(CropQuantity), is_integer(UseCoin)->
    PlayerFarmLog = select(Owner),
    ConsumptionLog = PlayerFarmLog#player_farm_log.consumption_log,
    NConsumptionLog = [#farm_log_consumption{type=Type,
					     log_date=datetime:localtime(),
					     crop_id=CropId,
					     crop_quantity=CropQuantity,
					     use_coin=UseCoin,
					     leave_coin=RestCoin}|ConsumptionLog],
    NNConsumptionLog = lists:sublist(NConsumptionLog, ?MAXLOGQTY),
    NPlayerFarmLog = PlayerFarmLog#player_farm_log{consumption_log=NNConsumptionLog},
    db_common:write(NPlayerFarmLog).



%% 获取日志
-spec select(atom()) -> tuple().
select(Account) when is_atom(Account) ->
    db_common:select_one(player_farm_log, Account).

%% 清空日志
-spec clear(tuple()) -> tuple().
clear(PlayerFarmLog) when is_tuple(PlayerFarmLog) ->
    PlayerFarmLog#player_farm_log{message_log=[]}.
%%%===================================================================
%%% Test case
%%%===================================================================
test() ->
    test_clear(),
    test_write(),
    test_get_farm_log_list(),
    ok.

test_write() ->
    write(help, test1, test2, test2),
    write(demage, test1, test2, test2),
    write(extract, test1, test2, 1, 2, test2),
    write(extract, test1, test1, 1, 2, test2),
    write(gain, test1, 1, 2),
    write(gain, test1, 1, 3),
    write(buy, test1, 1, 2, 10, 1000),
    write(sell, test1, 1, 2, 10, 1000),
    ok.

test_clear() ->
    router:cast(test1, ?msg_req_clear_farm_log, #req_clear_farm_log{}).

test_get_farm_log_list() ->
    router:cast(test1, ?msg_req_farm_log_list, #req_farm_log_list{type=?flt_help, account="test1"}),
    router:cast(test1, ?msg_req_farm_log_list, #req_farm_log_list{type=?flt_gain, account="test1"}),
    router:cast(test1, ?msg_req_farm_log_list, #req_farm_log_list{type=?flt_consumption, account="test1"}).
