%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  角色初始化模块, 就是在角色创建的过程中需要初始化的一些操作
%%%  比如包裹系统, 角色基本信息系统, 货币系统等的初始化
%%% @end
%%% Created : 23 Mar 2010 by  <>
%%%-------------------------------------------------------------------
-module(player_init).

-include("packet_def.hrl").
-include("common_def.hrl").
-include("tplt_def.hrl").
-include("table_info.hrl").
-include("enum_def.hrl").

%% API
-export([init/3, init/4, init_player_pack/1]).

%%%===================================================================
%%% API
%%%===================================================================

%% 初始化函数
init(Node, Account, BasicData, Equips) when is_atom(Account) ->
    rpc:call(Node, ?MODULE, init, [Account, BasicData, Equips]).

init(Account, BasicData, Equips) when is_atom(Account) ->
    EquipItems = make_equips_item(Equips),

    GameCoin = common_def:get_val(player_init_game_coin),
    EQCoin = common_def:get_val(player_init_eq_coin),
    HouseData = init_house_data(Account, BasicData),
    HouseRandGarbage = house_magic_box:init_house_rand_garbage(HouseData#house_data.instance_id),
    Ret = db_player:create([init_player_basic_data(Account, BasicData),
			    HouseData,
			    HouseRandGarbage,
			    init_player_pack(Account),
			    init_player_body(Account, EquipItems),
			    init_player_property(Account),
			    init_player_coin(Account, GameCoin, EQCoin),
			    player_depot:new(Account),
			    player_gift_box:init(Account),
			    player_task_data:init(Account, BasicData#player_basic_data.sex),
			    house_plot:init(Account)
			   ]
			  ),
    %% 后台物品金钱记录
    Logs = [
	    %% 初始金钱
	    item_money_log:build_record(Account, player_init_money, 
					{0, 0, 0}, {GameCoin, EQCoin}),
	    %% 初始装备记录
	    item_money_log:build_record(Account, player_init_items, 
					get_effect_item_infos(EquipItems)),
	    %% 初始家具记录
	    item_money_log:build_record(Account, player_init_furnitures, 
					get_effect_furnitures_infos(HouseData))	    
	   ],
    db_common:write(Logs),

    Ret. 
    
%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 初始化玩家基本数据
init_player_basic_data(Account, BasicData)->
    BasicData#player_basic_data{account=Account, health_status=?dt_normal, online_time=0, last_login_time=datetime:localtime(), logout_time=datetime:empty()}.

%% 根据性别获得指定的房屋的模板ID 
get_house_template_id(#player_basic_data{sex=Sex}) ->
    case Sex of 
	?ps_male -> common_def:get_val(player_init_male_house_id); % 男性
	?ps_female -> common_def:get_val(player_init_female_house_id)% 女性
    end.





%% 返回房屋的初始化数据
init_house_data(Account, BasicData) ->
    TempID = get_house_template_id(BasicData),
    house:get_house_data(Account, TempID).


%% 初始化玩家包裹
init_player_pack(Account) ->
    #player_pack{owner=Account, grid_vec=pack:new(?init_pack_grid_count)}.

%% 初始化玩家身体 
init_player_body(Account, EquipItems) ->
    #player_body{owner=Account, grid_vec=EquipItems}.  

init_player_property(Account) ->
    #player_property{account=Account, player_clean=100, player_health=100, player_charm=100, active_value=100}.

init_player_coin(Account, GameCoin, EQCoin) ->
    #player_coin{account=Account, gamecoin=GameCoin, eqcoin=EQCoin}.

%% 生成物品装备的集合
%% 返回装备物品列表
make_equips_item([]) ->
    Item=#item{},
    array:from_list([make_item(E) || E <- [Item,Item,Item,Item,Item,Item,Item,Item]]);
make_equips_item(Equips)->
    array:from_list([make_item(Item) || Item <- Equips]).

%% 生成一个物品
make_item(#item{template_id=0})->
    #pack_grid{};
make_item(#item{template_id=TempID})->
    #pack_grid{count=1,
	       item_data=item:make_item(TempID)}.   


%% 提取物品信息
get_effect_item_infos(Pack) ->
    F = fun(_Idx, {Item, Count}, List) ->
		case item:is_empty(Item) of
		    false ->
			[{item:get_tempid(Item), Count, item:get_instid(Item)} | List];	
		    true -> List	
		end
	end,
    pack:foldl(F, [], Pack).

%% 提取家具信息
get_effect_furnitures_infos(HouseData) ->
    FurList = HouseData#house_data.furniture_vec,
    [{furniture:get_itemid(X), 1, furniture:get_instid(X)} || X <- FurList].

