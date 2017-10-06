%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  ��ҵİ���(���������ϵ���Ʒ), װ��Ʒ������
%%% @end
%%% Created : 22 Mar 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_pack).

-include("router.hrl").
-include("packet_def.hrl").
-include("common_def.hrl").
-include("enum_def.hrl").
-include("sys_msg.hrl").
-include("tplt_def.hrl").
-include("item_type.hrl").
-include("player_data.hrl").
-include("mutex_actions.hrl").

%% API
-export([start/2]).
-export([add_item/3, add_item2/3, add_item3/3, 
	 del_item/2, del_item2/2, get_item/2, get_body_item/2, get_gift_bag_item/2, do_use_item/4,
	 move_item/5, do_move_item/4, do_move_item/5,is_lock/3, change_grids_lock_state/3, 
	 find_available_grid/3, putoff_old_dress/3, save/2, save/3, send/2, is_item_exist/2, broadcast_body_data/2,
	 make_package/3, make_package2/3, get_grids/1, player_enter_scene/2, get_notify_body_packet/1, check_item_count/3
	 ]).

-export([can_add_item/3, can_add_item/2]).

%% gen_server callbacks
-export([handle_call/3, handle_cast/2, register/1]). 

%% -record(state, {owner,
%% 		bag_grids,    % ��Ұ���
%% 		body_grids    % ��������װ��
%% 	       }).

-define(pd_hat, 0).       % ñ��
-define(pd_clothes, 1).   % �·�
-define(pd_glasses, 2).   % �۾�
-define(pd_trousers, 3).  % ����
-define(pd_back, 4).      % ����
-define(pd_shoes, 5).     % Ь��
-define(pd_hand, 6).      % �ֳ�
-define(pd_trinket, 7).   % ��Ʒ


%%%===================================================================
%%% API
%%%===================================================================

%%--------------------------------------------------------------------
%% @doc
%% Starts the server
%%
%% @spec start_link() -> {ok, Pid} | ignore | {error, Error}
%% @end
%%--------------------------------------------------------------------
start(Owner, PlayerData) ->
    init(Owner, PlayerData).

%% ������Ʒ����Ұ�����, �ú���ֻ���ڴ����, ��ͬ�����ݵ��ͻ���, Ҳ���������ݿ�
%% Items = [Item | ...] | Item
add_item(Owner, Items, PlayerData) ->
    {ok, NPack} = do_add_item(Owner, Items, player_data:get_pack(?pt_bag, PlayerData)),
    player_data:set_pack(?pt_bag, NPack, PlayerData).

%% ������Ʒ����Ұ�����, �ú���������ͬ�����ݵ��ͻ���, �������ݿ�
-spec add_item2(atom(), list()|tuple(), tuple()) -> {ok, tuple()} | {error, 'id not found'} | {error, full}.
add_item2(_Owner, [], PlayerData) ->
    {ok, PlayerData};
add_item2(Owner, Items, PlayerData) ->
    Pack = player_data:get_pack(?pt_bag, PlayerData),
    case do_add_item(Owner, Items, Pack) of
	{ok, NPack} ->
	    save(Owner, ?pt_bag, NPack),
	    pack:send_package(Owner, ?pt_bag, NPack),
	    {ok, player_data:set_pack(?pt_bag, NPack, PlayerData)};
	{error, full} ->
	    {error, full};
	{error, 'id not found', _Item} ->
	    sys_msg:send(Owner, ?err_not_exist_item_temp_id),
	    {error, 'id not found'}
    end.

add_item3(Owner, Items, Pack) ->
    do_add_item(Owner, Items, Pack).

%% �����Ʒ
get_item(Index, PlayerData)->
    pack:get_item(Index, player_data:get_pack(?pt_bag, PlayerData)).

get_body_item(Index, PlayerData) ->
    pack:get_item(Index, player_data:get_pack(?pt_body, PlayerData)).

get_gift_bag_item(Index, PlayerData) ->
    pack:get_item(Index, player_data:get_pack(?pt_gift_bag, PlayerData)).

%% �������ƶ���Ʒ, ����{NewGrids1, NewGrids2}.
do_move_item(Type1, Index1, Type2, Index2, PlayerData) when Index1>=0,Index2>=0->
    pack:move(Index1, player_data:get_pack(Type1, PlayerData), 
	      Index2, player_data:get_pack(Type2, PlayerData)).

do_move_item(Index1, Grid1, Index2, Grid2) when Index1>=0,Index2>=0 ->
    pack:move(Index1, Grid1, Index2, Grid2).

send(Account, PlayerData) ->
    pack:send_package(Account, ?pt_bag, player_data:get_pack(?pt_bag, PlayerData)).

%% �������������
save(Owner, PlayerData) when is_record(PlayerData, player_data) ->
    save(Owner, ?pt_bag, player_data:get_pack(?pt_bag, PlayerData)).

save(Owner, ?pt_bag, PlayerData) when is_record(PlayerData, player_data) ->
    db_common:write(make_package2(Owner, ?pt_bag, PlayerData));
save(Owner, ?pt_body, PlayerData) when is_record(PlayerData, player_data) ->
    db_common:write(make_package2(Owner, ?pt_body, PlayerData));
save(Owner, ?pt_depot, PlayerData) when is_record(PlayerData, player_data)->
    db_common:write(make_package2(Owner, ?pt_depot, PlayerData));
save(Owner, Type, Grids) -> 
    db_common:write(make_package(Owner, Type, Grids)).

save(Owner, Type1, Type2, PlayerData) when is_record(PlayerData, player_data) ->
    db_pack:write(make_package2(Owner, Type1, PlayerData), 
		  make_package2(Owner, Type2, PlayerData)).

del_item([], PlayerData) ->
    PlayerData;
del_item([TempId|ItemList], PlayerData) when is_list(ItemList) ->
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    Index = pack:get_index_by_tempid(TempId, Bag),    
    NPlayerData = del_item(Index, PlayerData),
    del_item(ItemList, NPlayerData);
