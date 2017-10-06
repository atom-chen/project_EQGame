-module(player_weibo).

-include("packet_def.hrl").
-include("router.hrl").
-include("enum_def.hrl").

-export([start/1]).
-export([handle_cast/2]).

-record(player_weibo, {account = "",
		       tx_access_key = "", 
		       tx_access_secret = "", 
		       sina_access_key = "", 
		       sina_access_secret = ""}).


start(Account) -> 
    [
     router:make_event_source(?msg_req_get_weibo_access_secret, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_save_access_secret, Account, {self(), ?MODULE})
    ].

%%--------------------------------------------------------------------
handle_cast({#msg{src=Account}, #req_get_weibo_access_secret{}}, State) ->
    Packet = case db_weibo:get(Account) of
		 [] ->
		     #notify_weibo_access_secret{
		   tx_access_key="",
		   tx_access_secret="",
		   sina_access_key="",
		   sina_access_secret=""};
		 Data ->
		     #notify_weibo_access_secret{
		   tx_access_key=Data#player_weibo.tx_access_key,
		   tx_access_secret=Data#player_weibo.tx_access_secret,
		   sina_access_key=Data#player_weibo.sina_access_key,
		   sina_access_secret=Data#player_weibo.sina_access_secret}
	     end,
    net_helper:send2client(Account, Packet),
    {noreply, State};
handle_cast({#msg{src=Account}, #req_save_access_secret{tx_access_key=TXKey, 
							tx_access_secret=TXSecret,
							sina_access_key=SinaKey,
							sina_access_secret=SinaSecret}}, State) ->
    db_weibo:write(#player_weibo{account = Account,
				 tx_access_key=TXKey,
				 tx_access_secret=TXSecret,
				 sina_access_key=SinaKey,
				 sina_access_secret=SinaSecret}),
    {noreply, State}.
