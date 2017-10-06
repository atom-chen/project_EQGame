#pragma once
enum VER_VALIDATE_RESULT
{
	ver_ok = 1,
	ver_fail = 2
};

enum LOGIN_RESULT
{
	lr_ok = 1,
	lr_fail = 2,
	lr_fail_pwd = 3,
	lr_fail_account = 4,
	lr_fail_other = 5
};

enum CREATE_ROLE_RESULT
{
	rr_ok = 1,
	rr_fail = 2
};

enum ENTER_SCENE_RESULT
{
	sr_ok = 1,
	sr_fail = 2
};

enum PACKAGE_TYPE
{
	pt_bag = 1,
	pt_body = 2,
	pt_farmdepot = 3,
	pt_depot = 4,
	pt_gift_bag = 5
};

enum GM_COMMAND_TYPE
{
	gct_add_item = 1,
	gct_get_money = 2,
	gct_replace_tex = 3,
	gct_add_farm_item = 4,
	gct_send_sys_mail = 5,
	gct_send_n_mail = 6,
	gct_send_gm_mail = 7
};

enum INVITATION_TYPE
{
	ivt_trade = 1,
	ivt_enter_house = 2,
	ivt_make_friend = 3,
	ivt_kiss = 4,
	ivt_hug = 5,
	ivt_shake_hands = 6
};

enum MAIL_READ_STATUS
{
	mrs_not_read = 1,
	mrs_read_title = 2,
	mrs_read_content = 3
};

enum MAIL_TYPE
{
	mt_person_mail = 1,
	mt_gm_mail = 2,
	mt_system_mail = 3,
	mt_reject_mail = 4
};

enum SYS_BROADCAST_TYPE
{
	sbt_sys_show = 1,
	sbt_at_time = 2,
	sbt_player_behavior = 3,
	sbt_gm = 4,
	sbt_farm = 5,
	sbt_per_day = 6
};

enum TRACK_TASK_RESULT
{
	ttr_ok = 1,
	ttr_fail = 2
};

enum CANCEL_TRACK_TASK_RESULT
{
	cttr_ok = 1,
	cttr_fail = 2
};

enum TASK_GIVE_UP_RESULT
{
	tgur_ok = 1,
	tgur_fail = 2
};

enum FARM_STATE
{
	fms_gain = 1,
	fms_pest = 2,
	fms_grass = 3,
	fms_drought = 4,
	fms_normal = 5
};

enum FARM_CROP_STAGE
{
	fcs_sprouting = 1,
	fcs_s_leafing = 2,
	fcs_b_leafing = 3,
	fcs_blooming = 4,
	fcs_fruiting = 5,
	fcs_picking = 6,
	fcs_die = 7
};

enum UI_TYPE
{
	uit_sysshop = 1,
	uit_npcshop = 2,
	uit_farmdepot = 3,
	uit_depot = 4,
	uit_bag = 5,
	uit_housetransaction = 6
};

enum SG_TYPE
{
	sgt_farm = 1
};

enum SET_FARM_SETTING_RESULT
{
	sfsr_ok = 1,
	sfsr_fail = 2
};

enum COMMENT_RESULT
{
	cr_ok = 1,
	cr_fail = 2
};

enum FARM_LOG_TYPE
{
	flt_help = 1,
	flt_gain = 2,
	flt_consumption = 3
};

enum FARM_LOG_MESSAGE_TYPE
{
	flmt_help = 1,
	flmt_demage = 2,
	flmt_extract = 3,
	flmt_bite = 4
};

enum FARM_LOG_CONSUMPTION_TYPE
{
	flct_buy = 1,
	flct_sell = 2
};

enum FARM_COMMENT_TYPE
{
	fct_owner = 1,
	fct_guest = 2,
	fct_reply = 3
};

enum DISEASE_TYPE
{
	dt_normal = 1,
	dt_white = 2,
	dt_yellow = 3,
	dt_purple = 4,
	dt_red = 5
};

enum MAGIC_BOX_TYPE
{
	mbt_magic_box = 1,
	mbt_garbage = 2
};

enum BUY_HOUSE_RESULT
{
	bhr_replace = 1,
	bhr_add = 2
};

enum SET_HOUSE_WELCOME_WORDS_RESULT
{
	shwwr_ok = 1,
	shwwr_fail = 2
};

enum SET_HOUSE_PERMISSION_RESULT
{
	shpr_ok = 1,
	shpr_fail = 2
};

enum HOUSE_PERMISSION_TYPE
{
	hpt_none = 1,
	hpt_friend = 2
};

enum FURNITURE_PERMISSION_TYPE
{
	fpt_yes = 1,
	fpt_no = 2
};

enum HOUSE_VISIT_TYPE
{
	hvt_visit = 1,
	hvt_leave = 2,
	hvt_pick_garbage = 3
};

enum CLEAR_HOUSE_LOG_RESULT
{
	chlr_ok = 1,
	chlr_fail = 2
};

enum ACCEPT_INVITE_TYPE
{
	accept_invite_yes = 1,
	accept_invite_no = 2
};

enum ANONYMOUS_TYPE
{
	at_yes = 1,
	at_no = 2
};

enum GIFT_SERVICE_TYPE
{
	gst_normal = 1,
	gst_pack = 2,
	gst_pickup = 3,
	gst_put = 4,
	gst_express = 5,
	gst_reply_express = 6
};

enum GIFT_LOG_TYPE
{
	glt_send_box = 1,
	glt_receive_box = 2,
	glt_viewed_box = 3,
	glt_send_express = 4,
	glt_receive_express = 5,
	glt_viewed_express = 6
};

enum BUY_GOODS_TYPE
{
	bgt_buy = 1,
	bgt_use = 2
};

enum PAY_GOODS_TYPE
{
	pgt_game_gold = 1,
	pgt_eq_gold = 2
};

enum BUY_HOUSE_REPLACE_TYPE
{
	bhrt_yes = 1,
	bhrt_no = 2
};

enum TASK_TYPE
{
	tt_yes = 1,
	tt_no = 2
};

enum SEX_TYPE
{
	st_boy = 1,
	st_girl = 2
};

enum GIFT_EXPRESS_TYPE
{
	gm = 1,
	player = 2
};

enum BOTTLE_TYPE
{
	bt_common = 1,
	bt_make_friend = 2
};

enum DAY_OR_NIGHT_FLAG
{
	dn_day = 1,
	dn_night = 2
};

enum TASK_ACTION_TYPE
{
	tat_visit_friend = 1,
	tat_work = 2,
	tat_wash_hands = 3,
	tat_pick_garbage = 4,
	tat_change_hair = 5,
	tat_drop_float_bottle = 6,
	tat_pick_float_bottle = 7,
	tat_send_egg = 8,
	tat_send_flower = 9,
	tat_domestic = 10,
	tat_bash = 11,
	tat_buy_shoes = 12,
	tat_buy_clothes = 13,
	tat_buy_desk = 14,
	tat_buy_chair = 15,
	tat_buy_storage = 16,
	tat_buy_electric = 17,
	tat_buy_hat = 18,
	tat_buy_decoration = 19
};

enum TASK_KIND
{
	tk_newer = 1,
	tk_daily = 2,
	tk_once = 3,
	tk_rate_once = 4
};

enum GM_LEVEL
{
	gm_enable = 1,
	gm_disable = 2
};


const int PROTO_VER = 290;