%% ɾ����Ʒ, ֻ�ı��ڴ��������, ����NewPlayerData
del_item(Index, PlayerData)->
    Pack = pack:del_item(Index, player_data:get_pack(?pt_bag, PlayerData)),
    player_data:set_pack(?pt_bag, Pack, PlayerData).

%% ɾ����Ʒ�����浽���ݿ�, ������Ϣ���ͻ���
del_item2(Index, PlayerData)->
    Owner = player_data:get_account(PlayerData),
    NPack = pack:del_item(Index, player_data:get_pack(?pt_bag, PlayerData)),
    save(Owner, ?pt_bag, NPack),
    pack:send_package(Owner, ?pt_bag, NPack),
    player_data:set_pack(?pt_bag, NPack, PlayerData).

%% �ж��ܷ��ָ����������Ʒ���뵽������
-spec can_add_item(pos_integer(), pos_integer(), tuple()) -> boolean().
can_add_item(ItemID, Count, PlayerData) ->
    Pack = player_data:get_pack(?pt_bag, PlayerData),
    case pack:find_available_grid(ItemID, Count, Pack) of
	full -> false;
	_Index -> true
    end.

%% �ж��ܷ��ָ����������Ʒ���϶����뵽������
-spec can_add_item(list(), tuple()) -> boolean().
can_add_item(ItemList, PlayerData) ->
    Pack = player_data:get_pack(?pt_bag, PlayerData),
    can_add_item_impl(ItemList, Pack).

can_add_item_impl([], _Pack) ->
    true;
can_add_item_impl([{ItemID, Count} | Rest], Pack) ->
    case pack:add_item(ItemID, Count, Pack) of
	full ->
	    false;
	NPack ->
	    can_add_item_impl(Rest, NPack)
    end.

%% �ж�ָ������Ʒ�Ƿ����
-spec is_item_exist([{pos_integer(), pos_integer()}], tuple()) -> boolean().
is_item_exist(ItemList, PlayerData) when is_list(ItemList)->
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    case check_item_count(ItemList, Bag, []) of
	true -> true;
	{false, _} -> false
    end.

broadcast_body_data(Owner, PlayerData) ->
    Body = player_data:get_pack(?pt_body, PlayerData),
    pack:send_package(Owner, ?pt_body, Body).

%% ��ҽ��볡�����¼�
%% Account: ���볡�������
player_enter_scene(Account, PlayerData)->
    Owner = player_data:get_account(PlayerData),
    Body = player_data:get_pack(?pt_body, PlayerData),
    net_helper:send2client(Account, #notify_package{account=Owner, 
						  type=?pt_body, 
						  grid_vec=Body}).

get_notify_body_packet(PlayerData) ->
    Owner = player_data:get_account(PlayerData),
    Body = player_data:get_pack(?pt_body, PlayerData),
    #notify_package{account=Owner, type=?pt_body, grid_vec=Body}.

find_available_grid(Index1, Grid1, Grid2) ->
    Item = pack:get_item(Index1, Grid1),
    Count = pack:get_item_count(Index1, Grid1),
    TempId = item:get_tempid(Item),
    pack:find_available_grid(TempId, Count, Grid2).
    
%%%===================================================================
%%% gen_server callbacks
%%%===================================================================

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Initializes the server
%%
%% @spec init(Args) -> {ok, State} |
%%                     {ok, State, Timeout} |
%%                     ignore |
%%                     {stop, Reason}
%% @end
%%--------------------------------------------------------------------
%% ����ָ������Ϣ
register(Owner)-> 
    [router:make_event_source(?msg_req_use_item, Owner, {self(), ?MODULE}),
     router:make_event_source(notify_update_item, Owner, {self(), ?MODULE}),
     router:make_event_source(?msg_req_swap_item, Owner, {self(), ?MODULE}),
     router:make_event_source(?msg_req_move_item, Owner, {self(), ?MODULE}),
     router:make_event_source(?msg_req_switch_item, Owner, {self(), ?MODULE}),
     router:make_event_source(?msg_req_move_money, Owner, {self(), ?MODULE}),
     router:make_event_source(?msg_req_delete_item, Owner, {self(), ?MODULE}),
     router:make_event_source(?msg_req_lock_bag_item, Owner, {self(), ?MODULE}),
     router:make_event_source(?msg_req_unlock_bag_item, Owner, {self(), ?MODULE}),
     router:make_event_source(?msg_req_split_item, Owner, {self(), ?MODULE}),
     router:make_event_source(?msg_req_resize_player_pack, Owner, {self(), ?MODULE}),
     router:make_event_target(add_item, Owner, {self(), ?MODULE}),
     router:make_event_target(get_item_temp_id, Owner, {self(), ?MODULE}),
     router:make_event_target(is_item_exist, Owner, {self(), ?MODULE})
    ].
init(Owner, PlayerData) ->
    gen_item:start([{?it_dress, item_dress}]),
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    Body = player_data:get_pack(?pt_body, PlayerData),
    pack:send_package(Owner, ?pt_bag, Bag),
    pack:send_package(Owner, ?pt_body, Body),    
    %% �����Զ�����
    %%ItemUpdateInterval = ?ITEM_UPDATE_INTERVAL,
    ItemUpdateInterval = common_def:get_val(scan_del_item_interval),
    sys_time:subscribe(?dt_interval, Owner, ItemUpdateInterval, notify_update_item),
    NewPlayerData = update_overtime_items(PlayerData),
    NewPlayerData.

%% �ж�ָ���ĸ����Ƿ�������
is_lock(PackType, GridIndex, State) ->
    pack:is_lock(GridIndex, player_data:get_pack(PackType, State)).

%% �ı���ӵ�����״̬
change_grids_lock_state(0, Pack, _IsLock) ->
    Pack;
