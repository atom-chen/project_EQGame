%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  农场
%%% @end
%%% Created :  2 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_farm).
  
-include("packet_def.hrl").
-include("router.hrl").
-include("enum_def.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("player_data.hrl").
-include("sys_msg.hrl").
-include("mutex_actions.hrl").

-export([start/1, stop/0, handle_cast/2]).
-export([get_crop/2, set_crop/3,
	 get_exp/1, add_exp/2, get_account/1,
	 get_coin/1, set_coin/2]).
-export([get_decoration/1, set_decoration/2, get_pet_food/1, set_pet_food/2]).
-export([refresh_farm_state/1, notify_farm_data/3, get_farm_data/1, get_username/1]).

-define(init_plot_num, 6). % 初始地块
-define(max_plot_num, 16). % 最大地块

-record(farm_op_data,      % 农场操作的数据
	{time=#stime{},    % 放置灾害的时间
	 count=0}          % 放置灾害的次数
       ). 

-record(player_farm,    % 农场数据
	{account,       % 农场主人
	 exp=0,         % 农场经验
	 decoration,    % 农场装饰
	 pet_food,      % 宠物食物
	 crops=[],      % 农作物
	 money=0,
	 fop_grass=#farm_op_data{},     % 放置草灾的数据
	 fop_pest=#farm_op_data{},      % 放置虫灾的数据
	 foc_grass=#farm_op_data{},     % 除草灾的数据
	 foc_pest=#farm_op_data{}       % 除虫灾的数据
	}
       ).

%%%===================================================================
%%% API
%%%===================================================================
start(Account) ->	
    [router:make_event_source(?msg_req_enter_farm, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_can_assart_plot, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_assart_plot, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_sow, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_pick_crop, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_pick_all_crops, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_get_friend_farm_state, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_weed, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_worm, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_water, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_put_grass, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_put_pest, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_hoeing, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_open_ui, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_eq_users_info, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_gm_command, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_can_upgrade_land, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_upgrade_land, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_sys_time, Account, {self(), ?MODULE}),
     router:make_event_source(received_eq_packet, Account, {self(), ?MODULE})
    ].

stop() ->
    ok.

new(Account)->
    Money = 5000,
    PlayerFarm = new_1(Account, Money),
    Depot = farm_depot:new(Account),
    {NDepot, NPlayerFarm} = farm_decoration:init(PlayerFarm, Depot),
    Ret = db_common:write([
		     NPlayerFarm,
		     farm_log:new(Account),
		     farm_comment:new(Account),
		     NDepot,
		     farm_setting:new(Account),
		     farm_task_data:init(Account)
		    ]),
    %% 后台物品金钱记录
    MoneyInfo = {Money, 0},
    item_money_log:log_money(Account, farm_init, MoneyInfo),
    Ret.


new_1(Owner, Money)->
    #player_farm{account = Owner, 
		 exp = 0,
		 money = Money,
		 pet_food = #farm_pet_food{period_date=datetime:localtime()},
		 crops = farm_crop_init:init(),
		 decoration=#farm_decoration{}
		}.

