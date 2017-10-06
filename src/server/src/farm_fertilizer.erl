%%% @author lyj <lyj@35info.cn>
%%% @copyright (C) 2010, lyj
%%% @doc
%%%  农场化肥
%%% @end
%%% Created : 29 Nov 2010 by lyj <lyj@35info.cn>

-module(farm_fertilizer).

-include("sys_msg.hrl").
-include("tplt_def.hrl").

-export([can_use/3, do_use/5, after_use/2]).

can_use(Farm, PlotIndex, _Item) ->
    Crop = player_farm:get_crop(PlotIndex, Farm),
    case farm_crop:is_crop(Crop) of 
	true ->
	    case farm_crop:is_crop_ripe(Crop) of
		true -> {false, ?err_fer_crop_ripe};
		false ->
		    case farm_crop:is_stage_effect_exist(Crop) of
			true -> {false, ?err_fertilizered};
			false -> true
		    end
	    end;
	false -> {false, ?err_plot_empty}
    end.
			   
%% 缩短生长时间类(化肥)
do_use(_Account, Farm, PlotIndex, #farm_prop_tplt{reduce_time=RT}, _Item)->
    Crop = player_farm:get_crop(PlotIndex, Farm),
    Stage = farm_crop:get_stage(Crop),
    E = farm_crop:new_fertilizer(Stage, datetime:localtime(), RT),
    Crop1 = farm_crop:add_effect_data(E, Crop),
    Crop2 = farm_crop:refresh_crop_fruit(Crop1),
    NFarm = player_farm:set_crop(Farm, PlotIndex, Crop2),
    {NFarm, ok}.

after_use(_Account, _Reason) ->
    ok.