change_grids_lock_state(ItemTempID, Pack, IsLock) when is_integer(ItemTempID)->
    #item_tplt{sub_id=SubID} = tplt:get_data(item_tplt, ItemTempID),
    #item_dress_tplt{equip_pos=EPos,equip_placeholder=Placeholders} 
	= tplt:get_data(item_dress_tplt, SubID),
    F = fun(Holder, BodyGrids)->
		case EPos =:= Holder of
		    true  -> Pack;
		    false -> 
			case IsLock of
			    false ->
				pack:unlock_grid(Holder, BodyGrids);
			    true ->
				pack:lock_grid(Holder, BodyGrids)
			end
		end
	end,
    lists:foldl(F, Pack, Placeholders).

%% �ж���ҵı����Ƿ����㹻�Ŀռ���������������Ʒ
putoff_old_dress(NewDressTempID, Bag, Body) ->
    % �ж��Ƿ���Էŵ�������
    L = get_putoff_dress(NewDressTempID, Body),
    L1 = [Item || {_BodyIndex, Item} <- L],

    NBag = lists:foldl(fun(Item, BagIn)->
			       case BagIn of
				   full -> full;
				   _ -> pack:add_item(Item, BagIn)
			       end
		       end, Bag, L1),

    case NBag of
	full -> 
	    {false, ?err_bag_full};
	_ ->
	    NBody = lists:foldl(fun({BodyIndex, _Item}, BodyIn)->
					pack:del_item(BodyIndex, BodyIn)
				end, Body, L),
	    [{?pt_bag, NBag}, {?pt_body, NBody}]
    end.

