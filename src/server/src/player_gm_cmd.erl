%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  gmָ��Ĵ���ģ��
%%% @end
%%% Created : 24 May 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_gm_cmd).

-include("common_def.hrl").
-include("router.hrl").
-include("packet_def.hrl").
-include("enum_def.hrl").
-include("player_data.hrl").

-export([handle_cast/2]).

%%------------handle_call--------------------------------------
handle_cast({#msg{src=Account}, #req_gm_command{type=Type,params=Params}}, State)->
    #player_data{basic_data=BasicData}=State,
    #player_basic_data{gm_enabled=GMEnabled}=BasicData,
    case GMEnabled of
	?gm_enable ->
	    NewState = 
		case Type of
		    ?gct_add_item -> 
			do_get_item_cmd(Account, Params, State);
		    ?gct_add_farm_item ->
			do_add_farm_item_cmd(Account, Params, State);
		    ?gct_get_money ->
			do_get_money(Account, Params, State);
		    ?gct_send_sys_mail ->
			do_send_sys_mail(Account, Params, State);
		    ?gct_send_gm_mail ->
			do_send_gm_mail(Account, Params, State);
		    ?gct_send_n_mail ->
			do_send_n_mail(Account, Params, State);
		    ?gct_replace_tex -> 
			do_replace_tex(Account, Params, State)
		end,
	    {noreply, NewState};
	?gm_disable ->
	    {noreply, State}
    end.

%% �����Ʒ
do_get_item_cmd(Account, [ItemTempID], State)->
    Item = item:make_item(list_to_integer(ItemTempID)),    
    case player_pack:add_item2(Account, Item, State) of
	{ok, NState} -> NState;
	_ -> State
    end.

%% ����ũ���ֿ���Ʒ
do_add_farm_item_cmd(Account, [ItemTempID], State)->
    Item = item:make_item(list_to_integer(ItemTempID)),   
    NDepot = farm_depot:add_item(Account, Item),
    farm_depot:save(NDepot),
    farm_depot:send_depot(Account, NDepot),
    State;
do_add_farm_item_cmd(_Account, Params, _State) ->
    erlang:error({bagarg, Params}).

%% ��ý�Ǯ
do_get_money(Account, [Type, Coin], State)->
    NState = player_coin:add_coin2(list_to_integer(Type), 
				       list_to_integer(Coin), 
				       State),
    PlayerCoin = player_data:get_coin(NState),
    db_memdisc:write(PlayerCoin),
    net_helper:send2client(Account, #notify_update_coin{data=PlayerCoin}),
    NState.

%% ��ǽֽ���ذ����� 
do_replace_tex(Account, [RoomID, Type, Tex]=_Params, State)->
    SceneName = player_data:get_scene_name(State),
    router:send(SceneName, replace_room_tex, {Account,
					      list_to_integer(RoomID),
					      list_to_integer(Type),
					      Tex}),
    State. 


%% ��ϵͳ�ʼ� 
do_send_sys_mail(_Account, [Param], State) ->
    [Recvers | Rest] = string:tokens(Param, " "),
    RecverList = [db_player:name_to_account(X) || X <- string:tokens(Recvers, ", ")],

    case Rest of
	[] ->
	    player_mail:send_sys_mail(RecverList, [], [], []),
	    ok;
	[Title] ->
	    player_mail:send_sys_mail(RecverList, Title, [], []),
	    ok;
	[Title, Content] ->
	    player_mail:send_sys_mail(RecverList, Title, Content, []),
	    ok;
	[Title, Content, Items] ->
	    MailItems = [#pack_grid{count=1, item_data=item:make_item(list_to_integer(X))} 
			 || X <- string:tokens(Items, ", ")],
	    
	    player_mail:send_sys_mail(RecverList, Title, Content, MailItems),
	    ok;
	_ ->
	    ok
    end,    

    State.
 
%% ��GMϵͳ�ʼ� 
do_send_gm_mail(_Account, [Param], State) ->
    [Recvers | Rest] = string:tokens(Param, " "),
    RecverList = [db_player:name_to_account(X) || X <- string:tokens(Recvers, ", ")],

    case Rest of
	[] ->
	    player_mail:send_gm_mail(RecverList, [], [], []),
	    ok;
	[Title] ->
	    player_mail:send_gm_mail(RecverList, Title, [], []),
	    ok;
	[Title, Content] ->
	    player_mail:send_gm_mail(RecverList, Title, Content, []),
	    ok;
	[Title, Content, Items] ->
	    MailItems = [#pack_grid{count=1, item_data=item:make_item(list_to_integer(X))} 
			 || X <- string:tokens(Items, ", ")],
	    
	    player_mail:send_gm_mail(RecverList, Title, Content, MailItems),
	    ok;
	_ ->
	    ok
    end,    

    State. 


%% ��n��ϵͳ�ʼ� 
do_send_n_mail(Account, [Times], State) ->
    N = list_to_integer(Times),
    case N =< 1000 of
	true ->
	    [begin
		 player_mail:send_sys_mail([Account], "title" ++ integer_to_list(X), "content" ++ integer_to_list(X), [])
	     end
	     || X <- lists:seq(1, N)]
    end,

    State. 
