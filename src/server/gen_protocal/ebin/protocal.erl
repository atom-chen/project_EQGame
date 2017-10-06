-module(protocal).
-include("packet_def.hrl").
-include("net_type.hrl").
-export([encode/1, decode/2]).


encode(#req_ver_validate{ver=VER}) ->
	{?msg_req_ver_validate, <<VER:?INT>>};
encode(#notify_ver_validate{result=RESULT}) ->
	{?msg_notify_ver_validate, <<RESULT:?INT>>};
encode(#stime{year=YEAR, month=MONTH, day=DAY, hour=HOUR, minute=MINUTE, second=SECOND}) ->
	{?msg_stime, <<YEAR:?INT, MONTH:?INT, DAY:?INT, HOUR:?INT, MINUTE:?INT, SECOND:?INT>>};
encode(#point{x=X, y=Y, z=Z}) ->
	{?msg_point, <<X:?FLOAT, Y:?FLOAT, Z:?FLOAT>>};
encode(#grid_pos{x=X, y=Y, z=Z}) ->
	{?msg_grid_pos, <<X:?INT, Y:?INT, Z:?INT>>};
encode(#item{instance_id=INSTANCE_ID, template_id=TEMPLATE_ID, del_time=DEL_TIME, decoration_val=DECORATION_VAL}) ->
	{?msg_item, <<INSTANCE_ID:?UINT64, TEMPLATE_ID:?INT, (net_helper:get_encode_binary_data(encode(DEL_TIME)))/binary, DECORATION_VAL:?INT>>};
encode(#player_basic_data{account=ACCOUNT, username=USERNAME, sex=SEX, skin_color=SKIN_COLOR, hair=HAIR, face=FACE, beard=BEARD, age=AGE, level=LEVEL, online_time=ONLINE_TIME, health_status=HEALTH_STATUS, personality_sign=PERSONALITY_SIGN, personality_describe=PERSONALITY_DESCRIBE, hair_color=HAIR_COLOR, last_login_time=LAST_LOGIN_TIME, gm_enabled=GM_ENABLED, logout_time=LOGOUT_TIME}) ->
	{?msg_player_basic_data, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_string(USERNAME))/binary, SEX:?INT, SKIN_COLOR:?INT, HAIR:?INT, FACE:?INT, BEARD:?INT, AGE:?INT, LEVEL:?INT, ONLINE_TIME:?FLOAT, HEALTH_STATUS:?INT, (net_helper:encode_string(PERSONALITY_SIGN))/binary, (net_helper:encode_string(PERSONALITY_DESCRIBE))/binary, HAIR_COLOR:?INT, (net_helper:get_encode_binary_data(encode(LAST_LOGIN_TIME)))/binary, GM_ENABLED:?INT, (net_helper:get_encode_binary_data(encode(LOGOUT_TIME)))/binary>>};
encode(#player_property{account=ACCOUNT, player_clean=PLAYER_CLEAN, player_health=PLAYER_HEALTH, player_charm=PLAYER_CHARM, active_value=ACTIVE_VALUE}) ->
	{?msg_player_property, <<(net_helper:encode_string(ACCOUNT))/binary, PLAYER_CLEAN:?INT, PLAYER_HEALTH:?INT, PLAYER_CHARM:?INT, ACTIVE_VALUE:?INT>>};
encode(#pack_grid{count=COUNT, lock=LOCK, item_data=ITEM_DATA}) ->
	{?msg_pack_grid, <<COUNT:?INT, LOCK:?INT, (net_helper:get_encode_binary_data(encode(ITEM_DATA)))/binary>>};
encode(#player_body{owner=OWNER, grid_vec=GRID_VEC}) ->
	{?msg_player_body, <<(net_helper:encode_string(OWNER))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GRID_VEC))/binary>>};
encode(#player_pack{owner=OWNER, grid_vec=GRID_VEC}) ->
	{?msg_player_pack, <<(net_helper:encode_string(OWNER))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GRID_VEC))/binary>>};
encode(#player_info{basic_data=BASIC_DATA, scenename=SCENENAME}) ->
	{?msg_player_info, <<(net_helper:get_encode_binary_data(encode(BASIC_DATA)))/binary, (net_helper:encode_string(SCENENAME))/binary>>};
encode(#friend_info{basic_data=BASIC_DATA, is_online=IS_ONLINE}) ->
	{?msg_friend_info, <<(net_helper:get_encode_binary_data(encode(BASIC_DATA)))/binary, IS_ONLINE:?INT>>};
encode(#mail_info{id=ID, type=TYPE, read_status=READ_STATUS, remain_time=REMAIN_TIME, sender_name=SENDER_NAME, sender_account=SENDER_ACCOUNT, title=TITLE, items=ITEMS}) ->
	{?msg_mail_info, <<ID:?UINT64, TYPE:?INT, READ_STATUS:?INT, REMAIN_TIME:?INT, (net_helper:encode_string(SENDER_NAME))/binary, (net_helper:encode_string(SENDER_ACCOUNT))/binary, (net_helper:encode_string(TITLE))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, ITEMS))/binary>>};
encode(#npc_info{npc_id=NPC_ID, body=BODY, head=HEAD, hair=HAIR, equip1=EQUIP1, equip2=EQUIP2, skeleton=SKELETON, npc_name=NPC_NAME}) ->
	{?msg_npc_info, <<NPC_ID:?INT, BODY:?INT, HEAD:?INT, HAIR:?INT, EQUIP1:?INT, EQUIP2:?INT, SKELETON:?INT, (net_helper:encode_string(NPC_NAME))/binary>>};
encode(#npc_map_mapping_info{id=ID, npc_id=NPC_ID, npc_name=NPC_NAME, pos=POS, script_id=SCRIPT_ID, action=ACTION, npc_key=NPC_KEY, towards=TOWARDS}) ->
	{?msg_npc_map_mapping_info, <<ID:?INT, NPC_ID:?INT, (net_helper:encode_string(NPC_NAME))/binary, (net_helper:get_encode_binary_data(encode(POS)))/binary, SCRIPT_ID:?INT, ACTION:?INT, (net_helper:encode_string(NPC_KEY))/binary, TOWARDS:?INT>>};
encode(#npc_command{template_id=TEMPLATE_ID, unique_id=UNIQUE_ID, page_index=PAGE_INDEX, command_index=COMMAND_INDEX}) ->
	{?msg_npc_command, <<TEMPLATE_ID:?INT, UNIQUE_ID:?UINT64, PAGE_INDEX:?INT, COMMAND_INDEX:?INT>>};
encode(#furniture_position{position_index=POSITION_INDEX, is_used=IS_USED, used_account=USED_ACCOUNT, status=STATUS, func_id=FUNC_ID, use_time=USE_TIME}) ->
	{?msg_furniture_position, <<POSITION_INDEX:?INT, IS_USED:?INT, (net_helper:encode_string(USED_ACCOUNT))/binary, STATUS:?INT, FUNC_ID:?INT, (net_helper:get_encode_binary_data(encode(USE_TIME)))/binary>>};
encode(#player_task{id=ID, account=ACCOUNT, task_id=TASK_ID, step_index=STEP_INDEX, track=TRACK, status=STATUS, create_date=CREATE_DATE}) ->
	{?msg_player_task, <<ID:?UINT64, (net_helper:encode_string(ACCOUNT))/binary, TASK_ID:?INT, STEP_INDEX:?INT, TRACK:?INT, STATUS:?INT, (net_helper:get_encode_binary_data(encode(CREATE_DATE)))/binary>>};
encode(#eq_user_info{full_name=FULL_NAME, nick_name=NICK_NAME, birthday=BIRTHDAY, phone=PHONE, url=URL, email=EMAIL, country=COUNTRY, region=REGION, city=CITY, street=STREET, org_name=ORG_NAME, org_unit=ORG_UNIT, title=TITLE, role=ROLE, desc=DESC, photo_type=PHOTO_TYPE, photo_data=PHOTO_DATA}) ->
	{?msg_eq_user_info, <<(net_helper:encode_string(FULL_NAME))/binary, (net_helper:encode_string(NICK_NAME))/binary, (net_helper:encode_string(BIRTHDAY))/binary, (net_helper:encode_string(PHONE))/binary, (net_helper:encode_string(URL))/binary, (net_helper:encode_string(EMAIL))/binary, (net_helper:encode_string(COUNTRY))/binary, (net_helper:encode_string(REGION))/binary, (net_helper:encode_string(CITY))/binary, (net_helper:encode_string(STREET))/binary, (net_helper:encode_string(ORG_NAME))/binary, (net_helper:encode_string(ORG_UNIT))/binary, (net_helper:encode_string(TITLE))/binary, (net_helper:encode_string(ROLE))/binary, (net_helper:encode_string(DESC))/binary, (net_helper:encode_string(PHOTO_TYPE))/binary, (net_helper:encode_string(PHOTO_DATA))/binary>>};
encode(#offline_invite_record{invite_type=INVITE_TYPE, invitor=INVITOR, invitor_name=INVITOR_NAME, date=DATE}) ->
	{?msg_offline_invite_record, <<INVITE_TYPE:?INT, (net_helper:encode_string(INVITOR))/binary, (net_helper:encode_string(INVITOR_NAME))/binary, (net_helper:get_encode_binary_data(encode(DATE)))/binary>>};
encode(#friend_setting_record{accept_all_invite=ACCEPT_ALL_INVITE}) ->
	{?msg_friend_setting_record, <<ACCEPT_ALL_INVITE:?INT>>};
encode(#magic_box{type=TYPE, inst_id=INST_ID, pos=POS, graphic_id=GRAPHIC_ID}) ->
	{?msg_magic_box, <<TYPE:?INT, INST_ID:?UINT64, (net_helper:get_encode_binary_data(encode(POS)))/binary, GRAPHIC_ID:?INT>>};
encode(#gift_box{inst_id=INST_ID, sender=SENDER, sender_name=SENDER_NAME, receiver=RECEIVER, receiver_name=RECEIVER_NAME, name=NAME, grid_vec=GRID_VEC, box_id=BOX_ID, card_id=CARD_ID, comment=COMMENT, pos=POS, date=DATE, is_anonymous=IS_ANONYMOUS}) ->
	{?msg_gift_box, <<INST_ID:?UINT64, (net_helper:encode_string(SENDER))/binary, (net_helper:encode_string(SENDER_NAME))/binary, (net_helper:encode_string(RECEIVER))/binary, (net_helper:encode_string(RECEIVER_NAME))/binary, (net_helper:encode_string(NAME))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GRID_VEC))/binary, BOX_ID:?INT, CARD_ID:?INT, (net_helper:encode_string(COMMENT))/binary, (net_helper:get_encode_binary_data(encode(POS)))/binary, (net_helper:get_encode_binary_data(encode(DATE)))/binary, IS_ANONYMOUS:?INT>>};
encode(#flower_log{time=TIME, who=WHO}) ->
	{?msg_flower_log, <<(net_helper:get_encode_binary_data(encode(TIME)))/binary, (net_helper:encode_string(WHO))/binary>>};
encode(#egg_log{time=TIME, who=WHO}) ->
	{?msg_egg_log, <<(net_helper:get_encode_binary_data(encode(TIME)))/binary, (net_helper:encode_string(WHO))/binary>>};
encode(#req_login{account=ACCOUNT, password=PASSWORD}) ->
	{?msg_req_login, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_string(PASSWORD))/binary>>};
encode(#notify_login_result{result=RESULT}) ->
	{?msg_notify_login_result, <<RESULT:?INT>>};
encode(#notify_repeat_login{account=ACCOUNT}) ->
	{?msg_notify_repeat_login, <<(net_helper:encode_string(ACCOUNT))/binary>>};
encode(#req_get_roles{}) ->
	{?msg_req_get_roles, null};
encode(#notify_get_roles_result{player=PLAYER, nick_name=NICK_NAME}) ->
	{?msg_notify_get_roles_result, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, PLAYER))/binary, (net_helper:encode_string(NICK_NAME))/binary>>};
encode(#req_create_role{basic_data=BASIC_DATA, equips=EQUIPS}) ->
	{?msg_req_create_role, <<(net_helper:get_encode_binary_data(encode(BASIC_DATA)))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, EQUIPS))/binary>>};
encode(#notify_create_role_result{result=RESULT}) ->
	{?msg_notify_create_role_result, <<RESULT:?INT>>};
encode(#req_enter_scene{}) ->
	{?msg_req_enter_scene, null};
encode(#notify_enter_scene_result{result=RESULT}) ->
	{?msg_notify_enter_scene_result, <<RESULT:?INT>>};
encode(#notify_body_data{grid_vec=GRID_VEC}) ->
	{?msg_notify_body_data, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GRID_VEC))/binary>>};
encode(#req_enter_player_house{account=ACCOUNT}) ->
	{?msg_req_enter_player_house, <<(net_helper:encode_string(ACCOUNT))/binary>>};
encode(#req_enter_common_scene{scene_id=SCENE_ID}) ->
	{?msg_req_enter_common_scene, <<SCENE_ID:?INT>>};
encode(#notify_enter_common_scene{scene_id=SCENE_ID, enter_pos=ENTER_POS}) ->
	{?msg_notify_enter_common_scene, <<SCENE_ID:?INT, (net_helper:get_encode_binary_data(encode(ENTER_POS)))/binary>>};
encode(#notify_enter_player_house{account=ACCOUNT, enter_pos=ENTER_POS, house_tplt_id=HOUSE_TPLT_ID}) ->
	{?msg_notify_enter_player_house, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:get_encode_binary_data(encode(ENTER_POS)))/binary, HOUSE_TPLT_ID:?INT>>};
encode(#req_teleport{id=ID}) ->
	{?msg_req_teleport, <<ID:?INT>>};
encode(#req_open_change_scene_guide{}) ->
	{?msg_req_open_change_scene_guide, null};
encode(#notify_open_change_scene_guide{}) ->
	{?msg_notify_open_change_scene_guide, null};
encode(#req_close_change_scene_guide{}) ->
	{?msg_req_close_change_scene_guide, null};
encode(#notify_close_change_scene_guide{}) ->
	{?msg_notify_close_change_scene_guide, null};
encode(#notify_finish_enter_scene{}) ->
	{?msg_notify_finish_enter_scene, null};
encode(#req_kick_guest{target_player=TARGET_PLAYER}) ->
	{?msg_req_kick_guest, <<(net_helper:encode_string(TARGET_PLAYER))/binary>>};
encode(#notify_other_player_data{player=PLAYER, property=PROPERTY, curr_pos=CURR_POS, dest_pos=DEST_POS, used_furni_inst_id=USED_FURNI_INST_ID, used_furni_func_id=USED_FURNI_FUNC_ID, used_furni_index=USED_FURNI_INDEX}) ->
	{?msg_notify_other_player_data, <<(net_helper:get_encode_binary_data(encode(PLAYER)))/binary, (net_helper:get_encode_binary_data(encode(PROPERTY)))/binary, (net_helper:get_encode_binary_data(encode(CURR_POS)))/binary, (net_helper:get_encode_binary_data(encode(DEST_POS)))/binary, USED_FURNI_INST_ID:?UINT64, USED_FURNI_FUNC_ID:?INT, USED_FURNI_INDEX:?INT>>};
encode(#notify_other_player_info{player=PLAYER, property=PROPERTY, body=BODY}) ->
	{?msg_notify_other_player_info, <<(net_helper:get_encode_binary_data(encode(PLAYER)))/binary, (net_helper:get_encode_binary_data(encode(PROPERTY)))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, BODY))/binary>>};
encode(#req_other_player_info{account=ACCOUNT}) ->
	{?msg_req_other_player_info, <<(net_helper:encode_string(ACCOUNT))/binary>>};
encode(#notify_player_enter_scene{account=ACCOUNT}) ->
	{?msg_notify_player_enter_scene, <<(net_helper:encode_string(ACCOUNT))/binary>>};
encode(#notify_player_leave_scene{account=ACCOUNT}) ->
	{?msg_notify_player_leave_scene, <<(net_helper:encode_string(ACCOUNT))/binary>>};
encode(#req_logout{}) ->
	{?msg_req_logout, null};
encode(#notify_player_data{basic_data=BASIC_DATA}) ->
	{?msg_notify_player_data, <<(net_helper:get_encode_binary_data(encode(BASIC_DATA)))/binary>>};
encode(#req_walk{path=PATH}) ->
	{?msg_req_walk, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, PATH))/binary>>};
encode(#req_start_walk{curr_pos=CURR_POS, dest_pos=DEST_POS, move_type=MOVE_TYPE}) ->
	{?msg_req_start_walk, <<(net_helper:get_encode_binary_data(encode(CURR_POS)))/binary, (net_helper:get_encode_binary_data(encode(DEST_POS)))/binary, MOVE_TYPE:?INT>>};
encode(#notify_start_walk{account=ACCOUNT, curr_pos=CURR_POS, dest_pos=DEST_POS, move_type=MOVE_TYPE}) ->
	{?msg_notify_start_walk, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:get_encode_binary_data(encode(CURR_POS)))/binary, (net_helper:get_encode_binary_data(encode(DEST_POS)))/binary, MOVE_TYPE:?INT>>};
encode(#req_stop_walk{pos=POS}) ->
	{?msg_req_stop_walk, <<(net_helper:get_encode_binary_data(encode(POS)))/binary>>};
encode(#notify_stop_walk{account=ACCOUNT, pos=POS}) ->
	{?msg_notify_stop_walk, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:get_encode_binary_data(encode(POS)))/binary>>};
encode(#req_sync_position{pos=POS}) ->
	{?msg_req_sync_position, <<(net_helper:get_encode_binary_data(encode(POS)))/binary>>};
encode(#req_walk_for_use_furniture{curr_pos=CURR_POS, dest_pos=DEST_POS, instance_id=INSTANCE_ID, function_id=FUNCTION_ID, move_type=MOVE_TYPE}) ->
	{?msg_req_walk_for_use_furniture, <<(net_helper:get_encode_binary_data(encode(CURR_POS)))/binary, (net_helper:get_encode_binary_data(encode(DEST_POS)))/binary, INSTANCE_ID:?UINT64, FUNCTION_ID:?INT, MOVE_TYPE:?INT>>};
encode(#req_add_friend{friend_account=FRIEND_ACCOUNT}) ->
	{?msg_req_add_friend, <<(net_helper:encode_string(FRIEND_ACCOUNT))/binary>>};
encode(#req_remove_friend{friend_account=FRIEND_ACCOUNT}) ->
	{?msg_req_remove_friend, <<(net_helper:encode_string(FRIEND_ACCOUNT))/binary>>};
encode(#notify_add_friend{friend_account=FRIEND_ACCOUNT, friend_infos=FRIEND_INFOS}) ->
	{?msg_notify_add_friend, <<(net_helper:encode_string(FRIEND_ACCOUNT))/binary, (net_helper:get_encode_binary_data(encode(FRIEND_INFOS)))/binary>>};
encode(#notify_remove_friend{friend_account=FRIEND_ACCOUNT}) ->
	{?msg_notify_remove_friend, <<(net_helper:encode_string(FRIEND_ACCOUNT))/binary>>};
encode(#req_friend_list{}) ->
	{?msg_req_friend_list, null};
encode(#notify_friend_list{friend_vec=FRIEND_VEC}) ->
	{?msg_notify_friend_list, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, FRIEND_VEC))/binary>>};
encode(#notify_friend_online{friend_account=FRIEND_ACCOUNT}) ->
	{?msg_notify_friend_online, <<(net_helper:encode_string(FRIEND_ACCOUNT))/binary>>};
encode(#notify_friend_offline{friend_account=FRIEND_ACCOUNT}) ->
	{?msg_notify_friend_offline, <<(net_helper:encode_string(FRIEND_ACCOUNT))/binary>>};
encode(#req_eq_users_info{account_list=ACCOUNT_LIST}) ->
	{?msg_req_eq_users_info, <<(net_helper:encode_list(fun(E)-> net_helper:encode_string(E) end, ACCOUNT_LIST))/binary>>};
encode(#notify_eq_user_info{account=ACCOUNT, info=INFO}) ->
	{?msg_notify_eq_user_info, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:get_encode_binary_data(encode(INFO)))/binary>>};
encode(#notify_eq_user_presence{account=ACCOUNT, presence=PRESENCE}) ->
	{?msg_notify_eq_user_presence, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_string(PRESENCE))/binary>>};
encode(#notify_offline_invites{inv_list=INV_LIST}) ->
	{?msg_notify_offline_invites, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, INV_LIST))/binary>>};
encode(#req_friend_setting{}) ->
	{?msg_req_friend_setting, null};
encode(#notify_friend_setting{setting=SETTING}) ->
	{?msg_notify_friend_setting, <<(net_helper:get_encode_binary_data(encode(SETTING)))/binary>>};
encode(#req_friend_setting_accept_all{accept_all=ACCEPT_ALL}) ->
	{?msg_req_friend_setting_accept_all, <<ACCEPT_ALL:?INT>>};
encode(#req_friend_setting_black_someone{who=WHO}) ->
	{?msg_req_friend_setting_black_someone, <<(net_helper:encode_string(WHO))/binary>>};
encode(#req_search_player_by_nick{nick=NICK}) ->
	{?msg_req_search_player_by_nick, <<(net_helper:encode_string(NICK))/binary>>};
encode(#req_search_player_by_mail{mail=MAIL}) ->
	{?msg_req_search_player_by_mail, <<(net_helper:encode_string(MAIL))/binary>>};
encode(#notify_search_player{players=PLAYERS}) ->
	{?msg_notify_search_player, <<(net_helper:encode_list(fun(E)-> net_helper:encode_string(E) end, PLAYERS))/binary>>};
encode(#req_chat_around{content=CONTENT}) ->
	{?msg_req_chat_around, <<(net_helper:encode_string(CONTENT))/binary>>};
encode(#notify_chat_around{account=ACCOUNT, player_name=PLAYER_NAME, content=CONTENT}) ->
	{?msg_notify_chat_around, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_string(PLAYER_NAME))/binary, (net_helper:encode_string(CONTENT))/binary>>};
encode(#req_chat_tell{target_player=TARGET_PLAYER, content=CONTENT}) ->
	{?msg_req_chat_tell, <<(net_helper:encode_string(TARGET_PLAYER))/binary, (net_helper:encode_string(CONTENT))/binary>>};
encode(#notify_chat_tell{speaker=SPEAKER, speaker_name=SPEAKER_NAME, content=CONTENT}) ->
	{?msg_notify_chat_tell, <<(net_helper:encode_string(SPEAKER))/binary, (net_helper:encode_string(SPEAKER_NAME))/binary, (net_helper:encode_string(CONTENT))/binary>>};
encode(#house_furniture{instance_id=INSTANCE_ID, template_id=TEMPLATE_ID, x=X, z=Z, height=HEIGHT, floor=FLOOR, face=FACE, item_tempid=ITEM_TEMPID, max_use_player=MAX_USE_PLAYER, position_list=POSITION_LIST, function_list=FUNCTION_LIST, use_sex=USE_SEX, permission=PERMISSION, use_type=USE_TYPE, status_qty=STATUS_QTY, del_time=DEL_TIME}) ->
	{?msg_house_furniture, <<INSTANCE_ID:?UINT64, TEMPLATE_ID:?INT, X:?INT, Z:?INT, HEIGHT:?INT, FLOOR:?INT, FACE:?INT, ITEM_TEMPID:?INT, MAX_USE_PLAYER:?INT, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, POSITION_LIST))/binary, (net_helper:encode_list(fun(E)-> <<E:?INT>>end, FUNCTION_LIST))/binary, USE_SEX:?INT, PERMISSION:?INT, USE_TYPE:?INT, STATUS_QTY:?INT, (net_helper:get_encode_binary_data(encode(DEL_TIME)))/binary>>};
encode(#room_tex{room_id=ROOM_ID, type=TYPE, tex=TEX, floor_id=FLOOR_ID}) ->
	{?msg_room_tex, <<ROOM_ID:?INT, TYPE:?INT, (net_helper:encode_string(TEX))/binary, FLOOR_ID:?INT>>};
encode(#house_component_tex{component_name=COMPONENT_NAME, new_tex=NEW_TEX}) ->
	{?msg_house_component_tex, <<(net_helper:encode_string(COMPONENT_NAME))/binary, (net_helper:encode_string(NEW_TEX))/binary>>};
encode(#house_component_mesh{component_name=COMPONENT_NAME, new_mesh=NEW_MESH}) ->
	{?msg_house_component_mesh, <<(net_helper:encode_string(COMPONENT_NAME))/binary, (net_helper:encode_string(NEW_MESH))/binary>>};
encode(#house_visit_log{type=TYPE, datetime=DATETIME, account=ACCOUNT, username=USERNAME, item_id=ITEM_ID}) ->
	{?msg_house_visit_log, <<TYPE:?INT, (net_helper:get_encode_binary_data(encode(DATETIME)))/binary, (net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_string(USERNAME))/binary, ITEM_ID:?INT>>};
encode(#house_data{owner=OWNER, instance_id=INSTANCE_ID, template_id=TEMPLATE_ID, furniture_permission=FURNITURE_PERMISSION, furniture_vec=FURNITURE_VEC, room_tex_vec=ROOM_TEX_VEC, component_tex_vec=COMPONENT_TEX_VEC, component_mesh_vec=COMPONENT_MESH_VEC, welcome_words=WELCOME_WORDS, house_permission=HOUSE_PERMISSION, visit_log=VISIT_LOG, buy_date=BUY_DATE, level=LEVEL, gift_box_vec=GIFT_BOX_VEC, magic_box_vec=MAGIC_BOX_VEC, house_clean=HOUSE_CLEAN}) ->
	{?msg_house_data, <<(net_helper:encode_string(OWNER))/binary, INSTANCE_ID:?UINT64, TEMPLATE_ID:?INT, FURNITURE_PERMISSION:?INT, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, FURNITURE_VEC))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, ROOM_TEX_VEC))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, COMPONENT_TEX_VEC))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, COMPONENT_MESH_VEC))/binary, (net_helper:encode_string(WELCOME_WORDS))/binary, HOUSE_PERMISSION:?INT, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, VISIT_LOG))/binary, (net_helper:get_encode_binary_data(encode(BUY_DATE)))/binary, LEVEL:?INT, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GIFT_BOX_VEC))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, MAGIC_BOX_VEC))/binary, HOUSE_CLEAN:?INT>>};
encode(#notify_house_clean{house_clean=HOUSE_CLEAN}) ->
	{?msg_notify_house_clean, <<HOUSE_CLEAN:?INT>>};
encode(#notify_house_data{data=DATA}) ->
	{?msg_notify_house_data, <<(net_helper:get_encode_binary_data(encode(DATA)))/binary>>};
encode(#notify_house_gift_box{gift_box_vec=GIFT_BOX_VEC}) ->
	{?msg_notify_house_gift_box, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GIFT_BOX_VEC))/binary>>};
encode(#req_placed_furniture{grid_index=GRID_INDEX, x=X, z=Z, height=HEIGHT, floor=FLOOR, face=FACE}) ->
	{?msg_req_placed_furniture, <<GRID_INDEX:?INT, X:?INT, Z:?INT, HEIGHT:?INT, FLOOR:?INT, FACE:?INT>>};
encode(#req_start_edit_house{}) ->
	{?msg_req_start_edit_house, null};
encode(#notify_start_edit_house{}) ->
	{?msg_notify_start_edit_house, null};
encode(#req_end_edit_house{}) ->
	{?msg_req_end_edit_house, null};
encode(#notify_end_edit_house{}) ->
	{?msg_notify_end_edit_house, null};
encode(#req_recovery_furniture{furniture_id=FURNITURE_ID}) ->
	{?msg_req_recovery_furniture, <<FURNITURE_ID:?UINT64>>};
encode(#req_replace_component_mesh{component_mesh=COMPONENT_MESH}) ->
	{?msg_req_replace_component_mesh, <<(net_helper:get_encode_binary_data(encode(COMPONENT_MESH)))/binary>>};
encode(#req_replace_component_tex{component_tex=COMPONENT_TEX}) ->
	{?msg_req_replace_component_tex, <<(net_helper:get_encode_binary_data(encode(COMPONENT_TEX)))/binary>>};
encode(#req_replace_room_tex{room_tex_info=ROOM_TEX_INFO, goods_id=GOODS_ID}) ->
	{?msg_req_replace_room_tex, <<(net_helper:get_encode_binary_data(encode(ROOM_TEX_INFO)))/binary, GOODS_ID:?INT>>};
encode(#notify_replace_room_tex{room_tex_info=ROOM_TEX_INFO}) ->
	{?msg_notify_replace_room_tex, <<(net_helper:get_encode_binary_data(encode(ROOM_TEX_INFO)))/binary>>};
encode(#req_move_furniture{instance_id=INSTANCE_ID, x=X, z=Z, height=HEIGHT, floor=FLOOR, face=FACE}) ->
	{?msg_req_move_furniture, <<INSTANCE_ID:?UINT64, X:?INT, Z:?INT, HEIGHT:?INT, FLOOR:?INT, FACE:?INT>>};
encode(#notify_add_furniture{instance_id=INSTANCE_ID, template_id=TEMPLATE_ID, x=X, z=Z, height=HEIGHT, floor=FLOOR, face=FACE, del_time=DEL_TIME}) ->
	{?msg_notify_add_furniture, <<INSTANCE_ID:?UINT64, TEMPLATE_ID:?INT, X:?INT, Z:?INT, HEIGHT:?INT, FLOOR:?INT, FACE:?INT, (net_helper:get_encode_binary_data(encode(DEL_TIME)))/binary>>};
encode(#notify_delete_furniture{instance_id=INSTANCE_ID}) ->
	{?msg_notify_delete_furniture, <<INSTANCE_ID:?UINT64>>};
encode(#req_house_basic_data{}) ->
	{?msg_req_house_basic_data, null};
encode(#notify_house_basic_data{data=DATA}) ->
	{?msg_notify_house_basic_data, <<(net_helper:get_encode_binary_data(encode(DATA)))/binary>>};
encode(#req_set_house_welcome_words{words=WORDS}) ->
	{?msg_req_set_house_welcome_words, <<(net_helper:encode_string(WORDS))/binary>>};
encode(#notify_set_house_welcome_words{result=RESULT}) ->
	{?msg_notify_set_house_welcome_words, <<RESULT:?INT>>};
encode(#req_set_house_permission{house_permission=HOUSE_PERMISSION, furniture_permission=FURNITURE_PERMISSION}) ->
	{?msg_req_set_house_permission, <<HOUSE_PERMISSION:?INT, FURNITURE_PERMISSION:?INT>>};
encode(#notify_set_house_permission{result=RESULT}) ->
	{?msg_notify_set_house_permission, <<RESULT:?INT>>};
encode(#req_clear_house_log{}) ->
	{?msg_req_clear_house_log, null};
encode(#notify_clear_house_log{result=RESULT}) ->
	{?msg_notify_clear_house_log, <<RESULT:?INT>>};
encode(#req_change_house{type=TYPE, inst_id=INST_ID}) ->
	{?msg_req_change_house, <<TYPE:?INT, INST_ID:?UINT64>>};
encode(#notify_change_house{inst_id=INST_ID}) ->
	{?msg_notify_change_house, <<INST_ID:?UINT64>>};
encode(#req_house_list{account=ACCOUNT}) ->
	{?msg_req_house_list, <<(net_helper:encode_string(ACCOUNT))/binary>>};
encode(#house_plot_info{instance_id=INSTANCE_ID, template_id=TEMPLATE_ID, buy_date=BUY_DATE, plot_id=PLOT_ID}) ->
	{?msg_house_plot_info, <<INSTANCE_ID:?UINT64, TEMPLATE_ID:?INT, (net_helper:get_encode_binary_data(encode(BUY_DATE)))/binary, PLOT_ID:?UINT64>>};
encode(#notify_house_list{account=ACCOUNT, house_vec=HOUSE_VEC}) ->
	{?msg_notify_house_list, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, HOUSE_VEC))/binary>>};
encode(#notify_player_property_changed{player_clean=PLAYER_CLEAN, player_health=PLAYER_HEALTH, player_charm=PLAYER_CHARM, active_value=ACTIVE_VALUE}) ->
	{?msg_notify_player_property_changed, <<PLAYER_CLEAN:?INT, PLAYER_HEALTH:?INT, PLAYER_CHARM:?INT, ACTIVE_VALUE:?INT>>};
encode(#control_furni_info{instance_id=INSTANCE_ID, function_id=FUNCTION_ID}) ->
	{?msg_control_furni_info, <<INSTANCE_ID:?UINT64, FUNCTION_ID:?INT>>};
encode(#mutli_furni_info{account=ACCOUNT, position_index=POSITION_INDEX, instance_id=INSTANCE_ID, status=STATUS, function_id=FUNCTION_ID}) ->
	{?msg_mutli_furni_info, <<(net_helper:encode_string(ACCOUNT))/binary, POSITION_INDEX:?INT, INSTANCE_ID:?UINT64, STATUS:?INT, FUNCTION_ID:?INT>>};
encode(#req_use_mutli_furni{furni_vec=FURNI_VEC}) ->
	{?msg_req_use_mutli_furni, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, FURNI_VEC))/binary>>};
encode(#notify_use_mutli_furni{furni_vec=FURNI_VEC}) ->
	{?msg_notify_use_mutli_furni, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, FURNI_VEC))/binary>>};
encode(#notify_use_furniture_result{account=ACCOUNT, position_index=POSITION_INDEX, instance_id=INSTANCE_ID, status=STATUS, function_id=FUNCTION_ID}) ->
	{?msg_notify_use_furniture_result, <<(net_helper:encode_string(ACCOUNT))/binary, POSITION_INDEX:?INT, INSTANCE_ID:?UINT64, STATUS:?INT, FUNCTION_ID:?INT>>};
encode(#req_stop_using_furniture{account=ACCOUNT}) ->
	{?msg_req_stop_using_furniture, <<(net_helper:encode_string(ACCOUNT))/binary>>};
encode(#notify_stop_using_furniture_result{account=ACCOUNT, position_index=POSITION_INDEX, instance_id=INSTANCE_ID, function_id=FUNCTION_ID, pre_status=PRE_STATUS, status=STATUS, pos=POS}) ->
	{?msg_notify_stop_using_furniture_result, <<(net_helper:encode_string(ACCOUNT))/binary, POSITION_INDEX:?INT, INSTANCE_ID:?UINT64, FUNCTION_ID:?INT, PRE_STATUS:?INT, STATUS:?INT, (net_helper:get_encode_binary_data(encode(POS)))/binary>>};
encode(#notify_package{account=ACCOUNT, type=TYPE, grid_vec=GRID_VEC}) ->
	{?msg_notify_package, <<(net_helper:encode_string(ACCOUNT))/binary, TYPE:?INT, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GRID_VEC))/binary>>};
encode(#req_swap_item{type=TYPE, index1=INDEX1, index2=INDEX2}) ->
	{?msg_req_swap_item, <<TYPE:?INT, INDEX1:?INT, INDEX2:?INT>>};
encode(#req_use_item{index=INDEX}) ->
	{?msg_req_use_item, <<INDEX:?INT>>};
encode(#req_move_item{pack1_type=PACK1_TYPE, index1=INDEX1, pack2_type=PACK2_TYPE, index2=INDEX2}) ->
	{?msg_req_move_item, <<PACK1_TYPE:?INT, INDEX1:?INT, PACK2_TYPE:?INT, INDEX2:?INT>>};
encode(#req_delete_item{grid_index=GRID_INDEX}) ->
	{?msg_req_delete_item, <<GRID_INDEX:?INT>>};
encode(#req_split_item{type=TYPE, src_gindex=SRC_GINDEX, target_gindex=TARGET_GINDEX, count=COUNT}) ->
	{?msg_req_split_item, <<TYPE:?INT, SRC_GINDEX:?INT, TARGET_GINDEX:?INT, COUNT:?INT>>};
encode(#req_resize_player_pack{}) ->
	{?msg_req_resize_player_pack, null};
encode(#req_lock_bag_item{item_inst_id=ITEM_INST_ID}) ->
	{?msg_req_lock_bag_item, <<ITEM_INST_ID:?UINT64>>};
encode(#req_unlock_bag_item{item_inst_id=ITEM_INST_ID}) ->
	{?msg_req_unlock_bag_item, <<ITEM_INST_ID:?UINT64>>};
encode(#req_move_money{pack1_type=PACK1_TYPE, pack2_type=PACK2_TYPE, money=MONEY}) ->
	{?msg_req_move_money, <<PACK1_TYPE:?INT, PACK2_TYPE:?INT, MONEY:?INT>>};
encode(#req_switch_item{pack1_type=PACK1_TYPE, instance_id=INSTANCE_ID, pack2_type=PACK2_TYPE}) ->
	{?msg_req_switch_item, <<PACK1_TYPE:?INT, INSTANCE_ID:?UINT64, PACK2_TYPE:?INT>>};
encode(#notify_sys_msg{code=CODE, params=PARAMS}) ->
	{?msg_notify_sys_msg, <<CODE:?INT, (net_helper:encode_list(fun(E)-> net_helper:encode_string(E) end, PARAMS))/binary>>};
encode(#notify_sys_broadcast{id=ID, type=TYPE, content=CONTENT, play_times=PLAY_TIMES, priority=PRIORITY, show_seconds=SHOW_SECONDS, start_time=START_TIME}) ->
	{?msg_notify_sys_broadcast, <<ID:?UINT64, TYPE:?INT, (net_helper:encode_string(CONTENT))/binary, PLAY_TIMES:?INT, PRIORITY:?INT, SHOW_SECONDS:?INT, (net_helper:get_encode_binary_data(encode(START_TIME)))/binary>>};
encode(#req_fixed_broadcast{type=TYPE}) ->
	{?msg_req_fixed_broadcast, <<TYPE:?INT>>};
encode(#notify_del_broadcast{type=TYPE, id=ID}) ->
	{?msg_notify_del_broadcast, <<TYPE:?INT, ID:?UINT64>>};
encode(#req_gm_command{type=TYPE, params=PARAMS}) ->
	{?msg_req_gm_command, <<TYPE:?INT, (net_helper:encode_list(fun(E)-> net_helper:encode_string(E) end, PARAMS))/binary>>};
encode(#notify_npc_list{npc_map_mapping_list=NPC_MAP_MAPPING_LIST}) ->
	{?msg_notify_npc_list, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, NPC_MAP_MAPPING_LIST))/binary>>};
encode(#notify_npc_close_dialog{}) ->
	{?msg_notify_npc_close_dialog, null};
encode(#notify_npc_show_dialog_by_option{template_id=TEMPLATE_ID, command_list=COMMAND_LIST}) ->
	{?msg_notify_npc_show_dialog_by_option, <<TEMPLATE_ID:?INT, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, COMMAND_LIST))/binary>>};
encode(#notify_npc_show_dialog_by_message{template_id=TEMPLATE_ID, unique_id=UNIQUE_ID, page_index=PAGE_INDEX, command_index=COMMAND_INDEX}) ->
	{?msg_notify_npc_show_dialog_by_message, <<TEMPLATE_ID:?INT, UNIQUE_ID:?UINT64, PAGE_INDEX:?INT, COMMAND_INDEX:?INT>>};
encode(#notify_npc_show_dialog_by_item{template_id=TEMPLATE_ID, item_list=ITEM_LIST}) ->
	{?msg_notify_npc_show_dialog_by_item, <<TEMPLATE_ID:?INT, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, ITEM_LIST))/binary>>};
encode(#req_npc_command{npc_key=NPC_KEY, unique_id=UNIQUE_ID, page_index=PAGE_INDEX, command_index=COMMAND_INDEX}) ->
	{?msg_req_npc_command, <<(net_helper:encode_string(NPC_KEY))/binary, UNIQUE_ID:?UINT64, PAGE_INDEX:?INT, COMMAND_INDEX:?INT>>};
encode(#req_player_basic_data{}) ->
	{?msg_req_player_basic_data, null};
encode(#notify_player_basic_data{basic_data=BASIC_DATA, property=PROPERTY}) ->
	{?msg_notify_player_basic_data, <<(net_helper:get_encode_binary_data(encode(BASIC_DATA)))/binary, (net_helper:get_encode_binary_data(encode(PROPERTY)))/binary>>};
encode(#req_start_body_action{target_account=TARGET_ACCOUNT, action=ACTION}) ->
	{?msg_req_start_body_action, <<(net_helper:encode_string(TARGET_ACCOUNT))/binary, (net_helper:encode_string(ACTION))/binary>>};
encode(#notify_start_body_action{player_account=PLAYER_ACCOUNT, target_account=TARGET_ACCOUNT, action=ACTION}) ->
	{?msg_notify_start_body_action, <<(net_helper:encode_string(PLAYER_ACCOUNT))/binary, (net_helper:encode_string(TARGET_ACCOUNT))/binary, (net_helper:encode_string(ACTION))/binary>>};
encode(#req_play_animation{target_account=TARGET_ACCOUNT, loop=LOOP, ani=ANI, action=ACTION}) ->
	{?msg_req_play_animation, <<(net_helper:encode_string(TARGET_ACCOUNT))/binary, LOOP:?INT, (net_helper:encode_string(ANI))/binary, (net_helper:encode_string(ACTION))/binary>>};
encode(#notify_play_animation{player_account=PLAYER_ACCOUNT, target_account=TARGET_ACCOUNT, loop=LOOP, ani=ANI, action=ACTION}) ->
	{?msg_notify_play_animation, <<(net_helper:encode_string(PLAYER_ACCOUNT))/binary, (net_helper:encode_string(TARGET_ACCOUNT))/binary, LOOP:?INT, (net_helper:encode_string(ANI))/binary, (net_helper:encode_string(ACTION))/binary>>};
encode(#req_end_body_action{}) ->
	{?msg_req_end_body_action, null};
encode(#notify_end_body_action{player_account=PLAYER_ACCOUNT, target_account=TARGET_ACCOUNT}) ->
	{?msg_notify_end_body_action, <<(net_helper:encode_string(PLAYER_ACCOUNT))/binary, (net_helper:encode_string(TARGET_ACCOUNT))/binary>>};
encode(#req_sync_body_state{body_state=BODY_STATE}) ->
	{?msg_req_sync_body_state, <<BODY_STATE:?INT>>};
encode(#notify_other_body_state{other_account=OTHER_ACCOUNT, body_state=BODY_STATE}) ->
	{?msg_notify_other_body_state, <<(net_helper:encode_string(OTHER_ACCOUNT))/binary, BODY_STATE:?INT>>};
encode(#req_start_change_looks{}) ->
	{?msg_req_start_change_looks, null};
encode(#notify_start_change_looks{type=TYPE}) ->
	{?msg_notify_start_change_looks, <<TYPE:?INT>>};
encode(#req_cancel_change_looks{}) ->
	{?msg_req_cancel_change_looks, null};
encode(#req_end_change_looks{new_hair=NEW_HAIR, new_hair_color=NEW_HAIR_COLOR, new_face=NEW_FACE, new_skin_color=NEW_SKIN_COLOR, new_beard=NEW_BEARD}) ->
	{?msg_req_end_change_looks, <<NEW_HAIR:?INT, NEW_HAIR_COLOR:?INT, NEW_FACE:?INT, NEW_SKIN_COLOR:?INT, NEW_BEARD:?INT>>};
encode(#notify_change_looks{account=ACCOUNT, new_hair=NEW_HAIR, new_hair_color=NEW_HAIR_COLOR, new_face=NEW_FACE, new_skin_color=NEW_SKIN_COLOR, new_beard=NEW_BEARD}) ->
	{?msg_notify_change_looks, <<(net_helper:encode_string(ACCOUNT))/binary, NEW_HAIR:?INT, NEW_HAIR_COLOR:?INT, NEW_FACE:?INT, NEW_SKIN_COLOR:?INT, NEW_BEARD:?INT>>};
encode(#notify_end_change_looks{}) ->
	{?msg_notify_end_change_looks, null};
encode(#notify_around_change_dress{account=ACCOUNT, pos=POS, template_id=TEMPLATE_ID}) ->
	{?msg_notify_around_change_dress, <<(net_helper:encode_string(ACCOUNT))/binary, POS:?INT, TEMPLATE_ID:?INT>>};
encode(#req_invite_someone{invitee=INVITEE, type=TYPE}) ->
	{?msg_req_invite_someone, <<(net_helper:encode_string(INVITEE))/binary, TYPE:?INT>>};
encode(#notify_invitation{invitor=INVITOR, invitor_name=INVITOR_NAME, type=TYPE}) ->
	{?msg_notify_invitation, <<(net_helper:encode_string(INVITOR))/binary, (net_helper:encode_string(INVITOR_NAME))/binary, TYPE:?INT>>};
encode(#req_agree_invitation{invitor=INVITOR, type=TYPE}) ->
	{?msg_req_agree_invitation, <<(net_helper:encode_string(INVITOR))/binary, TYPE:?INT>>};
encode(#req_disagree_invitation{invitor=INVITOR, type=TYPE}) ->
	{?msg_req_disagree_invitation, <<(net_helper:encode_string(INVITOR))/binary, TYPE:?INT>>};
encode(#notify_cancel_invitation{}) ->
	{?msg_notify_cancel_invitation, null};
encode(#notify_start_trade{account=ACCOUNT}) ->
	{?msg_notify_start_trade, <<(net_helper:encode_string(ACCOUNT))/binary>>};
encode(#req_update_trade_money{money=MONEY}) ->
	{?msg_req_update_trade_money, <<MONEY:?INT>>};
encode(#notify_update_trade_money{money=MONEY}) ->
	{?msg_notify_update_trade_money, <<MONEY:?INT>>};
encode(#notify_other_update_trade_money{money=MONEY}) ->
	{?msg_notify_other_update_trade_money, <<MONEY:?INT>>};
encode(#req_put_trade_item{packet_index=PACKET_INDEX, trade_index=TRADE_INDEX}) ->
	{?msg_req_put_trade_item, <<PACKET_INDEX:?INT, TRADE_INDEX:?INT>>};
encode(#notify_put_trade_item{trade_index=TRADE_INDEX, item_data=ITEM_DATA, item_count=ITEM_COUNT}) ->
	{?msg_notify_put_trade_item, <<TRADE_INDEX:?INT, (net_helper:get_encode_binary_data(encode(ITEM_DATA)))/binary, ITEM_COUNT:?INT>>};
encode(#notify_other_put_trade_item{trade_index=TRADE_INDEX, item_data=ITEM_DATA, item_count=ITEM_COUNT}) ->
	{?msg_notify_other_put_trade_item, <<TRADE_INDEX:?INT, (net_helper:get_encode_binary_data(encode(ITEM_DATA)))/binary, ITEM_COUNT:?INT>>};
encode(#req_unput_trade_item{trade_index=TRADE_INDEX, packet_index=PACKET_INDEX}) ->
	{?msg_req_unput_trade_item, <<TRADE_INDEX:?INT, PACKET_INDEX:?INT>>};
encode(#notify_unput_trade_item{trade_index=TRADE_INDEX}) ->
	{?msg_notify_unput_trade_item, <<TRADE_INDEX:?INT>>};
encode(#notify_other_unput_trade_item{trade_index=TRADE_INDEX}) ->
	{?msg_notify_other_unput_trade_item, <<TRADE_INDEX:?INT>>};
encode(#req_swap_trade_item{index1=INDEX1, index2=INDEX2}) ->
	{?msg_req_swap_trade_item, <<INDEX1:?INT, INDEX2:?INT>>};
encode(#notify_swap_trade_item{index1=INDEX1, index2=INDEX2}) ->
	{?msg_notify_swap_trade_item, <<INDEX1:?INT, INDEX2:?INT>>};
encode(#notify_other_swap_trade_item{index1=INDEX1, index2=INDEX2}) ->
	{?msg_notify_other_swap_trade_item, <<INDEX1:?INT, INDEX2:?INT>>};
encode(#req_sure_trade{}) ->
	{?msg_req_sure_trade, null};
encode(#notify_sure_trade{}) ->
	{?msg_notify_sure_trade, null};
encode(#notify_other_sure_trade{}) ->
	{?msg_notify_other_sure_trade, null};
encode(#req_cancel_trade{}) ->
	{?msg_req_cancel_trade, null};
encode(#notify_cancel_trade{}) ->
	{?msg_notify_cancel_trade, null};
encode(#notify_trade_success{}) ->
	{?msg_notify_trade_success, null};
encode(#furniture_goods_data{goods_id=GOODS_ID, x=X, z=Z, height=HEIGHT, floor=FLOOR, face=FACE, room_id=ROOM_ID, type=TYPE, tex=TEX}) ->
	{?msg_furniture_goods_data, <<GOODS_ID:?INT, X:?INT, Z:?INT, HEIGHT:?INT, FLOOR:?INT, FACE:?INT, ROOM_ID:?INT, TYPE:?INT, (net_helper:encode_string(TEX))/binary>>};
encode(#req_buy_sys_shop_goods{goods_id=GOODS_ID}) ->
	{?msg_req_buy_sys_shop_goods, <<GOODS_ID:?INT>>};
encode(#req_buy_sys_shop_goods_list{goods_list=GOODS_LIST}) ->
	{?msg_req_buy_sys_shop_goods_list, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GOODS_LIST))/binary>>};
encode(#notify_buy_furniture_list{furnitures=FURNITURES}) ->
	{?msg_notify_buy_furniture_list, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, FURNITURES))/binary>>};
encode(#player_coin{account=ACCOUNT, gamecoin=GAMECOIN, eqcoin=EQCOIN}) ->
	{?msg_player_coin, <<(net_helper:encode_string(ACCOUNT))/binary, GAMECOIN:?INT, EQCOIN:?INT>>};
encode(#notify_update_coin{data=DATA}) ->
	{?msg_notify_update_coin, <<(net_helper:get_encode_binary_data(encode(DATA)))/binary>>};
encode(#req_buy_npc_shop_goods{goods_id=GOODS_ID}) ->
	{?msg_req_buy_npc_shop_goods, <<GOODS_ID:?INT>>};
encode(#req_sell_goods{grid_index=GRID_INDEX}) ->
	{?msg_req_sell_goods, <<GRID_INDEX:?INT>>};
encode(#notify_open_shop{type=TYPE}) ->
	{?msg_notify_open_shop, <<TYPE:?INT>>};
encode(#farm_pet_food{period_date=PERIOD_DATE}) ->
	{?msg_farm_pet_food, <<(net_helper:get_encode_binary_data(encode(PERIOD_DATE)))/binary>>};
encode(#farm_log_message{log_type=LOG_TYPE, account=ACCOUNT, log_date=LOG_DATE, user_name=USER_NAME, crop_id=CROP_ID, crop_quantity=CROP_QUANTITY, pet_id=PET_ID, bite_coin=BITE_COIN}) ->
	{?msg_farm_log_message, <<LOG_TYPE:?INT, (net_helper:encode_string(ACCOUNT))/binary, (net_helper:get_encode_binary_data(encode(LOG_DATE)))/binary, (net_helper:encode_string(USER_NAME))/binary, CROP_ID:?INT, CROP_QUANTITY:?INT, PET_ID:?INT, BITE_COIN:?INT>>};
encode(#farm_log_gain{crop_id=CROP_ID, crop_quantity=CROP_QUANTITY, steal_quantity=STEAL_QUANTITY, gain_quantity=GAIN_QUANTITY}) ->
	{?msg_farm_log_gain, <<CROP_ID:?INT, CROP_QUANTITY:?INT, STEAL_QUANTITY:?INT, GAIN_QUANTITY:?INT>>};
encode(#farm_log_consumption{type=TYPE, log_date=LOG_DATE, crop_id=CROP_ID, crop_quantity=CROP_QUANTITY, use_coin=USE_COIN, leave_coin=LEAVE_COIN}) ->
	{?msg_farm_log_consumption, <<TYPE:?INT, (net_helper:get_encode_binary_data(encode(LOG_DATE)))/binary, CROP_ID:?INT, CROP_QUANTITY:?INT, USE_COIN:?INT, LEAVE_COIN:?INT>>};
encode(#farm_comment{type=TYPE, account=ACCOUNT, comment_date=COMMENT_DATE, username=USERNAME, content=CONTENT}) ->
	{?msg_farm_comment, <<TYPE:?INT, (net_helper:encode_string(ACCOUNT))/binary, (net_helper:get_encode_binary_data(encode(COMMENT_DATE)))/binary, (net_helper:encode_string(USERNAME))/binary, (net_helper:encode_string(CONTENT))/binary>>};
encode(#farm_decoration{background=BACKGROUND, house=HOUSE, doghouse=DOGHOUSE, dog=DOG, signpost=SIGNPOST, fence=FENCE, scarecrow=SCARECROW, billboard=BILLBOARD, billboard_content=BILLBOARD_CONTENT}) ->
	{?msg_farm_decoration, <<(net_helper:get_encode_binary_data(encode(BACKGROUND)))/binary, (net_helper:get_encode_binary_data(encode(HOUSE)))/binary, (net_helper:get_encode_binary_data(encode(DOGHOUSE)))/binary, (net_helper:get_encode_binary_data(encode(DOG)))/binary, (net_helper:get_encode_binary_data(encode(SIGNPOST)))/binary, (net_helper:get_encode_binary_data(encode(FENCE)))/binary, (net_helper:get_encode_binary_data(encode(SCARECROW)))/binary, (net_helper:get_encode_binary_data(encode(BILLBOARD)))/binary, (net_helper:get_encode_binary_data(encode(BILLBOARD_CONTENT)))/binary>>};
encode(#farm_crop_data{tempid=TEMPID, health=HEALTH, curr_damage=CURR_DAMAGE, start_time=START_TIME, pick_num=PICK_NUM, remain=REMAIN, output=OUTPUT, stage=STAGE, land_type=LAND_TYPE, picked=PICKED, stage_time=STAGE_TIME, aberrance_fruit=ABERRANCE_FRUIT}) ->
	{?msg_farm_crop_data, <<TEMPID:?INT, HEALTH:?INT, (net_helper:encode_list(fun(E)-> <<E:?INT>>end, CURR_DAMAGE))/binary, (net_helper:get_encode_binary_data(encode(START_TIME)))/binary, PICK_NUM:?INT, REMAIN:?INT, OUTPUT:?INT, STAGE:?INT, LAND_TYPE:?INT, PICKED:?INT, (net_helper:encode_list(fun(E)-> <<E:?INT>>end, STAGE_TIME))/binary, ABERRANCE_FRUIT:?INT>>};
encode(#player_farm_data{account=ACCOUNT, exp=EXP, decoration=DECORATION, pet_food=PET_FOOD, crops=CROPS, level=LEVEL}) ->
	{?msg_player_farm_data, <<(net_helper:encode_string(ACCOUNT))/binary, EXP:?INT, (net_helper:get_encode_binary_data(encode(DECORATION)))/binary, (net_helper:get_encode_binary_data(encode(PET_FOOD)))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, CROPS))/binary, LEVEL:?INT>>};
encode(#farm_exp_data{plot_index=PLOT_INDEX, add_exp=ADD_EXP}) ->
	{?msg_farm_exp_data, <<PLOT_INDEX:?INT, ADD_EXP:?INT>>};
encode(#farm_hint_data{plot_index=PLOT_INDEX, hint_id=HINT_ID, param1=PARAM1, param2=PARAM2}) ->
	{?msg_farm_hint_data, <<PLOT_INDEX:?INT, HINT_ID:?INT, PARAM1:?INT, PARAM2:?INT>>};
encode(#farm_pick_all_result{farm_data=FARM_DATA, exp_data=EXP_DATA, hint=HINT}) ->
	{?msg_farm_pick_all_result, <<(net_helper:get_encode_binary_data(encode(FARM_DATA)))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, EXP_DATA))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, HINT))/binary>>};
encode(#player_farm_log{account=ACCOUNT, message_log=MESSAGE_LOG, gain_log=GAIN_LOG, consumption_log=CONSUMPTION_LOG}) ->
	{?msg_player_farm_log, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, MESSAGE_LOG))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GAIN_LOG))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, CONSUMPTION_LOG))/binary>>};
encode(#player_farm_comment{account=ACCOUNT, comment=COMMENT}) ->
	{?msg_player_farm_comment, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, COMMENT))/binary>>};
encode(#farm_status{account=ACCOUNT, name=NAME, exp=EXP, status=STATUS}) ->
	{?msg_farm_status, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_string(NAME))/binary, EXP:?INT, STATUS:?INT>>};
encode(#req_enter_farm{owner=OWNER}) ->
	{?msg_req_enter_farm, <<(net_helper:encode_string(OWNER))/binary>>};
encode(#notify_farm_data{data=DATA}) ->
	{?msg_notify_farm_data, <<(net_helper:get_encode_binary_data(encode(DATA)))/binary>>};
encode(#req_leave_farm{}) ->
	{?msg_req_leave_farm, null};
encode(#notify_leave_farm{}) ->
	{?msg_notify_leave_farm, null};
encode(#req_assart_plot{}) ->
	{?msg_req_assart_plot, null};
encode(#req_sow{plot_index=PLOT_INDEX, item_inst_id=ITEM_INST_ID}) ->
	{?msg_req_sow, <<PLOT_INDEX:?INT, ITEM_INST_ID:?UINT64>>};
encode(#req_pick_crop{account=ACCOUNT, plot_index=PLOT_INDEX}) ->
	{?msg_req_pick_crop, <<(net_helper:encode_string(ACCOUNT))/binary, PLOT_INDEX:?INT>>};
encode(#req_pick_all_crops{account=ACCOUNT}) ->
	{?msg_req_pick_all_crops, <<(net_helper:encode_string(ACCOUNT))/binary>>};
encode(#req_get_friend_farm_state{}) ->
	{?msg_req_get_friend_farm_state, null};
encode(#notify_friend_farm_status{data=DATA}) ->
	{?msg_notify_friend_farm_status, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, DATA))/binary>>};
encode(#req_weed{account=ACCOUNT, plot_index=PLOT_INDEX}) ->
	{?msg_req_weed, <<(net_helper:encode_string(ACCOUNT))/binary, PLOT_INDEX:?INT>>};
encode(#req_worm{account=ACCOUNT, plot_index=PLOT_INDEX}) ->
	{?msg_req_worm, <<(net_helper:encode_string(ACCOUNT))/binary, PLOT_INDEX:?INT>>};
encode(#req_water{account=ACCOUNT, plot_index=PLOT_INDEX}) ->
	{?msg_req_water, <<(net_helper:encode_string(ACCOUNT))/binary, PLOT_INDEX:?INT>>};
encode(#notify_pet_bite{hurt=HURT}) ->
	{?msg_notify_pet_bite, <<HURT:?INT>>};
encode(#req_farm_log_list{type=TYPE, account=ACCOUNT}) ->
	{?msg_req_farm_log_list, <<TYPE:?INT, (net_helper:encode_string(ACCOUNT))/binary>>};
encode(#notify_farm_log_list{log=LOG}) ->
	{?msg_notify_farm_log_list, <<(net_helper:get_encode_binary_data(encode(LOG)))/binary>>};
encode(#req_clear_farm_log{}) ->
	{?msg_req_clear_farm_log, null};
encode(#req_buy_farm_shop_goods{type=TYPE, item_id=ITEM_ID, count=COUNT, pay_type=PAY_TYPE}) ->
	{?msg_req_buy_farm_shop_goods, <<TYPE:?INT, ITEM_ID:?INT, COUNT:?INT, PAY_TYPE:?INT>>};
encode(#req_sell_farm_shop_goods{instance_id=INSTANCE_ID, count=COUNT}) ->
	{?msg_req_sell_farm_shop_goods, <<INSTANCE_ID:?UINT64, COUNT:?INT>>};
encode(#req_sell_farm_shop_all_goods{type=TYPE}) ->
	{?msg_req_sell_farm_shop_all_goods, <<TYPE:?INT>>};
encode(#notify_farm_info{account=ACCOUNT, plot_index=PLOT_INDEX, exp=EXP, level=LEVEL, add_exp=ADD_EXP}) ->
	{?msg_notify_farm_info, <<(net_helper:encode_string(ACCOUNT))/binary, PLOT_INDEX:?INT, EXP:?INT, LEVEL:?INT, ADD_EXP:?INT>>};
encode(#req_farm_comment{account=ACCOUNT, comment=COMMENT}) ->
	{?msg_req_farm_comment, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_string(COMMENT))/binary>>};
encode(#notify_farm_comment{comment=COMMENT}) ->
	{?msg_notify_farm_comment, <<(net_helper:get_encode_binary_data(encode(COMMENT)))/binary>>};
encode(#req_reply_farm_comment{account=ACCOUNT, comment=COMMENT}) ->
	{?msg_req_reply_farm_comment, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_string(COMMENT))/binary>>};
encode(#notify_reply_farm_comment{comment=COMMENT}) ->
	{?msg_notify_reply_farm_comment, <<(net_helper:get_encode_binary_data(encode(COMMENT)))/binary>>};
encode(#req_farm_comment_list{account=ACCOUNT}) ->
	{?msg_req_farm_comment_list, <<(net_helper:encode_string(ACCOUNT))/binary>>};
encode(#notify_farm_comment_list{farm_comment=FARM_COMMENT}) ->
	{?msg_notify_farm_comment_list, <<(net_helper:get_encode_binary_data(encode(FARM_COMMENT)))/binary>>};
encode(#req_clear_farm_comment{}) ->
	{?msg_req_clear_farm_comment, null};
encode(#notify_clear_farm_comment{result=RESULT}) ->
	{?msg_notify_clear_farm_comment, <<RESULT:?INT>>};
encode(#req_remove_decoration{template_id=TEMPLATE_ID}) ->
	{?msg_req_remove_decoration, <<TEMPLATE_ID:?INT>>};
encode(#req_change_decoration{decoration_item=DECORATION_ITEM}) ->
	{?msg_req_change_decoration, <<(net_helper:get_encode_binary_data(encode(DECORATION_ITEM)))/binary>>};
encode(#req_put_grass{account=ACCOUNT, plot_index=PLOT_INDEX}) ->
	{?msg_req_put_grass, <<(net_helper:encode_string(ACCOUNT))/binary, PLOT_INDEX:?INT>>};
encode(#req_put_pest{account=ACCOUNT, plot_index=PLOT_INDEX}) ->
	{?msg_req_put_pest, <<(net_helper:encode_string(ACCOUNT))/binary, PLOT_INDEX:?INT>>};
encode(#farm_setting{account=ACCOUNT, water=WATER, weed=WEED, worm=WORM, put_grass=PUT_GRASS, put_pest=PUT_PEST}) ->
	{?msg_farm_setting, <<(net_helper:encode_string(ACCOUNT))/binary, (net_helper:encode_string(WATER))/binary, (net_helper:encode_string(WEED))/binary, (net_helper:encode_string(WORM))/binary, (net_helper:encode_string(PUT_GRASS))/binary, (net_helper:encode_string(PUT_PEST))/binary>>};
encode(#req_farm_setting{}) ->
	{?msg_req_farm_setting, null};
encode(#req_set_farm_setting{setting=SETTING}) ->
	{?msg_req_set_farm_setting, <<(net_helper:get_encode_binary_data(encode(SETTING)))/binary>>};
encode(#notify_set_farm_setting{result=RESULT}) ->
	{?msg_notify_set_farm_setting, <<RESULT:?INT>>};
encode(#req_reset_farm_setting{}) ->
	{?msg_req_reset_farm_setting, null};
encode(#notify_reset_farm_setting{setting=SETTING}) ->
	{?msg_notify_reset_farm_setting, <<(net_helper:get_encode_binary_data(encode(SETTING)))/binary>>};
encode(#notify_farm_setting{setting=SETTING}) ->
	{?msg_notify_farm_setting, <<(net_helper:get_encode_binary_data(encode(SETTING)))/binary>>};
encode(#req_lock_depot_item{item_inst_id=ITEM_INST_ID}) ->
	{?msg_req_lock_depot_item, <<ITEM_INST_ID:?UINT64>>};
encode(#req_unlock_depot_item{item_inst_id=ITEM_INST_ID}) ->
	{?msg_req_unlock_depot_item, <<ITEM_INST_ID:?UINT64>>};
encode(#req_hoeing{plot_index=PLOT_INDEX}) ->
	{?msg_req_hoeing, <<PLOT_INDEX:?INT>>};
encode(#req_use_farm_item{item_inst_id=ITEM_INST_ID, plot_index=PLOT_INDEX}) ->
	{?msg_req_use_farm_item, <<ITEM_INST_ID:?UINT64, PLOT_INDEX:?INT>>};
encode(#req_can_assart_plot{}) ->
	{?msg_req_can_assart_plot, null};
encode(#req_can_upgrade_land{}) ->
	{?msg_req_can_upgrade_land, null};
encode(#req_upgrade_land{}) ->
	{?msg_req_upgrade_land, null};
encode(#notify_farm_money{account=ACCOUNT, add_money=ADD_MONEY, money=MONEY}) ->
	{?msg_notify_farm_money, <<(net_helper:encode_string(ACCOUNT))/binary, ADD_MONEY:?INT, MONEY:?INT>>};
encode(#notify_farm_awards{type=TYPE, money=MONEY, itemlist=ITEMLIST, countlist=COUNTLIST}) ->
	{?msg_notify_farm_awards, <<TYPE:?INT, MONEY:?INT, (net_helper:encode_list(fun(E)-> <<E:?INT>>end, ITEMLIST))/binary, (net_helper:encode_list(fun(E)-> <<E:?INT>>end, COUNTLIST))/binary>>};
encode(#notify_farm_eq_coin{eq_coin=EQ_COIN}) ->
	{?msg_notify_farm_eq_coin, <<EQ_COIN:?INT>>};
encode(#req_task_list{}) ->
	{?msg_req_task_list, null};
encode(#notify_task_list{task_list=TASK_LIST}) ->
	{?msg_notify_task_list, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, TASK_LIST))/binary>>};
encode(#req_farm_task_list{}) ->
	{?msg_req_farm_task_list, null};
encode(#notify_farm_task_list{task_list=TASK_LIST}) ->
	{?msg_notify_farm_task_list, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, TASK_LIST))/binary>>};
encode(#req_track_task{id=ID}) ->
	{?msg_req_track_task, <<ID:?UINT64>>};
encode(#notify_track_task{id=ID}) ->
	{?msg_notify_track_task, <<ID:?UINT64>>};
encode(#req_get_track_list{}) ->
	{?msg_req_get_track_list, null};
encode(#notify_get_track_list{task_list=TASK_LIST}) ->
	{?msg_notify_get_track_list, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, TASK_LIST))/binary>>};
encode(#req_cancel_track_task{id=ID}) ->
	{?msg_req_cancel_track_task, <<ID:?UINT64>>};
encode(#notify_cancel_track_task{id=ID}) ->
	{?msg_notify_cancel_track_task, <<ID:?UINT64>>};
encode(#req_give_up_task{id=ID}) ->
	{?msg_req_give_up_task, <<ID:?UINT64>>};
encode(#notify_give_up_task{id=ID}) ->
	{?msg_notify_give_up_task, <<ID:?UINT64>>};
encode(#notify_task_complete{id=ID}) ->
	{?msg_notify_task_complete, <<ID:?UINT64>>};
encode(#notify_give_task{task=TASK}) ->
	{?msg_notify_give_task, <<(net_helper:get_encode_binary_data(encode(TASK)))/binary>>};
encode(#req_complete_task{task_id=TASK_ID}) ->
	{?msg_req_complete_task, <<TASK_ID:?INT>>};
encode(#notify_give_farm_task{task=TASK}) ->
	{?msg_notify_give_farm_task, <<(net_helper:get_encode_binary_data(encode(TASK)))/binary>>};
encode(#req_farm_complete_task{task_id=TASK_ID}) ->
	{?msg_req_farm_complete_task, <<TASK_ID:?INT>>};
encode(#notify_farm_task_complete{id=ID}) ->
	{?msg_notify_farm_task_complete, <<ID:?UINT64>>};
encode(#notify_mail_not_read{}) ->
	{?msg_notify_mail_not_read, null};
encode(#notify_add_mail{mail_data=MAIL_DATA}) ->
	{?msg_notify_add_mail, <<(net_helper:get_encode_binary_data(encode(MAIL_DATA)))/binary>>};
encode(#req_delete_mail{id=ID}) ->
	{?msg_req_delete_mail, <<ID:?UINT64>>};
encode(#notify_delete_mail{id=ID}) ->
	{?msg_notify_delete_mail, <<ID:?UINT64>>};
encode(#req_get_mail_item{id=ID, inst_id=INST_ID}) ->
	{?msg_req_get_mail_item, <<ID:?UINT64, INST_ID:?UINT64>>};
encode(#notify_delete_mail_item{id=ID, inst_id=INST_ID}) ->
	{?msg_notify_delete_mail_item, <<ID:?UINT64, INST_ID:?UINT64>>};
encode(#req_mail_list{}) ->
	{?msg_req_mail_list, null};
encode(#notify_mail_list{mails=MAILS}) ->
	{?msg_notify_mail_list, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, MAILS))/binary>>};
encode(#req_mail_content{id=ID}) ->
	{?msg_req_mail_content, <<ID:?UINT64>>};
encode(#notify_mail_content{id=ID, content=CONTENT}) ->
	{?msg_notify_mail_content, <<ID:?UINT64, (net_helper:encode_string(CONTENT))/binary>>};
encode(#req_know_new_mail_title{id_list=ID_LIST}) ->
	{?msg_req_know_new_mail_title, <<(net_helper:encode_list(fun(E)-> <<E:?UINT64>>end, ID_LIST))/binary>>};
encode(#req_reject_mail{id=ID}) ->
	{?msg_req_reject_mail, <<ID:?UINT64>>};
encode(#req_send_mail{recver_account=RECVER_ACCOUNT, recver_name=RECVER_NAME, title=TITLE, content=CONTENT, items=ITEMS}) ->
	{?msg_req_send_mail, <<(net_helper:encode_string(RECVER_ACCOUNT))/binary, (net_helper:encode_string(RECVER_NAME))/binary, (net_helper:encode_string(TITLE))/binary, (net_helper:encode_string(CONTENT))/binary, (net_helper:encode_list(fun(E)-> <<E:?UINT64>>end, ITEMS))/binary>>};
encode(#notify_send_mail_success{}) ->
	{?msg_notify_send_mail_success, null};
encode(#notify_reject_mail_success{id=ID}) ->
	{?msg_notify_reject_mail_success, <<ID:?UINT64>>};
encode(#req_open_ui{type=TYPE}) ->
	{?msg_req_open_ui, <<TYPE:?INT>>};
encode(#notify_open_ui{type=TYPE}) ->
	{?msg_notify_open_ui, <<TYPE:?INT>>};
encode(#req_sys_time{}) ->
	{?msg_req_sys_time, null};
encode(#notify_day_or_night{flag=FLAG}) ->
	{?msg_notify_day_or_night, <<FLAG:?INT>>};
encode(#notify_sys_time{sys_time=SYS_TIME}) ->
	{?msg_notify_sys_time, <<(net_helper:get_encode_binary_data(encode(SYS_TIME)))/binary>>};
encode(#notify_wallow_time{wallow_seconds=WALLOW_SECONDS}) ->
	{?msg_notify_wallow_time, <<WALLOW_SECONDS:?INT>>};
encode(#notify_player_health_status{account=ACCOUNT, status=STATUS}) ->
	{?msg_notify_player_health_status, <<(net_helper:encode_string(ACCOUNT))/binary, STATUS:?INT>>};
encode(#notify_disease_special_event{special_event_id=SPECIAL_EVENT_ID}) ->
	{?msg_notify_disease_special_event, <<SPECIAL_EVENT_ID:?INT>>};
encode(#req_start_buy_house{}) ->
	{?msg_req_start_buy_house, null};
encode(#notify_start_buy_house{}) ->
	{?msg_notify_start_buy_house, null};
encode(#req_end_buy_house{}) ->
	{?msg_req_end_buy_house, null};
encode(#notify_end_buy_house{}) ->
	{?msg_notify_end_buy_house, null};
encode(#req_buy_house{id=ID}) ->
	{?msg_req_buy_house, <<ID:?INT>>};
encode(#notify_buy_house{result=RESULT}) ->
	{?msg_notify_buy_house, <<RESULT:?INT>>};
encode(#req_buy_house_replace{type=TYPE, id=ID}) ->
	{?msg_req_buy_house_replace, <<TYPE:?INT, ID:?INT>>};
encode(#req_buy_house_add{id=ID}) ->
	{?msg_req_buy_house_add, <<ID:?INT>>};
encode(#req_deposit_money_in_depot{money=MONEY}) ->
	{?msg_req_deposit_money_in_depot, <<MONEY:?INT>>};
encode(#req_withdraw_money_in_depot{money=MONEY}) ->
	{?msg_req_withdraw_money_in_depot, <<MONEY:?INT>>};
encode(#notify_player_depot{money=MONEY, grid_vec=GRID_VEC}) ->
	{?msg_notify_player_depot, <<MONEY:?INT, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GRID_VEC))/binary>>};
encode(#req_start_domestic{}) ->
	{?msg_req_start_domestic, null};
encode(#notify_start_domestic{}) ->
	{?msg_notify_start_domestic, null};
encode(#req_domestic_service{id=ID}) ->
	{?msg_req_domestic_service, <<ID:?INT>>};
encode(#notify_domestic_result{id=ID, change_house_clean=CHANGE_HOUSE_CLEAN, cost_money=COST_MONEY, money_rate=MONEY_RATE}) ->
	{?msg_notify_domestic_result, <<ID:?INT, CHANGE_HOUSE_CLEAN:?INT, COST_MONEY:?INT, MONEY_RATE:?INT>>};
encode(#req_end_domestic{id=ID}) ->
	{?msg_req_end_domestic, <<ID:?INT>>};
encode(#notify_end_domestic{id=ID}) ->
	{?msg_notify_end_domestic, <<ID:?INT>>};
encode(#req_walk_for_pick_magic_box{curr_pos=CURR_POS, dest_pos=DEST_POS, box_id=BOX_ID, move_type=MOVE_TYPE}) ->
	{?msg_req_walk_for_pick_magic_box, <<(net_helper:get_encode_binary_data(encode(CURR_POS)))/binary, (net_helper:get_encode_binary_data(encode(DEST_POS)))/binary, BOX_ID:?UINT64, MOVE_TYPE:?INT>>};
encode(#req_pick_magic_box{box_id=BOX_ID}) ->
	{?msg_req_pick_magic_box, <<BOX_ID:?UINT64>>};
encode(#req_client_empty_box_positions{need_n=NEED_N}) ->
	{?msg_req_client_empty_box_positions, <<NEED_N:?INT>>};
encode(#notify_server_empty_box_positions{pos_list=POS_LIST}) ->
	{?msg_notify_server_empty_box_positions, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, POS_LIST))/binary>>};
encode(#notify_house_magic_box{box_list=BOX_LIST}) ->
	{?msg_notify_house_magic_box, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, BOX_LIST))/binary>>};
encode(#notify_del_house_magic_box{box_id=BOX_ID}) ->
	{?msg_notify_del_house_magic_box, <<BOX_ID:?UINT64>>};
encode(#notify_add_house_magic_box{box_list=BOX_LIST}) ->
	{?msg_notify_add_house_magic_box, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, BOX_LIST))/binary>>};
encode(#notify_pick_magic_box_gain{type=TYPE, item_id=ITEM_ID, money=MONEY}) ->
	{?msg_notify_pick_magic_box_gain, <<TYPE:?INT, ITEM_ID:?INT, MONEY:?INT>>};
encode(#notify_friend_garbage{friend_account=FRIEND_ACCOUNT, n=N}) ->
	{?msg_notify_friend_garbage, <<(net_helper:encode_string(FRIEND_ACCOUNT))/binary, N:?INT>>};
encode(#req_start_gift_service{type=TYPE}) ->
	{?msg_req_start_gift_service, <<TYPE:?INT>>};
encode(#notify_start_gift_service{type=TYPE}) ->
	{?msg_notify_start_gift_service, <<TYPE:?INT>>};
encode(#req_end_gift_service{}) ->
	{?msg_req_end_gift_service, null};
encode(#notify_end_gift_service{type=TYPE}) ->
	{?msg_notify_end_gift_service, <<TYPE:?INT>>};
encode(#req_pack_gift_box{pack1_type=PACK1_TYPE, index1=INDEX1, pack2_type=PACK2_TYPE, index2=INDEX2, gift=GIFT}) ->
	{?msg_req_pack_gift_box, <<PACK1_TYPE:?INT, INDEX1:?INT, PACK2_TYPE:?INT, INDEX2:?INT, (net_helper:get_encode_binary_data(encode(GIFT)))/binary>>};
encode(#notify_pack_gift_box{gift=GIFT}) ->
	{?msg_notify_pack_gift_box, <<(net_helper:get_encode_binary_data(encode(GIFT)))/binary>>};
encode(#req_unpack_gift_box{inst_id=INST_ID}) ->
	{?msg_req_unpack_gift_box, <<INST_ID:?UINT64>>};
encode(#notify_unpack_gift_box{gift=GIFT}) ->
	{?msg_notify_unpack_gift_box, <<(net_helper:get_encode_binary_data(encode(GIFT)))/binary>>};
encode(#req_put_gift_box{inst_id=INST_ID, pos=POS}) ->
	{?msg_req_put_gift_box, <<INST_ID:?UINT64, (net_helper:get_encode_binary_data(encode(POS)))/binary>>};
encode(#notify_put_gift_box{}) ->
	{?msg_notify_put_gift_box, null};
encode(#req_pickup_gift_box{inst_id=INST_ID}) ->
	{?msg_req_pickup_gift_box, <<INST_ID:?UINT64>>};
encode(#notify_pickup_gift_box{inst_id=INST_ID}) ->
	{?msg_notify_pickup_gift_box, <<INST_ID:?UINT64>>};
encode(#notify_gift_box{gift=GIFT}) ->
	{?msg_notify_gift_box, <<(net_helper:get_encode_binary_data(encode(GIFT)))/binary>>};
encode(#req_get_send_box_vec{page_index=PAGE_INDEX, page_count=PAGE_COUNT}) ->
	{?msg_req_get_send_box_vec, <<PAGE_INDEX:?INT, PAGE_COUNT:?INT>>};
encode(#notify_get_send_box_vec{send_box_vec=SEND_BOX_VEC, total=TOTAL}) ->
	{?msg_notify_get_send_box_vec, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, SEND_BOX_VEC))/binary, TOTAL:?INT>>};
encode(#req_get_receive_box_vec{page_index=PAGE_INDEX, page_count=PAGE_COUNT}) ->
	{?msg_req_get_receive_box_vec, <<PAGE_INDEX:?INT, PAGE_COUNT:?INT>>};
encode(#notify_get_receive_box_vec{receive_box_vec=RECEIVE_BOX_VEC, total=TOTAL}) ->
	{?msg_notify_get_receive_box_vec, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, RECEIVE_BOX_VEC))/binary, TOTAL:?INT>>};
encode(#req_get_viewed_box_vec{page_index=PAGE_INDEX, page_count=PAGE_COUNT}) ->
	{?msg_req_get_viewed_box_vec, <<PAGE_INDEX:?INT, PAGE_COUNT:?INT>>};
encode(#notify_get_viewed_box_vec{viewed_box_vec=VIEWED_BOX_VEC, total=TOTAL}) ->
	{?msg_notify_get_viewed_box_vec, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, VIEWED_BOX_VEC))/binary, TOTAL:?INT>>};
encode(#gift_express{type=TYPE, inst_id=INST_ID, sender=SENDER, sender_name=SENDER_NAME, receiver=RECEIVER, receiver_name=RECEIVER_NAME, grid_vec=GRID_VEC, card_id=CARD_ID, comment=COMMENT, send_date=SEND_DATE, date=DATE}) ->
	{?msg_gift_express, <<TYPE:?INT, INST_ID:?UINT64, (net_helper:encode_string(SENDER))/binary, (net_helper:encode_string(SENDER_NAME))/binary, (net_helper:encode_string(RECEIVER))/binary, (net_helper:encode_string(RECEIVER_NAME))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, GRID_VEC))/binary, CARD_ID:?INT, (net_helper:encode_string(COMMENT))/binary, (net_helper:get_encode_binary_data(encode(SEND_DATE)))/binary, (net_helper:get_encode_binary_data(encode(DATE)))/binary>>};
encode(#req_send_gift_express{pack1_type=PACK1_TYPE, index1=INDEX1, pack2_type=PACK2_TYPE, index2=INDEX2, gift=GIFT}) ->
	{?msg_req_send_gift_express, <<PACK1_TYPE:?INT, INDEX1:?INT, PACK2_TYPE:?INT, INDEX2:?INT, (net_helper:get_encode_binary_data(encode(GIFT)))/binary>>};
encode(#notify_send_gift_express{inst_id=INST_ID}) ->
	{?msg_notify_send_gift_express, <<INST_ID:?UINT64>>};
encode(#req_receive_gift_express{inst_id=INST_ID, page_index=PAGE_INDEX, page_count=PAGE_COUNT}) ->
	{?msg_req_receive_gift_express, <<INST_ID:?UINT64, PAGE_INDEX:?INT, PAGE_COUNT:?INT>>};
encode(#notify_receive_gift_express{inst_id=INST_ID}) ->
	{?msg_notify_receive_gift_express, <<INST_ID:?UINT64>>};
encode(#req_get_send_express_vec{page_index=PAGE_INDEX, page_count=PAGE_COUNT}) ->
	{?msg_req_get_send_express_vec, <<PAGE_INDEX:?INT, PAGE_COUNT:?INT>>};
encode(#notify_get_send_express_vec{send_express_vec=SEND_EXPRESS_VEC, total=TOTAL}) ->
	{?msg_notify_get_send_express_vec, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, SEND_EXPRESS_VEC))/binary, TOTAL:?INT>>};
encode(#req_get_receive_express_vec{page_index=PAGE_INDEX, page_count=PAGE_COUNT}) ->
	{?msg_req_get_receive_express_vec, <<PAGE_INDEX:?INT, PAGE_COUNT:?INT>>};
encode(#notify_express_vec{send_express_vec=SEND_EXPRESS_VEC, receive_express_vec=RECEIVE_EXPRESS_VEC}) ->
	{?msg_notify_express_vec, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, SEND_EXPRESS_VEC))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, RECEIVE_EXPRESS_VEC))/binary>>};
encode(#notify_send_express{send_express=SEND_EXPRESS}) ->
	{?msg_notify_send_express, <<(net_helper:get_encode_binary_data(encode(SEND_EXPRESS)))/binary>>};
encode(#req_send_express_by_subscribe{inst_id=INST_ID}) ->
	{?msg_req_send_express_by_subscribe, <<INST_ID:?UINT64>>};
encode(#notify_get_receive_express_vec{receive_express_vec=RECEIVE_EXPRESS_VEC, total=TOTAL}) ->
	{?msg_notify_get_receive_express_vec, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, RECEIVE_EXPRESS_VEC))/binary, TOTAL:?INT>>};
encode(#req_get_viewed_express_vec{page_index=PAGE_INDEX, page_count=PAGE_COUNT}) ->
	{?msg_req_get_viewed_express_vec, <<PAGE_INDEX:?INT, PAGE_COUNT:?INT>>};
encode(#notify_get_viewed_express_vec{viewed_express_vec=VIEWED_EXPRESS_VEC, total=TOTAL}) ->
	{?msg_notify_get_viewed_express_vec, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, VIEWED_EXPRESS_VEC))/binary, TOTAL:?INT>>};
encode(#req_clean_gift_log{type=TYPE}) ->
	{?msg_req_clean_gift_log, <<TYPE:?INT>>};
encode(#notify_show_picture{id=ID}) ->
	{?msg_notify_show_picture, <<ID:?INT>>};
encode(#req_start_work{}) ->
	{?msg_req_start_work, null};
encode(#notify_start_work{}) ->
	{?msg_notify_start_work, null};
encode(#req_work{id=ID}) ->
	{?msg_req_work, <<ID:?INT>>};
encode(#notify_work_result{id=ID, change_clean=CHANGE_CLEAN, change_health=CHANGE_HEALTH, change_charm=CHANGE_CHARM, change_active=CHANGE_ACTIVE, work_money=WORK_MONEY, event_id=EVENT_ID, reward_money=REWARD_MONEY, reward_item=REWARD_ITEM}) ->
	{?msg_notify_work_result, <<ID:?INT, CHANGE_CLEAN:?INT, CHANGE_HEALTH:?INT, CHANGE_CHARM:?INT, CHANGE_ACTIVE:?INT, WORK_MONEY:?INT, EVENT_ID:?INT, REWARD_MONEY:?INT, REWARD_ITEM:?INT>>};
encode(#req_end_work{id=ID}) ->
	{?msg_req_end_work, <<ID:?INT>>};
encode(#notify_end_work{id=ID}) ->
	{?msg_notify_end_work, <<ID:?INT>>};
encode(#req_get_weibo_access_secret{}) ->
	{?msg_req_get_weibo_access_secret, null};
encode(#notify_weibo_access_secret{tx_access_key=TX_ACCESS_KEY, tx_access_secret=TX_ACCESS_SECRET, sina_access_key=SINA_ACCESS_KEY, sina_access_secret=SINA_ACCESS_SECRET}) ->
	{?msg_notify_weibo_access_secret, <<(net_helper:encode_string(TX_ACCESS_KEY))/binary, (net_helper:encode_string(TX_ACCESS_SECRET))/binary, (net_helper:encode_string(SINA_ACCESS_KEY))/binary, (net_helper:encode_string(SINA_ACCESS_SECRET))/binary>>};
encode(#req_save_access_secret{tx_access_key=TX_ACCESS_KEY, tx_access_secret=TX_ACCESS_SECRET, sina_access_key=SINA_ACCESS_KEY, sina_access_secret=SINA_ACCESS_SECRET}) ->
	{?msg_req_save_access_secret, <<(net_helper:encode_string(TX_ACCESS_KEY))/binary, (net_helper:encode_string(TX_ACCESS_SECRET))/binary, (net_helper:encode_string(SINA_ACCESS_KEY))/binary, (net_helper:encode_string(SINA_ACCESS_SECRET))/binary>>};
encode(#req_preview_house{account=ACCOUNT, inst_id=INST_ID}) ->
	{?msg_req_preview_house, <<(net_helper:encode_string(ACCOUNT))/binary, INST_ID:?UINT64>>};
encode(#notify_preview_house{house=HOUSE}) ->
	{?msg_notify_preview_house, <<(net_helper:get_encode_binary_data(encode(HOUSE)))/binary>>};
encode(#req_preview_shop_house{house_id=HOUSE_ID}) ->
	{?msg_req_preview_shop_house, <<HOUSE_ID:?INT>>};
encode(#req_end_preview_house{}) ->
	{?msg_req_end_preview_house, null};
encode(#notify_end_preview_house{}) ->
	{?msg_notify_end_preview_house, null};
encode(#bottle_info{id=ID, account=ACCOUNT, type=TYPE, msg=MSG, time=TIME}) ->
	{?msg_bottle_info, <<ID:?UINT64, (net_helper:encode_string(ACCOUNT))/binary, TYPE:?INT, (net_helper:encode_string(MSG))/binary, (net_helper:get_encode_binary_data(encode(TIME)))/binary>>};
encode(#req_del_bottle_log{id=ID}) ->
	{?msg_req_del_bottle_log, <<ID:?UINT64>>};
encode(#req_drop_bottle{msg=MSG, type=TYPE, sex=SEX, account_visible=ACCOUNT_VISIBLE}) ->
	{?msg_req_drop_bottle, <<(net_helper:encode_string(MSG))/binary, TYPE:?INT, SEX:?INT, ACCOUNT_VISIBLE:?INT>>};
encode(#notify_drop_bottle{bottle=BOTTLE}) ->
	{?msg_notify_drop_bottle, <<(net_helper:get_encode_binary_data(encode(BOTTLE)))/binary>>};
encode(#req_pick_bottle{}) ->
	{?msg_req_pick_bottle, null};
encode(#notify_pick_bottle{bottle=BOTTLE}) ->
	{?msg_notify_pick_bottle, <<(net_helper:get_encode_binary_data(encode(BOTTLE)))/binary>>};
encode(#req_all_bottle_info{}) ->
	{?msg_req_all_bottle_info, null};
encode(#notify_bottles_info{pick_bottles=PICK_BOTTLES, drop_bottles=DROP_BOTTLES}) ->
	{?msg_notify_bottles_info, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, PICK_BOTTLES))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, DROP_BOTTLES))/binary>>};
encode(#notify_today_bottle_info{year=YEAR, month=MONTH, day=DAY, pick_count=PICK_COUNT, drop_count=DROP_COUNT}) ->
	{?msg_notify_today_bottle_info, <<YEAR:?INT, MONTH:?INT, DAY:?INT, PICK_COUNT:?INT, DROP_COUNT:?INT>>};
encode(#notify_open_float_bottle_pool{}) ->
	{?msg_notify_open_float_bottle_pool, null};
encode(#notify_house_decoration{account=ACCOUNT, decoration=DECORATION}) ->
	{?msg_notify_house_decoration, <<(net_helper:encode_string(ACCOUNT))/binary, DECORATION:?INT>>};
encode(#notify_new_day_reset{}) ->
	{?msg_notify_new_day_reset, null};
encode(#notify_total_flower_count{account=ACCOUNT, flower_count=FLOWER_COUNT}) ->
	{?msg_notify_total_flower_count, <<(net_helper:encode_string(ACCOUNT))/binary, FLOWER_COUNT:?INT>>};
encode(#req_send_flower{}) ->
	{?msg_req_send_flower, null};
encode(#notify_send_flower_success{player=PLAYER, body=BODY}) ->
	{?msg_notify_send_flower_success, <<(net_helper:get_encode_binary_data(encode(PLAYER)))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, BODY))/binary>>};
encode(#req_flower_logs{}) ->
	{?msg_req_flower_logs, null};
encode(#notify_flower_logs{logs=LOGS}) ->
	{?msg_notify_flower_logs, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, LOGS))/binary>>};
encode(#notify_house_flower_change{house_owner=HOUSE_OWNER, flower_count=FLOWER_COUNT}) ->
	{?msg_notify_house_flower_change, <<(net_helper:encode_string(HOUSE_OWNER))/binary, FLOWER_COUNT:?INT>>};
encode(#req_send_egg{}) ->
	{?msg_req_send_egg, null};
encode(#notify_send_egg_success{player=PLAYER, body=BODY}) ->
	{?msg_notify_send_egg_success, <<(net_helper:get_encode_binary_data(encode(PLAYER)))/binary, (net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, BODY))/binary>>};
encode(#req_egg_logs{}) ->
	{?msg_req_egg_logs, null};
encode(#notify_egg_logs{logs=LOGS}) ->
	{?msg_notify_egg_logs, <<(net_helper:encode_list(fun(E)-> net_helper:get_encode_binary_data(encode(E)) end, LOGS))/binary>>};
encode(#notify_house_egg_change{house_owner=HOUSE_OWNER, egg_count=EGG_COUNT}) ->
	{?msg_notify_house_egg_change, <<(net_helper:encode_string(HOUSE_OWNER))/binary, EGG_COUNT:?INT>>};
encode(#req_is_active_game{type=TYPE}) ->
	{?msg_req_is_active_game, <<TYPE:?INT>>};
encode(#notify_active_game_result{result=RESULT}) ->
	{?msg_notify_active_game_result, <<RESULT:?INT>>}.


decode(?msg_req_ver_validate, Binary) ->
	net_helper:decode([int], Binary, [req_ver_validate]);
decode(?msg_notify_ver_validate, Binary) ->
	net_helper:decode([int], Binary, [notify_ver_validate]);
decode(?msg_stime, Binary) ->
	net_helper:decode([int, int, int, int, int, int], Binary, [stime]);
decode(?msg_point, Binary) ->
	net_helper:decode([float, float, float], Binary, [point]);
decode(?msg_grid_pos, Binary) ->
	net_helper:decode([int, int, int], Binary, [grid_pos]);
decode(?msg_item, Binary) ->
	net_helper:decode([uint64, int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, int], Binary, [item]);
decode(?msg_player_basic_data, Binary) ->
	net_helper:decode([string, string, int, int, int, int, int, int, int, float, int, string, string, int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [player_basic_data]);
decode(?msg_player_property, Binary) ->
	net_helper:decode([string, int, int, int, int], Binary, [player_property]);
decode(?msg_pack_grid, Binary) ->
	net_helper:decode([int, int, {user_define, fun(Bin)->decode(?msg_item, Bin)end}], Binary, [pack_grid]);
decode(?msg_player_body, Binary) ->
	net_helper:decode([string, {array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}], Binary, [player_body]);
decode(?msg_player_pack, Binary) ->
	net_helper:decode([string, {array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}], Binary, [player_pack]);
decode(?msg_player_info, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_basic_data, Bin)end}, string], Binary, [player_info]);
decode(?msg_friend_info, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_basic_data, Bin)end}, int], Binary, [friend_info]);
decode(?msg_mail_info, Binary) ->
	net_helper:decode([uint64, int, int, int, string, string, string, {array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}], Binary, [mail_info]);
decode(?msg_npc_info, Binary) ->
	net_helper:decode([int, int, int, int, int, int, int, string], Binary, [npc_info]);
decode(?msg_npc_map_mapping_info, Binary) ->
	net_helper:decode([int, int, string, {user_define, fun(Bin)->decode(?msg_point, Bin)end}, int, int, string, int], Binary, [npc_map_mapping_info]);
decode(?msg_npc_command, Binary) ->
	net_helper:decode([int, uint64, int, int], Binary, [npc_command]);
decode(?msg_furniture_position, Binary) ->
	net_helper:decode([int, int, string, int, int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [furniture_position]);
decode(?msg_player_task, Binary) ->
	net_helper:decode([uint64, string, int, int, int, int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [player_task]);
decode(?msg_eq_user_info, Binary) ->
	net_helper:decode([string, string, string, string, string, string, string, string, string, string, string, string, string, string, string, string, string], Binary, [eq_user_info]);
decode(?msg_offline_invite_record, Binary) ->
	net_helper:decode([int, string, string, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [offline_invite_record]);
decode(?msg_friend_setting_record, Binary) ->
	net_helper:decode([int], Binary, [friend_setting_record]);
decode(?msg_magic_box, Binary) ->
	net_helper:decode([int, uint64, {user_define, fun(Bin)->decode(?msg_grid_pos, Bin)end}, int], Binary, [magic_box]);
decode(?msg_gift_box, Binary) ->
	net_helper:decode([uint64, string, string, string, string, string, {array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}, int, int, string, {user_define, fun(Bin)->decode(?msg_grid_pos, Bin)end}, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, int], Binary, [gift_box]);
decode(?msg_flower_log, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_stime, Bin)end}, string], Binary, [flower_log]);
decode(?msg_egg_log, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_stime, Bin)end}, string], Binary, [egg_log]);
decode(?msg_req_login, Binary) ->
	net_helper:decode([string, string], Binary, [req_login]);
decode(?msg_notify_login_result, Binary) ->
	net_helper:decode([int], Binary, [notify_login_result]);
decode(?msg_notify_repeat_login, Binary) ->
	net_helper:decode([string], Binary, [notify_repeat_login]);
decode(?msg_req_get_roles, Binary) ->
	net_helper:decode([], Binary, [req_get_roles]);
decode(?msg_notify_get_roles_result, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_player_info, Bin)end}, string], Binary, [notify_get_roles_result]);
decode(?msg_req_create_role, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_basic_data, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_item, Bin)end}], Binary, [req_create_role]);
decode(?msg_notify_create_role_result, Binary) ->
	net_helper:decode([int], Binary, [notify_create_role_result]);
decode(?msg_req_enter_scene, Binary) ->
	net_helper:decode([], Binary, [req_enter_scene]);
decode(?msg_notify_enter_scene_result, Binary) ->
	net_helper:decode([int], Binary, [notify_enter_scene_result]);
decode(?msg_notify_body_data, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}], Binary, [notify_body_data]);
decode(?msg_req_enter_player_house, Binary) ->
	net_helper:decode([string], Binary, [req_enter_player_house]);
decode(?msg_req_enter_common_scene, Binary) ->
	net_helper:decode([int], Binary, [req_enter_common_scene]);
decode(?msg_notify_enter_common_scene, Binary) ->
	net_helper:decode([int, {user_define, fun(Bin)->decode(?msg_point, Bin)end}], Binary, [notify_enter_common_scene]);
decode(?msg_notify_enter_player_house, Binary) ->
	net_helper:decode([string, {user_define, fun(Bin)->decode(?msg_point, Bin)end}, int], Binary, [notify_enter_player_house]);
decode(?msg_req_teleport, Binary) ->
	net_helper:decode([int], Binary, [req_teleport]);
decode(?msg_req_open_change_scene_guide, Binary) ->
	net_helper:decode([], Binary, [req_open_change_scene_guide]);
decode(?msg_notify_open_change_scene_guide, Binary) ->
	net_helper:decode([], Binary, [notify_open_change_scene_guide]);
decode(?msg_req_close_change_scene_guide, Binary) ->
	net_helper:decode([], Binary, [req_close_change_scene_guide]);
decode(?msg_notify_close_change_scene_guide, Binary) ->
	net_helper:decode([], Binary, [notify_close_change_scene_guide]);
decode(?msg_notify_finish_enter_scene, Binary) ->
	net_helper:decode([], Binary, [notify_finish_enter_scene]);
decode(?msg_req_kick_guest, Binary) ->
	net_helper:decode([string], Binary, [req_kick_guest]);
decode(?msg_notify_other_player_data, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_info, Bin)end}, {user_define, fun(Bin)->decode(?msg_player_property, Bin)end}, {user_define, fun(Bin)->decode(?msg_point, Bin)end}, {user_define, fun(Bin)->decode(?msg_point, Bin)end}, uint64, int, int], Binary, [notify_other_player_data]);
decode(?msg_notify_other_player_info, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_info, Bin)end}, {user_define, fun(Bin)->decode(?msg_player_property, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}], Binary, [notify_other_player_info]);
decode(?msg_req_other_player_info, Binary) ->
	net_helper:decode([string], Binary, [req_other_player_info]);
decode(?msg_notify_player_enter_scene, Binary) ->
	net_helper:decode([string], Binary, [notify_player_enter_scene]);
decode(?msg_notify_player_leave_scene, Binary) ->
	net_helper:decode([string], Binary, [notify_player_leave_scene]);
decode(?msg_req_logout, Binary) ->
	net_helper:decode([], Binary, [req_logout]);
decode(?msg_notify_player_data, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_basic_data, Bin)end}], Binary, [notify_player_data]);
decode(?msg_req_walk, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_point, Bin)end}], Binary, [req_walk]);
decode(?msg_req_start_walk, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_point, Bin)end}, {user_define, fun(Bin)->decode(?msg_point, Bin)end}, int], Binary, [req_start_walk]);
decode(?msg_notify_start_walk, Binary) ->
	net_helper:decode([string, {user_define, fun(Bin)->decode(?msg_point, Bin)end}, {user_define, fun(Bin)->decode(?msg_point, Bin)end}, int], Binary, [notify_start_walk]);
decode(?msg_req_stop_walk, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_point, Bin)end}], Binary, [req_stop_walk]);
decode(?msg_notify_stop_walk, Binary) ->
	net_helper:decode([string, {user_define, fun(Bin)->decode(?msg_point, Bin)end}], Binary, [notify_stop_walk]);
decode(?msg_req_sync_position, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_point, Bin)end}], Binary, [req_sync_position]);
decode(?msg_req_walk_for_use_furniture, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_point, Bin)end}, {user_define, fun(Bin)->decode(?msg_point, Bin)end}, uint64, int, int], Binary, [req_walk_for_use_furniture]);
decode(?msg_req_add_friend, Binary) ->
	net_helper:decode([string], Binary, [req_add_friend]);
decode(?msg_req_remove_friend, Binary) ->
	net_helper:decode([string], Binary, [req_remove_friend]);
decode(?msg_notify_add_friend, Binary) ->
	net_helper:decode([string, {user_define, fun(Bin)->decode(?msg_friend_info, Bin)end}], Binary, [notify_add_friend]);
decode(?msg_notify_remove_friend, Binary) ->
	net_helper:decode([string], Binary, [notify_remove_friend]);
decode(?msg_req_friend_list, Binary) ->
	net_helper:decode([], Binary, [req_friend_list]);
decode(?msg_notify_friend_list, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_friend_info, Bin)end}], Binary, [notify_friend_list]);
decode(?msg_notify_friend_online, Binary) ->
	net_helper:decode([string], Binary, [notify_friend_online]);
decode(?msg_notify_friend_offline, Binary) ->
	net_helper:decode([string], Binary, [notify_friend_offline]);
decode(?msg_req_eq_users_info, Binary) ->
	net_helper:decode([{array, string}], Binary, [req_eq_users_info]);
decode(?msg_notify_eq_user_info, Binary) ->
	net_helper:decode([string, {user_define, fun(Bin)->decode(?msg_eq_user_info, Bin)end}], Binary, [notify_eq_user_info]);
decode(?msg_notify_eq_user_presence, Binary) ->
	net_helper:decode([string, string], Binary, [notify_eq_user_presence]);
decode(?msg_notify_offline_invites, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_offline_invite_record, Bin)end}], Binary, [notify_offline_invites]);
decode(?msg_req_friend_setting, Binary) ->
	net_helper:decode([], Binary, [req_friend_setting]);
decode(?msg_notify_friend_setting, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_friend_setting_record, Bin)end}], Binary, [notify_friend_setting]);
decode(?msg_req_friend_setting_accept_all, Binary) ->
	net_helper:decode([int], Binary, [req_friend_setting_accept_all]);
decode(?msg_req_friend_setting_black_someone, Binary) ->
	net_helper:decode([string], Binary, [req_friend_setting_black_someone]);
decode(?msg_req_search_player_by_nick, Binary) ->
	net_helper:decode([string], Binary, [req_search_player_by_nick]);
decode(?msg_req_search_player_by_mail, Binary) ->
	net_helper:decode([string], Binary, [req_search_player_by_mail]);
decode(?msg_notify_search_player, Binary) ->
	net_helper:decode([{array, string}], Binary, [notify_search_player]);
decode(?msg_req_chat_around, Binary) ->
	net_helper:decode([string], Binary, [req_chat_around]);
decode(?msg_notify_chat_around, Binary) ->
	net_helper:decode([string, string, string], Binary, [notify_chat_around]);
decode(?msg_req_chat_tell, Binary) ->
	net_helper:decode([string, string], Binary, [req_chat_tell]);
decode(?msg_notify_chat_tell, Binary) ->
	net_helper:decode([string, string, string], Binary, [notify_chat_tell]);
decode(?msg_house_furniture, Binary) ->
	net_helper:decode([uint64, int, int, int, int, int, int, int, int, {array, user_define, fun(Bin)->decode(?msg_furniture_position, Bin)end}, {array, int}, int, int, int, int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [house_furniture]);
decode(?msg_room_tex, Binary) ->
	net_helper:decode([int, int, string, int], Binary, [room_tex]);
decode(?msg_house_component_tex, Binary) ->
	net_helper:decode([string, string], Binary, [house_component_tex]);
decode(?msg_house_component_mesh, Binary) ->
	net_helper:decode([string, string], Binary, [house_component_mesh]);
decode(?msg_house_visit_log, Binary) ->
	net_helper:decode([int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, string, string, int], Binary, [house_visit_log]);
decode(?msg_house_data, Binary) ->
	net_helper:decode([string, uint64, int, int, {array, user_define, fun(Bin)->decode(?msg_house_furniture, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_room_tex, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_house_component_tex, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_house_component_mesh, Bin)end}, string, int, {array, user_define, fun(Bin)->decode(?msg_house_visit_log, Bin)end}, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, int, {array, user_define, fun(Bin)->decode(?msg_gift_box, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_magic_box, Bin)end}, int], Binary, [house_data]);
decode(?msg_notify_house_clean, Binary) ->
	net_helper:decode([int], Binary, [notify_house_clean]);
decode(?msg_notify_house_data, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_house_data, Bin)end}], Binary, [notify_house_data]);
decode(?msg_notify_house_gift_box, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_gift_box, Bin)end}], Binary, [notify_house_gift_box]);
decode(?msg_req_placed_furniture, Binary) ->
	net_helper:decode([int, int, int, int, int, int], Binary, [req_placed_furniture]);
decode(?msg_req_start_edit_house, Binary) ->
	net_helper:decode([], Binary, [req_start_edit_house]);
decode(?msg_notify_start_edit_house, Binary) ->
	net_helper:decode([], Binary, [notify_start_edit_house]);
decode(?msg_req_end_edit_house, Binary) ->
	net_helper:decode([], Binary, [req_end_edit_house]);
decode(?msg_notify_end_edit_house, Binary) ->
	net_helper:decode([], Binary, [notify_end_edit_house]);
decode(?msg_req_recovery_furniture, Binary) ->
	net_helper:decode([uint64], Binary, [req_recovery_furniture]);
decode(?msg_req_replace_component_mesh, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_house_component_mesh, Bin)end}], Binary, [req_replace_component_mesh]);
decode(?msg_req_replace_component_tex, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_house_component_tex, Bin)end}], Binary, [req_replace_component_tex]);
decode(?msg_req_replace_room_tex, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_room_tex, Bin)end}, int], Binary, [req_replace_room_tex]);
decode(?msg_notify_replace_room_tex, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_room_tex, Bin)end}], Binary, [notify_replace_room_tex]);
decode(?msg_req_move_furniture, Binary) ->
	net_helper:decode([uint64, int, int, int, int, int], Binary, [req_move_furniture]);
decode(?msg_notify_add_furniture, Binary) ->
	net_helper:decode([uint64, int, int, int, int, int, int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [notify_add_furniture]);
decode(?msg_notify_delete_furniture, Binary) ->
	net_helper:decode([uint64], Binary, [notify_delete_furniture]);
decode(?msg_req_house_basic_data, Binary) ->
	net_helper:decode([], Binary, [req_house_basic_data]);
decode(?msg_notify_house_basic_data, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_house_data, Bin)end}], Binary, [notify_house_basic_data]);
decode(?msg_req_set_house_welcome_words, Binary) ->
	net_helper:decode([string], Binary, [req_set_house_welcome_words]);
decode(?msg_notify_set_house_welcome_words, Binary) ->
	net_helper:decode([int], Binary, [notify_set_house_welcome_words]);
decode(?msg_req_set_house_permission, Binary) ->
	net_helper:decode([int, int], Binary, [req_set_house_permission]);
decode(?msg_notify_set_house_permission, Binary) ->
	net_helper:decode([int], Binary, [notify_set_house_permission]);
decode(?msg_req_clear_house_log, Binary) ->
	net_helper:decode([], Binary, [req_clear_house_log]);
decode(?msg_notify_clear_house_log, Binary) ->
	net_helper:decode([int], Binary, [notify_clear_house_log]);
decode(?msg_req_change_house, Binary) ->
	net_helper:decode([int, uint64], Binary, [req_change_house]);
decode(?msg_notify_change_house, Binary) ->
	net_helper:decode([uint64], Binary, [notify_change_house]);
decode(?msg_req_house_list, Binary) ->
	net_helper:decode([string], Binary, [req_house_list]);
decode(?msg_house_plot_info, Binary) ->
	net_helper:decode([uint64, int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, uint64], Binary, [house_plot_info]);
decode(?msg_notify_house_list, Binary) ->
	net_helper:decode([string, {array, user_define, fun(Bin)->decode(?msg_house_plot_info, Bin)end}], Binary, [notify_house_list]);
decode(?msg_notify_player_property_changed, Binary) ->
	net_helper:decode([int, int, int, int], Binary, [notify_player_property_changed]);
decode(?msg_control_furni_info, Binary) ->
	net_helper:decode([uint64, int], Binary, [control_furni_info]);
decode(?msg_mutli_furni_info, Binary) ->
	net_helper:decode([string, int, uint64, int, int], Binary, [mutli_furni_info]);
decode(?msg_req_use_mutli_furni, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_control_furni_info, Bin)end}], Binary, [req_use_mutli_furni]);
decode(?msg_notify_use_mutli_furni, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_mutli_furni_info, Bin)end}], Binary, [notify_use_mutli_furni]);
decode(?msg_notify_use_furniture_result, Binary) ->
	net_helper:decode([string, int, uint64, int, int], Binary, [notify_use_furniture_result]);
decode(?msg_req_stop_using_furniture, Binary) ->
	net_helper:decode([string], Binary, [req_stop_using_furniture]);
decode(?msg_notify_stop_using_furniture_result, Binary) ->
	net_helper:decode([string, int, uint64, int, int, int, {user_define, fun(Bin)->decode(?msg_point, Bin)end}], Binary, [notify_stop_using_furniture_result]);
decode(?msg_notify_package, Binary) ->
	net_helper:decode([string, int, {array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}], Binary, [notify_package]);
decode(?msg_req_swap_item, Binary) ->
	net_helper:decode([int, int, int], Binary, [req_swap_item]);
decode(?msg_req_use_item, Binary) ->
	net_helper:decode([int], Binary, [req_use_item]);
decode(?msg_req_move_item, Binary) ->
	net_helper:decode([int, int, int, int], Binary, [req_move_item]);
decode(?msg_req_delete_item, Binary) ->
	net_helper:decode([int], Binary, [req_delete_item]);
decode(?msg_req_split_item, Binary) ->
	net_helper:decode([int, int, int, int], Binary, [req_split_item]);
decode(?msg_req_resize_player_pack, Binary) ->
	net_helper:decode([], Binary, [req_resize_player_pack]);
decode(?msg_req_lock_bag_item, Binary) ->
	net_helper:decode([uint64], Binary, [req_lock_bag_item]);
decode(?msg_req_unlock_bag_item, Binary) ->
	net_helper:decode([uint64], Binary, [req_unlock_bag_item]);
decode(?msg_req_move_money, Binary) ->
	net_helper:decode([int, int, int], Binary, [req_move_money]);
decode(?msg_req_switch_item, Binary) ->
	net_helper:decode([int, uint64, int], Binary, [req_switch_item]);
decode(?msg_notify_sys_msg, Binary) ->
	net_helper:decode([int, {array, string}], Binary, [notify_sys_msg]);
decode(?msg_notify_sys_broadcast, Binary) ->
	net_helper:decode([uint64, int, string, int, int, int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [notify_sys_broadcast]);
decode(?msg_req_fixed_broadcast, Binary) ->
	net_helper:decode([int], Binary, [req_fixed_broadcast]);
decode(?msg_notify_del_broadcast, Binary) ->
	net_helper:decode([int, uint64], Binary, [notify_del_broadcast]);
decode(?msg_req_gm_command, Binary) ->
	net_helper:decode([int, {array, string}], Binary, [req_gm_command]);
decode(?msg_notify_npc_list, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_npc_map_mapping_info, Bin)end}], Binary, [notify_npc_list]);
decode(?msg_notify_npc_close_dialog, Binary) ->
	net_helper:decode([], Binary, [notify_npc_close_dialog]);
decode(?msg_notify_npc_show_dialog_by_option, Binary) ->
	net_helper:decode([int, {array, user_define, fun(Bin)->decode(?msg_npc_command, Bin)end}], Binary, [notify_npc_show_dialog_by_option]);
decode(?msg_notify_npc_show_dialog_by_message, Binary) ->
	net_helper:decode([int, uint64, int, int], Binary, [notify_npc_show_dialog_by_message]);
decode(?msg_notify_npc_show_dialog_by_item, Binary) ->
	net_helper:decode([int, {array, user_define, fun(Bin)->decode(?msg_npc_command, Bin)end}], Binary, [notify_npc_show_dialog_by_item]);
decode(?msg_req_npc_command, Binary) ->
	net_helper:decode([string, uint64, int, int], Binary, [req_npc_command]);
decode(?msg_req_player_basic_data, Binary) ->
	net_helper:decode([], Binary, [req_player_basic_data]);
decode(?msg_notify_player_basic_data, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_basic_data, Bin)end}, {user_define, fun(Bin)->decode(?msg_player_property, Bin)end}], Binary, [notify_player_basic_data]);
decode(?msg_req_start_body_action, Binary) ->
	net_helper:decode([string, string], Binary, [req_start_body_action]);
decode(?msg_notify_start_body_action, Binary) ->
	net_helper:decode([string, string, string], Binary, [notify_start_body_action]);
decode(?msg_req_play_animation, Binary) ->
	net_helper:decode([string, int, string, string], Binary, [req_play_animation]);
decode(?msg_notify_play_animation, Binary) ->
	net_helper:decode([string, string, int, string, string], Binary, [notify_play_animation]);
decode(?msg_req_end_body_action, Binary) ->
	net_helper:decode([], Binary, [req_end_body_action]);
decode(?msg_notify_end_body_action, Binary) ->
	net_helper:decode([string, string], Binary, [notify_end_body_action]);
decode(?msg_req_sync_body_state, Binary) ->
	net_helper:decode([int], Binary, [req_sync_body_state]);
decode(?msg_notify_other_body_state, Binary) ->
	net_helper:decode([string, int], Binary, [notify_other_body_state]);
decode(?msg_req_start_change_looks, Binary) ->
	net_helper:decode([], Binary, [req_start_change_looks]);
decode(?msg_notify_start_change_looks, Binary) ->
	net_helper:decode([int], Binary, [notify_start_change_looks]);
decode(?msg_req_cancel_change_looks, Binary) ->
	net_helper:decode([], Binary, [req_cancel_change_looks]);
decode(?msg_req_end_change_looks, Binary) ->
	net_helper:decode([int, int, int, int, int], Binary, [req_end_change_looks]);
decode(?msg_notify_change_looks, Binary) ->
	net_helper:decode([string, int, int, int, int, int], Binary, [notify_change_looks]);
decode(?msg_notify_end_change_looks, Binary) ->
	net_helper:decode([], Binary, [notify_end_change_looks]);
decode(?msg_notify_around_change_dress, Binary) ->
	net_helper:decode([string, int, int], Binary, [notify_around_change_dress]);
decode(?msg_req_invite_someone, Binary) ->
	net_helper:decode([string, int], Binary, [req_invite_someone]);
decode(?msg_notify_invitation, Binary) ->
	net_helper:decode([string, string, int], Binary, [notify_invitation]);
decode(?msg_req_agree_invitation, Binary) ->
	net_helper:decode([string, int], Binary, [req_agree_invitation]);
decode(?msg_req_disagree_invitation, Binary) ->
	net_helper:decode([string, int], Binary, [req_disagree_invitation]);
decode(?msg_notify_cancel_invitation, Binary) ->
	net_helper:decode([], Binary, [notify_cancel_invitation]);
decode(?msg_notify_start_trade, Binary) ->
	net_helper:decode([string], Binary, [notify_start_trade]);
decode(?msg_req_update_trade_money, Binary) ->
	net_helper:decode([int], Binary, [req_update_trade_money]);
decode(?msg_notify_update_trade_money, Binary) ->
	net_helper:decode([int], Binary, [notify_update_trade_money]);
decode(?msg_notify_other_update_trade_money, Binary) ->
	net_helper:decode([int], Binary, [notify_other_update_trade_money]);
decode(?msg_req_put_trade_item, Binary) ->
	net_helper:decode([int, int], Binary, [req_put_trade_item]);
decode(?msg_notify_put_trade_item, Binary) ->
	net_helper:decode([int, {user_define, fun(Bin)->decode(?msg_item, Bin)end}, int], Binary, [notify_put_trade_item]);
decode(?msg_notify_other_put_trade_item, Binary) ->
	net_helper:decode([int, {user_define, fun(Bin)->decode(?msg_item, Bin)end}, int], Binary, [notify_other_put_trade_item]);
decode(?msg_req_unput_trade_item, Binary) ->
	net_helper:decode([int, int], Binary, [req_unput_trade_item]);
decode(?msg_notify_unput_trade_item, Binary) ->
	net_helper:decode([int], Binary, [notify_unput_trade_item]);
decode(?msg_notify_other_unput_trade_item, Binary) ->
	net_helper:decode([int], Binary, [notify_other_unput_trade_item]);
decode(?msg_req_swap_trade_item, Binary) ->
	net_helper:decode([int, int], Binary, [req_swap_trade_item]);
decode(?msg_notify_swap_trade_item, Binary) ->
	net_helper:decode([int, int], Binary, [notify_swap_trade_item]);
decode(?msg_notify_other_swap_trade_item, Binary) ->
	net_helper:decode([int, int], Binary, [notify_other_swap_trade_item]);
decode(?msg_req_sure_trade, Binary) ->
	net_helper:decode([], Binary, [req_sure_trade]);
decode(?msg_notify_sure_trade, Binary) ->
	net_helper:decode([], Binary, [notify_sure_trade]);
decode(?msg_notify_other_sure_trade, Binary) ->
	net_helper:decode([], Binary, [notify_other_sure_trade]);
decode(?msg_req_cancel_trade, Binary) ->
	net_helper:decode([], Binary, [req_cancel_trade]);
decode(?msg_notify_cancel_trade, Binary) ->
	net_helper:decode([], Binary, [notify_cancel_trade]);
decode(?msg_notify_trade_success, Binary) ->
	net_helper:decode([], Binary, [notify_trade_success]);
decode(?msg_furniture_goods_data, Binary) ->
	net_helper:decode([int, int, int, int, int, int, int, int, string], Binary, [furniture_goods_data]);
decode(?msg_req_buy_sys_shop_goods, Binary) ->
	net_helper:decode([int], Binary, [req_buy_sys_shop_goods]);
decode(?msg_req_buy_sys_shop_goods_list, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_furniture_goods_data, Bin)end}], Binary, [req_buy_sys_shop_goods_list]);
decode(?msg_notify_buy_furniture_list, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_house_furniture, Bin)end}], Binary, [notify_buy_furniture_list]);
decode(?msg_player_coin, Binary) ->
	net_helper:decode([string, int, int], Binary, [player_coin]);
decode(?msg_notify_update_coin, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_coin, Bin)end}], Binary, [notify_update_coin]);
decode(?msg_req_buy_npc_shop_goods, Binary) ->
	net_helper:decode([int], Binary, [req_buy_npc_shop_goods]);
decode(?msg_req_sell_goods, Binary) ->
	net_helper:decode([int], Binary, [req_sell_goods]);
decode(?msg_notify_open_shop, Binary) ->
	net_helper:decode([int], Binary, [notify_open_shop]);
decode(?msg_farm_pet_food, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [farm_pet_food]);
decode(?msg_farm_log_message, Binary) ->
	net_helper:decode([int, string, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, string, int, int, int, int], Binary, [farm_log_message]);
decode(?msg_farm_log_gain, Binary) ->
	net_helper:decode([int, int, int, int], Binary, [farm_log_gain]);
decode(?msg_farm_log_consumption, Binary) ->
	net_helper:decode([int, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, int, int, int, int], Binary, [farm_log_consumption]);
decode(?msg_farm_comment, Binary) ->
	net_helper:decode([int, string, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, string, string], Binary, [farm_comment]);
decode(?msg_farm_decoration, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_item, Bin)end}, {user_define, fun(Bin)->decode(?msg_item, Bin)end}, {user_define, fun(Bin)->decode(?msg_item, Bin)end}, {user_define, fun(Bin)->decode(?msg_item, Bin)end}, {user_define, fun(Bin)->decode(?msg_item, Bin)end}, {user_define, fun(Bin)->decode(?msg_item, Bin)end}, {user_define, fun(Bin)->decode(?msg_item, Bin)end}, {user_define, fun(Bin)->decode(?msg_item, Bin)end}, {user_define, fun(Bin)->decode(?msg_item, Bin)end}], Binary, [farm_decoration]);
decode(?msg_farm_crop_data, Binary) ->
	net_helper:decode([int, int, {array, int}, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, int, int, int, int, int, int, {array, int}, int], Binary, [farm_crop_data]);
decode(?msg_player_farm_data, Binary) ->
	net_helper:decode([string, int, {user_define, fun(Bin)->decode(?msg_farm_decoration, Bin)end}, {user_define, fun(Bin)->decode(?msg_farm_pet_food, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_farm_crop_data, Bin)end}, int], Binary, [player_farm_data]);
decode(?msg_farm_exp_data, Binary) ->
	net_helper:decode([int, int], Binary, [farm_exp_data]);
decode(?msg_farm_hint_data, Binary) ->
	net_helper:decode([int, int, int, int], Binary, [farm_hint_data]);
decode(?msg_farm_pick_all_result, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_farm_data, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_farm_exp_data, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_farm_hint_data, Bin)end}], Binary, [farm_pick_all_result]);
decode(?msg_player_farm_log, Binary) ->
	net_helper:decode([string, {array, user_define, fun(Bin)->decode(?msg_farm_log_message, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_farm_log_gain, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_farm_log_consumption, Bin)end}], Binary, [player_farm_log]);
decode(?msg_player_farm_comment, Binary) ->
	net_helper:decode([string, {array, user_define, fun(Bin)->decode(?msg_farm_comment, Bin)end}], Binary, [player_farm_comment]);
decode(?msg_farm_status, Binary) ->
	net_helper:decode([string, string, int, int], Binary, [farm_status]);
decode(?msg_req_enter_farm, Binary) ->
	net_helper:decode([string], Binary, [req_enter_farm]);
decode(?msg_notify_farm_data, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_farm_data, Bin)end}], Binary, [notify_farm_data]);
decode(?msg_req_leave_farm, Binary) ->
	net_helper:decode([], Binary, [req_leave_farm]);
decode(?msg_notify_leave_farm, Binary) ->
	net_helper:decode([], Binary, [notify_leave_farm]);
decode(?msg_req_assart_plot, Binary) ->
	net_helper:decode([], Binary, [req_assart_plot]);
decode(?msg_req_sow, Binary) ->
	net_helper:decode([int, uint64], Binary, [req_sow]);
decode(?msg_req_pick_crop, Binary) ->
	net_helper:decode([string, int], Binary, [req_pick_crop]);
decode(?msg_req_pick_all_crops, Binary) ->
	net_helper:decode([string], Binary, [req_pick_all_crops]);
decode(?msg_req_get_friend_farm_state, Binary) ->
	net_helper:decode([], Binary, [req_get_friend_farm_state]);
decode(?msg_notify_friend_farm_status, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_farm_status, Bin)end}], Binary, [notify_friend_farm_status]);
decode(?msg_req_weed, Binary) ->
	net_helper:decode([string, int], Binary, [req_weed]);
decode(?msg_req_worm, Binary) ->
	net_helper:decode([string, int], Binary, [req_worm]);
decode(?msg_req_water, Binary) ->
	net_helper:decode([string, int], Binary, [req_water]);
decode(?msg_notify_pet_bite, Binary) ->
	net_helper:decode([int], Binary, [notify_pet_bite]);
decode(?msg_req_farm_log_list, Binary) ->
	net_helper:decode([int, string], Binary, [req_farm_log_list]);
decode(?msg_notify_farm_log_list, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_farm_log, Bin)end}], Binary, [notify_farm_log_list]);
decode(?msg_req_clear_farm_log, Binary) ->
	net_helper:decode([], Binary, [req_clear_farm_log]);
decode(?msg_req_buy_farm_shop_goods, Binary) ->
	net_helper:decode([int, int, int, int], Binary, [req_buy_farm_shop_goods]);
decode(?msg_req_sell_farm_shop_goods, Binary) ->
	net_helper:decode([uint64, int], Binary, [req_sell_farm_shop_goods]);
decode(?msg_req_sell_farm_shop_all_goods, Binary) ->
	net_helper:decode([int], Binary, [req_sell_farm_shop_all_goods]);
decode(?msg_notify_farm_info, Binary) ->
	net_helper:decode([string, int, int, int, int], Binary, [notify_farm_info]);
decode(?msg_req_farm_comment, Binary) ->
	net_helper:decode([string, string], Binary, [req_farm_comment]);
decode(?msg_notify_farm_comment, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_farm_comment, Bin)end}], Binary, [notify_farm_comment]);
decode(?msg_req_reply_farm_comment, Binary) ->
	net_helper:decode([string, string], Binary, [req_reply_farm_comment]);
decode(?msg_notify_reply_farm_comment, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_farm_comment, Bin)end}], Binary, [notify_reply_farm_comment]);
decode(?msg_req_farm_comment_list, Binary) ->
	net_helper:decode([string], Binary, [req_farm_comment_list]);
decode(?msg_notify_farm_comment_list, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_farm_comment, Bin)end}], Binary, [notify_farm_comment_list]);
decode(?msg_req_clear_farm_comment, Binary) ->
	net_helper:decode([], Binary, [req_clear_farm_comment]);
decode(?msg_notify_clear_farm_comment, Binary) ->
	net_helper:decode([int], Binary, [notify_clear_farm_comment]);
decode(?msg_req_remove_decoration, Binary) ->
	net_helper:decode([int], Binary, [req_remove_decoration]);
decode(?msg_req_change_decoration, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_item, Bin)end}], Binary, [req_change_decoration]);
decode(?msg_req_put_grass, Binary) ->
	net_helper:decode([string, int], Binary, [req_put_grass]);
decode(?msg_req_put_pest, Binary) ->
	net_helper:decode([string, int], Binary, [req_put_pest]);
decode(?msg_farm_setting, Binary) ->
	net_helper:decode([string, string, string, string, string, string], Binary, [farm_setting]);
decode(?msg_req_farm_setting, Binary) ->
	net_helper:decode([], Binary, [req_farm_setting]);
decode(?msg_req_set_farm_setting, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_farm_setting, Bin)end}], Binary, [req_set_farm_setting]);
decode(?msg_notify_set_farm_setting, Binary) ->
	net_helper:decode([int], Binary, [notify_set_farm_setting]);
decode(?msg_req_reset_farm_setting, Binary) ->
	net_helper:decode([], Binary, [req_reset_farm_setting]);
decode(?msg_notify_reset_farm_setting, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_farm_setting, Bin)end}], Binary, [notify_reset_farm_setting]);
decode(?msg_notify_farm_setting, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_farm_setting, Bin)end}], Binary, [notify_farm_setting]);
decode(?msg_req_lock_depot_item, Binary) ->
	net_helper:decode([uint64], Binary, [req_lock_depot_item]);
decode(?msg_req_unlock_depot_item, Binary) ->
	net_helper:decode([uint64], Binary, [req_unlock_depot_item]);
decode(?msg_req_hoeing, Binary) ->
	net_helper:decode([int], Binary, [req_hoeing]);
decode(?msg_req_use_farm_item, Binary) ->
	net_helper:decode([uint64, int], Binary, [req_use_farm_item]);
decode(?msg_req_can_assart_plot, Binary) ->
	net_helper:decode([], Binary, [req_can_assart_plot]);
decode(?msg_req_can_upgrade_land, Binary) ->
	net_helper:decode([], Binary, [req_can_upgrade_land]);
decode(?msg_req_upgrade_land, Binary) ->
	net_helper:decode([], Binary, [req_upgrade_land]);
decode(?msg_notify_farm_money, Binary) ->
	net_helper:decode([string, int, int], Binary, [notify_farm_money]);
decode(?msg_notify_farm_awards, Binary) ->
	net_helper:decode([int, int, {array, int}, {array, int}], Binary, [notify_farm_awards]);
decode(?msg_notify_farm_eq_coin, Binary) ->
	net_helper:decode([int], Binary, [notify_farm_eq_coin]);
decode(?msg_req_task_list, Binary) ->
	net_helper:decode([], Binary, [req_task_list]);
decode(?msg_notify_task_list, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_player_task, Bin)end}], Binary, [notify_task_list]);
decode(?msg_req_farm_task_list, Binary) ->
	net_helper:decode([], Binary, [req_farm_task_list]);
decode(?msg_notify_farm_task_list, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_player_task, Bin)end}], Binary, [notify_farm_task_list]);
decode(?msg_req_track_task, Binary) ->
	net_helper:decode([uint64], Binary, [req_track_task]);
decode(?msg_notify_track_task, Binary) ->
	net_helper:decode([uint64], Binary, [notify_track_task]);
decode(?msg_req_get_track_list, Binary) ->
	net_helper:decode([], Binary, [req_get_track_list]);
decode(?msg_notify_get_track_list, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_player_task, Bin)end}], Binary, [notify_get_track_list]);
decode(?msg_req_cancel_track_task, Binary) ->
	net_helper:decode([uint64], Binary, [req_cancel_track_task]);
decode(?msg_notify_cancel_track_task, Binary) ->
	net_helper:decode([uint64], Binary, [notify_cancel_track_task]);
decode(?msg_req_give_up_task, Binary) ->
	net_helper:decode([uint64], Binary, [req_give_up_task]);
decode(?msg_notify_give_up_task, Binary) ->
	net_helper:decode([uint64], Binary, [notify_give_up_task]);
decode(?msg_notify_task_complete, Binary) ->
	net_helper:decode([uint64], Binary, [notify_task_complete]);
decode(?msg_notify_give_task, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_task, Bin)end}], Binary, [notify_give_task]);
decode(?msg_req_complete_task, Binary) ->
	net_helper:decode([int], Binary, [req_complete_task]);
decode(?msg_notify_give_farm_task, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_task, Bin)end}], Binary, [notify_give_farm_task]);
decode(?msg_req_farm_complete_task, Binary) ->
	net_helper:decode([int], Binary, [req_farm_complete_task]);
decode(?msg_notify_farm_task_complete, Binary) ->
	net_helper:decode([uint64], Binary, [notify_farm_task_complete]);
decode(?msg_notify_mail_not_read, Binary) ->
	net_helper:decode([], Binary, [notify_mail_not_read]);
decode(?msg_notify_add_mail, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_mail_info, Bin)end}], Binary, [notify_add_mail]);
decode(?msg_req_delete_mail, Binary) ->
	net_helper:decode([uint64], Binary, [req_delete_mail]);
decode(?msg_notify_delete_mail, Binary) ->
	net_helper:decode([uint64], Binary, [notify_delete_mail]);
decode(?msg_req_get_mail_item, Binary) ->
	net_helper:decode([uint64, uint64], Binary, [req_get_mail_item]);
decode(?msg_notify_delete_mail_item, Binary) ->
	net_helper:decode([uint64, uint64], Binary, [notify_delete_mail_item]);
decode(?msg_req_mail_list, Binary) ->
	net_helper:decode([], Binary, [req_mail_list]);
decode(?msg_notify_mail_list, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_mail_info, Bin)end}], Binary, [notify_mail_list]);
decode(?msg_req_mail_content, Binary) ->
	net_helper:decode([uint64], Binary, [req_mail_content]);
decode(?msg_notify_mail_content, Binary) ->
	net_helper:decode([uint64, string], Binary, [notify_mail_content]);
decode(?msg_req_know_new_mail_title, Binary) ->
	net_helper:decode([{array, uint64}], Binary, [req_know_new_mail_title]);
decode(?msg_req_reject_mail, Binary) ->
	net_helper:decode([uint64], Binary, [req_reject_mail]);
decode(?msg_req_send_mail, Binary) ->
	net_helper:decode([string, string, string, string, {array, uint64}], Binary, [req_send_mail]);
decode(?msg_notify_send_mail_success, Binary) ->
	net_helper:decode([], Binary, [notify_send_mail_success]);
decode(?msg_notify_reject_mail_success, Binary) ->
	net_helper:decode([uint64], Binary, [notify_reject_mail_success]);
decode(?msg_req_open_ui, Binary) ->
	net_helper:decode([int], Binary, [req_open_ui]);
decode(?msg_notify_open_ui, Binary) ->
	net_helper:decode([int], Binary, [notify_open_ui]);
decode(?msg_req_sys_time, Binary) ->
	net_helper:decode([], Binary, [req_sys_time]);
decode(?msg_notify_day_or_night, Binary) ->
	net_helper:decode([int], Binary, [notify_day_or_night]);
decode(?msg_notify_sys_time, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [notify_sys_time]);
decode(?msg_notify_wallow_time, Binary) ->
	net_helper:decode([int], Binary, [notify_wallow_time]);
decode(?msg_notify_player_health_status, Binary) ->
	net_helper:decode([string, int], Binary, [notify_player_health_status]);
decode(?msg_notify_disease_special_event, Binary) ->
	net_helper:decode([int], Binary, [notify_disease_special_event]);
decode(?msg_req_start_buy_house, Binary) ->
	net_helper:decode([], Binary, [req_start_buy_house]);
decode(?msg_notify_start_buy_house, Binary) ->
	net_helper:decode([], Binary, [notify_start_buy_house]);
decode(?msg_req_end_buy_house, Binary) ->
	net_helper:decode([], Binary, [req_end_buy_house]);
decode(?msg_notify_end_buy_house, Binary) ->
	net_helper:decode([], Binary, [notify_end_buy_house]);
decode(?msg_req_buy_house, Binary) ->
	net_helper:decode([int], Binary, [req_buy_house]);
decode(?msg_notify_buy_house, Binary) ->
	net_helper:decode([int], Binary, [notify_buy_house]);
decode(?msg_req_buy_house_replace, Binary) ->
	net_helper:decode([int, int], Binary, [req_buy_house_replace]);
decode(?msg_req_buy_house_add, Binary) ->
	net_helper:decode([int], Binary, [req_buy_house_add]);
decode(?msg_req_deposit_money_in_depot, Binary) ->
	net_helper:decode([int], Binary, [req_deposit_money_in_depot]);
decode(?msg_req_withdraw_money_in_depot, Binary) ->
	net_helper:decode([int], Binary, [req_withdraw_money_in_depot]);
decode(?msg_notify_player_depot, Binary) ->
	net_helper:decode([int, {array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}], Binary, [notify_player_depot]);
decode(?msg_req_start_domestic, Binary) ->
	net_helper:decode([], Binary, [req_start_domestic]);
decode(?msg_notify_start_domestic, Binary) ->
	net_helper:decode([], Binary, [notify_start_domestic]);
decode(?msg_req_domestic_service, Binary) ->
	net_helper:decode([int], Binary, [req_domestic_service]);
decode(?msg_notify_domestic_result, Binary) ->
	net_helper:decode([int, int, int, int], Binary, [notify_domestic_result]);
decode(?msg_req_end_domestic, Binary) ->
	net_helper:decode([int], Binary, [req_end_domestic]);
decode(?msg_notify_end_domestic, Binary) ->
	net_helper:decode([int], Binary, [notify_end_domestic]);
decode(?msg_req_walk_for_pick_magic_box, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_point, Bin)end}, {user_define, fun(Bin)->decode(?msg_point, Bin)end}, uint64, int], Binary, [req_walk_for_pick_magic_box]);
decode(?msg_req_pick_magic_box, Binary) ->
	net_helper:decode([uint64], Binary, [req_pick_magic_box]);
decode(?msg_req_client_empty_box_positions, Binary) ->
	net_helper:decode([int], Binary, [req_client_empty_box_positions]);
decode(?msg_notify_server_empty_box_positions, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_grid_pos, Bin)end}], Binary, [notify_server_empty_box_positions]);
decode(?msg_notify_house_magic_box, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_magic_box, Bin)end}], Binary, [notify_house_magic_box]);
decode(?msg_notify_del_house_magic_box, Binary) ->
	net_helper:decode([uint64], Binary, [notify_del_house_magic_box]);
decode(?msg_notify_add_house_magic_box, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_magic_box, Bin)end}], Binary, [notify_add_house_magic_box]);
decode(?msg_notify_pick_magic_box_gain, Binary) ->
	net_helper:decode([int, int, int], Binary, [notify_pick_magic_box_gain]);
decode(?msg_notify_friend_garbage, Binary) ->
	net_helper:decode([string, int], Binary, [notify_friend_garbage]);
decode(?msg_req_start_gift_service, Binary) ->
	net_helper:decode([int], Binary, [req_start_gift_service]);
decode(?msg_notify_start_gift_service, Binary) ->
	net_helper:decode([int], Binary, [notify_start_gift_service]);
decode(?msg_req_end_gift_service, Binary) ->
	net_helper:decode([], Binary, [req_end_gift_service]);
decode(?msg_notify_end_gift_service, Binary) ->
	net_helper:decode([int], Binary, [notify_end_gift_service]);
decode(?msg_req_pack_gift_box, Binary) ->
	net_helper:decode([int, int, int, int, {user_define, fun(Bin)->decode(?msg_gift_box, Bin)end}], Binary, [req_pack_gift_box]);
decode(?msg_notify_pack_gift_box, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_gift_box, Bin)end}], Binary, [notify_pack_gift_box]);
decode(?msg_req_unpack_gift_box, Binary) ->
	net_helper:decode([uint64], Binary, [req_unpack_gift_box]);
decode(?msg_notify_unpack_gift_box, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_gift_box, Bin)end}], Binary, [notify_unpack_gift_box]);
decode(?msg_req_put_gift_box, Binary) ->
	net_helper:decode([uint64, {user_define, fun(Bin)->decode(?msg_grid_pos, Bin)end}], Binary, [req_put_gift_box]);
decode(?msg_notify_put_gift_box, Binary) ->
	net_helper:decode([], Binary, [notify_put_gift_box]);
decode(?msg_req_pickup_gift_box, Binary) ->
	net_helper:decode([uint64], Binary, [req_pickup_gift_box]);
decode(?msg_notify_pickup_gift_box, Binary) ->
	net_helper:decode([uint64], Binary, [notify_pickup_gift_box]);
decode(?msg_notify_gift_box, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_gift_box, Bin)end}], Binary, [notify_gift_box]);
decode(?msg_req_get_send_box_vec, Binary) ->
	net_helper:decode([int, int], Binary, [req_get_send_box_vec]);
decode(?msg_notify_get_send_box_vec, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_gift_box, Bin)end}, int], Binary, [notify_get_send_box_vec]);
decode(?msg_req_get_receive_box_vec, Binary) ->
	net_helper:decode([int, int], Binary, [req_get_receive_box_vec]);
decode(?msg_notify_get_receive_box_vec, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_gift_box, Bin)end}, int], Binary, [notify_get_receive_box_vec]);
decode(?msg_req_get_viewed_box_vec, Binary) ->
	net_helper:decode([int, int], Binary, [req_get_viewed_box_vec]);
decode(?msg_notify_get_viewed_box_vec, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_gift_box, Bin)end}, int], Binary, [notify_get_viewed_box_vec]);
decode(?msg_gift_express, Binary) ->
	net_helper:decode([int, uint64, string, string, string, string, {array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}, int, string, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [gift_express]);
decode(?msg_req_send_gift_express, Binary) ->
	net_helper:decode([int, int, int, int, {user_define, fun(Bin)->decode(?msg_gift_express, Bin)end}], Binary, [req_send_gift_express]);
decode(?msg_notify_send_gift_express, Binary) ->
	net_helper:decode([uint64], Binary, [notify_send_gift_express]);
decode(?msg_req_receive_gift_express, Binary) ->
	net_helper:decode([uint64, int, int], Binary, [req_receive_gift_express]);
decode(?msg_notify_receive_gift_express, Binary) ->
	net_helper:decode([uint64], Binary, [notify_receive_gift_express]);
decode(?msg_req_get_send_express_vec, Binary) ->
	net_helper:decode([int, int], Binary, [req_get_send_express_vec]);
decode(?msg_notify_get_send_express_vec, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_gift_express, Bin)end}, int], Binary, [notify_get_send_express_vec]);
decode(?msg_req_get_receive_express_vec, Binary) ->
	net_helper:decode([int, int], Binary, [req_get_receive_express_vec]);
decode(?msg_notify_express_vec, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_gift_express, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_gift_express, Bin)end}], Binary, [notify_express_vec]);
decode(?msg_notify_send_express, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_gift_express, Bin)end}], Binary, [notify_send_express]);
decode(?msg_req_send_express_by_subscribe, Binary) ->
	net_helper:decode([uint64], Binary, [req_send_express_by_subscribe]);
decode(?msg_notify_get_receive_express_vec, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_gift_express, Bin)end}, int], Binary, [notify_get_receive_express_vec]);
decode(?msg_req_get_viewed_express_vec, Binary) ->
	net_helper:decode([int, int], Binary, [req_get_viewed_express_vec]);
decode(?msg_notify_get_viewed_express_vec, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_gift_express, Bin)end}, int], Binary, [notify_get_viewed_express_vec]);
decode(?msg_req_clean_gift_log, Binary) ->
	net_helper:decode([int], Binary, [req_clean_gift_log]);
decode(?msg_notify_show_picture, Binary) ->
	net_helper:decode([int], Binary, [notify_show_picture]);
decode(?msg_req_start_work, Binary) ->
	net_helper:decode([], Binary, [req_start_work]);
decode(?msg_notify_start_work, Binary) ->
	net_helper:decode([], Binary, [notify_start_work]);
decode(?msg_req_work, Binary) ->
	net_helper:decode([int], Binary, [req_work]);
decode(?msg_notify_work_result, Binary) ->
	net_helper:decode([int, int, int, int, int, int, int, int, int], Binary, [notify_work_result]);
decode(?msg_req_end_work, Binary) ->
	net_helper:decode([int], Binary, [req_end_work]);
decode(?msg_notify_end_work, Binary) ->
	net_helper:decode([int], Binary, [notify_end_work]);
decode(?msg_req_get_weibo_access_secret, Binary) ->
	net_helper:decode([], Binary, [req_get_weibo_access_secret]);
decode(?msg_notify_weibo_access_secret, Binary) ->
	net_helper:decode([string, string, string, string], Binary, [notify_weibo_access_secret]);
decode(?msg_req_save_access_secret, Binary) ->
	net_helper:decode([string, string, string, string], Binary, [req_save_access_secret]);
decode(?msg_req_preview_house, Binary) ->
	net_helper:decode([string, uint64], Binary, [req_preview_house]);
decode(?msg_notify_preview_house, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_house_data, Bin)end}], Binary, [notify_preview_house]);
decode(?msg_req_preview_shop_house, Binary) ->
	net_helper:decode([int], Binary, [req_preview_shop_house]);
decode(?msg_req_end_preview_house, Binary) ->
	net_helper:decode([], Binary, [req_end_preview_house]);
decode(?msg_notify_end_preview_house, Binary) ->
	net_helper:decode([], Binary, [notify_end_preview_house]);
decode(?msg_bottle_info, Binary) ->
	net_helper:decode([uint64, string, int, string, {user_define, fun(Bin)->decode(?msg_stime, Bin)end}], Binary, [bottle_info]);
decode(?msg_req_del_bottle_log, Binary) ->
	net_helper:decode([uint64], Binary, [req_del_bottle_log]);
decode(?msg_req_drop_bottle, Binary) ->
	net_helper:decode([string, int, int, int], Binary, [req_drop_bottle]);
decode(?msg_notify_drop_bottle, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_bottle_info, Bin)end}], Binary, [notify_drop_bottle]);
decode(?msg_req_pick_bottle, Binary) ->
	net_helper:decode([], Binary, [req_pick_bottle]);
decode(?msg_notify_pick_bottle, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_bottle_info, Bin)end}], Binary, [notify_pick_bottle]);
decode(?msg_req_all_bottle_info, Binary) ->
	net_helper:decode([], Binary, [req_all_bottle_info]);
decode(?msg_notify_bottles_info, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_bottle_info, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_bottle_info, Bin)end}], Binary, [notify_bottles_info]);
decode(?msg_notify_today_bottle_info, Binary) ->
	net_helper:decode([int, int, int, int, int], Binary, [notify_today_bottle_info]);
decode(?msg_notify_open_float_bottle_pool, Binary) ->
	net_helper:decode([], Binary, [notify_open_float_bottle_pool]);
decode(?msg_notify_house_decoration, Binary) ->
	net_helper:decode([string, int], Binary, [notify_house_decoration]);
decode(?msg_notify_new_day_reset, Binary) ->
	net_helper:decode([], Binary, [notify_new_day_reset]);
decode(?msg_notify_total_flower_count, Binary) ->
	net_helper:decode([string, int], Binary, [notify_total_flower_count]);
decode(?msg_req_send_flower, Binary) ->
	net_helper:decode([], Binary, [req_send_flower]);
decode(?msg_notify_send_flower_success, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_info, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}], Binary, [notify_send_flower_success]);
decode(?msg_req_flower_logs, Binary) ->
	net_helper:decode([], Binary, [req_flower_logs]);
decode(?msg_notify_flower_logs, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_flower_log, Bin)end}], Binary, [notify_flower_logs]);
decode(?msg_notify_house_flower_change, Binary) ->
	net_helper:decode([string, int], Binary, [notify_house_flower_change]);
decode(?msg_req_send_egg, Binary) ->
	net_helper:decode([], Binary, [req_send_egg]);
decode(?msg_notify_send_egg_success, Binary) ->
	net_helper:decode([{user_define, fun(Bin)->decode(?msg_player_info, Bin)end}, {array, user_define, fun(Bin)->decode(?msg_pack_grid, Bin)end}], Binary, [notify_send_egg_success]);
decode(?msg_req_egg_logs, Binary) ->
	net_helper:decode([], Binary, [req_egg_logs]);
decode(?msg_notify_egg_logs, Binary) ->
	net_helper:decode([{array, user_define, fun(Bin)->decode(?msg_egg_log, Bin)end}], Binary, [notify_egg_logs]);
decode(?msg_notify_house_egg_change, Binary) ->
	net_helper:decode([string, int], Binary, [notify_house_egg_change]);
decode(?msg_req_is_active_game, Binary) ->
	net_helper:decode([int], Binary, [req_is_active_game]);
decode(?msg_notify_active_game_result, Binary) ->
	net_helper:decode([int], Binary, [notify_active_game_result]).
