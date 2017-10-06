%%%-------------------------------------------------------------------
%%% @author  <linyibin>
%%% @copyright (C) 2010, 
%%% @doc
%%% 农场装饰
%%% @end
%%% Created : 21 Jun 2010 by  <>
%%%-------------------------------------------------------------------
-module(farm_decoration).

-include("packet_def.hrl").
-include("router.hrl").
-include("player_data.hrl").
-include("tplt_def.hrl").

%% API
-export([init/2, recycle/2, start/1, is_exist_dog/1, set/5, get_dog/1, handle_cast/2]).
-export([test/0]).

start(Account) ->
    [router:make_event_source(?msg_req_change_decoration, Account, {self(), ?MODULE}),
     router:make_event_source(?msg_req_remove_decoration, Account, {self(), ?MODULE})
    ].

%%%===================================================================
%%% API
%%%===================================================================
is_exist_dog(Decoration) ->
    Item = Decoration#farm_decoration.dog,
    InstId = item:get_instid(Item),
    TempId = item:get_tempid(Item),
    (InstId > 0) and (TempId > 0).

get_dog(#farm_decoration{dog=Dog}) ->
    Dog.

recycle(Account, Decoration) ->
    InitTplt = get_tplt(),
    Now = datetime:localtime(),
    Depot = farm_depot:read(Account),
    {Decoration1, Depot1} = recycle(1, Now, InitTplt, Depot, Decoration),
    {Decoration2, Depot2} = recycle(2, Now, InitTplt, Depot1, Decoration1),
    {Decoration3, Depot3} = recycle(3, Now, InitTplt, Depot2, Decoration2),
    {Decoration5, Depot5} = recycle(5, Now, InitTplt, Depot3, Decoration3),
    {Decoration6, Depot6} = recycle(6, Now, InitTplt, Depot5, Decoration5),
    recycle(7, Now, InitTplt, Depot6, Decoration6).

get_item(DelDate, Now, Depot, InitItem, Item) ->
    case item:has_del_time(DelDate) of
	true ->
	    case datetime:diff_time(Now, DelDate) > 0 of
		true -> %% 不超过日期
		    {Item, Depot};
		false ->
		    TempId = item:get_tempid(Item),
		    Index = farm_depot:get_index_by_tempid(TempId, Depot),
		    NDepot = farm_depot:del_item(Index, 1, Depot),
		    {InitItem, NDepot}
	    end;
	false ->
	    {Item, Depot}
    end.

%% 设置背景
recycle(1, Now, InitTplt, Depot, Decoration) ->
    Item = Decoration#farm_decoration.background,
    case item:is_empty(Item) of
	true ->
	    {Decoration, Depot};
	false ->
	    TempId = InitTplt#farm_decoration_init_tplt.background,
	    Index = farm_depot:get_index_by_tempid(TempId, Depot),
	    InitItem = farm_depot:get_item(Index, Depot),
	    DelDate = item:get_del_time(item:get_tempid(Item)),
	    {NewItem, NDepot} = get_item(DelDate, Now, Depot, InitItem, Item),
	    NDecoration = Decoration#farm_decoration{background=NewItem},
	    {NDecoration, NDepot}
    end;
%% 房屋
recycle(2, Now, InitTplt, Depot, Decoration) ->
    Item = Decoration#farm_decoration.house,
    case item:is_empty(Item) of
	true ->
	    {Decoration, Depot};
	false ->
	    TempId = InitTplt#farm_decoration_init_tplt.house,
	    Index = farm_depot:get_index_by_tempid(TempId, Depot),
	    InitItem = farm_depot:get_item(Index, Depot),
	    DelDate = item:get_del_time(item:get_tempid(Item)),
	    {NewItem, NDepot} = get_item(DelDate, Now, Depot, InitItem, Item),
	    NDecoration = Decoration#farm_decoration{house=NewItem},
	    {NDecoration, NDepot}
    end;
%% 狗窝
recycle(3, Now, InitTplt, Depot, Decoration) ->
    Item = Decoration#farm_decoration.doghouse,
    case item:is_empty(Item) of
	true ->
	    {Decoration, Depot};
	false ->
	    TempId = InitTplt#farm_decoration_init_tplt.doghouse,
	    Index = farm_depot:get_index_by_tempid(TempId, Depot),
	    InitItem = farm_depot:get_item(Index, Depot),
	    DelDate = item:get_del_time(item:get_tempid(Item)),
	    {NewItem, NDepot} = get_item(DelDate, Now, Depot, InitItem, Item),
	    NDecoration = Decoration#farm_decoration{doghouse=NewItem},
	    {NDecoration, NDepot}
    end;
%% %% 狗
%% recycle(4, Now, InitTplt, Depot, Decoration) ->
%%     Item = Decoration#farm_decoration.dog,
%%     TempId = InitTplt#farm_decoration_init_tplt.dog,
%%     Index = farm_depot:get_index_by_tempid(TempId, Depot),
%%     InitItem = farm_depot:get_item(Index, Depot),
%%     DelDate = item:get_del_time(Item),
%%     {NewItem, NDepot} = get_item(DelDate, Now, Index, Depot, InitItem, Item),
%%     NDecoration = Decoration#farm_decoration{dog=NewItem},
%%     {NDecoration, NDepot};
%% 路标
recycle(5, Now, InitTplt, Depot, Decoration) ->
    Item = Decoration#farm_decoration.signpost,
    case item:is_empty(Item) of
	true ->
	    {Decoration, Depot};
	false ->
	    TempId = InitTplt#farm_decoration_init_tplt.signpost,
	    Index = farm_depot:get_index_by_tempid(TempId, Depot),
	    InitItem = farm_depot:get_item(Index, Depot),
	    DelDate = item:get_del_time(Item),
	    {NewItem, NDepot} = get_item(DelDate, Now, Depot, InitItem, Item),
	    NDecoration = Decoration#farm_decoration{signpost=NewItem},
	    {NDecoration, NDepot}
    end;
%% 栅栏
recycle(6, Now, InitTplt, Depot, Decoration) ->
    Item = Decoration#farm_decoration.fence,
    case item:is_empty(Item) of
	true ->
	    {Decoration, Depot};
	false ->
	    TempId = InitTplt#farm_decoration_init_tplt.fence,
	    Index = farm_depot:get_index_by_tempid(TempId, Depot),
	    InitItem = farm_depot:get_item(Index, Depot),
	    DelDate = item:get_del_time(item:get_tempid(Item)),
	    {NewItem, NDepot} = get_item(DelDate, Now, Depot, InitItem, Item),
	    NDecoration = Decoration#farm_decoration{fence=NewItem},
	    {NDecoration, NDepot}
    end;
%% 稻草人
recycle(7, Now, InitTplt, Depot, Decoration) ->
    Item = Decoration#farm_decoration.scarecrow,
    case item:is_empty(Item) of
	true ->
	    {Decoration, Depot};
	false ->
	    TempId = InitTplt#farm_decoration_init_tplt.scarecrow,
	    Index = farm_depot:get_index_by_tempid(TempId, Depot),
	    InitItem = farm_depot:get_item(Index, Depot),
	    DelDate = item:get_del_time(item:get_tempid(Item)),
	    {NewItem, NDepot} = get_item(DelDate, Now, Depot, InitItem, Item),
	    NDecoration = Decoration#farm_decoration{scarecrow=NewItem},
	    {NDecoration, NDepot}
    end.
%%%===================================================================
%%% Handle cast
%%%===================================================================
%% 请求移除装饰
handle_cast({_Msg, #req_remove_decoration{template_id=TempId}}, State) ->
    Account = farm:get_account(State),
    DecorationTplt = tplt:get_data(farm_decoration_tplt, TempId),
    PlayerFarm = db_farm:select(Account),
    NPlayerFarm = player_farm:refresh_farm_state(PlayerFarm),
    Decoration = player_farm:get_decoration(NPlayerFarm),
    Type = DecorationTplt#farm_decoration_tplt.type,
    NNPlayerFarm = set(Type, NPlayerFarm, Decoration, 0, 0),
    db_farm:write(NNPlayerFarm),
    player_farm:notify_farm_data(Account, Account, NNPlayerFarm), 
    {noreply, State};

%% 请求改变装饰
handle_cast({_Msg, #req_change_decoration{decoration_item=Item}}, State)->
    Account = farm:get_account(State),
    InstId = item:get_instid(Item),
    case farm_depot:is_exist(Account, InstId) of %% 是否存在该实例Id
	true ->
	    TempId=item:get_tempid(Item),
	    DecorationTplt = tplt:get_data(farm_decoration_tplt, TempId),
	    PlayerFarm = db_farm:select(Account),
	    Decoration = player_farm:get_decoration(PlayerFarm),
	    NPlayerFarm = player_farm:refresh_farm_state(PlayerFarm),
	    Type = DecorationTplt#farm_decoration_tplt.type,
	    NNPlayerFarm = set(Type, NPlayerFarm, Decoration, InstId, TempId),
	    db_farm:write(NNPlayerFarm),
	    player_farm:notify_farm_data(Account, Account, NNPlayerFarm);
	_ ->
	    erlang:error(badarg, Item)
    end,
    {noreply, State}.

%%%===================================================================
%%% API
%%%===================================================================
%% 初始化装饰
-spec init(tuple(), tuple()) -> tuple().
init(PlayerFarm, Depot) ->
    {NDepot, Decoration} = new(Depot),
    NPlayerFarm = player_farm:set_decoration(PlayerFarm, Decoration),
    {NDepot, NPlayerFarm}.

%%%===================================================================
%%% Internal functions
%%%===================================================================
%% 创建装饰物
new(Depot) ->
    {BackGroundItem, HouseItem, DogHouseItem, 
     SignPostItem, FenceItem, 
     ScareCrowItem, BillBoardItem, BillBoardContentItem} = init_item(),

    NDepot = farm_depot:add_item(Depot, 
				 [BackGroundItem, HouseItem, DogHouseItem, SignPostItem,
				  FenceItem, ScareCrowItem, BillBoardItem, BillBoardContentItem]),
    NDecoration = #farm_decoration{background = BackGroundItem,
				   house = HouseItem,
				   doghouse = DogHouseItem,
				   signpost = SignPostItem,
				   fence = FenceItem,
				   scarecrow = ScareCrowItem,
				   billboard = BillBoardItem,
				   billboard_content = BillBoardContentItem},
    {NDepot, NDecoration}.

%% 初始化装饰物品
-spec init_item() -> tuple().
init_item() ->
    InitTplt = get_tplt(),
    {item:make_item(InitTplt#farm_decoration_init_tplt.background),
     item:make_item(InitTplt#farm_decoration_init_tplt.house),
     item:make_item(InitTplt#farm_decoration_init_tplt.doghouse),
     item:make_item(InitTplt#farm_decoration_init_tplt.signpost),
     item:make_item(InitTplt#farm_decoration_init_tplt.fence),
     item:make_item(InitTplt#farm_decoration_init_tplt.scarecrow),
     item:make_item(InitTplt#farm_decoration_init_tplt.billboard),
     item:make_item(InitTplt#farm_decoration_init_tplt.billboard_content)}.

get_tplt() ->
    [InitTplt|_InitTpltList] = tplt:get_all_data(farm_decoration_init_tplt),
    InitTplt.

%% 设置背景
-spec set(integer(), tuple(), tuple(), integer(), integer()) -> tuple().
set(1, PlayerFarm, Decoration, 0, 0) ->
    Account = player_farm:get_account(PlayerFarm),
    InitTplt = get_tplt(),
    TempId = InitTplt#farm_decoration_init_tplt.background,
    Depot = farm_depot:read(Account),
    GridIndex = farm_depot:get_index_by_tempid(TempId, Depot),
    Item = farm_depot:get_item(GridIndex, Depot),
    NDecoration = Decoration#farm_decoration{background=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration);
set(1, PlayerFarm, Decoration, InstId, TempId) ->
    Item = item:set_item(InstId, TempId),
    NDecoration = Decoration#farm_decoration{background=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration);
%% 设置房屋
set(2, PlayerFarm, Decoration, InstId, TempId) ->
    Item = item:set_item(InstId, TempId),
    NDecoration = Decoration#farm_decoration{house=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration);
%% 设置狗窝
set(3, PlayerFarm, Decoration, InstId, TempId) ->
    Item = item:set_item(InstId, TempId),
    NDecoration = Decoration#farm_decoration{doghouse=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration);
%% 设置狗
set(4, PlayerFarm, Decoration, 0, 0) ->
    Item = item:set_item(0, 0),
    NDecoration = Decoration#farm_decoration{dog=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration);
set(4, PlayerFarm, Decoration, InstId, TempId) ->
    Item = item:set_item(InstId, TempId),
    NDecoration = Decoration#farm_decoration{dog=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration);
%% 设置路标
set(5, PlayerFarm, Decoration, InstId, TempId) ->
    Item = item:set_item(InstId, TempId),
    NDecoration = Decoration#farm_decoration{signpost=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration);
%% 设置篱笆（栅栏）
set(6, PlayerFarm, Decoration, InstId, TempId) ->
    Item = item:set_item(InstId, TempId),
    NDecoration = Decoration#farm_decoration{fence=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration);
%% 设置稻草人
set(7, PlayerFarm, Decoration, InstId, TempId) ->
    Item = item:set_item(InstId, TempId),
    NDecoration = Decoration#farm_decoration{scarecrow=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration);
%% 设置公告板
set(8, PlayerFarm, Decoration, InstId, TempId) ->
    Item = item:set_item(InstId, TempId),
    NDecoration = Decoration#farm_decoration{billboard=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration);
%% 设置公告板内容
set(9, PlayerFarm, Decoration, InstId, TempId) ->
    Item = item:set_item(InstId, TempId),
    NDecoration = Decoration#farm_decoration{billboard_content=Item},
    player_farm:set_decoration(PlayerFarm, NDecoration).

%%%===================================================================
%%% Test case
%%%===================================================================
-include_lib("eunit/include/eunit.hrl").
test() ->
    test_init_item(),
    test_req_change_decoration(),
    test_req_remove_decoration().

test_init_item() ->
    init_item().

test_req_change_decoration() ->
    router:cast(test1, ?msg_req_change_decoration, 
		#req_change_decoration{decoration_item=item:new(1100000, 72155590578798593)}).

test_req_remove_decoration() ->
    router:cast(test1, ?msg_req_remove_decoration, #req_remove_decoration{template_id=1100000}),
    router:cast(test1, ?msg_req_remove_decoration, #req_remove_decoration{template_id=1100001}).