%% 获得指定的农作物
get_crop(PlotIndex, #player_farm{crops=Crops})->
    farm_crop:get_crop(Crops, PlotIndex).


set_crop(#player_farm{crops=CropList}=Farm, PlotIndex, NCrop) ->
    Farm#player_farm{crops=util:replace(CropList, PlotIndex, NCrop)};
set_crop(CropList, PlotIndex, NCrop) when is_list(CropList) ->
    util:replace(CropList, PlotIndex, NCrop).

set_crop_list(CropList, Farm) when is_list(CropList)->
    Farm#player_farm{crops=CropList}.

%% 获得农场经验
get_exp(#player_farm{exp=Exp}) ->
    Exp.

add_exp(PlayerFarm, Exp) ->
    PlayerFarm#player_farm{exp=Exp}.

%% 获得帐号
get_account(#player_farm{account=Acc}) ->
    Acc.

get_decoration(#player_farm{decoration=Decoration}) ->
    Decoration.

get_pet_food(#player_farm{pet_food=PetFood}) ->
    PetFood.

set_pet_food(PlayerFarm, PetFood) ->
    PlayerFarm#player_farm{pet_food=PetFood}.

set_decoration(PlayerFarm, Decoration) ->
    PlayerFarm#player_farm{decoration=Decoration}.

get_coin(#player_farm{money=Money}) ->
    Money.
set_coin(Coin, Farm) ->
    Farm#player_farm{money=Coin}.
%%--------------------------------------------------------------------
%% 请求进入农场
handle_cast({#msg{src=Guest}, #req_enter_farm{owner=Owner1}}, State) ->
    %% 开始农场状态
    Owner = list_to_atom(Owner1),
    Farm = do_init_farm(Owner, Guest),
    do_enter_farm(Owner, Guest, Farm, State),
    NState = do_farm_task(Owner, Guest, State),
    case Guest of
	Owner ->
	    get_friend_list(Owner, State);
	_ -> ok
    end,
    {noreply, NState};

%% 请求退出农场
handle_cast({_Msg, #req_leave_farm{}}, State) ->
    %% TODO: 是否还需要这个消息
    {noreply, State};

%% 请求获得好友农场的状态
handle_cast({#msg{src=Owner}, #req_get_friend_farm_state{}}, State) ->
    get_friend_list(Owner, State),
    
    {noreply, State};


%% 处理与EQ通讯获得的数据
handle_cast({#msg{event=received_eq_packet}, Packet}, State) ->
    RawPacket = eq_manager:get_raw_packet(Packet),
    %% ID的格式 add_xxx, del_xxx, list_xxx 
    RawPacketId = eq_manager:get_raw_packet_id(RawPacket),
    %% 分解ID, 使用分割符 _
    {Prefix, RawWho} = lists:splitwith(fun(A) -> A =/= $_ end, RawPacketId),
    NewState = process_eq_message(Prefix, RawWho, Packet, State),
    {noreply, NewState};

%% 请求获取系统时间
handle_cast({#msg{event=?msg_req_sys_time, src=Account}, #req_sys_time{}}, State)->
    net_helper:farm2client(Account, #notify_sys_time{sys_time=datetime:localtime()}),
    {noreply, State};

%% 请求能否开垦 
handle_cast({#msg{src=Owner}, #req_can_assart_plot{}}, State) ->
    Farm = get_farm_data(Owner),
    Coin = get_coin(Farm),
    Exp = Farm#player_farm.exp,
    case can_assart(farm_exp:get_level(Exp), get_plot_num(Farm), Coin) of
	{true, Money} ->
	    farm_sys_msg:send(Owner, ?msg_assart_plot, [Money]);
	{false, Reason, NeedMoney, NeedLevel} -> 	    
	    farm_sys_msg:send(Owner, Reason, [NeedLevel, NeedMoney])
    end,
    {noreply, State};

%% 请求开垦 
handle_cast({#msg{src=Owner}, #req_assart_plot{}}, State) ->
    TFarm = get_farm_data(Owner),
    Farm = refresh_farm_state(TFarm),
    Coin = get_coin(Farm),
    Exp = Farm#player_farm.exp,
    case can_assart(farm_exp:get_level(Exp), get_plot_num(Farm), Coin) of
	{true, Money} ->
	    NFarm = assart_plot(Farm),
	    NFarm1 = farm_coin:dec_coin(Money, NFarm),
	    db_farm:write(NFarm1),
	    farm_coin:send_coin(Owner, NFarm1),
	    
	    %% 物品金钱后台记录
	    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, -Money),
	    item_money_log:log_money(Owner, farm_assart_plot, MoneyInfo),
	    
	    notify_farm_data(Owner, Owner, NFarm1);
	{false, Reason, NeedMoney, NeedLevel} ->
	    farm_sys_msg:send(Owner, Reason, [NeedLevel, NeedMoney])
    end,
    {noreply, State};


%% 播种
handle_cast({#msg{src=Account}, #req_sow{plot_index=PlotIndex,item_inst_id=ItemInstID}}, State) ->
    TempFarm = get_farm_data(Account),
    Farm = refresh_farm_state(TempFarm),
    Depot = farm_depot:read(Account),
    case farm_depot:get_index(ItemInstID, Depot) of
	not_found ->
	    farm_depot:send_depot(Account, Depot),
	    farm_sys_msg:send(Account, ?err_farm_depot_no_item),
	    {noreply, State};
	Index ->
	    %% 防沉迷提示
	    notify_wallow_message(Account, State),
	    Item = farm_depot:get_item(Index, Depot),
	    ItemID = item:get_tempid(Item),
	    TpltData = get_seed_data(ItemID),
	    case sow(Farm, PlotIndex, ItemID, TpltData) of
		{ok, NFarm} ->
		    ItemCount = 1,
		    NDepot = farm_depot:del_item(Index, ItemCount, Depot),
		    NFarm1 = proc_work_awards(NFarm, sow, PlotIndex, State),
		    db_farm:write(NFarm1, NDepot),

		    %% 金钱物品记录
		    ItemInfo = {ItemID, 
				-ItemCount,
				ItemInstID},
		    item_money_log:log_item(Account, farm_sow, ItemInfo),

		    farm_depot:send_depot(Account, NDepot),
		    notify_farm_data(Account, Account, NFarm1);
		{error, Reason} -> 
		    farm_sys_msg:send(Account, Reason)
	    end,
	    {noreply, State}
    end;



%% 采摘 
handle_cast({#msg{src=Laborer}, #req_pick_crop{account=Owner1, plot_index=PlotIndex}}, State) ->
    Owner = list_to_atom(Owner1),
    TempFarm = get_farm_data(Owner),
    OwnerFarm = refresh_farm_state(TempFarm),
    Crop = get_crop(PlotIndex, OwnerFarm),
    LaborerFarm = get_farm_data(Laborer),
    Coin = get_coin(LaborerFarm),
    case farm_crop:can_pick_fruit(Laborer, Owner, Coin, Crop, OwnerFarm) of
	true ->
	    case wallow:in_normal_time(State) of
		true ->
		    ResultList = farm_crop:pick_fruit(Laborer, Owner, Crop),
		    TDepot = farm_depot:read(Laborer),
		    {Depot, NCrop1, OwnerFarm1, ItemList, Money}=
			proc_pick_result(ResultList,TDepot,Crop, OwnerFarm, State),
		    NCrop2 = farm_crop:add_picker(Laborer, NCrop1),
		    OwnerFarm2 = set_crop(OwnerFarm1, PlotIndex, NCrop2), 
		    NFarm3 = 
			case Owner =:= Laborer of %% 只有采摘自己农场的果实才能获得经验
			    true -> 
				SeedID = farm_crop:get_crop_id(Crop),
				proc_work_awards(OwnerFarm2, pick, SeedID, PlotIndex, State);
			    false -> OwnerFarm2
			end,

		    case ItemList =/= [] of
			true ->
			    [case Type of
				 pick_item ->
				     EQPid = farm:get_eq_pid(State),
				     UserName = eq_manager:get_user_name(EQPid),
				     farm_log:write(extract, Owner, Laborer, 
						    item:get_tempid(Item), Count, UserName),
				     notify_farm_data(Laborer, Owner, NFarm3, [{PlotIndex, Count}]);
				 award_item ->
				     notify_farm_awards(Laborer, 1, Money, 
							[{item:get_tempid(Item), Count}]) 
			     end || {Type, Item, Count} <- ItemList];
			false ->
			    notify_farm_data(Laborer, Owner, NFarm3)
		    end,
		    case Laborer =:= Owner of
			true -> 
			    Farm4 = farm_coin:add_coin(Money, NFarm3),
			    db_farm:write(Farm4, Depot),
			    farm_coin:send_coin(Laborer, Money, farm_coin:get_coin(Farm4));
			false -> 
			    Farm4 = farm_coin:add_coin(Money, LaborerFarm),
			    db_farm:write(NFarm3, Depot),
			    db_farm:write(Farm4),
			    farm_coin:send_coin(Laborer, Money, farm_coin:get_coin(Farm4))
		    end,
		    %% 物品金钱后台记录
		    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, Money),
		    ItemInfo = [{item:get_tempid(Item), Count, item:get_instid(Item)} 
				|| {_Type, Item, Count} <- ItemList],
		    item_money_log:log(Laborer, farm_pick_crop, ItemInfo, MoneyInfo),

		    proc_farm_event(Laborer, ResultList);
		false ->
		    farm_sys_msg:send(Laborer, ?err_wallow_pick_crop)
	    end;
	{false, bite_dec_money} ->
	    case wallow:in_normal_time(State) of
		true ->
		    UserName = get_username(State),
		    Crop2 = farm_crop:add_picker(Laborer, Crop),
		    Farm2 = set_crop(OwnerFarm, PlotIndex, Crop2),
		    {Farm3, LaborerFarm1, Hurt} = 
			farm_pet:do_pet_bite(Farm2, LaborerFarm, Owner, Laborer, UserName, State),
		    farm_coin:send_coin(Laborer, Coin, farm_coin:get_coin(LaborerFarm1)),
		    db_farm:write(Farm3),
		    db_farm:write(LaborerFarm1),
		    notify_farm_data(Laborer, Owner, Farm3),

		    %% 物品金钱后台记录
		    item_money_log:log_money(Owner, farm_pet_bite, 
					     item_money_log:get_money_info(?GAMECOIN, Hurt)),
		    item_money_log:log_money(Laborer, farm_pet_bite, 
					     item_money_log:get_money_info(?GAMECOIN, -Hurt)),

		    farm_sys_msg:send(Laborer, ?msg_pet_bite, [Hurt]);
		false ->
		    farm_sys_msg:send(Laborer, ?err_wallow_pick_crop)
	    end;
	{false, bite} ->
	    case wallow:in_normal_time(State) of
		true ->
		    Crop2 = farm_crop:add_picker(Laborer, Crop),
		    Farm2 = set_crop(OwnerFarm, PlotIndex, Crop2),
		    db_farm:write(Farm2),
		    notify_farm_data(Laborer, Owner, Farm2),
		    farm_sys_msg:send(Laborer, ?msg_pet_bite, [0]);
		false ->
		    farm_sys_msg:send(Laborer, ?err_wallow_pick_crop)
	    end;
	{false, Reason} ->
	    farm_sys_msg:send(Laborer, Reason)
    end,
    {noreply, State};

%% 一键采摘 
handle_cast({#msg{src=Laborer}, #req_pick_all_crops{account=Owner1}}, State) ->
    Owner = list_to_atom(Owner1),
    TempFarm = get_farm_data(Owner),
    Farm = refresh_farm_state(TempFarm),
    LaborerFarm = get_farm_data(Laborer),
    LaborerCoin = farm_coin:get_coin(LaborerFarm),
    case can_pick_crops(Laborer, Owner, LaborerCoin, Farm) of
	true ->
	    case wallow:in_normal_time(State) of
		true ->
		    UserName = get_username(State),
		    {NFarm, LaborerFarm1, Money, PickList, Hint} = 
			do_pick_crops(Laborer, Owner, Farm, UserName, LaborerFarm, State),

		    %% 日志处理
		    L = [{Type, item:get_tempid(Item4), Count4} 
			 || {_Idx, _Crop3, ItemList4,_Exp} <- PickList,
			    {Type, Item4, Count4} <- ItemList4],
		    ItemList = calc_pick_items_count(L, []),
		    %% %% 根据防沉迷系统的要求，将摘取的果实数量减半
		    %% ItemList = [{Type, {Item, wallow:calc_income(Count, State)}} || {Type, {Item, Count}} <- ItemList1],
		    [farm_log:write(extract, Owner, Laborer, ItemID, Count4, UserName)
		     || {pick_item, ItemID, Count4} <- ItemList],

		    %% 通知客户端
		    PickList2 = [{Idx, Count2} || {Idx, _Crop2, ItemList2, _Exp} <- PickList,
						  {pick_item, _Item2, Count2} <- ItemList2],
		    AddExpList = [{Idx, Exp} || {Idx, _Crop2, _ItemList, Exp} <- PickList],

		    db_farm:write(NFarm),
		    case Owner == Laborer of
			false ->
			    farm_coin:send_coin(Laborer, LaborerFarm1),
			    db_farm:write(LaborerFarm1);
			true -> ok
		    end,

		    notify_farm_pick_all_result(Laborer, Owner, NFarm, PickList2, AddExpList, Hint),

		    %% 物品金钱后台记录
		    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, Money),
		    ItemInstID = 0,
		    ItemInfo = [{ItemTempID, Count, ItemInstID} 
				|| {_Type, ItemTempID, Count} <- ItemList],
		    item_money_log:log(Laborer, farm_pick_all_crop, ItemInfo, MoneyInfo),
		    AwardItems = [{ItemID, Count} || {award_item, ItemID, Count} <- ItemList],
		    notify_farm_awards(Laborer, 1, 0, AwardItems);
		false ->
		    farm_sys_msg:send(Laborer, ?err_wallow_pick_crop)
	    end;
	{false, Reason} ->
	    farm_sys_msg:send(Laborer, Reason)
    end,
    {noreply, State};

%% 除草
handle_cast({#msg{src=Laborer}, #req_weed{account=Owner1, plot_index=PlotIndex}}, State)->
    Owner = list_to_atom(Owner1),
    OwnTempFarm = get_farm_data(Owner),
    OwnFarm = refresh_farm_state(OwnTempFarm),
    Crop = get_crop(PlotIndex, OwnFarm),
    case farm_crop:can_clear_op_damage(Laborer, ?fms_grass, Crop) of
	true ->
	    %% 防沉迷提示
	    notify_wallow_message(Laborer, State),
	    NCrop = farm_crop:clear_crop_state(Laborer, ?fms_grass, Crop),
	    NOwnFarm = set_crop(OwnFarm, PlotIndex, NCrop),
	    case Laborer =:= Owner of
		true -> 
		    NFocGrass = do_farm_op_data(NOwnFarm#player_farm.foc_grass),
		    case NFocGrass#farm_op_data.count > 150 of
			true -> 
			    db_farm:write(NOwnFarm),
			    notify_farm_data(Laborer, Owner, NOwnFarm);
			false -> 
			    NOwnFarm1 = NOwnFarm#player_farm{foc_grass=NFocGrass},
			    NFarm2 = proc_work_awards(NOwnFarm1, weed, PlotIndex, State),
			    db_farm:write(NFarm2),
			    notify_farm_data(Laborer, Owner, NFarm2)
		    end;
		false ->
		    db_farm:write(NOwnFarm),
		    notify_farm_data(Laborer, Owner, NOwnFarm),

		    %% 帮别人会给金钱, 经验
		    LaborerFarm = get_farm_data(Laborer),
		    NFocGrass = do_farm_op_data(LaborerFarm#player_farm.foc_grass),
		    NLaborerFarm = LaborerFarm#player_farm{foc_grass=NFocGrass},
		    case NFocGrass#farm_op_data.count > 150 of
			true ->
			    ok;
			false ->
			    RewardMoney = wallow:calc_income(1, State),
			    Coin = farm_coin:add_coin(RewardMoney, get_coin(NLaborerFarm)),
			    NLaborerFarm2 = proc_work_awards(NLaborerFarm, weed, PlotIndex, State),
			    db_farm:write(set_coin(Coin, NLaborerFarm2)),
			    farm_coin:send_coin(Laborer, Coin),
			    %% 物品金钱后台记录
			    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, RewardMoney),
			    item_money_log:log_money(Laborer, farm_weed, MoneyInfo, Owner)
		    end

	    end,
	    %% 提示农场设置的内容
	    farm_setting:prompt(weed, Laborer, Owner),
	    EQPid = farm:get_eq_pid(State),
	    UserName = eq_manager:get_user_name(EQPid),
	    farm_log:write(help, Owner, Laborer, UserName),
	    {noreply, State};
	{false, Reason} ->
	    farm_sys_msg:send(Laborer,  Reason),
	    {noreply, State}
    end;

%% 除虫
handle_cast({#msg{src=Laborer}, #req_worm{account=Owner1, plot_index=PlotIndex}}, State) ->
    Owner = list_to_atom(Owner1),
    OwnTempFarm = get_farm_data(Owner),
    OwnFarm = refresh_farm_state(OwnTempFarm),
    Crop = get_crop(PlotIndex, OwnFarm),
    case farm_crop:can_clear_op_damage(Laborer, ?fms_pest, Crop) of
	true ->
	    %% 防沉迷提示
	    notify_wallow_message(Laborer, State),
	    NCrop = farm_crop:clear_crop_state(Laborer, ?fms_pest, Crop),
	    NOwnFarm = set_crop(OwnFarm, PlotIndex, NCrop),
	    case Laborer =:= Owner of
		true -> 
		    NFocPest = do_farm_op_data(NOwnFarm#player_farm.foc_pest),
		    case NFocPest#farm_op_data.count > 150 of
			true -> 				
			    db_farm:write(NOwnFarm),
			    notify_farm_data(Laborer, Owner, NOwnFarm);
			false ->
			    NOwnFarm1 = NOwnFarm#player_farm{foc_pest=NFocPest},
			    NFarm2 = proc_work_awards(NOwnFarm1, worm, PlotIndex, State),
			    db_farm:write(NFarm2),
			    notify_farm_data(Laborer, Owner, NFarm2)
		    end;
		false ->
		    %% 帮别人会给金钱
		    db_farm:write(NOwnFarm),
		    notify_farm_data(Laborer, Owner, NOwnFarm),

		    LaborerFarm = get_farm_data(Laborer),
		    NFocPest = do_farm_op_data(LaborerFarm#player_farm.foc_pest),
		    NLaborerFarm = LaborerFarm#player_farm{foc_pest=NFocPest},
		    case NFocPest#farm_op_data.count > 150 of
			true ->
			    ok;
			false ->
			    RewardMoney = wallow:calc_income(1, State),
			    Coin = farm_coin:add_coin(RewardMoney, get_coin(NLaborerFarm)),
			    NLaborerFarm2 = proc_work_awards(NLaborerFarm, worm, PlotIndex, State),
			    db_farm:write(set_coin(Coin, NLaborerFarm2)),
			    farm_coin:send_coin(Laborer, Coin),
			    %% 物品金钱后台记录
			    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, RewardMoney),
			    item_money_log:log_money(Laborer, farm_worm, MoneyInfo, Owner)
		    end
	    end,
	    %% 提示农场设置的内容
	    farm_setting:prompt(worm, Laborer, Owner),
	    EQPid = farm:get_eq_pid(State),
	    UserName = eq_manager:get_user_name(EQPid),
	    farm_log:write(help, Owner, Laborer, UserName),
	    {noreply, State};
	{false, Reason} ->
	    farm_sys_msg:send(Laborer, Reason),
	    {noreply, State}
    end;

%% 浇水
handle_cast({#msg{src=Laborer}, #req_water{account=Owner1, plot_index=PlotIndex}}, State)->
    Owner = list_to_atom(Owner1),
    TempFarm = get_farm_data(Owner),
    Farm = refresh_farm_state(TempFarm),
    Crop = get_crop(PlotIndex, Farm),
    case farm_crop:has_crop_state(?fms_drought, Crop) of
	true -> 
	    %% 防沉迷提示
	    notify_wallow_message(Laborer, State),
	    NCrop = farm_crop:clear_drought_state(Crop),
	    OwnFarm = set_crop(Farm, PlotIndex, NCrop),
	    case Laborer =:= Owner of
		true ->
		    NOwnFarm = proc_work_awards(OwnFarm, water, PlotIndex, State),
		    db_farm:write(NOwnFarm),
		    notify_farm_data(Laborer, Owner, NOwnFarm);
		false ->
		    %% 帮别人会给金钱
		    db_farm:write(OwnFarm),
		    notify_farm_data(Laborer, Owner, OwnFarm),
		    LaborerFarm = get_farm_data(Laborer),
		    RewardMoney = wallow:calc_income(1, State),
		    Coin = farm_coin:add_coin(RewardMoney, get_coin(LaborerFarm)),
		    NLaborerFarm = proc_work_awards(LaborerFarm, water, PlotIndex, State),
		    db_farm:write(set_coin(Coin, NLaborerFarm)),
		    farm_coin:send_coin(Laborer, Coin),
		    %% 物品金钱后台记录
		    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, RewardMoney),
		    item_money_log:log_money(Laborer, farm_water, MoneyInfo, Owner) 
	    end,
	    %% 提示农场设置的内容
	    farm_setting:prompt(water, Laborer, Owner),
	    EQPid = farm:get_eq_pid(State),
	    UserName = eq_manager:get_user_name(EQPid),
	    farm_log:write(help, Owner, Laborer, UserName),
	    {noreply, State};
	false -> 
	    %% 通知客户端不需要浇水
	    farm_sys_msg:send(Laborer, ?err_farm_no_drought),
	    {noreply, State}
    end;

%% 放草
handle_cast({#msg{src=Account}, #req_put_grass{account=Owner1, plot_index=Index}}, State)->
    Owner = list_to_atom(Owner1),
    case Owner =/= Account of
	true ->
	    TempFarm = get_farm_data(Owner),
	    Farm = refresh_farm_state(TempFarm),
	    NotAllowStage = [picking_stage, die_stage],
	    GuestFarm = get_farm_data(Account),
	    OpCount = get_farm_op_count(GuestFarm#player_farm.fop_grass),
	    Crop = get_crop(Index, Farm),
	    case farm_crop:put_damage(Account, Crop,
				      ?fms_grass, NotAllowStage, OpCount) of
		{true, NCrop} ->
		    NFarm = set_crop(Farm, Index, NCrop),
		    db_farm:write(NFarm),
		    notify_farm_data(Account, Owner, NFarm),

		    FopGrass = do_farm_op_data(GuestFarm#player_farm.fop_grass),
		    NGuestFarm = GuestFarm#player_farm{fop_grass=FopGrass},
		    db_farm:write(NGuestFarm),

		    %% 提示农场设置的内容
		    farm_setting:prompt(put_grass, Account, Owner),
		    EQPid = farm:get_eq_pid(State),
		    UserName = eq_manager:get_user_name(EQPid),
		    farm_log:write(demage, Owner, Account, UserName);
		{false, Reason} -> 
		    farm_sys_msg:send(Account,  Reason)
	    end
    end,
    {noreply, State}; 

%% 放虫
handle_cast({#msg{src=Account}, #req_put_pest{account=Owner1, plot_index=Index}}, State) ->
    Owner = list_to_atom(Owner1),
    case Owner =/= Account of
	true ->
	    TempFarm = get_farm_data(Owner),
	    Farm = refresh_farm_state(TempFarm),
	    NotAllowStage = [sprouting_stage, picking_stage, die_stage],
	    GuestFarm = get_farm_data(Account),
	    OpCount = get_farm_op_count(GuestFarm#player_farm.fop_pest),
	    Crop = get_crop(Index, Farm),
	    case farm_crop:put_damage(Account, Crop,
				      ?fms_pest, NotAllowStage, OpCount) of
		{true, NCrop} ->
		    NFarm = set_crop(Farm, Index, NCrop),
		    db_farm:write(NFarm),
		    notify_farm_data(Account, Owner, NFarm),

		    FopPest = do_farm_op_data(GuestFarm#player_farm.fop_pest),
		    NGuestFarm = GuestFarm#player_farm{fop_pest=FopPest},
		    db_farm:write(NGuestFarm),

		    %% 提示农场设置的内容
		    farm_setting:prompt(put_pest, Account, Owner),
		    EQPid = farm:get_eq_pid(State),
		    UserName = eq_manager:get_user_name(EQPid),
		    farm_log:write(demage, Owner, Account, UserName);
		{false, Reason} -> 
		    farm_sys_msg:send(Account, Reason)
	    end
    end,
    {noreply, State};

%% 锄地
handle_cast({#msg{src=Account}, #req_hoeing{plot_index=PlotIndex}}, State) ->
    TempFarm = get_farm_data(Account),
    Farm = refresh_farm_state(TempFarm),
    Crop = get_crop(PlotIndex, Farm),
    case farm_hoeing:can_hoeing(Crop) of
	true -> 
	    %% 防沉迷提示
	    notify_wallow_message(Account, State),
	    Level = farm_exp:get_level(Farm#player_farm.exp),
	    {NCrop, ItemID, Count} = farm_hoeing:do_hoeing(Account, Level, Crop, State),
	    NFarm = set_crop(Farm, PlotIndex, NCrop),
	    NFarm1 = case farm_crop:is_crop_dead(Crop) of
			 true ->
			     proc_work_awards(NFarm, hoeing, PlotIndex, State);
			 false ->
			     NFarm
		     end,
	    db_farm:write(NFarm1),
	    notify_farm_data(Account, Account, NFarm1),
	    case ItemID of
		0 -> ok;
		_ ->
		    %% TODO 因为可堆叠，物品实例ID可能被合并
		    %% 金钱物品记录
		    ItemInstID = 0,
		    ItemInfo = {ItemID, 
				Count,
				ItemInstID},
		    item_money_log:log_item(Account, farm_hoeing_award, ItemInfo),

		    notify_farm_awards(Account, 2, 0, [{ItemID, Count}])
	    end;
	{false, Reason} ->
	    farm_sys_msg:send(Account,  Reason)
    end,
    {noreply, State};

%% 询问能否升级到红土地
handle_cast({#msg{src=Account}, #req_can_upgrade_land{}}, State) ->
    TempFarm = get_farm_data(Account),
    Farm = refresh_farm_state(TempFarm),
    Level = farm_exp:get_level(Farm#player_farm.exp),
    Money = Farm#player_farm.money,
    {_, Reason, Params} = 
	farm_land:can_upgrade(Level, Money, ?max_plot_num, Farm#player_farm.crops),
    farm_sys_msg:send(Account, Reason, Params),
    {noreply, State};

%% 升级红土地
handle_cast({#msg{src=Account}, #req_upgrade_land{}}, State) ->
    TempFarm = get_farm_data(Account),
    Farm = refresh_farm_state(TempFarm),
    Level = farm_exp:get_level(Farm#player_farm.exp),
    Money = Farm#player_farm.money,
    case farm_land:can_upgrade(Level, Money, ?max_plot_num, Farm#player_farm.crops) of
	{true, _, _} ->
	    {NCrops, NMoney} = farm_land:upgrade(?max_plot_num, Money, Farm#player_farm.crops),
	    NFarm = set_crop_list(NCrops, Farm),
	    NFarm1 = set_coin(NMoney, NFarm),
	    db_farm:write(NFarm1),
	    farm_coin:send_coin(Account, NFarm1),
	    
	    %% 后台金钱物品记录
	    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, NMoney - Money),
	    item_money_log:log_money(Account, upgrade_land, MoneyInfo),
	    
	    notify_farm_data(Account, Account, NFarm1);
	{false, Reason, Params} ->
	    farm_sys_msg:send(Account, Reason, Params)
    end,
    {noreply, State};

handle_cast({#msg{src=Account}, #req_open_ui{type=Type}}, State)->
    case Type of
	?uit_farmdepot ->
	    farm_depot:open_depot(Account)
    end,
    net_helper:farm2client(Account, #notify_open_ui{type=Type}),
    {noreply, State};

handle_cast({#msg{src=Account}, #req_gm_command{type=Type,params=Params}}, State)->
    BasicData=db_common:select_one(player_basic_data, Account),
    #player_basic_data{gm_enabled=GMEnabled}=BasicData,
    case GMEnabled of
	?gm_enable ->
	    NewState = 
		case Type of
		    ?gct_add_farm_item ->
			do_add_farm_item_cmd(Account, Params, State);
		    ?gct_get_money ->
			do_get_money_cmd(Account, Params, State)
		end,
	    {noreply, NewState};
	?gm_disable ->
	    {noreply, State}
    end;

%% 请求EQ用户信息(批量请求)
handle_cast({_Msg, #req_eq_users_info{account_list=AccountList}}, 
	    State) ->
    EQPid = farm:get_eq_pid(State),
    case eq_manager:is_login(EQPid) of
	true ->	    
	    %% 请求35EQ用户信息
	    [eq_manager:req_vcard(EQPid, "vcard_" ++ Who, Who) || Who <- AccountList];
	_ -> 
	    ok
    end,

    {noreply, State}.


%% 增加农场仓库物品
do_add_farm_item_cmd(Account, [ItemTempID], State)->
    Item = item:make_item(list_to_integer(ItemTempID)),   
    NDepot = farm_depot:add_item(Account, Item),
    farm_depot:save(NDepot),
    farm_depot:send_depot(Account, NDepot),
    State.

%% 获得金钱
do_get_money_cmd(Account, [_Type, Coin], State)->
    Farm = get_farm_data(Account),
    NFarm = farm_coin:add_coin(list_to_integer(Coin), Farm),
    db_farm:write(NFarm),
    farm_coin:send_coin(Account, NFarm),
    State.


%%-------------------------private--------------------------------
notify_wallow_message(Laborer, State) ->
    case wallow:in_warn_time(State) of
	true ->
	    farm_sys_msg:send(Laborer, ?err_wallow_warn_time);
	false ->
	    case wallow:in_bad_time(State) of
		true ->
		    farm_sys_msg:send(Laborer, ?err_wallow_bad_time);
		false ->
		    ok
	    end
    end.

%% 初始化农场
do_init_farm(Owner, Owner) ->
    case db_farm:select(Owner) of
	[] -> 
	    new(Owner),
	    do_init_farm(Owner, Owner);
	Farm -> Farm
    end;
do_init_farm(Owner, _Guest) ->
    case db_farm:select(Owner) of
	[] -> erlang:error({no_farm, Owner});
	Farm -> Farm
    end.

%% 进入农场的操作
do_enter_farm(Owner, Guest, Farm, State) ->
    NFarm = refresh_farm_state(Farm),
    Decoration = get_decoration(NFarm),
    {NDecoration, Depot} = farm_decoration:recycle(Owner, Decoration),
    NNFarm = set_decoration(NFarm, NDecoration),
    db_farm:write(NNFarm, Depot),
    farm_depot:send_depot(Guest, Depot),
    notify_farm_data(Guest, Owner, NNFarm),
    notify_user_info(Guest, Owner, State),

    net_helper:farm2client(Guest, #notify_finish_enter_scene{}),    
    case Owner =:= Guest of
	true ->
	    farm_coin:send_coin(Guest, NNFarm),
	    farm_coin:send_eq_coin(Guest);
	false ->
	    GuestFarm = get_farm_data(Guest),
	    farm_coin:send_coin(Guest, GuestFarm)
    end.

do_farm_task(Owner, Guest, State) ->
    case Owner == Guest of
	true ->
	    FarmTask = db_common:select_one(farm_task, Owner),
	    NState = farm:set_task(FarmTask, State),
	    farm_task:get_task_list(NState),
	    NState;
	false ->
	    State
    end.


get_farm_data(Owner) ->
    case db_farm:select(Owner) of
	[] -> erlang:error({not_found, Owner});
	Farm -> Farm
    end.

%% 计算农场状态
refresh_farm_state(#player_farm{crops=Crops}=Farm) ->
    NCrops = [farm_crop:refresh_crop_fruit(farm_crop:refresh_crop_state(Crop)) || Crop <- Crops],
    Farm#player_farm{crops=NCrops}.

%% 获得农场当前的状态
get_farm_state(Guest, #player_farm{account=Owner, crops=Crops}) ->
    State = [farm_crop:get_crop_state(Guest, Owner, Crop) 
	     || Crop <- Crops, farm_crop:is_crop(Crop)],
    case lists:sort(State) of
	[] -> ?fms_normal;
	[Dmg|_] -> Dmg
    end.

%% 开垦地块
assart_plot(#player_farm{crops=CropList}=Farm)->
    Farm#player_farm{crops=lists:append(CropList, [farm_crop:new_crop()])}.

%% 获得地块的数量
get_plot_num(#player_farm{crops=CropList}) ->
    length(CropList).

%% 判断地块是否是空的(上面没有农作物)
is_plot_empty(Farm, PlotIndex)->
    Crop = get_crop(PlotIndex, Farm),
    farm_crop:get_crop_id(Crop) =:= 0.

%% 播种
can_sow(Farm, PlotIndex, Exp, UseLevel, Redland) ->
    case farm_exp:get_level(Exp) >= UseLevel of
	true ->
	    case is_plot_empty(Farm, PlotIndex) of
		true ->
		    Crop = get_crop(PlotIndex, Farm),
		    case farm_land:can_sow(Redland, Crop) of
			true -> true; 
			false -> {error, ?msg_need_redland}
		    end;
		false ->
		    {error, ?err_plot_not_empty}
	    end;
	false ->
	    {error, ?err_not_enough_level}
    end.
 
sow(#player_farm{exp=Exp}=Farm, PlotIndex, TempID, 
    #farm_seed_tplt{use_level=UL, redland=Redland}=SeedTplt) 
  when PlotIndex > 0 ->
    case can_sow(Farm, PlotIndex, Exp, UL, Redland) of
	true ->
	    DamageList = farm_damage:make_all_damage(SeedTplt),
	    Crop = get_crop(PlotIndex, Farm),
	    NCrop = farm_crop:new_crop(TempID, DamageList, Crop),
	    NFarm = set_crop(Farm, PlotIndex, NCrop),
	    {ok, NFarm};
	Error -> Error
    end;
sow(_Farm, PlotIndex, _TempID, SeedTplt) ->
    erlang:error({badarg, PlotIndex, SeedTplt}).

%% 获得种子的数据
get_seed_data(TempID)->
    #item_tplt{sub_id=SeedID} = tplt:get_data(item_tplt, TempID),
    case tplt:get_data(farm_seed_tplt, SeedID) of
	empty -> erlang:error({not_found, TempID, SeedID});
	Data -> Data
    end.

notify_user_info(Owner, Owner, State) ->
    EQPid = farm:get_eq_pid(State),
    UserInfo = eq_manager:get_user_info(EQPid),
    net_helper:farm2client(Owner, #notify_eq_user_info{account=Owner, info=UserInfo});
notify_user_info(_Guest, Owner, State) ->
    EQPid = farm:get_eq_pid(State),
    Who = atom_to_list(Owner),
    eq_manager:req_vcard(EQPid, "vcard_" ++ Who, Who).

%% 通知数据给客户端
notify_farm_data(Account, Owner, Farm)->
    notify_farm_data(Account, Owner, Farm, []).

make_notify_farm_data(Account, Owner, #player_farm{crops=Crops}=Farm, PickList) ->
    NCrops = translate(Account, Owner, Crops, PickList),
    #player_farm_data{
		       account= Farm#player_farm.account,
		       exp = farm_exp:get_curr_exp(Farm#player_farm.exp),
		       decoration = Farm#player_farm.decoration,
		       pet_food = Farm#player_farm.pet_food,
		       crops=NCrops, 
		       level=farm_exp:get_level(Farm#player_farm.exp)
		     }.

%% 通知数据给客户端, PickList=[{Crop, PickNum} |...]
notify_farm_data(Account, Owner, Farm, PickList)->
    Packet = make_notify_farm_data(Account, Owner, Farm, PickList),
    net_helper:farm2client(Account, #notify_farm_data{data = Packet}).

notify_farm_pick_all_result(Account, Owner, Farm, PickList, AddExpList, Hint)->
    Packet = make_notify_farm_data(Account, Owner, Farm, PickList),
    AddExpList2 = [#farm_exp_data{plot_index=Index, add_exp=Exp} || {Index, Exp} <- AddExpList],
    net_helper:farm2client(Account, #farm_pick_all_result{farm_data = Packet,
							  exp_data = AddExpList2,
							  hint = Hint}).

notify_farm_awards(Account, Type, Money, ItemDataList) ->
    ItemList = [ItemID || {ItemID, _} <- ItemDataList],
    CountList = [Count || {_, Count} <- ItemDataList],
    Packet = #notify_farm_awards{type=Type, money = Money, 
				 itemlist=ItemList, countlist=CountList},
    net_helper:farm2client(Account, Packet).

translate(Account, Owner, Crops, PickList) ->
    F= fun(Crop, {Index, List}) ->
	       {Index + 1, 
		[case [Count || {PlotIndex, Count} <- PickList, Index =:= PlotIndex] of
		    [] -> 
			farm_crop:translate(Account, Owner, Crop, 0);
		    [PickNum] -> 
			farm_crop:translate(Account, Owner, Crop, PickNum)
		 end | List]}
       end,
    {_Idx, List2} = lists:foldl(F, {1, []}, Crops),
    lists:reverse(List2).
    

%% 能否开垦
can_assart(Level, PlotNum, Coin) ->
    case tplt:get_data(farm_assart_tplt, (PlotNum-?init_plot_num) + 1) of
	#farm_assart_tplt{money=Money, level=VLevel} ->
	    case Level >= VLevel of
		true ->
		    case Money > Coin of
			true -> {false, ?err_cannot_assart, Money, VLevel};
			false -> {true, Money}
		    end;
		false ->
		    {false, ?err_cannot_assart, Money, VLevel}
	    end;
	_ -> {false, ?err_cannot_assart, 0, 0}
    end.

%% 处理劳动的奖励, 并且通知客户端
proc_work_awards(LaborerFarm, Type, PlotIndex, State) ->
    proc_work_awards(LaborerFarm, Type, 0, PlotIndex, State).

proc_work_awards(#player_farm{account=Laborer}=LaborerFarm, Type, SeedID, PlotIndex, State)->
    Exp = LaborerFarm#player_farm.exp,
    NFarm = LaborerFarm#player_farm{exp=Exp + wallow:calc_income(farm_exp:get_work_exp(Exp, Type, SeedID), State)},
    Packet = #notify_farm_info{account=Laborer,
			       plot_index=PlotIndex,
			       exp=farm_exp:get_curr_exp(NFarm#player_farm.exp),
			       level=farm_exp:get_level(NFarm#player_farm.exp),
			       add_exp=NFarm#player_farm.exp-LaborerFarm#player_farm.exp},
    net_helper:farm2client(Laborer, Packet), 
    NFarm.



get_date()->
    {Y, M, D} = datetime:date(),
    datetime:make_time(Y, M, D, 0, 0, 0).

get_farm_op_count(#farm_op_data{count=0}) ->
    0;
get_farm_op_count(#farm_op_data{time=Time, count=Count}) ->
    case datetime:diff_time(Time, datetime:localtime()) >= 24*3600 of
	true -> 0;
	false -> Count
    end.

do_farm_op_data(#farm_op_data{count=0}) ->
    #farm_op_data{time=get_date(), count=1};
do_farm_op_data(#farm_op_data{time=Time, count=Count}) ->
    case datetime:diff_time(Time, datetime:localtime()) >= 24*3600 of
	true -> #farm_op_data{time=get_date(), count=1};
	false -> #farm_op_data{time=get_date(), count=Count + 1}
    end.

can_pick_crops(Account, Owner, LaborerCoin, #player_farm{crops=Crops}=Farm)->
    Result = [case farm_crop:can_pick_fruit(Account, Owner, LaborerCoin, Crop, Farm) of
		  {false, ?err_not_crop_pick} -> false;
		  {false, ?err_picked} -> false;
		  {false, ?err_not_remain_fruit} -> false;
		  {false, ?err_cannot_pick} -> false;
		  {false, _Reason} ->true;
		  true -> true
	      end || Crop <- Crops],
    case [R || R <- Result, R == true] of
	[] -> {false, ?err_not_crop_pick};
	_ -> true
    end.

do_pick_crops(Laborer, Owner, #player_farm{crops=Crops, exp=Exp}=Farm, UserName, LaborerFarm, State) ->
    F = fun(Crop, {Index, NDepot, NFarm, PickList, AwardedList, 
		   TotalMoney, Hint, TotalHurt, LaborerFarm1}) ->
		LaborerCoin = farm_coin:get_coin(LaborerFarm1),
		case farm_crop:can_pick_fruit(Laborer, Owner, LaborerCoin, Crop, NFarm) of
		    true ->
			ResultList = farm_crop:pick_fruit(Laborer, Owner, AwardedList, Crop), 
			%% %% 根据防沉迷系统的要求，将摘取的果实数量减半
			%% ItemList = [{Type, {Item, wallow:calc_income(Count, State)}} || {Type, {Item, Count}} <- ItemList1],
			{Depot, NCrop, NFarm2, ItemList, Money}=
			    proc_pick_result(ResultList,NDepot,Crop,NFarm, State),
			AddExp1 = 
			    case Owner =:= Laborer of %% 只有采摘自己农场的果实才能获得经验
				true -> 
				    SeedID = farm_crop:get_crop_id(Crop),
				    farm_exp:get_work_exp(Exp, pick, SeedID);
				false -> 0
			    end,
			AddExp = wallow:calc_income(AddExp1, State),
			NCrop2 = farm_crop:add_picker(Laborer, NCrop),
			NFarm3 = set_crop(NFarm2, Index, NCrop2),
			L1 = [L || {awarded, L} <- ResultList],
			%%L1 = [{Item, wallow:calc_income(Count, State)} || {awarded, {Item, Count}} <- ResultList],
			NAwardedList = lists:flatten(L1) ++ AwardedList,
			NFarm4 = NFarm3#player_farm{exp=NFarm3#player_farm.exp+AddExp},
			Hint1 = case do_proc_farm_event(ResultList) of
				    ok -> Hint;
				    Msg -> [{farm_hint_data, Index, Msg, 0, 0} | Hint]
				end,
			NPickList = [{Index, NCrop2, ItemList, AddExp} | PickList],
			LaborerFarm2 = farm_coin:add_coin(Money, LaborerFarm1),
			{Index + 1, Depot, NFarm4, NPickList, NAwardedList, 
			 TotalMoney + Money, Hint1, TotalHurt, LaborerFarm2};
		    {false, bite_dec_money} ->
			{Farm4, LaborerFarm2, Hurt}=
			    farm_pet:do_pet_bite(NFarm, LaborerFarm1, Owner, Laborer, UserName, State),
			Hint1 = [{farm_hint_data, Index, ?msg_pet_bite, Hurt, 0} | Hint],
			Crop2 = farm_crop:add_picker(Laborer, Crop),
			Farm5 = set_crop(Farm4, Index, Crop2),
			{Index + 1, NDepot, Farm5, PickList, 
			 AwardedList, TotalMoney, Hint1, TotalHurt + Hurt, LaborerFarm2};
		    {false, bite} ->
			Hint1 = [{farm_hint_data, Index, ?msg_pet_bite, 0, 0} | Hint],
			Crop2 = farm_crop:add_picker(Laborer, Crop),
			Farm5 = set_crop(NFarm, Index, Crop2),
			{Index + 1, NDepot, Farm5, PickList, 
			 AwardedList, TotalMoney, Hint1, TotalHurt, LaborerFarm1};
		    {false, ?msg_not_money_cannot_steal} ->
			Hint1 = [{farm_hint_data, Index,?msg_not_money_cannot_steal,0,0} | Hint],
			Crop2 = farm_crop:add_picker(Laborer, Crop),
			Farm5 = set_crop(NFarm, Index, Crop2),
			{Index + 1, NDepot, Farm5, PickList, 
			 AwardedList, TotalMoney, Hint1, TotalHurt, LaborerFarm1};
		    {false, _Reason} ->
			%% Hint1 = [{farm_hint_data, Index, Reason, 0, 0} | Hint],
			{Index + 1, NDepot, NFarm, PickList, 
			 AwardedList, TotalMoney, Hint, TotalHurt, LaborerFarm1}
		end
	end,
    TDepot = farm_depot:read(Laborer),
    {_Idx, NDepot, Farm2, PickList2, _, TotalMoney, Hint1, TotalHurt1, LaborerFarm4} = 
	lists:foldl(F, {1, TDepot, Farm, [], [], 0, [], 0, LaborerFarm}, Crops),
    farm_depot:save(NDepot),
    Count = lists:sum([1 || H <- Hint1, H#farm_hint_data.hint_id == ?msg_pet_bite]),
    Hint2 = case Count > 0 of
		true ->
		    [{farm_hint_data, -1, ?msg_bited_dec_money, Count, TotalHurt1} | Hint1];
		false ->
		    Hint1
	    end,
    {Farm2, LaborerFarm4, TotalMoney, PickList2, lists:reverse(Hint2)}.

%% 把相同的物品的数量归为一堆
calc_pick_items_count([], ItemsList) ->
    ItemsList;
calc_pick_items_count([{Type, ItemID, _Count} | _]=L, ItemsList) ->
    {L1, L2} = lists:partition(fun({_T, ID, _C})->  
				       ItemID =:= ID
			       end, L),
    NL1 = [C || {_, _, C}<- L1],
    NItemsList = [{Type, ItemID, lists:sum(NL1)} | ItemsList],
    calc_pick_items_count(L2, NItemsList).
  

proc_pick_result(PickResultList, Depot, Crop, Farm, State) ->
    do_proc_pick_result(PickResultList, Depot, Crop, Farm, [], 0, State).

do_proc_pick_result([], Depot, Crop, Farm, ItemList, Money, _State) ->
    {Depot, Crop, Farm, ItemList, Money};
do_proc_pick_result([{pick_item, {Item, Count}} | Rest],Depot, Crop, 
		    Farm, ItemList, Money, State) ->
    NDepot = farm_depot:add_item(Depot, Item, wallow:calc_income(Count,State)),
    NCrop = farm_crop:dec_remain(Crop, Count),
    NItemList = [{pick_item, Item, Count} | ItemList],
    do_proc_pick_result(Rest, NDepot, NCrop, Farm, NItemList, Money, State);
do_proc_pick_result([{award_item, {Item, Count}} | Rest],Depot, Crop, 
		    Farm, ItemList, Money, State) ->
    NDepot = farm_depot:add_item(Depot, Item, wallow:calc_income(Count, State)),
    NItemList = [{award_item, Item, Count} | ItemList],
    do_proc_pick_result(Rest, NDepot, Crop, Farm, NItemList, Money, State);
do_proc_pick_result([{coin, Coin} | Rest], Depot, Crop, Farm, ItemList, Money, State) ->
    do_proc_pick_result(Rest, Depot, Crop, Farm, ItemList, Money + Coin, State);
do_proc_pick_result([{_Type, _Data} | Rest], Depot, Crop, Farm, ItemList, Money, State) ->
    do_proc_pick_result(Rest, Depot, Crop, Farm, ItemList, Money, State).  

proc_farm_event(Laborer, ResultList) ->
    case do_proc_farm_event(ResultList) of
	ok -> ok;
	Msg -> farm_sys_msg:send(Laborer, Msg)
    end.

do_proc_farm_event(ResultList) ->
    case [Coin || {coin, Coin} <- ResultList] of
	[] -> ok;
	[Coin1] -> 
	    case Coin1 > 0 of
		true -> ?msg_farm_event_2;
		false -> ?msg_farm_event_1
	    end
    end.

%% 处理35EQ回复
%% 返回NewState
process_eq_message("list", _RawWh, _Packet, State) ->
    EQPid = farm:get_eq_pid(State),
    FriendList = eq_manager:get_friend_list(EQPid),
    Owner = farm:get_account(State),
    do_notify_eq_friend_list(Owner, FriendList, State);
process_eq_message("vcard", [$_ | Who], Packet, State) ->
    UserInfo = eq_manager:extract_user_info(Packet),
    case UserInfo of
	#eq_user_info{} ->
	    NotifyPacket = #notify_eq_user_info{account=Who, info=UserInfo},
	    Owner = farm:get_account(State),
	    net_helper:farm2client(Owner, NotifyPacket);
	_ ->
	    ok
    end,
    State.


do_notify_eq_friend_list(Owner, EQFriendList, State) when is_atom(Owner) ->
    {Friends, _Names} = lists:unzip(EQFriendList),
    DBFriends = db_friends:get_friends(Owner),
    NewFriends = merage_friend_list(DBFriends, Friends),
    notify_friends_farm_status(Owner, NewFriends, EQFriendList, State),
    State.

notify_friends_farm_status(Owner, Friends, EQFriendList, State) when is_list(EQFriendList) ->
    %% 农场的好友需包含自已 
    EQPid = farm:get_eq_pid(State),
    UserName = eq_manager:get_user_name(EQPid),
    NewFriendList = [{Owner, UserName} |  EQFriendList],

    %% TODO: 事务方式批量读取数据, 可能存在潜在的性能瓶颈
    Farms = db_farm:select([Owner | Friends]),
    StatusList = [begin
		      Acc = FarmData#player_farm.account,
		      FriendName =
			  case lists:keyfind(Acc, 1, NewFriendList) of
			      {Acc, FriendName0} when length(FriendName0) > 0 -> 
				  FriendName0;
			      _ -> 
				  atom_to_list(Acc)
			  end,
		      
		      #farm_status{account = Acc,
				   name = FriendName,
				   exp = FarmData#player_farm.exp,
				   status = get_farm_state(Owner, refresh_farm_state(FarmData))}  
		  end
		  || FarmData <- Farms, FarmData =/= []],
    %% 通知客户端
    net_helper:farm2client(Owner, #notify_friend_farm_status{data=StatusList}).


merage_friend_list(OldFriends, EQFriends) ->
    F = fun(X, Acc) -> 
		case lists:member(X, OldFriends) of
		    true ->
			Acc;
		    _ ->
			[X | Acc]
		end
	end,
    lists:foldl(F, OldFriends, EQFriends).


get_username(State) ->
    EQPid = farm:get_eq_pid(State),
    eq_manager:get_user_name(EQPid).

get_friend_list(Owner, State) ->
    EQPid = farm:get_eq_pid(State),
    case eq_manager:is_login(EQPid) of
	true ->
	    eq_manager:req_friend_list(EQPid),

	    %% 发布个性签名
	    %% (目前自已的个性签名为空)
	    Signature = "",
	    eq_manager:broadcast_signature(EQPid, Signature);
	_ -> 
	    do_notify_eq_friend_list(Owner, [], State)
    end.

%%------------------------test------------------------------------
-include_lib("eunit/include/eunit.hrl").
-ifdef(TEST).

calc_pick_items_count_test() ->
    PickList = [{pick_item, 1,1}, {pick_item, 1,3}, {pick_item, 1,5},
		{pick_item, 2,2}, {pick_item, 5,3}, {pick_item, 2,3}],
    ?assertEqual([{pick_item, 5,3}, {pick_item, 2,5}, {pick_item, 1,9}], 
		 calc_pick_items_count(PickList, [])),
    ?assertEqual([], calc_pick_items_count([], [])),
    ok.

-endif.
