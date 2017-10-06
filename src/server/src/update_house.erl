%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%%
%%% @end
%%% Created :  3 Mar 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(update_house).
-export([update/0, update_position_list/0, update_position_list/1, 
	 update_gift_pos/0,
	 clear_house_log/0,	 
	 clear_magic_box/0]).
-export([add_house_clean/0]).
-export([update_room_tex/0]).

-include("packet_def.hrl").

%% API


%%%===================================================================
%%% API
%%%===================================================================
update() ->
    Keys = get_table_keys(house_data),
    [update_house(Key) || Key <- Keys],
    {atomic, ok}.

update_house(Key) ->
    HouseData = db_common:select_one(house_data, Key),

    FurnitureVec = HouseData#house_data.furniture_vec,
    NFurnitureVec = update_furniture_vec(FurnitureVec, []),
    NHouseData = HouseData#house_data{furniture_vec=NFurnitureVec},
    db_common:write(NHouseData).

update_furniture_vec([], NFurnitureVec) ->
    NFurnitureVec;
update_furniture_vec([Furniture|FurnitureVec], NFurnitureVec) ->
    case Furniture of
	{house_furniture, InstanceId, TemplateId, X, Z, Height, Floor, Face, ItemTempId, MaxUsePlayer, PositionList, FunctionList, UseSex, Permission,DelTime} ->
	    NFurniture = #house_furniture{instance_id=InstanceId,
					  template_id=TemplateId,
					  x=X,
					  z=Z,
					  height=Height,
					  floor=Floor,
					  face=Face,
					  item_tempid=ItemTempId,
					  max_use_player=MaxUsePlayer,
					  position_list=PositionList,
					  function_list=FunctionList,
					  use_sex=UseSex,
					  permission=Permission,
					  del_time=DelTime,
					  use_type=0,
					  status_qty=0
					 },
	    update_furniture_vec(FurnitureVec, [NFurniture|NFurnitureVec]);
	_ ->
	    update_furniture_vec(FurnitureVec, [Furniture|NFurnitureVec])
    end.

update_position_list() ->
    Keys = get_table_keys(house_data),
    [do_update_position_list(Key) || Key <- Keys],
    {atomic, ok}.

update_position_list(Account) ->
    Keys = [Account],
    [do_update_position_list(Key) || Key <- Keys],
    {atomic, ok}.

do_update_position_list(Owner) ->
    HouseData = db_house:select(Owner),
    FurniVec = HouseData#house_data.furniture_vec,
    NFurniVec = do_update_position_list1(FurniVec, []),
    NHouseData = HouseData#house_data{furniture_vec=NFurniVec},
    db_common:write(NHouseData).

do_update_position_list1([], NFurniVec) ->
    lists:reverse(NFurniVec);
do_update_position_list1([#house_furniture{position_list=PositionList}=Furniture|FurnitureVec], NFurniVec) ->
    NFurniture = Furniture#house_furniture{position_list = set_position(PositionList, [])},
    do_update_position_list1(FurnitureVec, [NFurniture|NFurniVec]).

set_position([], NPositionList) ->
    lists:reverse(NPositionList);
set_position([Position|PositionList], NPositionList) ->
    NPosition = 
	case Position of
	    {furniture_position, PositionIndex, IsUsed, UsedAccount} ->
		#furniture_position{position_index=PositionIndex,
				    is_used=IsUsed,
				    used_account=UsedAccount,
				    status=0,
				    func_id=0,
				    use_time=datetime:localtime()};		
	    {furniture_position, PositionIndex, IsUsed, UsedAccount, UseTime} ->
		#furniture_position{position_index=PositionIndex,
				    is_used=IsUsed,
				    used_account=UsedAccount,
				    status=0,
				    func_id=0,
				    use_time=UseTime};
	    {furniture_position, PositionIndex, IsUsed, UsedAccount, Status, UseTime} ->
		#furniture_position{position_index=PositionIndex,
				    is_used=IsUsed,
				    used_account=UsedAccount,
				    status=Status,
				    func_id=0,
				    use_time=UseTime};
	    _ ->
		Position
	end,
    set_position(PositionList, [NPosition|NPositionList]).


