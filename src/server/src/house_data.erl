%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%%
%%% @end
%%% Created :  4 Mar 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(house_data).

-include("packet_def.hrl").
%% API
-compile(export_all).



%%%===================================================================
%%% API
%%%===================================================================
get_owner(#house_data{owner=Owner}) ->
    Owner.

set_owner(HouseData, Owner) ->
    HouseData#house_data{owner=Owner}.

get_instance_id(#house_data{instance_id=InstId}) ->
    InstId.

set_instance_id(HouseData, InstId) ->
    HouseData#house_data{instance_id=InstId}.

get_template_id(#house_data{template_id=TempId}) ->
    TempId.

set_template_id(HouseData, TempId) ->
    HouseData#house_data{template_id=TempId}.

get_furniture_permission(#house_data{furniture_permission=FurniPermission}) ->
    FurniPermission.

set_furniture_permission(HouseData, FurniPermission) ->
    HouseData#house_data{furniture_permission=FurniPermission}.

get_furniture_vec(#house_data{furniture_vec=FurniVec}) ->
    FurniVec.

set_furniture_vec(HouseData, FurniVec) ->
    HouseData#house_data{furniture_vec=FurniVec}.

get_room_tex_vec(#house_data{room_tex_vec=RoomTexVec}) ->
    RoomTexVec.

set_room_tex_vec(HouseData, RoomTexVec) ->
    HouseData#house_data{room_tex_vec=RoomTexVec}.

get_component_tex_vec(#house_data{component_tex_vec=ComponentTexVec}) ->
    ComponentTexVec.

set_component_tex_vec(HouseData, ComponentTexVec) ->
    HouseData#house_data{component_tex_vec=ComponentTexVec}.

get_component_mesh_vec(#house_data{component_mesh_vec=ComponentMeshVec}) ->
    ComponentMeshVec.

set_component_mesh_vec(HouseData, ComponentMeshVec) ->
    HouseData#house_data{component_mesh_vec=ComponentMeshVec}.

get_welcome_words(#house_data{welcome_words=WelWords}) ->
    WelWords.

set_welcome_words(HouseData, WelWords) ->
    HouseData#house_data{welcome_words=WelWords}.

get_house_permission(#house_data{house_permission=HousePermission}) ->
    HousePermission.

set_house_permission(HouseData, HousePermission) ->
    HouseData#house_data{house_permission=HousePermission}.

get_visit_log(#house_data{visit_log=VisitLog}) ->
    VisitLog.

set_visit_log(HouseData, VisitLog) ->
    HouseData#house_data{visit_log=VisitLog}.

get_buy_date(#house_data{buy_date=BuyDate}) ->
    BuyDate.

set_buy_date(HouseData, BuyDate) ->
    HouseData#house_data{buy_date=BuyDate}.

get_level(#house_data{level=Level}) ->
    Level.

set_level(HouseData, Level) ->
    HouseData#house_data{level=Level}.

get_gift_box_vec(#house_data{gift_box_vec=GiftBoxVec}) ->
    GiftBoxVec.

set_gift_box_vec(HouseData, GiftBoxVec) ->
    HouseData#house_data{gift_box_vec=GiftBoxVec}.

get_house_clean(HouseData) ->
    HouseData#house_data.house_clean.

set_house_clean(HouseData, HouseClean) ->
    HouseData#house_data{house_clean=HouseClean}.
%%%===================================================================
%%% Internal functions
%%%===================================================================
