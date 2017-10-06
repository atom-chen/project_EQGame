-module(player_data).

-include("packet_def.hrl"). 
-include("player_data.hrl").
-include("enum_def.hrl").

-compile(export_all).

new()->
    #player_data{}.


get_account(#player_data{account=Account}) ->
    Account.

get_name(#player_data{basic_data=BD}) ->
    basic_data:get_data(username, BD).

get_player_basic_data(#player_data{basic_data=BasicData})->
    BasicData.

get_onlinetime(#player_data{basic_data=BasicData}) ->
    BasicData#player_basic_data.online_time.

get_login_time(#player_data{login_time=LoginTime}) ->
    LoginTime.

%% 获得玩家身上的货币
get_coin(#player_data{player_coin=Coin})->
   Coin. 

get_property(#player_data{player_property=PlayerProperty}) ->
    PlayerProperty.

set_property(PlayerProperty, PlayerData) ->
    PlayerData#player_data{player_property=PlayerProperty}.

%% 设置玩家身上的货币
set_coin(Coin, PlayerData)->
    PlayerData#player_data{player_coin=Coin}.

get_scene_name(#player_data{scene_name=SceneName}) ->
    SceneName.

change_scene(NewSceneName, NewPos, PlayerData) ->
    PlayerData#player_data{scene_name=NewSceneName, 
					   curr_pos=NewPos,
					   dest_pos=NewPos}.


is_at_home(#player_data{account=Account, scene_name=SceneName}) ->
    AtHome = (house:get_house_name(Account) =:= SceneName),
    AtHome.

%% 玩家当前动作
get_actions(#player_data{actions=Actions}) ->
    Actions.

set_actions(NewActions, PlayerData) when is_list(NewActions) ->
    PlayerData#player_data{actions=NewActions}.


%% 交易缓存数据
get_trade_info(#player_data{trade_info=TradeInfo}) ->
    TradeInfo.

set_trade_info(TradeInfo, PlayerData) ->
    PlayerData#player_data{trade_info=TradeInfo}.


%% 邀请缓存数据
get_invite_info(#player_data{invite_info=InviteInfo}) ->
    InviteInfo.

set_invite_info(InviteInfo, PlayerData) ->
    PlayerData#player_data{invite_info=InviteInfo}.

%% 玩家当前位置
get_pos(#player_data{curr_pos=Pos}) ->
    Pos.

set_pos(Pos, PlayerData) ->
    PlayerData#player_data{curr_pos=Pos}.

set_dest_pos(Pos, PlayerData) ->
    PlayerData#player_data{dest_pos=Pos}.

%% 打工信息
get_work_info(#player_data{work_info=WorkInfo}) ->
    WorkInfo.

set_work_info(WorkInfo, PlayerData) ->
    PlayerData#player_data{work_info=WorkInfo}.

%% 动作表情信息
get_body_action_info(#player_data{body_action_info=Info}) ->
    Info.

set_body_action_info(Info, PlayerData) ->
    PlayerData#player_data{body_action_info=Info}.


% 坐，躺等状态
get_body_state(#player_data{body_state=BodyState}) ->
    BodyState.

set_body_state(BodyState, PlayerData) ->
    PlayerData#player_data{body_state=BodyState}.


%% 邮件数
get_mail_count(#player_data{mail_count=MailCount}) ->
    MailCount.

set_mail_count(MailCount, PlayerData) ->
    PlayerData#player_data{mail_count=MailCount}.


%% 家政信息
get_domestic_info(#player_data{domestic_info=DomesticInfo}) ->
    DomesticInfo.

set_domestic_info(DomesticInfo, PlayerData) ->
    PlayerData#player_data{domestic_info=DomesticInfo}.

%% 基础数据
get_basic_data(#player_data{basic_data=BasicData}) ->
    BasicData.
set_basic_data(BasicData, PlayerData) ->
    PlayerData#player_data{basic_data=BasicData}.

%% 获得玩家的pack
get_pack(?pt_bag, #player_data{bag_grids=BagGrids})->
    BagGrids;
get_pack(?pt_body, #player_data{body_grids=BodyGrids}) ->
    BodyGrids;
get_pack(?pt_depot, #player_data{depot=Depot}) ->
    player_depot:get_pack(Depot);
get_pack(?pt_gift_bag, #player_data{gift=PlayerGift}) ->
    SendBox = player_gift_data:get_send_box(PlayerGift),
    player_gift_data:get_grid_vec(SendBox);
get_pack(?pt_farmdepot, #player_data{account=Acc}) ->
    farm_depot:get_grids(farm_depot:read(Acc));
get_pack(Type, PlayerData)->
    erlang:error(badarg, [Type, PlayerData]).

set_pack(?pt_bag, Grids, PD)->
    PD#player_data{bag_grids = Grids};
set_pack(?pt_body, Grids, PD) ->
    PD#player_data{body_grids=Grids};
set_pack(?pt_depot, Grids, PD) ->
    NDepot = player_depot:change(PD#player_data.depot, Grids),
    PD#player_data{depot=NDepot};
set_pack(?pt_gift_bag, Grids, #player_data{gift=PlayerGift}=PD) ->
    SendBox = player_gift_data:get_send_box(PlayerGift),
    NSendBox = player_gift_data:set_grid_vec(SendBox, Grids),
    NPlayerGift = player_gift_data:set_send_box(PlayerGift, NSendBox),
    PD#player_data{gift=NPlayerGift};
set_pack(Type, _Grids, _PD)->
    erlang:error(badarg, [Type]).

get_sex(#player_data{basic_data=BasicData}) ->
    basic_data:get_data(sex, BasicData).

%% 获得玩家仓库的数据
get_depot(#player_data{depot=Depot}) ->
    Depot.
set_depot(NewDepot, PlayerData) ->
    PlayerData#player_data{depot=NewDepot}.

%% 获取玩家礼物的数据
get_gift(#player_data{gift=Gift}) ->
    Gift.

%% 设置玩家礼物的数据
set_gift(Gift, PlayerData) ->
    PlayerData#player_data{gift=Gift}.

get_gift_service_type(#player_data{gift_service_type=Type}) ->
    Type.

set_gift_service_type(Type, PlayerData) ->
    PlayerData#player_data{gift_service_type=Type}.

get_task(#player_data{task=Task}) ->
    Task.

set_task(Task, PlayerData) ->
    PlayerData#player_data{task=Task}.


get_start_edit_house_time(#player_data{start_edit_house_time=StartTime}) ->
    StartTime.

set_start_edit_house_time(StartTime, PlayerData) ->
    PlayerData#player_data{start_edit_house_time=StartTime}.

%% 防沉迷相关信息
get_wallow_info(#player_data{wallow_info=WallowInfo}) ->
    WallowInfo.
    
set_property_timer_ref(TimerRef, PlayerData) ->
    PlayerData#player_data{property_timer_ref=TimerRef}.

%% 鲜花数
get_flower(#player_data{flower_count=Count}) -> 
    Count.

set_flower(Count, PlayerData) ->
    PlayerData#player_data{flower_count=Count}.
