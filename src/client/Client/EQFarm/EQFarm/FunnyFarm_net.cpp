/*******************************************************************
* 作者: XiaoMing
* 时间: 2010-04-23
* 描述: 场景消息处理
********************************************************************/
#include "EQFarmPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "LuaManager.h"

#include "FunnyFarm.h"

#include "EnumDef.h"
#include "NetPacket.h"
#include "NetMsgType.h"

#include "MessageManager.h"
#include "GameEventMgr.h"
#include "ObjMgr.h"
#include "MainPlayer.h"
#include "FriendSystem.h"
#include "Timekeeper.h"
#include "SmallModules.h"
#include "CommFunc.h"
#include "DynamicUIMgr.h"
#include "FarmTable.h"
#include "EQGameSceneMgr.h"
#include "ItemTable.h"
#include "RecycleResource.h"
#include "StringHandle.h"
//-------------------------------------------------------------------------
void FunnyFarm::reqEnterFarm(string owner)
{
	static req_enter_farm enter;
	if(strcmp(owner.c_str() ,"") == 0)
		enter.owner = sMainPlayer.getAccount();
	else
		enter.owner = owner;
	//DynamicUIMgr::getSingleton().clearFarmPaoPaoBuffer();
	LOGERR( CEGUI::PropertyHelper::boolToString(mIsOneKeyHavSta).c_str());
	if (!mIsOneKeyHavSta)
	{
		sMsgMgr.Send(enter);
	}

	/// 准备回收资源
	PrepareRecycleResource();
}

