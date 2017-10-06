#include "EQCommPCH.h"
#include "DriftBottleMgr.h"
#include "NormalDriftBottle.h"
#include "CommunicateDriftBottle.h"
#include "CommFunc.h"
#include "CommonHelper.h"
#include "StringHandle.h"
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"

//---------------------------------------------------------
DriftBottleMgr::DriftBottleMgr()
{

}
//---------------------------------------------------------
DriftBottleMgr::~DriftBottleMgr()
{
	for (BottleIterType iter = mDropDriftBottleList.begin(); iter != mDropDriftBottleList.end(); ++iter)
	{
		delete iter->second;
	}
	for (BottleIterType iter = mPickDriftBottleList.begin(); iter != mPickDriftBottleList.end(); ++iter)
	{
		delete iter->second;
	}
	mDropDriftBottleList.clear();
	mPickDriftBottleList.clear();
}
//----------------------------------------------------------
bool DriftBottleMgr::createDriftBottle(bottle_info &bt_info, int own_list, std::string imageset, std::string bottle_image)
{
	DriftBottle* create_bottle = NULL;
	if (isBottleExist(bt_info,own_list))
	{
		LOGMSG("The bottle is already exist!");
		return false;
	}
	switch (bt_info.type)
	{
	case BT_NORMAL:
		create_bottle = new NormalDriftBottle(bt_info.id,bt_info.account,bt_info.type,bt_info.msg,bt_info.time,imageset,bottle_image);
		break;
	case BT_COMMUNICATE:
		create_bottle = new CommunicateDriftBottle(bt_info.id,bt_info.account,bt_info.type,bt_info.msg,bt_info.time,imageset,bottle_image);
		break;
	}
	
	addDriftBottle(bt_info.type,create_bottle,own_list);
	return true;
}
//---------------------------------------------------------
bool DriftBottleMgr::addDriftBottle(int bottle_type,DriftBottle *bottle, int own_list)
{
	if (bottle)
	{
		if (own_list == 1)
		{
			mDropDriftBottleList.insert(std::pair<BottleType, DriftBottle*>(bottle_type, bottle));
			
		}
		else if (own_list == 2)
		{
			mPickDriftBottleList.insert(std::pair<BottleType, DriftBottle*>(bottle_type, bottle));
		}
		else
		{

		}
		bottleListUpdate(bottle,own_list);
		return true;
	}
	else
	{
		//Êä³öÊ§°Ü´íÎóÐÅÏ¢
		return false;
	}
}
//---------------------------------------------------------
bool DriftBottleMgr::deleteDriftBottle(DriftBottle *bottle)
{
	return true;
}
//---------------------------------------------------------
bool DriftBottleMgr::deleteDriftBottle(uint64 id, int own_list)
{
	DriftBottleList* match_list = NULL;
	if (own_list == 1)
	{
		match_list = &mDropDriftBottleList;
	}
	else if (own_list == 2)
	{
		match_list = &mPickDriftBottleList;
	}
	BottleIterType begin_iter = match_list->begin();
	for ( ; begin_iter != match_list->end(); ++begin_iter)
	{
		if (begin_iter->second->getBottleInfo()->mBottleID == id)
		{
			match_list->erase(begin_iter);
		}
	}
// 	BottleIterType _iter = match_list->begin();
// 	for ( ; _iter != match_list->end(); ++_iter)
// 	{
// 		bottleListUpdate(_iter->second, own_list);
// 	}
	luabind::call_function<void>(sLuaMgr.getL(), "DBS_update",own_list);
	return true;
}
//---------------------------------------------------------
bool DriftBottleMgr::clearDropBottlesList()
{
	mDropDriftBottleList.clear();
	return true;
}
//---------------------------------------------------------
bool DriftBottleMgr::isBottleExist(DriftBottle* bottle)
{
// 	BottleTypeRangePair range_iter = mDropDriftBottleList.equal_range(bottle->mType);
// 	for (Const_BottleIterType c_iter = range_iter.first; c_iter != range_iter.second; ++c_iter)
// 	{
// 		if (c_iter->second->mDriftBottle_info.mAccount)
// 		{
// 		}
// 	}
	return true;
}
//---------------------------------------------------------
bool DriftBottleMgr::isBottleExist(bottle_info& bt_info, int own_list)
{
	DriftBottleList* match_list = NULL;
	if (own_list == 1)
	{
		match_list = &mDropDriftBottleList;
	}
	else if (own_list == 2)
	{
		match_list = &mPickDriftBottleList;
	}
	BottleTypeRangePair range_iter = match_list->equal_range(bt_info.type);
	for (Const_BottleIterType c_iter = range_iter.first; c_iter != range_iter.second; ++c_iter)
	{
		if (c_iter->second->getBottleInfo()->mBottleID == bt_info.id)
		{
			return true;
		}
		else
		{
			continue;
		}
	}
	return false;
}
//-------------------------------------------------------
void DriftBottleMgr::bottleListUpdate(DriftBottle* bottle,int own_list)
{
// 	for (	Const_BottleIterType c_iter = mDropDriftBottleList.begin();
// 			c_iter != mDropDriftBottleList.end(); 
// 			++c_iter)
// 	{
// 
// 	}
	struct tm createBt_time;
	CommonHelper::translate(&createBt_time,&bottle->getBottleInfo()->mTime);
	uint64 elapsedsecs = mktime(&createBt_time);
	std::string _buf = bottle->getBottleInfo()->mContent;
	std::string output = sStringHandle.stringFilter(const_cast<char *>(_buf.c_str()),0);
	std::string elapsedsecs_str = transformInsIdToStr(elapsedsecs);
	std::string bottle_idStr = transformInsIdToStr(bottle->getBottleInfo()->mBottleID);
	std::string bottle_createTime = FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx",	bottle->getBottleInfo()->mTime.year,
																				bottle->getBottleInfo()->mTime.month,
																				bottle->getBottleInfo()->mTime.day,
																				bottle->getBottleInfo()->mTime.hour,
																				bottle->getBottleInfo()->mTime.minute,
																				bottle->getBottleInfo()->mTime.second,
																				0);
	luabind::call_function<void>(sLuaMgr.getL(), "DBS_updateBottleList",own_list,
																		bottle_idStr,
																		bottle->getBottleInfo()->mAccount,
																		output,
																		bottle->getBottleInfo()->mType,
																		bottle_createTime,
																		elapsedsecs_str
																		);
}
//---------------------------------------------------------------------
std::string DriftBottleMgr::transformInsIdToStr(uint64 insID)
{
	char temp[20] = {0};
	_ui64toa( insID, temp, 10);
	std::string insIdStr(temp);
	return insIdStr;
}
//---------------------------------------------------------------------
uint64 DriftBottleMgr::transformInsIdToUi64(std::string insId_str)
{
	uint64 instanceId = _atoi64(insId_str.c_str());
	return instanceId;
}