%% ��ô���һ����װ��ʱ, ��Ҫ�������ķ�װ
%% ����[#item{}].
-spec get_putoff_dress(non_neg_integer(), tuple()) -> list().
get_putoff_dress(NewDressTempID, _BodyGrids) when NewDressTempID < 0 ->
    erlang:error({badarg, NewDressTempID});
get_putoff_dress(NewDressTempID, _BodyGrids) when NewDressTempID =:= 0 ->
    [];
get_putoff_dress(NewDressTempID, BodyGrids) ->
    #item_dress_tplt{equip_pos=EPos, equip_placeholder=Holders}
	= tplt:get_dress_data(NewDressTempID),
    NewHolders = get_more_putoff_holders(BodyGrids, Holders),
    [{H, Item} || H <- NewHolders, H /= EPos, 
	     begin Item = pack:get_item(H, BodyGrids), Item /= empty end].

%% ȡ�ø�����Ҫ���µ���λ
get_more_putoff_holders(BodyGrids, OldHolders) when is_list(OldHolders) ->
    F = fun(_Index, {Item, Count}, Acc) ->  
		case Count of
		    0 ->
			Acc;
		    _ ->
			ItemTempID = item:get_tempid(Item),
			#item_dress_tplt{equip_placeholder=Holders}
			    = tplt:get_dress_data(ItemTempID),
			
			%% ȡ����X
			{X, B} = lists:partition(fun(E) -> lists:member(E, Acc) end, Holders),
			%% ����н���, ˵������װ��ҲҪ����, ���벢��
			case length(X) > 0 of
			    true ->
				Acc ++ B;
			    _ ->
				Acc
			end
		end
	end,
    NewHolders = pack:foldl(F, OldHolders, BodyGrids),
    NewHolders.


%% �ж���Щ��Ʒ�Ƿ���һ���Զ����뱳����
%% -spec is_enough_store_grid(list(), array()) -> boolean().
%% is_enough_store_grid([], _Bag) ->
%%     false;
%% is_enough_store_grid([#item{template_id = TempID}], Bag) ->
%%     case pack:find_available_grid(TempID, Bag) of
%% 	full -> false;
%% 	_Index -> true
%%     end;
%% is_enough_store_grid([#item{template_id = TempID} | Rest]=L, Bag) ->
%%     case length(L) > pack:get_size(Bag) of
%% 	true -> false;
%% 	false ->
%% 	    case pack:find_available_grid(TempID, Bag) of
%% 		full -> false;
%% 		_Index -> is_enough_store_grid(Rest, Bag)
%% 	    end
%%     end.

%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling call messages
%%
%% @spec handle_call(Request, From, State) ->
%%                                   {reply, Reply, State} |
%%                                   {reply, Reply, State, Timeout} |
%%                                   {noreply, State} |
%%                                   {noreply, State, Timeout} |
%%                                   {stop, Reason, Reply, State} |
%%                                   {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
%% ��Ӧ������Ʒ����
handle_call({#msg{event=add_item}, Items}, _From, PlayerData) ->
    Owner = player_data:get_account(PlayerData),
    {ok, NPlayerData} = add_item2(Owner, Items, PlayerData),
    {reply, ok, NPlayerData};
%% �ж���Ʒ�Ƿ����
handle_call({#msg{event=is_item_exist}, ItemList}, _From, PlayerData) ->
    IsExist = is_item_exist(ItemList, PlayerData),
    {reply, IsExist, PlayerData};
handle_call(Request, _From, State) ->
    logger:log(error, "player_pack handle call Request:~p~n", [Request]),
    {reply, ok, State}.
%%--------------------------------------------------------------------
%% @private
%% @doc
%% Handling cast messages
%%
%% @spec handle_cast(Msg, State) -> {noreply, State} |
%%                                  {noreply, State, Timeout} |
%%                                  {stop, Reason, State}
%% @end
%%--------------------------------------------------------------------
%% ��Ӧ�ͻ���ʹ����Ʒ������
handle_cast({#msg{src=Account}, #req_use_item{index=Index}}, PlayerData)->    
    Grids = player_data:get_pack(?pt_bag, PlayerData),
    Item = pack:get_item(Index, Grids),
    TempID = item:get_tempid(Item),
    #item_tplt{type=Type} = tplt:get_data(item_tplt, TempID), 

    NPD = 
	case Type of
	    ?it_dress ->
		%% ���������ж��ܷ���
		mutex_actions:try_start_action(?action_type_wear, 
					       {Account, Index, Grids}, PlayerData);
	    _ ->
		do_use_item(Account, Index, Grids, PlayerData)
	end,

    {noreply, NPD};

%% ��Ӧ�ͻ��˵Ľ�����Ʒλ�õ�����
handle_cast({_Msg, #req_swap_item{type=Type, index1=Index1, index2=Index2}}, PlayerData)->
    Owner = player_data:get_account(PlayerData),
    Bag = player_data:get_pack(Type, PlayerData),
    NPlayerData1 = case pack:swap(Index1, Index2, Bag) of
		       NBag ->
			   NPlayerData = player_data:set_pack(Type, NBag, PlayerData),
			   save(Owner, Type, NPlayerData),
			   %% �ѱ仯�˵����ݴ��͸��ͻ���
			   pack:send_package(Owner, Type, NBag),
			   NPlayerData
		   end,
    {noreply, NPlayerData1};

%% ��Ӧ�ͻ��˵İ���֮���ƶ���Ʒ������
handle_cast({_Msg, #req_move_item{pack1_type=Type1,index1=Index1,
				  pack2_type=Type2,index2=Index2}}, PlayerData) ->
    NPlayerData =
	case (Type1 =:= ?pt_body) and (Type2=:= ?pt_bag) of
	    true ->
		%% ���������ж��ܷ�����
		mutex_actions:try_start_action(?action_type_wear, 
					       {Type1, Index1, Type2, Index2}, PlayerData);
	    _ ->
		move_item(Type1, Index1, Type2, Index2, PlayerData)
	end,
    
    {noreply, NPlayerData};

%% ��Ӧ�ͻ��˵���Ϸ֮�����Ʒ����
handle_cast({_Msg, #req_switch_item{pack1_type=Type1,instance_id=InstanceID, pack2_type=Type2}}, 
	    PlayerData) ->
    {noreply, switch_item(Type1, InstanceID, Type2, PlayerData)};

handle_cast({_Msg, #req_move_money{pack1_type=Type1,pack2_type=_Type2,money=Money}}, PlayerData) ->
    Owner = player_data:get_account(PlayerData),
    NPlayerData = 
	case Type1 of
	    ?pt_bag -> 
		Coin = player_coin:get_coin(?GAMECOIN, PlayerData),
		case Coin >= Money of
		    true -> 			
			Farm = player_farm:get_farm_data(Owner),
			FCoin = farm_coin:get_coin(Farm),
			NFCoin = Money + FCoin,
			case player_coin:is_out_of_range(NFCoin) of
			    true ->
				send_coin_out_of_range_msg(PlayerData,Owner),
				PlayerData;				
			    false ->
				NFarm = farm_coin:add_coin(Money, Farm),
				db_farm:write(NFarm),
				PlayerData1 = player_coin:dec_coin2(?GAMECOIN, Money, PlayerData),    
				db_common:write(player_data:get_coin(PlayerData1)),
				
				%% ��ʱ�޸�ˢǮbug
				%% TODO: ��Ҫ�����޸�����Ҫ�޸����н�Ǯʹ�ô���
				GameCoin = player_coin:get_coin(?GAMECOIN, PlayerData1),
				MemdiscCoin = db_memdisc:read(player_coin, Owner),
				NMemdiscCoin = player_coin:set_coin(?GAMECOIN, GameCoin, MemdiscCoin),
				db_memdisc:write(NMemdiscCoin),

				player_coin:send_coin(player_data:get_coin(PlayerData1)),
				net_helper:send2client(Owner, #notify_farm_money{account = Owner, 
										 add_money = 0,
										 money = player_farm:get_coin(NFarm)}),
				sys_msg:send(Owner,?msg_move_farm_coin_success,[Money]),
				PlayerData1
			end;
		    false ->
			player_coin:send_coin(player_data:get_coin(PlayerData)),
			sys_msg:send(Owner, ?err_not_enough_money),
			PlayerData
		end;
	    ?pt_farmdepot ->
		Farm = player_farm:get_farm_data(Owner),
		Coin = farm_coin:get_coin(Farm),
		case Coin >= Money of
		    true -> 
			NCoin = player_coin:get_coin(?GAMECOIN,PlayerData) + Money,
			case player_coin:is_out_of_range(NCoin) of
			    true->
				send_coin_out_of_range_msg(PlayerData,Owner),
				PlayerData;			
			    false ->
				Farm = player_farm:get_farm_data(Owner),		
				NFarm = farm_coin:dec_coin(Money, Farm),
				db_farm:write(NFarm),		
				PlayerData1 = player_coin:add_coin2(?GAMECOIN, Money, PlayerData),    
				db_common:write(player_data:get_coin(PlayerData1)),

				%% ��ʱ�޸�ˢǮbug
				%% TODO: ��Ҫ�����޸�����Ҫ�޸����н�Ǯʹ�ô���
				GameCoin = player_coin:get_coin(?GAMECOIN, PlayerData1),
				MemdiscCoin = db_memdisc:read(player_coin, Owner),
				NMemdiscCoin = player_coin:set_coin(?GAMECOIN, GameCoin, MemdiscCoin),
				db_memdisc:write(NMemdiscCoin),

				player_coin:send_coin(player_data:get_coin(PlayerData1)),
				net_helper:send2client(Owner, #notify_farm_money{account = Owner, 
							      add_money = 0,
							       money = player_farm:get_coin(NFarm)}),
				sys_msg:send(Owner,?msg_move_game_coin_success,[Money]),
				PlayerData1
			end;
		    false ->
			net_helper:send2client(Owner, #notify_farm_money{account = Owner, 
									 add_money = 0,
									 money = player_farm:get_coin(Farm)}),
			sys_msg:send(Owner, ?err_not_enough_money),
			PlayerData
		end
	end,
    {noreply, NPlayerData};

%% ��Ӧ�ͻ���ɾ�������е���Ʒ������
handle_cast({_Msg, #req_delete_item{grid_index=GIndex}}, PlayerData) ->
    Owner = player_data:get_account(PlayerData),
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    NPlayerData = player_data:set_pack(?pt_bag, do_del_item(Owner, GIndex, Bag), PlayerData),
    %% ��Ǯ��Ʒ��¼
    Item = pack:get_item(GIndex, Bag),
    ItemInfo = {item:get_tempid(Item), 
		-pack:get_item_count(GIndex, Bag),
	       item:get_instid(Item)},
    item_money_log:log_item(Owner, del_item, ItemInfo),

    {noreply, NPlayerData};


%% ������Ʒ����
handle_cast({_Msg, #req_lock_bag_item{item_inst_id=InstID}}, PlayerData) ->
    Owner = player_data:get_account(PlayerData),
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    NewBag = pack:lock(InstID, Bag),
    pack:send_package(Owner, ?pt_bag, NewBag),
    {noreply, player_data:set_pack(?pt_bag, NewBag, PlayerData)};

%% ������Ʒ����
handle_cast({_Msg, #req_unlock_bag_item{item_inst_id=InstID}}, PlayerData) ->
    Owner = player_data:get_account(PlayerData),
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    NewBag = pack:unlock(InstID, Bag),
    pack:send_package(Owner, ?pt_bag, NewBag),
    {noreply, player_data:set_pack(?pt_bag, NewBag, PlayerData)};


%% ��Ӧ�ͻ��˲�ְ����е���Ʒ
handle_cast({#msg{src=Account}, 
	     #req_split_item{type=Type, src_gindex=SrcIdx, target_gindex=TargetIdx, count=Count}}, 
	     PlayerData) ->
    Bag = player_data:get_pack(Type, PlayerData),
    case pack:add_item_from_grid(Count, SrcIdx, TargetIdx, Bag) of
	{ok, NBag} ->
	    NPlayerData = player_data:set_pack(Type, NBag, PlayerData),
	    save(Account, Type, NPlayerData),
	    pack:send_package(Account, Type, NBag),
	    {noreply, NPlayerData};
	{error, full} ->
	    sys_msg:send(Account, ?err_item_split_error),
	    {noreply, PlayerData}
    end;

%% ��Ӧ�ͻ������ݰ���
handle_cast({#msg{src=Account}, #req_resize_player_pack{}}, PlayerData) ->
    %% TODO: ��ʱ�趨Ϊ����10��eq��
    Cost = 10,
    %% ���ֻ��4ҳ, �����������, �øý��̹ҵ�
    MaxCount = ?init_pack_grid_count * 4, 
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    true = MaxCount > pack:get_size(Bag),
    case player_coin:get_coin(?EQCOIN, PlayerData) >= Cost of
	true ->
	    Size = pack:get_size(Bag) + ?init_pack_grid_count,
	    NBag = pack:resize(Size, Bag),
	    CostType = ?EQCOIN,
	    NPD = player_coin:dec_coin2(CostType, Cost, PlayerData),
	    Coin = player_data:get_coin(NPD),
	    
	    db_memdisc:write(Coin, make_package(Account, ?pt_bag, NBag)),

	    pack:send_package(Account, ?pt_bag, NBag),
	    player_coin:send_coin(Coin),

	    %% ����, ��Ʒ��log��¼
	    MoneyInfo = item_money_log:get_money_info(CostType, -Cost),
	    item_money_log:log_money(Account, resize_pack,  MoneyInfo),

	    sys_msg:send(Account, ?success_resize_player_pack),
	    {noreply, player_data:set_pack(?pt_bag, NBag, NPD)};
	false ->
	    sys_msg:send(Account, ?err_not_enough_coin),
	    {noreply, PlayerData}
    end;


%% ������Ʒ, �Զ�ɾ��������Ʒ
handle_cast({#msg{event=notify_update_item}, _TimerRef}, PlayerData) ->
    NewPlayerData = update_overtime_items(PlayerData),
    {noreply, NewPlayerData};
handle_cast(Request, State) ->
    logger:log(warn, "player_pack unknown request:~p~n", [Request]),
    {noreply, State}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% �ƶ���Ʒ, ���漰�����ݿ�Ĵ洢��������Ϣ�ķ���
move_item(Account, Index1, Grid1, Index2, Grid2)
  when is_atom(Account), Index1>=0, Index2>=0 ->
    case do_move_item(Index1, Grid1, Index2, Grid2) of
	fail ->
	    sys_msg:send(Account, ?err_move_item),
	    false;
	{_NewGrids1, _NewGrids2} = Data ->
	    Data
    end;
move_item(?pt_bag, Index1, ?pt_gift_bag, Index2, PlayerData) when Index1>=0, Index2>=0 ->
    Account = player_data:get_account(PlayerData),
    case do_move_item(?pt_bag, Index1, ?pt_gift_bag, Index2, PlayerData) of
	fail ->
	    sys_msg:send(Account, ?err_move_item),
	    false;
	{NewGrids1, NewGrids2} = Data ->
	    Count = pack:get_item_count(Index2, NewGrids2),
	    case Count > 0 of
		true ->
		    Item = pack:get_item(Index2, NewGrids2),
		    %% �ŵ��������Ʒ���۳�ת��ʱ��
		    NewItem = item:cut_trade_time(Item),

		    {NewGrids1, pack:set_item(NewItem, Count, Index2, NewGrids2)};
		_ ->
		    Data
	    end
    end;

move_item(Type1, Index1, Type2, Index2, PlayerData) when Index1>=0,Index2>=0->
    Owner = player_data:get_account(PlayerData),
    Result = 
    case (Type1 =:= ?pt_body) and (Type2=:= ?pt_bag) of
	true ->
	    case item_dress:putoff_dress(Type1, Index1, Type2, Index2, Owner, PlayerData) of
		{ok, [{?pt_bag, BagGrids}, {?pt_body, BodyGrids}]} ->
		    {BodyGrids, BagGrids};
		false -> false
	    end;
	false -> 
	    case do_move_item(Type1, Index1, Type2, Index2, PlayerData) of
		fail ->
		    sys_msg:send(Owner, ?err_move_item),
		    false;
		{_NewGrids1, _NewGrids2} = Data ->
		    Data
	    end
    end,
    case Result of
	{NewGrids1, NewGrids2} ->
	    PlayerData1 = player_data:set_pack(Type1, NewGrids1, PlayerData),
	    PlayerData2 = player_data:set_pack(Type2, NewGrids2, PlayerData1),
	    save(Owner, Type1, Type2, PlayerData2),
	    % �ѱ仯�˵����ݷ��͸��ͻ���
	    pack:send_package(Owner, Type1, NewGrids1),
	    pack:send_package(Owner, Type2, NewGrids2),
	    PlayerData2;
	_ -> PlayerData
    end.

switch_item(BagType1, InstanceID, _BagType2, PlayerData) ->
    %% ��ũ������Ұ���֮���ƶ���Ʒ
    Owner = player_data:get_account(PlayerData),
    Bag = player_data:get_pack(?pt_bag, PlayerData),
    case BagType1 =:= ?pt_farmdepot of
	true -> %% ũ���ֿ� -> ����
	    FarmDepot = farm_depot:read(Owner),
	    Index = farm_depot:get_index(InstanceID, FarmDepot),
	    case Index =:= not_found of
		true ->
		    pack:send_package(Owner, ?pt_farmdepot, farm_depot:get_grids(FarmDepot)),
		    sys_msg:send(Owner, ?msg_bag_no_item),
		    PlayerData;
		false  ->
		    Item = farm_depot:get_item(Index, FarmDepot),
		    Count = farm_depot:get_item_count_by_index(Index, FarmDepot),
		    NFarmDepot = farm_depot:del_item(Index, Count, FarmDepot),
		    case pack:add_item(Item, Count, Bag) of
			full ->
			    sys_msg:send(Owner, ?err_bag_full),
			    PlayerData;
			NBag ->
			    PlayerData1 = player_data:set_pack(?pt_bag, NBag, PlayerData),
			    save(Owner, ?pt_bag, PlayerData1),
			    farm_depot:save(NFarmDepot),
			    pack:send_package(Owner, ?pt_bag, NBag),
			    pack:send_package(Owner, ?pt_farmdepot, farm_depot:get_grids(NFarmDepot)),
			    PlayerData1
		    end
	    end;
	false -> %% ���� -> ũ���ֿ�
	    Index = pack:get_index(InstanceID, Bag),
	    case Index =:= not_found of
		true -> 
		    pack:send_package(Owner, ?pt_bag, Bag),
		    sys_msg:send(Owner, ?msg_bag_no_item),
		    PlayerData;
		false ->
		    Item = pack:get_item(Index, Bag),
		    Count = pack:get_item_count(Index, Bag),
		    NFarmDepot = farm_depot:add_item(Owner, Item, Count),
		    NBag = pack:del_item(Index, Count, Bag),
		    PlayerData1 = player_data:set_pack(?pt_bag, NBag, PlayerData),
		    save(Owner, ?pt_bag, PlayerData1),
		    farm_depot:save(NFarmDepot),
		    pack:send_package(Owner, ?pt_bag, NBag),
		    pack:send_package(Owner, ?pt_farmdepot, farm_depot:get_grids(NFarmDepot)),
		    PlayerData1
	    end
    end.


%% ʹ����Ʒ
%% ����NewState
do_use_item(Owner, Index, BagGrids, PlayerData) when Index >= 0->
    case gen_item:use_item(Index, Owner, BagGrids, PlayerData) of
	{ok, DataList} ->
	    D = [make_package(Owner, Type1, Grids1) || {Type1, Grids1} <- DataList],
	    db_common:write(D),
	    [pack:send_package(Owner, NType, Grids) || {NType, Grids}<-DataList],
	    change_state(DataList, PlayerData);
	false ->
	    PlayerData
    end;
do_use_item(_Owner, Index, _BagGrids, _PlayerData) ->
    erlang:error({badarg, Index}).

%% ɾ����Ʒ
%% �����µ�Grids
do_del_item(Owner, GIndex, Grids)->
    % ɾ����Ʒ
    Count = pack:get_item_count(GIndex, Grids),
    NewGrids = pack:del_item(GIndex, Count, Grids),
    save(Owner, ?pt_bag, NewGrids),
    pack:send_package(Owner, ?pt_bag, NewGrids),
    NewGrids.

change_state(DataList, PlayerData)->
    F = fun({Type, Grids}, PD)-> 
		player_data:set_pack(Type, Grids, PD)
	end,
    lists:foldl(F, PlayerData, DataList).
    
get_grids(#player_pack{grid_vec=Grids})->
    Grids;
get_grids(#player_body{grid_vec=Grids}) ->
    Grids;
get_grids(State) ->
    erlang:error(badarg, [State]).

%% �����������ɰ���
make_package(Owner, ?pt_bag, Grids)->
    #player_pack{owner=Owner, grid_vec=Grids};
make_package(Owner, ?pt_body, Grids) ->
    #player_body{owner=Owner, grid_vec=Grids}.

make_package2(Owner, ?pt_bag, PlayerData)->
    #player_pack{owner=Owner, grid_vec=player_data:get_pack(?pt_bag, PlayerData)};
make_package2(Owner, ?pt_body, PlayerData) ->
    #player_body{owner=Owner, grid_vec=player_data:get_pack(?pt_body, PlayerData)};
make_package2(_Owner, ?pt_depot, PlayerData) ->
    player_data:get_depot(PlayerData).

%% ������Ʒ
do_add_item(_Owner, [], BagGrids) ->
    {ok, BagGrids};
do_add_item(Owner, [Item | Rest], BagGrids) ->
    case do_add_item_impl(Item, BagGrids) of
	{ok, NewGrids} -> 
	    do_add_item(Owner, Rest, NewGrids);
	Result -> 
	    Result
    end;
do_add_item(_Owner, Item, BagGrids) ->
    case do_add_item_impl(Item, BagGrids) of
	{ok, NewGrids} ->
	    {ok, NewGrids};
	Result ->
	    Result
    end.

do_add_item_impl(Item, Bag) ->
    case check_item_temp_id(Item) of
	true ->
	    case pack:add_item(Item, Bag) of
		full -> {error, full};
		NBag -> {ok, NBag}
	    end;
	false->
	    {error, 'id not found', Item}
    end.

check_item_temp_id(#item{template_id=TempID}) ->
    check_item_temp_id(TempID);
check_item_temp_id(ItemTempID) when ItemTempID > 0 ->
    tplt:is_exist(item_tplt, ItemTempID).

%% �����Ʒ�����Ƿ����
-spec check_item_count(list(), array(), list()) -> true | {false, list()}.
check_item_count([], _BagGrids, ErrorList) ->
    case length(ErrorList) =:= 0 of
	true  -> true;
	false -> {false, ErrorList}
    end;
check_item_count([{ItemId, ItemCount}|ItemList], BagGrids, ErrorList) ->
    FHasItem = fun(X) ->
		       ItemData = X#pack_grid.item_data,
		       ItemData#item.template_id =:= ItemId 
	       end,
    Count = pack:count_if(FHasItem, BagGrids),  
    case Count >= ItemCount of
	true ->
	    check_item_count(ItemList, BagGrids, ErrorList);
	false ->
	    check_item_count(ItemList, BagGrids, [{ItemId, ItemCount - Count}|ErrorList])
    end.


%% ɾ��������Ʒ
update_overtime_items(PlayerData) ->
    PlayerData1 = update_overtime_items(?pt_bag, PlayerData),
    PlayerData2 = update_overtime_items(?pt_body, PlayerData1),

    PlayerData3 = update_overtime_items(?pt_depot, PlayerData2),
    
    PlayerData3.

-spec update_overtime_items(integer(), tuple()) -> tuple().			       
update_overtime_items(Type, PlayerData) ->
    Pack = player_data:get_pack(Type, PlayerData),

    {CountDelItems, DelItems, NextDelItems, NewPack0} = split_pack(Pack),
    FUnlockEquipPos = fun(X, AccPack) -> 
		ItemTempID = item:get_tempid(X),
		change_grids_lock_state(ItemTempID, AccPack, false) 
	end,
    NewPack = 
	case Type =:= ?pt_body of %% �����װ��Ҫ����ռ��λ
	    true -> lists:foldl(FUnlockEquipPos, NewPack0, DelItems);
	    _ -> NewPack0
	end,

    Owner = player_data:get_account(PlayerData),
    NewPlayerData = player_data:set_pack(Type, NewPack, PlayerData),
    case length(DelItems) > 0 of
	true ->
	    %% ���ݿ����
	    save(Owner, Type, NewPlayerData),	    

	    %% ֪ͨ�°���
	    pack:send_package(Owner, Type, NewPack),
	    
	    %% �����װ��, Ҫ֪ͨ��Χ
	    case Type =:= ?pt_body of
		true ->
		    SceneName = player_data:get_scene_name(NewPlayerData),
		    Packet = #notify_package{account=Owner, type=?pt_body, grid_vec=NewPack},
		    router:cast(undefined, SceneName, broadcast_other_players, {Owner, Packet});
		_ ->
		    ok
	    end,
	    
	    %% ֪ͨ��Ʒ����(���ʼ���ʽ)
	    Title = binary_to_list(tplt:get_common_def("item_overtime_mail_title")),
	    MsgFormat = binary_to_list(tplt:get_common_def("item_overtime_msg")) ++ [10],	    
	    F = fun(X, Str) -> Str ++ (item:get_item_name(X) ++ MsgFormat) end,
	    Content = lists:foldl(F, "", DelItems),
	    player_mail:send_sys_mail(Owner, Title, Content, []),	
	    %%[sys_msg:send(Owner, ?msg_del_over_time_item, [item:get_item_name(X)]) || X <- DelItems];
	    %% ��̨��Ʒ��Ǯ��־
	    FGetLogs = fun(MyItem, MyItemCount) ->
				   ItemInfo = {item:get_tempid(MyItem), -MyItemCount, 
					       item:get_instid(MyItem)},
				   item_money_log:build_record(Owner, auto_del_item, ItemInfo)
			   end,
	    Logs = lists:zipwith(FGetLogs, DelItems, CountDelItems),
	    db_common:write(Logs);
	_ ->
	    ok
    end,

    case length(NextDelItems) > 0 of
	true ->
	    %% �����װ��, ��֪ͨ
	    case Type =:= ?pt_body of
		true ->
		    %% ֪ͨ��Ʒ��������
		    [sys_msg:send(Owner, ?msg_item_will_be_del, [item:get_item_name(X)]) 
		     || X <- NextDelItems];
		_ ->
		    ok 
	    end;
	_ ->
	    ok
    end,    

    NewPlayerData.

%% ���������ֳ� ���ڣ���������, �ǹ���������
split_pack(Pack) ->
    Now = datetime:local_time(),

    %%ItemUpdateInterval = ?ITEM_UPDATE_INTERVAL,
    ItemUpdateInterval = common_def:get_val(scan_del_item_interval),
    NextDelTime = datetime:add_time(Now, ItemUpdateInterval),

    F = fun(Idx, {Item, Count}, {CountDelItems, DelItems, NextDelItems, OldPack}=Acc) ->
		#stime{year=Y, month=M, day=D, 
		       hour=H, minute=N, second=S} = Item#item.del_time, 
		DelTime = {{Y, M, D}, {H, N, S}},
		case item:has_del_time(Item) of
		    true ->
			case DelTime < Now of
			    true -> %% ����
				{[Count | CountDelItems],
				 [Item | DelItems], 
				 NextDelItems, 
				 pack:del_item(Idx, Count, OldPack)};
			    _ ->
				case DelTime < NextDelTime of
				    true -> %% ��������
					{CountDelItems, DelItems, [Item | NextDelItems], OldPack};
				    _ ->
					Acc
				end
			end;	
		    _ -> %% ��Ʒ��ʱЧ���� 
			Acc
		end				    
	end,
    
    pack:foldl(F, {[], [], [], Pack}, Pack).

%% ������Ϸ������޶�Ĵ���
send_coin_out_of_range_msg(PlayerData,Owner)->
    player_coin:send_coin(player_data:get_coin(PlayerData)),
    sys_msg:send(Owner, ?err_exceed_max_game_coin).

%%-----------------------------���Դ���---------------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

new_player_data(GridCount) ->
    PD1 = player_data:new(),
    player_data:set_pack(?pt_bag, pack:new(GridCount), PD1).    

add_item_test() ->
    erlymock:start(),
    erlymock:stub(tplt, is_exist, [item_tplt, 2], [{return, true}]),
    erlymock:stub(tplt, get_data2, [item_tplt, 2], [{return, #item_tplt{overlap=100}}]),
    erlymock:replay(), 

    PD = new_player_data(8),
    Item = #item{instance_id=1, template_id=2},
    PD3 = add_item(test, Item, PD),
    ?assertEqual(Item, get_item(0, PD3)),
    erlymock:verify().

change_grids_lock_state_test() ->
    erlymock:start(),
    erlymock:stub(tplt, get_data, [item_tplt, 1], [{return, #item_tplt{sub_id=1}}]),
    erlymock:stub(tplt, get_data, [item_dress_tplt, 1], 
		    [{return, #item_dress_tplt{equip_pos=0,equip_placeholder=[0, 1, 2]}}]),
    erlymock:replay(), 

    change_grids_lock_state(1, pack:new(10), true),
    erlymock:verify().

%% is_enough_store_grid_test() ->
%%     erlymock:start(),
%%     erlymock:stub(tplt, get_data2, [item_tplt, 1], [{return, #item_tplt{overlap=100}}]),
%%     erlymock:stub(tplt, get_data2, [item_tplt, 2], [{return, #item_tplt{overlap=100}}]),
%%     erlymock:replay(),

%%     Item1 = #item{template_id = 1},
%%     Item2 = #item{template_id = 2},
%%     ?assertEqual(false, is_enough_store_grid([Item1, Item2], pack:new(1))),
%%     ?assertEqual(false, is_enough_store_grid([], pack:new(1))),
%%     ?assertEqual(true, is_enough_store_grid([Item1], pack:new(1))),
%%     Pack = pack:add_item(Item1, pack:new(1)),
%%     ?assertEqual(true, is_enough_store_grid([Item1], Pack)),
%%     ?assertEqual(false, is_enough_store_grid([Item2], Pack)),

%%     erlymock:verify(),
%%     ok.

get_putoff_dress_test()->
    erlymock:start(),
    erlymock:stub(tplt, get_dress_data, [1], 
		  [{return, #item_dress_tplt{equip_pos=0, equip_placeholder=[0,1,2,3]}}]),
    erlymock:stub(tplt, get_data2, [item_tplt, 1], [{return, #item_tplt{overlap=100}}]),
    erlymock:stub(tplt, get_data2, [item_tplt, 2], [{return, #item_tplt{overlap=100}}]),
    erlymock:replay(), 

    Item1 = #item{template_id=1},
    Item2 = #item{template_id=2},
    [] = get_putoff_dress(1, pack:new(10)),
    Pack = pack:add_item(Item1, pack:new(10)),
    Pack1 = pack:add_item(Item2, Pack),
    ?assertEqual([{1, #item{template_id=2}}], get_putoff_dress(1, Pack1)),
    erlymock:verify(),
    ok.

%% can_add_item_impl_test() ->
%%     erlymock:start(),
%%     erlymock:stub(guid, make, [2], [{return, 1}]),
%%     erlymock:stub(tplt, get_data2, [item_tplt, 1], [{return, #item_tplt{overlap=10}}]),
%%     erlymock:stub(tplt, get_data2, [item_tplt, 2], [{return, #item_tplt{overlap=10}}]),
%%     erlymock:replay(), 

%%     Pack = pack:new(1),
%%     ?assertEqual(false, can_add_item_impl([{1, 1}, {2, 2}], Pack)),
%%     ?assertEqual(false, can_add_item_impl([{1, 10}, {1, 1}], Pack)),
%%     ?assertEqual(true, can_add_item_impl([{1, 10}], Pack)),

%%     erlymock:verify(),
%%     ok.

%% putoff_old_dress_test()->
%%     erlymock:start(),
%%     erlymock:stub(tplt, get_dress_data, [1], 
%% 		  [{return, #item_dress_tplt{equip_pos=0, equip_placeholder=[0,1,2,3]}}]),
%%     erlymock:replay(), 

%%     Item1 = item:new(1, 0),
%%     Item2 = item:new(2, 0),
%%     PD = new_player_data(10),
%%     PD1 = player_data:set_pack(?pt_body, pack:new(10), PD),
%%     Pack = pack:add_item(Item1, pack:new(10)),
%%     Pack1 = pack:add_item(Item2, Pack),
%%     ?assertEqual(PD1, putoff_old_dress(1, Pack, Pack1)),
%%     erlymock:verify(),
%%     ok.

-endif.
