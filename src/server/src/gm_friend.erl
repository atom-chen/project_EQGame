%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  gm 好友
%%% @end
%%% Created : 28 Sep 2011 by hongjx <hongjx@35info.cn>

-module(gm_friend).
-include("tplt_def.hrl").
-include("packet_def.hrl").
-include("enum_def.hrl").


-export([get_gm_friends/0, is_gm_account/1, get_data/1, update_house/3]).
-export([update/0, get_house_owner/2, is_friend/2, get_gm/2, get_day_night_now/0]).
-export([update_gm_body/1, update_gm_house/1]).

get_gm_friends() ->
    GMFriends = [list_to_atom(binary_to_list(X#gm_friend_tplt.account)) 
		 || X <- tplt:get_all_data(gm_friend_tplt)],
    GMFriends.

is_gm_account(Account) ->
    lists:member(Account, get_gm_friends()).


get_data(Account) when is_atom(Account) ->
    tplt:get_data(gm_friend_tplt, atom_to_list(Account)).

update() ->
    [begin 
	 Account = list_to_atom(binary_to_list(X#gm_friend_tplt.account)),
	 UserName = binary_to_list(X#gm_friend_tplt.name),
	 HouseID = X#gm_friend_tplt.house_id,
	 update(Account, UserName, HouseID)
     end || X <- tplt:get_all_data(gm_friend_tplt)].

update(Account, UserName, HouseID) ->
    update_gm_body(Account),
    update_basic_data(Account, UserName),    
    update_house_data(Account, HouseID).

update_gm_body(Account) ->
    case db_common:select_one(player_body, Account) of
	[] ->
	    ok;
	#player_body{grid_vec=GridVec}=R ->
	    NGridVec = convert_grid_vec(GridVec),
	    db_common:write(R#player_body{grid_vec=NGridVec})	    
    end.

convert_grid_vec(GridVec) ->
    F = fun(Idx, Grid, Acc) -> 
		NGrid = convert_grid(Grid),
		array:set(Idx, NGrid, Acc)
	end,
    array:foldl(F, pack:new(array:size(GridVec)), GridVec).

convert_grid(#pack_grid{item_data=Item}=Grid) ->
    NItem = convert_item(Item), 
    Grid#pack_grid{item_data=NItem}.

%% 清掉时效
convert_item(Item) -> 
    item:set_del_time(Item, datetime:empty()).


update_house_data(Account, HouseID) ->
    HouseData = house:get_house_data(Account, HouseID),    
    %% 去除物品时效
    NHouseData = clear_item_del_time(HouseData),
    db_common:write(NHouseData).

update_basic_data(Account, UserName) ->
    NewR =
	case db_common:select_one(player_basic_data, Account) of
	    [] ->
		ok;
	    #player_basic_data{}=R ->
		R#player_basic_data{username=UserName}
	end,
    db_common:write(NewR).


clear_item_del_time(#house_data{furniture_vec=FurList}=HouseData) ->
    NFurList = [furniture:clear_del_time(X) || X <- FurList],
    HouseData#house_data{furniture_vec=NFurList}.


update_gm_house(Account) ->
    case db_common:select_one(house_data, Account) of
	[] -> ok;
	#house_data{}=HouseData ->
	    %% 去除物品时效
	    NHouseData = clear_item_del_time(HouseData),

	    db_common:write(NHouseData)
    end.

update_house(HouseOwner, #gm_friend_tplt{house_id=HouseID,					  
			    first_time_magic_box=NMagicBox,
			    account=GM,
			    first_time_garbage_box=NGarbage}, Creator) ->
    case db_house:select(HouseOwner) of
	#house_data{template_id=HouseID} ->
	    ok;
	_ ->
	    HouseData = house:get_house_data(HouseOwner, HouseID),
	    BoxList = [#magic_box{type=?mbt_garbage} || _X <- lists:seq(1, NMagicBox)] ++
		[#magic_box{type=?mbt_magic_box} || _X <- lists:seq(1, NGarbage)],

	    %% GM好友房间, 通知玩家垃圾信息
	    Packet = #notify_friend_garbage{friend_account=binary_to_list(GM), n=NGarbage},
	    net_helper:send2client(Creator, Packet),

	    NHouseData = HouseData#house_data{magic_box_vec=BoxList},

	    %% 去除物品时效
	    NNHouseData = clear_item_del_time(NHouseData),

	    db_common:write(NNHouseData)
    end.


is_friend(Account, GMAccount) ->
    S = atom_to_list(GMAccount),
    S2 = atom_to_list(Account) ++ "_gm_",
    string:str(S, S2) > 0.

get_house_owner(Account, GMAccount) ->
    list_to_atom(atom_to_list(Account) ++ "_gm_" ++ atom_to_list(GMAccount)).


get_gm(Creator, GMHouseOwner) when is_atom(Creator), is_atom(GMHouseOwner) ->
    HeadN = length(atom_to_list(Creator) ++ "_gm_"),
    AccountStr = lists:nthtail(HeadN, atom_to_list(GMHouseOwner)),
    list_to_atom(AccountStr).


get_day_night_now() ->
    All = tplt:get_all_data(day_night_config_tplt),
    List = [{list_to_tuple(HHMMSS), IsDay} 
	    || #day_night_config_tplt{hh_mm_ss=HHMMSS, is_day=IsDay} <- All],
    
    Now = datetime:time(),
    {_T, IsDay} =
	case lists:takewhile(fun({Time, _IsDay}) -> Time =< Now end, List) of
	    [] -> lists:last(List);
	    FrontList -> lists:last(FrontList)
	end,
    IsDay.
