-record(house_tplt,{id,name,level,type,scene_id,house_furniture,born_id,max_players,preview_born}).
-record(test_tplt,{id,name}).
-record(gm_friend_tplt,{account,name,house_id,first_time_magic_box,first_time_garbage_box}).
-record(house_furniture_tplt,{id,furniture_id,item_temp_id,x,z,height,flr,face}).
-record(item_tplt,{id,name,type,imageset,icon,intro_id,overlap,bind,use_del,sell_price,sub_id,effect_time_id,use_id,voiceid,dvoiceid,uvoiceid,decoration}).
-record(item_dress_tplt,{id,name,sex,equip_pos,equip_placeholder,equip_disp_id,place,action,type1,type2}).
-record(house_comp,{id,name,prop_id,dis_id,pos,rot,scale}).
-record(common_scene,{id,name,file_res,born_id,max_players,busy_players,loose_players}).
-record(scene_copy,{id,scene_id,copy_id,name}).
-record(mutex_actions,{id,name,allow_actions}).
-record(domestic_service_tplt,{id,name,house_clean,money_rate,discount_rate,image}).
-record(domestic_price_tplt,{house_level,money}).
-record(work_tplt,{id,grade,money,need_clean,need_health,need_charm,need_active,need_disease,property_id,event_rate,event_ids,event_rates,workImage,typeImageSet,typeImage}).
-record(house_magic_box_tplt,{house_level,box_ids,box_rates,max_box_count,player_box_range,max_create_box_times}).
-record(magic_box_tplt,{id,money_range,item_ids,item_rates}).
-record(player_level_magic_box_tplt,{player_level,max_box_per_day,reward_times}).
-record(house_garbage_tplt,{house_level,garbage_ids,garbage_rates,max_count}).
-record(garbage_tplt,{id,money_range,item_ids,item_rates}).
-record(random_garbage_tplt,{id,interval,garbage_count}).
-record(work_event_tplt,{id,reward_money,reward_items,item_rate,image}).
-record(change_looks_tplt,{id,money_type,money}).
-record(teleport_tplt,{id,src_scene_id,born_id,src_x,src_y,src_z,src_radius,dest_scene_id,dest_dir}).
-record(create_random_dress,{id,sex,fashion1,fashion2,fashion3,fashion4,fashion5}).
-record(npc_tplt,{npc_id,body,head,hair,equip1,equip2,skeleton,npc_name}).
-record(npc_map_mapping_tplt,{id,npc_id,npc_name,born_id,script_id,action,towards}).
-record(player_level_tplt,{level,online_time}).
-record(player_property_tplt,{property_id,player_clean,player_health,player_charm,active_value}).
-record(player_property_by_stime_tplt,{min_property,max_property,ref_id}).
-record(sys_shop_tplt,{goods_id,item_id,type,sex,money_type,price,intro,commend,broadcast_id}).
-record(npc_shop_tplt,{goods_id,item_id,type,sex,price,intro,sale_type}).
-record(furni_tplt,{furniture_id,max_use_player,use_sex,permission,function_list,use_type,status_qty}).
-record(item_effect_time_tplt,{id,type,effect_time,trade_cut_time,del_time}).
-record(furni_interact_tplt,{function_id,property_id,status,action_type}).
-record(common_scene_furniture,{id,object_id}).
-record(player_task_action_tplt,{id,desc}).
-record(player_task_tplt,{task_id,title,intention,step_count,props,props_count,require_items,require_items_count,describe,select_reward_items,select_reward_items_count,fixed_reward_items,fixed_reward_items_count,reward_game_coin,reward_eq_coin,time_limit,property_id,is_repeat,is_show,is_give_up,sub_id,require_actions,require_actions_times,kind}).
-record(farm_task_tplt,{task_id,title,intention,describe,imageset,image,fixed_reward_items,fixed_reward_items_count,reward_game_coin,reward_eq_coin,exp,time_limit,is_repeat}).
-record(chain_task_tplt,{task_id,tasks,is_repeat,is_restart}).
-record(farm_seed_tplt,{id,name,expect_output,fruit_id,sprout_time,s_leaf_time,b_leaf_time,bloom_time,fruit_time,pick_time,drought_id,pest_id,grass_id,use_level,pick_exp,redland}).
-record(farm_damage_tplt,{id,sprout_ratio,s_leaf_ratio,b_leaf_ratio,bloom_ratio,fruit_ratio,interval,reduce_health}).
-record(farm_pet_tplt,{pet_id,rate,min_coin,max_coin}).
-record(common_define_tplt,{key,value}).
-record(farm_shop_tplt,{item_id,gameprice,eqprice,intro,special,level,repeat_buy,add_exp,rewards}).
-record(farm_assart_tplt,{plot_index,level,money}).
-record(farm_level_tplt,{level,next_exp,sow_exp,water_exp,weed_exp,worm_exp,hoeing_exp,title}).
-record(farm_decoration_tplt,{id,type,static_model,dynamic_model,scene_name}).
-record(farm_decoration_init_tplt,{background,house,doghouse,signpost,fence,scarecrow,billboard,billboard_content}).
-record(farm_setting_tplt,{water,weed,worm,put_grass,put_pest}).
-record(player_disease_probability_tplt,{player_health,probability}).
-record(player_disease_type_tplt,{type,rate,upgrate_probability,level,treatment_costs,min,max,special_event_id,event_id_probability,special_event_probability}).
-record(player_disease_special_event_tplt,{special_event_id,property_id,image}).
-record(farm_prop_tplt,{id,type,effect_range,effect_time,effect_output,effect_stage,reduce_time}).
-record(house_transaction_tplt,{id,name,type,recommend,level,pay_type,price,cost_flower,need_decoration,house_id,intro,broadcast_id}).
-record(sys_broadcast_tplt,{id,content,show_seconds}).
-record(player_property_interaction_tplt,{id,monitor_player_clean,monitor_player_health,monitor_player_charm,monitor_active_value,ref_id}).
-record(farmland_upgrade_tplt,{land_num,level,money}).
-record(red_farmland_harvest_tplt,{id,ratio,type,data,count}).
-record(farmfruit_aberrance_tplt,{seed_id,ratio1,fruit_id1,ratio2,fruit_id2,ratio3,fruit_id3,ratio4,fruit_id4,ratio5,fruit_id5,ratio6,fruit_id6,ratio7,fruit_id7,ratio8,fruit_id8,ratio9,fruit_id9,ratio10,fruit_id10,ratio11,fruit_id11,ratio12,fruit_id12}).
-record(gift_box_tplt,{id,type,name,game_coin,eq_coin,model_id,imageset,image,desc}).
-record(gift_card_tplt,{id,type,name,game_coin,eq_coin,imageset,image,desc,url}).
-record(day_night_config_tplt,{hh_mm_ss,is_day}).
-record(farmhoeing_awards_tplt,{level,ratio,item_id,count}).
-record(born,{id,pos,dir,lookat,pitch,yaw,zoom}).
-record(cupid_tag,{tag}).