get_table_keys(Tab) ->
    F = fun() ->
		mnesia:all_keys(Tab)
	end,
    mnesia:activity(transaction, F, [], mnesia_frag).


%%%===================================================================
%%% ������������
%%%===================================================================
clear_magic_box() ->
    Keys = get_table_keys(house_data),
    [clear_magic_box(Key) || Key <- Keys],    

    PlotKeys = get_table_keys(house_plot),
    [clear_plot_magic_box(Key) || Key <- PlotKeys],    

    {atomic, ok}.

clear_magic_box(Key) ->
    HouseData = db_common:select_one(house_data, Key),

    NHouseData = clear_magic_box_2011_04_11(HouseData),

    db_common:write(NHouseData).

clear_plot_magic_box(Key) ->
    {TabName, Account, Plots} = db_common:select_one(house_plot, Key),
    NewPlots = [{plot, ID, clear_magic_box_2011_04_11(HouseData)} 
		|| {plot, ID, HouseData} <- Plots],

    db_common:write({TabName, Account, NewPlots}).

clear_magic_box_2011_04_11([]) ->
    [];
clear_magic_box_2011_04_11(HouseData) ->
    N = house_data_size_2011_04_11(),
    N1 = N -1,
    case size(HouseData) of
	N -> 
	    erlang:setelement(N, HouseData, []);
	N1 ->
	    erlang:append_element(HouseData, [])
    end.


house_data_size_2011_04_11() ->
    length(
     [house_data,                                         % 1 ��������
      {string, owner},                                    % 2 ���ݵ�����
      {uint64, instance_id},                              % 3 ����ʵ��ID
      {int, template_id},                                 % 4 ���ݵ�ģ��ID
      {furniture_permission_type, furniture_permission},  % 5 ʹ�üҾ�Ȩ��
      {array, house_furniture, furniture_vec},            % 6 �����ڵļҾ߼���
      {array, room_tex, room_tex_vec},                    % 7 ����ǽֽ�ļ���
      {array, house_component_tex, component_tex_vec},    % 8 �����ڱ��滻�Ĳ��ʵļ���
      {array, house_component_mesh, component_mesh_vec},  % 9 �����ڱ��滻�Ĺ̶�����ļ���
      {string, welcome_words},                            % 10 ��ӭ��
      {house_permission_type, house_permission},          % 11 ����Ȩ��
      {array, house_visit_log, visit_log},                % 12 ���ݷ��ʼ�¼
      {stime, buy_date},                                  % 13 ��������
      {int, level},                                       % 14 �ȼ�  
      {array, gift_box, gift_box_vec},                    % 15 ���Ｏ��
      {array, magic_box, magic_box_vec}                   % 16 ����(����)����
     ]).


%%%===================================================================
%%% ���������־
%%%===================================================================
clear_house_log() ->
    Keys = get_table_keys(house_data),
    [clear_house_log(Key) || Key <- Keys],    

    PlotKeys = get_table_keys(house_plot),
    [clear_plot_house_log(Key) || Key <- PlotKeys],    

    {atomic, ok}.

clear_house_log(Key) ->
    HouseData = db_common:select_one(house_data, Key),

    NHouseData = do_clear_house_log(HouseData),

    db_common:write(NHouseData).

clear_plot_house_log(Key) ->
    {TabName, Account, Plots} = db_common:select_one(house_plot, Key),
    NewPlots = [{plot, ID, do_clear_house_log(HouseData)} 
		|| {plot, ID, HouseData} <- Plots],

    db_common:write({TabName, Account, NewPlots}).

do_clear_house_log([]) ->
    [];
do_clear_house_log(HouseData) ->
    Index = 12, %% ���ݷ��ʼ�¼
    erlang:setelement(Index, HouseData, []).


%%%===================================================================
%%% �������������
%%%===================================================================
update_gift_pos() ->
    Keys = get_table_keys(house_data),
    [update_gift_pos(Key) || Key <- Keys],    

    PlotKeys = get_table_keys(house_plot),
    [update_plot_gift_pos(Key) || Key <- PlotKeys],    

    {atomic, ok}.

