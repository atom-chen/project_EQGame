#include "EQCommPCH.h"
#include "DriftBottleConnectMgr.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "DriftBottleSystem.h"
#include "EQMsgManager.h"
#include "GameEventMgr.h"

//
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"

//---------------------------------------------------------
DriftBottleConnectMgr::DriftBottleConnectMgr() : mHadReqAllBottlesInfo(false)
{
	_regist_msgHandler();
}
//---------------------------------------------------------
DriftBottleConnectMgr::~DriftBottleConnectMgr()
{

}
//---------------------------------------------------------
void DriftBottleConnectMgr::_regist_msgHandler()
{
	SetMsgRegist((IMsgHandleRegist *)sGameMgr.getEQMsgManager());
	Regist(NetMsgType::msg_notify_drop_bottle, NewMsgHandle(DriftBottleConnectMgr, rec_msg_dropBottle));
	Regist(NetMsgType::msg_notify_pick_bottle, NewMsgHandle(DriftBottleConnectMgr, rec_msg_pickBottle));
	Regist(NetMsgType::msg_notify_bottles_info, NewMsgHandle(DriftBottleConnectMgr, rec_msg_bottlesInfo));
	Regist(NetMsgType::msg_notify_today_bottle_info, NewMsgHandle(DriftBottleConnectMgr, rec_msg_todayBottleInfo));
	Regist(NetMsgType::msg_notify_open_float_bottle_pool, NewMsgHandle(DriftBottleConnectMgr, rec_msg_openBottlePool));

}
//---------------------------------------------------------
void DriftBottleConnectMgr::rec_msg_dropBottle(MsgPack &recBuff)
{
	notify_drop_bottle drop_bottle;
	drop_bottle.decode(recBuff);
	sDriftBottleSys.getDriftBottleMgrPtr()->createDriftBottle(drop_bottle.bottle,1);
	luabind::call_function<void>(sLuaMgr.getL(), "DBS_update",1);
	//luabind::call_function<void>(sLuaMgr.getL(), "DBS_dropSuccess");
}
//---------------------------------------------------------
void DriftBottleConnectMgr::rec_msg_pickBottle(MsgPack &recBuff)
{
	notify_pick_bottle pick_bottle;
	pick_bottle.decode(recBuff);
	sDriftBottleSys.getDriftBottleMgrPtr()->createDriftBottle(pick_bottle.bottle,2);
	luabind::call_function<void>(sLuaMgr.getL(), "DBS_pickWhatBottle",pick_bottle.bottle.type);
	luabind::call_function<void>(sLuaMgr.getL(), "DBS_update",2);
}
//---------------------------------------------------------
void DriftBottleConnectMgr::rec_msg_bottlesInfo(MsgPack &recBuff)
{
	notify_bottles_info bottles_info;
	bottles_info.decode(recBuff);
	//sDriftBottleSys.getDriftBottleMgrPtr()->clearDropBottlesList();
	uint notify_dropBottle_num = bottles_info.drop_bottles.size();
	uint notify_pickBottle_num = bottles_info.pick_bottles.size();
	for (uint i = 0; i < notify_dropBottle_num; ++i)
	{
		sDriftBottleSys.getDriftBottleMgrPtr()->createDriftBottle(bottles_info.drop_bottles[i],1);
	}
	for (uint i = 0;i < notify_pickBottle_num; ++i)
	{
		sDriftBottleSys.getDriftBottleMgrPtr()->createDriftBottle(bottles_info.pick_bottles[i],2);
	}
	GameEventMgr::getSingleton().fireEvent("DRIFTBOTTLELISTUPDATE");
}
//---------------------------------------------------------
void DriftBottleConnectMgr::rec_msg_todayBottleInfo(MsgPack &recBuff)
{
	notify_today_bottle_info today_info;
	today_info.decode(recBuff);
	sDriftBottleSys.setPickCount(today_info.pick_count);
	sDriftBottleSys.setDropCount(today_info.drop_count);
}
//---------------------------------------------------------
void DriftBottleConnectMgr::rec_msg_openBottlePool(MsgPack &recBuff)
{
	luabind::call_function<void>(sLuaMgr.getL(), "DBS_openStartUI");
	if (mHadReqAllBottlesInfo == false)
	{
		reqAllBottlesInfo();
	}
}
//---------------------------------------------------------
void DriftBottleConnectMgr::reqDelBottleLog(uint64 del_id)
{
	req_del_bottle_log req_del;
	req_del.id = del_id;
	sGameMgr.getEQMsgManager()->Send(req_del);
}
//---------------------------------------------------------
void DriftBottleConnectMgr::reqDropBottle(std::string content, int bottle_type, int req_sex, int acc_visible)
{
	req_drop_bottle req;
	req.msg = content;
	req.type = bottle_type;
	if (req_sex != -1)
	{
		req.sex = req_sex;
	}
	if (acc_visible != -1)
	{
		req.account_visible = acc_visible;
	}
	sGameMgr.getEQMsgManager()->Send(req);
}
//---------------------------------------------------------
void DriftBottleConnectMgr::reqPickBottle()
{
	req_pick_bottle req_pick;
	sGameMgr.getEQMsgManager()->Send(req_pick);
}
//---------------------------------------------------------
void DriftBottleConnectMgr::reqAllBottlesInfo()
{
	req_all_bottle_info req;
	sGameMgr.getEQMsgManager()->Send(req);
	mHadReqAllBottlesInfo = true;
}
//---------------------------------------------------------
void DriftBottleConnectMgr::reqEqUserInfo(std::string req_acc)
{
	if(req_acc != "")
	{
		req_eq_users_info req_eq_info;
		req_eq_info.account_list.push_back(req_acc);
		sGameMgr.getEQMsgManager()->Send(req_eq_info);
	}

}