//-------------------------------------------------------------------------
void FunnyFarm::reqComment(string account,string comment)
{
	static req_farm_comment req;
	if(strcmp(account.c_str() ,"") == 0)
		req.account = sMainPlayer.getAccount();
	else
		req.account = account;
	// 去除字符串结尾的回车 空格
	comment = comment.substr(0,comment.find_last_not_of(" \n") + 1);
	//return str.substr(str.find_first_not_of(" \n\r\t")); 

	std::string output;
	output = sStringHandle.stringFilter(const_cast<char *>(comment.c_str()),0);

	req.comment = output;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqCommentList(const std::string & account)
{
	static req_farm_comment_list req;
	req.account = account;
	sMsgMgr.Send(req);
}

//-------------------------------------------------------------------------
void FunnyFarm::reqReplyComment(string account,string comment)
{
	static req_reply_farm_comment req;

	req.account = account;

	req.comment = comment;

	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqClearComment()
{
	static req_clear_farm_comment req;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqclearLog()
{
	req_clear_farm_log req;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqAssartPlot()
{
// 	req_assart_plot req;
	req_can_assart_plot req;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqHoeing(int plotIdx)
{
	req_hoeing req;
	req.plot_index		= plotIdx;
	sMsgMgr.Send(req);
}
//------------------------------------------------------------------------------
void FunnyFarm::_reqSowAction(string owner, int plotIdx)
{
	if ((uint)plotIdx <= mCropsCtrl.size()
		&& plotIdx <= mCropNum)
	{
		std::string strid = luabind::call_function<std::string>(sLuaMgr.getL(), "GetClickSeed");
		unsigned __int64 id = _strtoui64(strid.c_str(),NULL,10);
		this->reqSow(plotIdx,id);
	}
}
//-------------------------------------------------------------------------
void FunnyFarm::reqSow(int plotIdx,uint64 item_inst_id)
{
	req_sow req;
	req.plot_index		= plotIdx;
	req.item_inst_id	= item_inst_id;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::_reqUseProp(int plotIdx)
{
	if ((uint)plotIdx <= mCropsCtrl.size()
		&& plotIdx <= mCropNum)
	{
		std::string strid = luabind::call_function<std::string>(sLuaMgr.getL(), "GetClickSeed");
		unsigned __int64 id = _strtoui64(strid.c_str(),NULL,10);
		this->reqUseProp(plotIdx, id);
	}
}
//-------------------------------------------------------------------------
void FunnyFarm::reqUsePropStr(int plotIdx, std::string item_inst_id)
{
	req_use_farm_item req;
	unsigned __int64 id = _strtoui64(item_inst_id.c_str(),NULL,10);
	req.plot_index = plotIdx;
	req.item_inst_id = id;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqUseProp(int plotIdx, uint64 item_inst_id)
{
	req_use_farm_item req;
	req.plot_index = plotIdx;
	req.item_inst_id = item_inst_id;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqPickCrop(string owner, int plotIdx)
{
	req_pick_crop req;
	req.account		= owner;
	req.plot_index	= plotIdx;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqPickAllCrops(string owner)
{
	req_pick_all_crops req;
	req.account = owner;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqChgBulletin(int id)
{
	/*req_change_farm_bulletin_board req;
	req.id		= id;
	sMsgMgr.Send(req);*/
}
//-------------------------------------------------------------------------
void FunnyFarm::reqFriendFarmState()
{
	req_get_friend_farm_state req;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqPutGrass( string owner, int plotIdx )
{
	req_put_grass req;
	req.account		= owner;
	req.plot_index	= plotIdx;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqPutPest( string owner, int plotIdx )
{
	req_put_pest req;
	req.account		= owner;
	req.plot_index	= plotIdx;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqWeed(string owner, int plotIdx)
{
	req_weed req;
	req.account		= owner;
	req.plot_index	= plotIdx;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqWorn(string owner, int plotIdx)
{
	req_worm req;
	req.account		= owner;
	req.plot_index	= plotIdx;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqWater(string owner, int plotIdx)
{
	req_water req;
	req.account		= owner;
	req.plot_index	= plotIdx;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqAnnounceList()
{
	/*req_farm_announcement_list req;
	sMsgMgr.Send(req);*/
}
//-------------------------------------------------------------------------
void FunnyFarm::reqLogList(int type,const std::string & account)
{
	req_farm_log_list req;
	req.type = type;
	req.account = account;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqLockStorageItem(std::string  instanceID)
{
	uint64 insID = _atoi64(instanceID.c_str());
	req_lock_depot_item reqLock;
	reqLock.item_inst_id = insID;
	sMsgMgr.Send(reqLock);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqUnLockStorageItem(std::string  instanceID)
{
	uint64 insID = _atoi64(instanceID.c_str());
	req_unlock_depot_item reqUnLock;
	reqUnLock.item_inst_id = insID;
	sMsgMgr.Send(reqUnLock);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqAllSell(int type)
{
	req_sell_farm_shop_all_goods req;
	req.type = type;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqOneSell(std::string instanceID, int count)
{
	uint64 insID = _atoi64(instanceID.c_str());
	req_sell_farm_shop_goods req;
	req.instance_id = insID;
	req.count = count;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqBuyGoods(int templateId,int type, int count,int pay_type)
{
	req_buy_farm_shop_goods req;
	req.item_id = templateId;
	req.type = type;
	req.count = count;
	req.pay_type = pay_type;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::notiAllSell()
{
	
}
//-------------------------------------------------------------------------
void FunnyFarm::reqSetting()
{
	req_farm_setting req;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::reqSetSetting()
{
	req_set_farm_setting req;

	if(true == mResetFlag)
	{
		CEGUI::WindowManager* winMgr = CEGUI::WindowManager::getSingletonPtr();
		req.setting.account = mAccount;
		std::string _tmp;
		_tmp = sStringHandle.stringFilter(const_cast<char*>(winMgr->getWindow("MSystemSetting/bg/TextInput0")->getText().c_str()),0);
		replacestring(_tmp,"***");
		replacestring(_tmp,"**");
		req.setting.water	  = _tmp;

		_tmp = sStringHandle.stringFilter(const_cast<char*>(winMgr->getWindow("MSystemSetting/bg/TextInput1")->getText().c_str()),0);
		replacestring(_tmp,"***");
		replacestring(_tmp,"**");
		req.setting.worm	  = _tmp;

		_tmp = sStringHandle.stringFilter(const_cast<char*>(winMgr->getWindow("MSystemSetting/bg/TextInput2")->getText().c_str()),0);
		replacestring(_tmp,"***");
		replacestring(_tmp,"**");
		req.setting.weed      = _tmp;

		_tmp = sStringHandle.stringFilter(const_cast<char*>(winMgr->getWindow("MSystemSetting/bg/TextInput3")->getText().c_str()),0);
		replacestring(_tmp,"***");
		replacestring(_tmp,"**");
		req.setting.put_grass = _tmp;

		_tmp = sStringHandle.stringFilter(const_cast<char*>(winMgr->getWindow("MSystemSetting/bg/TextInput4")->getText().c_str()),0);
		replacestring(_tmp,"***");
		replacestring(_tmp,"**");
		req.setting.put_pest  = _tmp;
		sMsgMgr.Send(req);
	}else
		return;
}
//-------------------------------------------------------------------------
std::string& FunnyFarm::replacestring(std::string& source_str,std::string find_str)
{
	size_t find_str_len = find_str.length();
	size_t find_ret = -1;
	while (true)
	{
		find_ret = source_str.find(find_str);
		if (find_ret != -1)
		{
			source_str.replace(find_ret,find_str_len,"*");
		}
		else
			break;
	}
	return source_str;
}
//-------------------------------------------------------------------------
void FunnyFarm::reqSettingDefault()
{
	req_reset_farm_setting req;
	sMsgMgr.Send(req);
}
//-------------------------------------------------------------------------
void FunnyFarm::notiSettingDefault()
{// 暂时和notiFarmSetting 做一样的功能，以后UI上应有不同的表现
	notify_farm_setting data;
	data.decode(sMsgMgr.getRecebuff());
	setSetting(data);
}
//-------------------------------------------------------------------------
void FunnyFarm::notiFarmSetting()
{
	//收到内容时通知界面弹出对话框
	notify_farm_setting data;
	data.decode(sMsgMgr.getRecebuff());
	setSetting(data);
}
void FunnyFarm::notiSetResult()
{
	notify_set_farm_setting data;
	data.decode(sMsgMgr.getRecebuff());
	mSetResult = data.result;
	// 通知UI
}
//-------------------------------------------------------------------------
void FunnyFarm::setSetting(const notify_farm_setting & data)
{
	CEGUI::WindowManager* winMgr = CEGUI::WindowManager::getSingletonPtr();
	winMgr->getWindow("MSystemSetting/bg/TextInput0")->setText((CEGUI::utf8*)(data.setting.water.c_str()));
	winMgr->getWindow("MSystemSetting/bg/TextInput1")->setText((CEGUI::utf8*)(data.setting.worm.c_str()));
	winMgr->getWindow("MSystemSetting/bg/TextInput2")->setText((CEGUI::utf8*)(data.setting.weed.c_str()));
	winMgr->getWindow("MSystemSetting/bg/TextInput3")->setText((CEGUI::utf8*)(data.setting.put_grass.c_str()));
	winMgr->getWindow("MSystemSetting/bg/TextInput4")->setText((CEGUI::utf8*)(data.setting.put_pest.c_str()));
	sEventMgr.fireEvent("POP_FARM_SETTING");
}
//-------------------------------------------------------------------------
bool CMessageManager::handleByFarm()
{
	switch(mMsgtype)
	{
	case NetMsgType::msg_notify_farm_data:	
			sFunnyFarm.notiFarmData();
		break;
	case NetMsgType::msg_notify_friend_farm_status:
		sFunnyFarm.notiFriendFarmState();
		break;
	case NetMsgType::msg_notify_pet_bite:
		sFunnyFarm.notiBite();
		break;

	//case NetMsgType::msg_notify_use_pet:
	//	sFunnyFarm.notiUsePet();
	//	break;
	//case NetMsgType::msg_notify_stop_use_pet:
	//	sFunnyFarm.notiStopPet();
	//	break;
	//case NetMsgType::msg_notify_food_is_empty:
	//	sFunnyFarm.notiFoodEmpty();

	case NetMsgType::msg_notify_farm_log_list:
		sFunnyFarm.notiLogList();
		
		break;

		case NetMsgType::msg_notify_farm_comment_list:
			sFunnyFarm.notiCommentList();
		break;
		case NetMsgType::msg_notify_farm_comment:
			sFunnyFarm.notiComment();
		break;
			
		case NetMsgType::msg_notify_farm_setting:
			sFunnyFarm.notiFarmSetting();
		break;
		case NetMsgType::msg_notify_set_farm_setting:
			sFunnyFarm.notiSetResult();
		break;
		case NetMsgType::msg_notify_reset_farm_setting:
			sFunnyFarm.notiSettingDefault();
			break;
		case NetMsgType::msg_notify_farm_info:
			sFunnyFarm.notiFarmInfo();
			break;
		case NetMsgType::msg_farm_exp_data:
			sFunnyFarm.notiExpData();
			break;
		case NetMsgType::msg_farm_pick_all_result:
			sFunnyFarm.notiAllPickData();
			break;
		case NetMsgType::msg_notify_farm_money:
			sFunnyFarm.notiFarmMoney();
			break;
		case NetMsgType::msg_notify_farm_eq_coin:
			sFunnyFarm.notiFarmEqGold();
			break;
		case NetMsgType::msg_notify_farm_awards:
			sFunnyFarm.notifyFarmAwards();
			break;
// 		case NetMsgType::msg_notify_exist_purchase_item:
// 			sFunnyFarm.notiExistPurchase();
// 			break;
	//case NetMsgType::msg_notify_change_bulletin_board:
	//	sFunnyFarm.notiChgBulletin();
		//break;
			
	case NetMsgType::msg_notify_sys_time:
		sTimekeeper.recvServerTime();
		EQGameSceneMgr::getSingleton().initDayNightTimer();
		break;
	case NetMsgType::msg_notify_eq_user_info:
		sFunnyFarm.notiPlayerEqInfo();
		break;
	case NetMsgType::msg_notify_farm_task_list:
		sFunnyFarm.handle_farm_task_list();
		break;
	case NetMsgType::msg_notify_task_complete:
		sFunnyFarm.handle_farm_task_complete();
		break;
	case NetMsgType::msg_notify_wallow_time:
		sFunnyFarm.handle_wallow_time();
		break;
	case NetMsgType::msg_notify_finish_enter_scene:
		EQGameSceneMgr::getSingleton().endLoading();
		break;
	default:
		return false;
	}

	return true;
}	
//-------------------------------------------------------------------------

// void FunnyFarm::notiFriendFarmState()
// {
// 	notify_friend_farm_status data;
// 	data.decode(sMsgMgr.getRecebuff());
// 
// 	//sEventMgr.fireEvent("UPDATE_UI_FARM_DATA");
// }	
//-------------------------------------------------------------------------
void FunnyFarm::notiBite()
{
	notify_pet_bite data;
	data.decode(sMsgMgr.getRecebuff());

	//sEventMgr.fireEvent("UPDATE_UI_FARM_DATA");
}	
//-------------------------------------------------------------------------
void FunnyFarm::notiUsePet()
{
	//notify_use_pet data;
	//data.decode(sMsgMgr.getRecebuff());

	//sEventMgr.fireEvent("UPDATE_UI_FARM_DATA");
}	
//-------------------------------------------------------------------------
void FunnyFarm::notiStopPet()
{
	//notify_stop_use_pet data;
	//data.decode(sMsgMgr.getRecebuff());

	//sEventMgr.fireEvent("UPDATE_UI_FARM_DATA");
}	
//-------------------------------------------------------------------------
void FunnyFarm::notiFoodEmpty()
{
	//notify_food_is_empty data;
	//data.decode(sMsgMgr.getRecebuff());

	//sEventMgr.fireEvent("UPDATE_UI_FARM_DATA");
}	
//-------------------------------------------------------------------------
void FunnyFarm::notiChgBulletin()
{
	//notify_change_bulletin_board data;
	//data.decode(sMsgMgr.getRecebuff());

	//sEventMgr.fireEvent("UPDATE_UI_FARM_DATA");
}	
//-------------------------------------------------------------------------
void FunnyFarm::notiLogList()
{
	notify_farm_log_list data;
	data.decode(sMsgMgr.getRecebuff());

	std::string time;
	int num = data.log.message_log.size();
	for (int i=0; i < num; i++)
	{
		farm_log_message msg = data.log.message_log[i];
		time = SmallModules::getSingleton().getTimeString(msg.log_date);
		luabind::call_function<void>(sLuaMgr.getL(), "lua_addLogMsg",msg, (GB2312ToUTF8)(time.c_str()), i+1, num);
	}

	num = data.log.gain_log.size();
	for (int i=0; i < num; i++)
	{
		farm_log_gain gain = data.log.gain_log[i];
		luabind::call_function<void>(sLuaMgr.getL(), "lua_addLogGain",gain, i+1, num);
	}

	num = data.log.consumption_log.size();
	for (int i=0; i < num; i++)
	{
		farm_log_consumption cons = data.log.consumption_log[i];
		time = SmallModules::getSingleton().getTimeString(cons.log_date);
		luabind::call_function<void>(sLuaMgr.getL(), "lua_addLogCons",cons, (GB2312ToUTF8)(time.c_str()), i+1, num);
	}

	
}	
//-------------------------------------------------------------------------
void FunnyFarm::notiFriendFarmState()
{
	notify_friend_farm_status data;
	data.decode(sMsgMgr.getRecebuff());

	int num = data.data.size();

	luabind::call_function<void>(sLuaMgr.getL(), "lua_clearFriendFarmStateTable");
	for(int i=0; i < num; i++)
	{
		const farm_status* state = &data.data[i];
		//const player_basic_data* basicData;
// 		if (sObjMgr.isMainPlayer(state->account))
// 		{
// 			basicData = sMainPlayer.getPlayerData();
// 		}
// 		else
// 		{
// 			basicData = FriendSystem::getSingleton().searchFriendBasicInfo(state->account);
// 		}
		luabind::call_function<void>(sLuaMgr.getL(), "lua_addFriendFarmState", state->account/*basicData*/, state->name, state);
	}
	if (num > 0)
	{
		CLuaManager::getSingleton().CallFunction("triggerSort");
	}
	luabind::call_function<void>(sLuaMgr.getL(), "FarmFL_openFL");
}	
//-------------------------------------------------------------------------
void FunnyFarm::notiCommentList()
{
	notify_farm_comment_list data;
	data.decode(sMsgMgr.getRecebuff());

	int num = data.farm_comment.comment.size();
	std::string time;
	luabind::call_function<void>(sLuaMgr.getL(), "lua_cleanComment");

	for(int i=0; i < num; i++)
	{
		time = SmallModules::getSingleton().getTimeString(data.farm_comment.comment[i].comment_date);
		luabind::call_function<void>(sLuaMgr.getL(), "lua_addComment",&data.farm_comment.comment[i], (GB2312ToUTF8)(time.c_str()), i+1, num);
	}
}		
//-------------------------------------------------------------------------
void FunnyFarm::notiComment()
{
	notify_farm_comment data;
	data.decode(sMsgMgr.getRecebuff());

	farm_comment comm = data.comment;

	std::string time = SmallModules::getSingleton().getTimeString(comm.comment_date);
	luabind::call_function<void>(sLuaMgr.getL(), "lua_notiComment",comm, (GB2312ToUTF8)(time.c_str()));
}
//-----------------------------------------------------------------------------
void FunnyFarm::notiExistPurchase()
{
// 	notify_exist_purchase_item data;
// 	data.decode(sMsgMgr.getRecebuff());
// 	item itm = data.item_data;
	sEventMgr.fireEvent("EXIST_PURCHASE_ITEM");
}
//-----------------------------------------------------------------------------
void FunnyFarm::reqChangeDec(std::string instanceID,int tpltID)
{
	uint64 insID = _atoi64(instanceID.c_str());
	req_change_decoration reqDecData;
	reqDecData.decoration_item.instance_id = insID;
	reqDecData.decoration_item.template_id = tpltID;
	sMsgMgr.Send(reqDecData);
}
//-----------------------------------------------------------------------------
void FunnyFarm::reqRemoveDec(int tpltID)
{
	req_remove_decoration req_removeDecData;
	req_removeDecData.template_id = tpltID;
	sMsgMgr.Send(req_removeDecData);
}
//-----------------------------------------------------------------------------
void FunnyFarm::notiExpData()
{
	farm_exp_data expData;
	expData.decode(sMsgMgr.getRecebuff());
	int farmIdx;
	Ogre::Vector3 landPos;
	farmIdx = expData.plot_index - 1;
	landPos = mCropsCtrl[farmIdx].mFarmScene->getLandUintPos(farmIdx);
	luabind::call_function<void>(sLuaMgr.getL(), "lua_updateFarmInfo",
																	expData.add_exp,
																	landPos.x,
																	landPos.y,
																	landPos.z,
																	farmIdx);
}
//-----------------------------------------------------------------------------
void FunnyFarm::notiFarmInfo()
{
	notify_farm_info farmInfo;
	farmInfo.decode(sMsgMgr.getRecebuff());
	int farmIdx;
	Ogre::Vector3 landPos;
	farmIdx = farmInfo.plot_index - 1;
	landPos = mCropsCtrl[farmIdx].mFarmScene->getLandUintPos(farmIdx);
	luabind::call_function<void>(sLuaMgr.getL(), "lua_updateFarmInfo1",farmInfo.account,
		farmInfo.exp,
		farmInfo.level,
		farmInfo.add_exp,
		landPos.x,
		landPos.y,
		landPos.z,
		farmIdx);
	// 农场新手任务用到(2011/4/21 add by hezhr)
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTask_clickUnit", mAccount, m_curOperateType);
}
//-----------------------------------------------------------------------
void FunnyFarm::notiAllPickData()
{
	farm_pick_all_result allPickData;
	allPickData.decode(sMsgMgr.getRecebuff());
	int cropNum = allPickData.farm_data.crops.size();
	std::vector<farm_exp_data> pickDexp = allPickData.exp_data;
	std::vector<farm_exp_data>::size_type expData_size = allPickData.exp_data.size();
	std::vector<farm_hint_data> farmHintData = allPickData.hint;
	std::vector<farm_exp_data>::size_type hint_size = farmHintData.size();

	mAllHintData.msg_id = 0;
	mAllHintData.hintCount = 0;
	mAllHintData.loseGold = 0;

	for (uint i = 0; i < hint_size; i++)
	{
		if (farmHintData[i].plot_index == -1)
		{
			mAllHintData.msg_id = farmHintData[i].hint_id;
			mAllHintData.hintCount = farmHintData[i].param1;
			mAllHintData.loseGold = farmHintData[i].param2;
		}
		else
		{
			//setHarvestDymanicUI(farmHintData[i].plot_index,allPickData.farm_data.crops[i].tempid,allPickData.farm_data.crops[i].pick_num);
		}
	}
	int farmIdx = 0;
	Ogre::Vector3 landPos;
	mCropNum = allPickData.farm_data.crops.size();
	std::vector<int> expArray;	//把farm_exp_data中的数据与地块匹配起来，索引为地块索引号，值为相应地块获得的经验值
	std::vector<int> hint_index_vec;
	std::vector<int> hint_each_losegold_vec;
	for (int i = 0; i < mCropNum; i++)
	{
		expArray.push_back(0);
		hint_index_vec.push_back(0);
		hint_each_losegold_vec.push_back(0);
	}
	for (size_t i= 0; i < expData_size; i++)
	{
		expArray[allPickData.exp_data[expData_size-(i+1)].plot_index-1]	= allPickData.exp_data[expData_size-(i+1)].add_exp;
	}
	for (size_t i=0; i < hint_size; i++)
	{
		if (farmHintData[i].plot_index != -1)
		{
			hint_index_vec[farmHintData[i].plot_index - 1] = farmHintData[i].hint_id;
			hint_each_losegold_vec[farmHintData[i].plot_index - 1] = farmHintData[i].param1;
		}
	}
	for (int i=0; i<mCropNum; i++)
	{
		if (allPickData.farm_data.crops[i].pick_num != 0)
		{
			pushAllPickInfo(i, expArray[i], allPickData.farm_data.crops[i]);
			setHarvestDymanicUI(i,allPickData.farm_data.crops[i].tempid,allPickData.farm_data.crops[i].pick_num);
			
			if (sObjMgr.isMainPlayer(mAccount))
			{
					farmIdx = i;
					landPos = mCropsCtrl[farmIdx].mFarmScene->getLandUintPos(farmIdx);
					luabind::call_function<void>(sLuaMgr.getL(), "lua_updateFarmInfo",
						expArray[i],
						landPos.x,
						landPos.y,
						landPos.z,
						farmIdx,
						allPickData.farm_data.exp,
						allPickData.farm_data.level);

			}
			mIsOneKeyHavSta = true;
		}
		else
		{
			if (hint_index_vec[i] != 0 && hint_index_vec[i] == 149)
			{
				pushAllPickInfo(i, expArray[i], allPickData.farm_data.crops[i]);
				setHintDymanicUI(i, 0,hint_each_losegold_vec[i]);
				if (i == hint_index_vec.size()-1)
				{
					mIsOneKeyHavSta = true;
				}
			}
			else if(hint_index_vec[i] != 0 && hint_index_vec[i] != 149)
			{
				pushNomoneyPickInfo(i,hint_index_vec[i]);
			}
		}
	}
// 	mIsOneKeyHavSta = false;
}
//-----------------------------------------------------------------
//队列中存放地块信息，以便延后处理
void FunnyFarm::pushAllPickInfo(int index,int aexp,farm_crop_data cropData)
{
	AllPickInfo tempInfo;
	tempInfo.index = index;
	tempInfo.dexp = aexp;
	tempInfo.cropData = cropData;
	mAllPickBuffer.push(tempInfo);
}
//-----------------------------------------------------------------
void FunnyFarm::pushNomoneyPickInfo(int plot_id,int msg_id)
{
	NomoneyPickData npData;
	npData.plot_id = plot_id;
	npData.msg_id = msg_id;
	mNomoneyPickData.push(npData);
}
//-----------------------------------------------------------------
bool FunnyFarm::popAllPickInfo()
{
	if (mAllPickBuffer.empty())
	{
		return false;
	}
	AllPickInfo info = mAllPickBuffer.front();
	if(sObjMgr.isMainPlayer(mAccount))
	{
		const FarmLevelRow* flr = FarmTableMgr::getSingleton().getFarmLevelRow(mLev);
		int next_Exp = flr->next_exp;
		mExp = mExp + info.dexp;
		if (mExp > next_Exp)
		{
			mExp = mExp - next_Exp;
			mLev = mLev + 1;
			flr = FarmTableMgr::getSingleton().getFarmLevelRow(mLev);
			next_Exp = flr->next_exp;
		}
		luabind::call_function<void>(sLuaMgr.getL(), "FarmTip_updateExpInfo",mLev,mExp,next_Exp);
	}
	mCropsCtrl[info.index].crop = info.cropData;
	sFunnyFarm.removePickUI(info.index);
	mAllPickBuffer.pop();

	if (!mAllPickBuffer.empty())
	{
		AllPickInfo info1 = mAllPickBuffer.front();
		// 	if(!sFarmObjEvent.getIsPickUI())
		{
			if (mCropsCtrl[info1.index].crop.stage == fcs_picking)
			{
				if (mCropsCtrl[info1.index].crop.picked == 0)
				{
					mCropsCtrl[info1.index].setPickUI(info1.index);
				}
			}
			// 		else
			{
				
			}
		}
	}
	return true;
}
//-----------------------------------------------------------------------------
bool FunnyFarm::popNomoneyPickInfo()
{
	//----------------
	if (mNomoneyPickData.empty())
	{
		return false;
	}
	//
	NomoneyPickData noPickInfo = mNomoneyPickData.front();
	Ogre::Vector3 pos = mFarmScene->getLandUintPos(noPickInfo.plot_id);
	const SysMsgRow* msgRow = ItemTableManager::getSingleton().getSysMsgRow(noPickInfo.msg_id);
	std::string msgContent = msgRow->content;
	DynamicUIMgr::getSingleton().createFarmPaoPao(noPickInfo.plot_id,pos.x,20,pos.z,msgContent.c_str(),"","");
	mNomoneyPickData.pop();
	return true;
}
//-----------------------------------------------------------------------------
void FunnyFarm::reqCanUpgradeLand()
{
	req_can_upgrade_land req;
	sMsgMgr.Send(req);
}
//-----------------------------------------------------------------------------
void FunnyFarm::reqUpgradeLand()
{
	req_upgrade_land req;
	sMsgMgr.Send(req);
}
//-----------------------------------------------------------------------------
void FunnyFarm::notiPlayerEqInfo()
{
	notify_eq_user_info info;
	info.decode(sMsgMgr.getRecebuff());
	if (sObjMgr.isMainPlayer(info.account))
	{
		luabind::call_function<void>(sLuaMgr.getL(), "UpdatePlayerEqInfo", info.account, info.info);
	}
	else
	{
	luabind::call_function<void>(sLuaMgr.getL(), "UpdateOtherPlayerEqInfo", info.account, info.info);
	}
}
//-----------------------------------------------------------------------------
void FunnyFarm::notiFarmMoney()
{
	notify_farm_money farmMoney;
	farmMoney.decode(sMsgMgr.getRecebuff());
	money = farmMoney.money;
	GameEventMgr::getSingleton().fireEvent("UPDATE_FARMGOLD");
}
//------------------------------------------------------------------------------
void FunnyFarm::notiFarmEqGold()
{
	notify_farm_eq_coin farmEqCoin;
	farmEqCoin.decode(sMsgMgr.getRecebuff());
	mEqmoney = farmEqCoin.eq_coin;
	GameEventMgr::getSingleton().fireEvent("UPDATE_FARMEQGOLD");
}
//------------------------------------------------------------------------------
void FunnyFarm::notifyFarmAwards()
{
	notify_farm_awards awardsData;
	awardsData.decode(sMsgMgr.getRecebuff());
	int awardSize = awardsData.itemlist.size();
	//type (1-采摘；2-翻地)
	mPickAwardsVec.clear();
	if (awardsData.type == 1)
	{
		
		for (int i = 0;i < awardSize; i++)
		{
			if (getAction() == CS_FARM_ONEKEYHARVEST)
			{
				PickAwards award;
				award.money = awardsData.money;
				award.awardIdx = awardsData.itemlist[i];
				award.count = awardsData.countlist[i];
				mPickAwardsVec.push_back(award);
			} 
			else
			{
				luabind::call_function<void>(sLuaMgr.getL(), "RedLand_saveRewardSerData", 
																(i+1),
																awardsData.money, 
																awardsData.itemlist[i], 
																awardsData.countlist[i]);
			}
		}
		if (getAction() == CS_FARM_HARVEST)
			GameEventMgr::getSingleton().fireEvent("NOTIFY_PICKAWARD");
	}
	else if(awardsData.type == 2)
	{
		for (int i = 0;i < awardSize; i++)
		{
		luabind::call_function<void>(sLuaMgr.getL(), "RedLand_saveTUTSRewards", 
													(i+1),
													awardsData.money, 
													awardsData.itemlist[i], 
													awardsData.countlist[i]);
		}
		GameEventMgr::getSingleton().fireEvent("NOTIFY_TUTSAWARD");
	}
}
//----------------------------------------------------------------------
void FunnyFarm::checkAllpickAward()
{
	if (mPickAwardsVec.empty())
	{
		return;
	}
	luabind::call_function<void>(sLuaMgr.getL(), "RedLand_clearRewardTable");
	for (size_t i = 0; i < mPickAwardsVec.size();i++)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "RedLand_saveRewardSerData", 
													(i+1),
													mPickAwardsVec[i].money,
													mPickAwardsVec[i].awardIdx,
													mPickAwardsVec[i].count
													);

	}
	luabind::call_function<void>(sLuaMgr.getL(), "RedLand_popHavestRewardUI");
}
//
void FunnyFarm::checkAllHintData()
{
	luabind::call_function<void>(sLuaMgr.getL(), "popHintDataUI", 
		mAllHintData.msg_id,
		mAllHintData.hintCount,
		mAllHintData.loseGold
		);
}
//----------------------------------------------------------------------
void FunnyFarm::reqEqUsersInfo(void)
{
	if (!m_accountList.empty())
	{
		req_eq_users_info req;
		req.account_list = m_accountList;
		sMsgMgr.Send(req);
		m_accountList.clear();
	}
}
//----------------------------------------------------------------------
void FunnyFarm::reqFarmTaskList(void)
{
	req_farm_task_list req;
	sMsgMgr.Send(req);
}
//----------------------------------------------------------------------
void FunnyFarm::handle_farm_task_list(void)
{
	notify_farm_task_list data;
	data.decode(sMsgMgr.getRecebuff());

	m_farmTaskList.clear();
	for(size_t i=0; data.task_list.size()!=i; ++i)
	{
		m_farmTaskList.push_back(data.task_list[i]);
	}
	luabind::call_function<void>(sLuaMgr.getL(), "FarmTask_taskUpdate");
}
//----------------------------------------------------------------------
void FunnyFarm::reqFarmTaskComplete(int task_id)
{
	req_farm_complete_task msg;
	msg.task_id = task_id;
	sMsgMgr.Send(msg);
}
//----------------------------------------------------------------------
void FunnyFarm::handle_farm_task_complete(void)
{
	notify_farm_task_complete data;
	data.decode(sMsgMgr.getRecebuff());

	int task_id = 0;
	std::vector<player_task>::iterator it = m_farmTaskList.begin();
	for(it; m_farmTaskList.end()!=it; ++it)
	{
		if((*it).id == data.id)
		{
			task_id = (*it).task_id;
			m_farmTaskList.erase(it);
			break;
		}
	}
	luabind::call_function<void>(sLuaMgr.getL(), "FarmTask_taskComplete", task_id);
}
//----------------------------------------------------------------------
int FunnyFarm::getFarmTaskCount(void)
{
	return m_farmTaskList.size();
}
//----------------------------------------------------------------------
int FunnyFarm::getFarmTaskIdByIndex(int index)
{
	if ((index<0) || (size_t)index>=m_farmTaskList.size())
	{
		return 0;
	}
	return m_farmTaskList[index].task_id;
}
//----------------------------------------------------------------------
bool FunnyFarm::existFarmTask(int task_id)
{
	std::vector<player_task>::iterator it = m_farmTaskList.begin();
	for(it; m_farmTaskList.end()!=it; ++it)
	{
		if((*it).task_id == task_id)
			return true;
	}
	return false;
}
//----------------------------------------------------------------------
void FunnyFarm::handle_wallow_time(void)
{
	notify_wallow_time data;
	data.decode(sMsgMgr.getRecebuff());

	luabind::call_function<void>(sLuaMgr.getL(), "WallowTime_handle", data.wallow_seconds);
}
//----------------------------------------------------------------------