update_gift_pos(Key) ->
    HouseData = db_common:select_one(house_data, Key),
    NHouseData = update_gift_pos_2011_04_18(HouseData),
    db_common:write(NHouseData).

update_plot_gift_pos(Key) ->
    {TabName, Account, Plots} = db_common:select_one(house_plot, Key),
    NewPlots = [{plot, ID, update_gift_pos_2011_04_18(HouseData)} 
		|| {plot, ID, HouseData} <- Plots],

    db_common:write({TabName, Account, NewPlots}).

update_gift_pos_2011_04_18([]) ->
    [];
update_gift_pos_2011_04_18(HouseData) ->
    GiftBoxVecField = 15, %% 15 ���Ｏ��
    GiftBoxList = erlang:element(GiftBoxVecField, HouseData),
    NewList = [do_update_gift_pos(X) || X <- GiftBoxList],
    erlang:setelement(GiftBoxVecField, HouseData, NewList).


do_update_gift_pos(GiftBox) when is_tuple(GiftBox) ->
    PosField = 12, %% ����λ��
    {point, A, B, C} = erlang:element(PosField, GiftBox),
    NewPos = {grid_pos, trunc(A), trunc(B), trunc(C)},
    erlang:setelement(PosField, GiftBox, NewPos).


     %% 1 [gift_box,                                          % �����
     %% 2 {uint64, inst_id},                                 % �����ʵ��Id
     %% 3 {string, sender},                                  % ������
     %% 4 {string, sender_name},                             % ����������
     %% 5 {string, receiver},                                % ������
     %% 6 {string, receiver_name},                           % ����������
     %% 7 {string, name},                                    % ��������
     %% 8 {array, pack_grid, grid_vec},                      % ����
     %% 9 {int, box_id},                                     % �����
     %% 10 {int, card_id},                                    % ���￨
     %% 11 {string, comment},                                 % ����
     %% 12 {grid_pos, pos},                                   % ����λ��
     %% 13 {stime, date},                                     % ��¼����
     %% 14 {anonymous_type, is_anonymous}                     % �Ƿ�����
     %% ],


%%%===================================================================
%%% ���ӷ�������
%%%===================================================================
add_house_clean() ->
    Keys = get_table_keys(house_data),
    [add_house_clean(Key) || Key <- Keys],
    {atomic, ok}.

add_house_clean(Key) ->
    HousePlot = db_common:select_one(house_plot, Key),
    Plots = house_plot:get_plots(HousePlot),
    NPlots = update_plots(Plots, []),
    NHousePlot = house_plot:set_plots(HousePlot, NPlots),
    db_common:write(NHousePlot).

update_plots([], NPlots) ->
    NPlots;
update_plots([Plot|Plots], NPlots) ->
    case Plot of
	{plot, Id, {house_data, Owner, InstId, TempId, FurniPer, FurniVec, RoomTexVec, 
		    ComponentTexVec, ComponentMeshVec, WelWord, HousePer, VisitLog, 
		    BuyDate, Level, GiftBoxVec, MagicBoxVec}} ->
	    NPlot = {plot, Id, {house_data, Owner, InstId, TempId, FurniPer, FurniVec, RoomTexVec, 
				ComponentTexVec, ComponentMeshVec, WelWord, HousePer, VisitLog, 
				BuyDate, Level, GiftBoxVec, MagicBoxVec, 100}},
	    update_plots(Plots, [NPlot|NPlots]);
	_ ->
	    update_plots(Plots, [Plot|NPlots])
    end.
%%%===================================================================
%%% Internal functions
%%%===================================================================


%����ǽֽ�ذ�¥����Ϣ
update_room_tex()->
    Keys = get_table_keys(house_data),
    [update_room_tex_1(Key) || Key <- Keys],    
    {atomic,ok}.

update_room_tex_1(Key)->
    HouseData = db_common:select_one(house_data, Key),
    NHouseData = update_room_tex_2011_08_01(HouseData),
    db_common:write(NHouseData).

update_room_tex_2011_08_01(HouseData)->
    house_data:set_room_tex_vec(HouseData,[]).
