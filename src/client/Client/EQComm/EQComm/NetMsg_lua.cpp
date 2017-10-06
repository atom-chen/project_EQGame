#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "LuaManager.h"
#include "NetPacket.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"

using namespace luabind;

bool CLuaManager::RegisterNetMsgClass()
{
	module( EQ_GLOBA_CLASS )
	[
		luabind::def("getMsgMgr",&EQGameMgr::getMsgManager)
	,
	class_<EQMsgManager>("EQMsgManager")
		.def("GMRequest", (BOOL (EQMsgManager::*)(int, const char*))&EQMsgManager::GMRequest)
		.def("GMRequest", (BOOL (EQMsgManager::*)(int, const char*,const char*))&EQMsgManager::GMRequest)
		.def("GMRequest", (BOOL (EQMsgManager::*)(int, const char*,const char*,const char*))&EQMsgManager::GMRequest)
		.def("GMRequest", (BOOL (EQMsgManager::*)(int, const char*,const char*,const char*,const char*))&EQMsgManager::GMRequest)
		.def("send", &EQMsgManager::Send)
		.def("Logout",&EQMsgManager::LogoutRequest)
	,
	/**
	网络协议类型 （由服务端定义和生成）
	*/
	class_<NetMsgType>("NetMsgType")
		.enum_("net_type")
		[
		value("msg_notify_player_data", 
				NetMsgType::msg_notify_player_data),
		value("msg_notify_start_edit_house", 
				NetMsgType::msg_notify_start_edit_house),
		value("msg_notify_end_edit_house", 
				NetMsgType::msg_notify_end_edit_house),
		value("msg_notify_sys_msg", 
				NetMsgType::msg_notify_sys_msg),
		value("msg_notify_add_friend", 
				NetMsgType::msg_notify_add_friend)
		]
	,

	//inetpacket
	class_<INetPacket>("INetPacket")
		.def(constructor<>())
	,
	
	//player_basic_data
	class_<player_basic_data,INetPacket>("player_basic_data")
		.def(constructor<>())
		.def_readwrite( "account", &player_basic_data::account )
		.def_readwrite( "username", &player_basic_data::username )
		.def_readwrite( "sex", &player_basic_data::sex )
		.def_readwrite( "skin_color", &player_basic_data::skin_color )
		.def_readwrite( "hair", &player_basic_data::hair )
		.def_readwrite( "face", &player_basic_data::face )
		.def_readwrite( "beard", &player_basic_data::beard )
		.def_readwrite( "level", &player_basic_data::level )
	,
	class_<player_info,INetPacket>("player_info")
		.def(constructor<>())
		.def_readwrite( "basic_data", &player_info::basic_data )
		.def_readwrite( "scenename", &player_info::scenename )
	,
	
	//notify_player_data
	class_<notify_player_data,INetPacket>("notify_player_data")
		.def(constructor<>())
		.def_readwrite( "basic_data", &notify_player_data::basic_data )
	,

	//req_create_role
	class_<req_create_role,INetPacket>("req_create_role")
		.def(constructor<>())
		.def_readwrite( "basic_data", &req_create_role::basic_data )
	,

	//req_create_role
	class_<req_swap_item,INetPacket>("req_swap_item")
		.def(constructor<>())
		.def_readwrite( "index1", &req_swap_item::index1 )
		.def_readwrite( "index2", &req_swap_item::index2 )
		.def_readwrite( "type",   &req_swap_item::type )
	,

	//req_move_item
	class_<req_move_item,INetPacket>("req_move_item")
		.def(constructor<>())
		.def_readwrite( "index1", &req_move_item::index1 )
		.def_readwrite( "pack1_type", &req_move_item::pack1_type )
		.def_readwrite( "index2", &req_move_item::index2 )
		.def_readwrite( "pack2_type", &req_move_item::pack2_type )
	,

	//req_use_item
	class_<req_use_item,INetPacket>("req_use_item")
		.def(constructor<>())
		.def_readwrite( "index", &req_use_item::index )
	,
	//
	class_<req_start_edit_house,INetPacket>("req_start_edit_house")
		.def(constructor<>())
	,
	//
	class_<req_end_edit_house,INetPacket>("req_end_edit_house")
		.def(constructor<>())
	,
	//
	class_<notify_start_edit_house,INetPacket>("notify_start_edit_house")
		.def(constructor<>())
	//	.def_readonly("result" , &notify_start_edit_house::result)
	,
	//
	class_<notify_end_edit_house,INetPacket>("notify_end_edit_house")
		.def(constructor<>())
	//	.def_readonly("result" , &notify_end_edit_house::result)
	,
	//
	class_<req_recovery_furniture,INetPacket>("req_recovery_furniture")
		.def(constructor<>())
		.def_readwrite("furniture_id" , &req_recovery_furniture::furniture_id)
	,
	//
	class_<req_delete_item,INetPacket>("req_delete_item")
		.def(constructor<>())
		.def_readwrite("grid_index" , &req_delete_item::grid_index)
	,
	//
	class_<notify_sys_msg,INetPacket>("notify_sys_msg")
		.def(constructor<>())
		.def_readwrite("code" , &notify_sys_msg::code)
	,
	//
	class_<npc_command,INetPacket>("npc_command")
		.def(constructor<>())
		.def_readwrite("template_id" , &npc_command::template_id)
		.def_readwrite("page_index" , &npc_command::page_index)
		.def_readwrite("command_index" , &npc_command::command_index)
		.def_readwrite("unique_id" , &npc_command::unique_id)
	,
	//
	class_<notify_npc_show_dialog_by_message,INetPacket>("notify_npc_show_dialog_by_message")
		.def(constructor<>())
		.def_readwrite("template_id" , &notify_npc_show_dialog_by_message::template_id)
		.def_readwrite("page_index" , &notify_npc_show_dialog_by_message::page_index)
		.def_readwrite("command_index" , &notify_npc_show_dialog_by_message::command_index)
	,

	//  [5/24/2010 huangdj1]
	class_<notify_add_friend,INetPacket>("notify_add_friend")
		.def(constructor<>())
		.def_readwrite("friend_account" , &notify_add_friend::friend_account)
	,

	//req_buy_sys_shop_goods
	class_<req_buy_sys_shop_goods,INetPacket>("req_buy_sys_shop_goods")
	.def(constructor<>())
	.def_readwrite( "goods_id", &req_buy_sys_shop_goods::goods_id )
	,

	//req_buy_sys_shop_goods
	class_<req_buy_npc_shop_goods,INetPacket>("req_buy_npc_shop_goods")
	.def(constructor<>())
	.def_readwrite( "goods_id", &req_buy_npc_shop_goods::goods_id )
	,

	//req_buy_sys_shop_goods
	class_<req_open_ui,INetPacket>("req_open_ui")
	.def(constructor<>())
	.def_readwrite( "type", &req_open_ui::type )
	,

	//
	class_<req_other_player_info,INetPacket>("req_other_player_info")
	.def(constructor<>())
	.def_readwrite("account",&req_other_player_info::account)
	,

	class_<req_agree_invitation,INetPacket>("req_agree_invitation")
	.def(constructor<>())
	.def_readwrite("type",&req_agree_invitation::type)
	.def_readwrite("invitor",&req_agree_invitation::invitor)
	,

	class_<req_disagree_invitation,INetPacket>("req_disagree_invitation")
	.def(constructor<>())
	.def_readwrite("type",&req_disagree_invitation::type)
	.def_readwrite("invitor",&req_disagree_invitation::invitor)
	,

	class_<req_friend_setting_black_someone,INetPacket>("req_friend_setting_black_someone")
	.def(constructor<>())
	.def_readwrite("who",&req_friend_setting_black_someone::who)
	,

	class_<offline_invite_record,INetPacket>("offline_invite_record")
	.def(constructor<>())
	.def_readonly("invitor",&offline_invite_record::invitor)
	.def_readonly("invitor_name",&offline_invite_record::invitor_name)
	.def_readonly("invite_type",&offline_invite_record::invite_type)
	.def_readonly("date",&offline_invite_record::date)
	,
	
	//notify_eq_user_info
	class_<eq_user_info, INetPacket>("eq_user_info")
	.def(constructor<>())
	.def_readonly( "full_name", &eq_user_info::full_name)
	.def_readonly( "nick_name", &eq_user_info::nick_name)
	.def_readonly( "birthday",&eq_user_info::birthday )
	.def_readonly( "phone", &eq_user_info::phone)
	.def_readonly( "url", &eq_user_info::url)
	.def_readonly( "email", &eq_user_info::email)
	.def_readonly( "country", &eq_user_info::country)
	.def_readonly( "region", &eq_user_info::region)
	.def_readonly( "city", &eq_user_info::city)
	.def_readonly( "street", &eq_user_info::street)
	.def_readonly( "org_name", &eq_user_info::org_name)
	.def_readonly( "org_unit", &eq_user_info::org_unit)
	.def_readonly( "title", &eq_user_info::title)
	.def_readonly( "role", &eq_user_info::role)
	.def_readonly( "desc", &eq_user_info::desc)
	.def_readonly( "photo_type", &eq_user_info::photo_type)
	.def_readonly( "photo_data", &eq_user_info::photo_data)
	,

	//stime
	class_<stime,INetPacket>("stime")
		.def(constructor<>())
		.def_readwrite( "year", &stime::year )
		.def_readwrite( "month", &stime::month )
		.def_readwrite( "day", &stime::day )
		.def_readwrite( "hour", &stime::hour )
		.def_readwrite( "minute", &stime::minute )
		.def_readwrite( "second", &stime::second )
	,
	//farm_log_message
	class_<farm_log_message,INetPacket>("farm_log_message")
		.def(constructor<>())
		.def_readwrite( "log_type", &farm_log_message::log_type )
		.def_readwrite( "log_date", &farm_log_message::log_date )
		.def_readwrite( "user_name", &farm_log_message::user_name )
		.def_readwrite( "crop_id", &farm_log_message::crop_id )
		.def_readwrite( "crop_quantity", &farm_log_message::crop_quantity )
		.def_readwrite( "bite_coin", &farm_log_message::bite_coin )
		.def_readwrite( "pet_id", &farm_log_message::pet_id )
	,
	//farm_log_message
	class_<farm_status,INetPacket>("farm_status")
		.def(constructor<>())
		.def_readwrite( "account", &farm_status::account )
		.def_readwrite( "exp", &farm_status::exp )
		.def_readwrite( "status", &farm_status::status )
	,
	//farm_log_message
	class_<farm_comment,INetPacket>("farm_comment")
		.def(constructor<>())
		.def_readwrite( "type", &farm_comment::type )
		.def_readwrite( "account", &farm_comment::account )
		.def_readwrite( "comment_date", &farm_comment::comment_date )
		.def_readwrite( "username", &farm_comment::username )
		.def_readwrite( "content", &farm_comment::content )
	,
	//farm_log_message
	class_<notify_farm_comment,INetPacket>("notify_farm_comment")
		.def(constructor<>())
		/*.def_readwrite( "result", &notify_farm_comment::result )*/
	,
	//farm_log_gain
	class_<farm_log_gain,INetPacket>("farm_log_gain")
		.def(constructor<>())
		.def_readwrite( "crop_id", &farm_log_gain::crop_id )
		.def_readwrite( "crop_quantity", &farm_log_gain::crop_quantity )
		.def_readwrite( "steal_quantity", &farm_log_gain::steal_quantity )
		.def_readwrite( "gain_quantity", &farm_log_gain::gain_quantity )
	,
	//farm_log_consumption
	class_<farm_log_consumption,INetPacket>("farm_log_consumption")
		.def(constructor<>())
		.def_readwrite( "type", &farm_log_consumption::type )
		.def_readwrite( "log_date", &farm_log_consumption::log_date )
		.def_readwrite( "crop_id", &farm_log_consumption::crop_id )
		.def_readwrite( "crop_quantity", &farm_log_consumption::crop_quantity )
		.def_readwrite( "use_coin", &farm_log_consumption::use_coin )
		.def_readwrite( "leave_coin", &farm_log_consumption::leave_coin )
	,

	//req_task_list
	class_<req_task_list,INetPacket>("req_task_list")
	.def(constructor<>())
	,

	//req_give_up_task
	class_<req_give_up_task,INetPacket>("req_give_up_task")
	.def(constructor<>())
	.def_readwrite( "id", &req_give_up_task::id )
	,

	//req_split_item
	class_<req_split_item,INetPacket>("req_split_item")
		.def(constructor<>())
		.def_readwrite( "src_gindex", &req_split_item::src_gindex )
		.def_readwrite( "target_gindex", &req_split_item::target_gindex )
		.def_readwrite( "count", &req_split_item::count )
		.def_readwrite( "type",  &req_split_item::type )
	,
	//req_give_up_task
	class_<req_resize_player_pack,INetPacket>("req_resize_player_pack")
		.def(constructor<>())
	,
	class_<req_hoeing,INetPacket>("req_hoeing")
		.def(constructor<>())
		.def_readwrite("plot_index",	&req_hoeing::plot_index)
	,
	//
	class_<req_assart_plot,INetPacket>("req_assart_plot")
	.def(constructor<>())
	,
	//
	class_<house_plot_info,INetPacket>("house_plot_info")
		.def(constructor<>())
		.def_readonly("instance_id",&house_plot_info::instance_id)
		.def_readonly("template_id",&house_plot_info::template_id)
		.def_readonly("buy_date",&house_plot_info::buy_date)
		.def_readonly("plot_id",&house_plot_info::plot_id)
	,

	class_<req_send_flower,INetPacket>("req_send_flower")
	.def(constructor<>())
	,

	class_<req_send_egg,INetPacket>("req_send_egg")
	.def(constructor<>())

	];
	
	return true;
}
//----------------------------------------------------------------------------------
void CLuaManager::CallFunction(std::string szFun, int msgtype, INetPacket* ba)
{
	switch (msgtype)
	{
		//
	case NetMsgType::msg_notify_player_data:
		luabind::call_function<void>(m_pLuaState, szFun.c_str(), msgtype,
			(notify_player_data*)ba);//boost::ref(ba));
		break;
		//
	case NetMsgType::msg_notify_house_data:
		luabind::call_function<void>(m_pLuaState, szFun.c_str(), msgtype,
			(notify_house_data*)ba);
		break;
	case NetMsgType::msg_notify_package:
		luabind::call_function<void>(m_pLuaState, szFun.c_str(), msgtype,
			(notify_package*)ba);
		break;
	case NetMsgType::msg_notify_start_edit_house:
		luabind::call_function<void>(m_pLuaState, szFun.c_str(), msgtype,
			(notify_start_edit_house*)ba);
		break;
	case NetMsgType::msg_notify_end_edit_house:
		luabind::call_function<void>(m_pLuaState, szFun.c_str(), msgtype,
			(notify_end_edit_house*)ba);
		break;
	case NetMsgType::msg_notify_sys_msg:
		luabind::call_function<void>(m_pLuaState, szFun.c_str(), msgtype,
			(notify_sys_msg*)ba);
		break;
	case NetMsgType::msg_notify_add_friend:
		luabind::call_function<void>(m_pLuaState, szFun.c_str(), msgtype,
			(notify_add_friend*)ba);
		break;
	default:
		string title = "msgtype: ";
		title+=msgtype;
		MessageBoxA( NULL,"no such msgtype support...","",MB_OK);
	}
}

