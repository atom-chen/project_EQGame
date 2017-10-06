#include "EQCommPCH.h"
#include "DriftBottleSystem.h"
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"
#include "UIAnimationMgr.h"
#include "Timekeeper.h"
#include "CommonHelper.h"
//------------------------------------------------------------
DriftBottleSystem::DriftBottleSystem() 
					: mDbMgr(new DriftBottleMgr())
					, mDbCMgr(new DriftBottleConnectMgr())
					,mIsdrop(0)
					,mIspick(0)
					,mDropCount(0)
					,mPickCount(0)
{
	
}
//------------------------------------------------------------
DriftBottleSystem::~DriftBottleSystem()
{
	if (mDbMgr)
	{
		delete mDbMgr;
	}
	if (mDbCMgr)
	{
		delete mDbCMgr;
	}
}
//------------------------------------------------------------
bool DriftBottleSystem::isDbMgrCreate()
{
	if (mDbMgr)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//-------------------------------------------------------------
bool DriftBottleSystem::isDbCMgrConnect()
{
	if (mDbCMgr)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------
void DriftBottleSystem::dropBottle(std::string content, int bottle_type, int req_sex, int acc_visible)
{
	if (mDbCMgr != NULL)
	{
		if (mDropCount != 0)
		{
			mIsdrop = 1;
			mDbCMgr->reqDropBottle(content,bottle_type,req_sex,acc_visible);
			luabind::call_function<void>(sLuaMgr.getL(), "DBS_playDropAnimation");
		}
		else
		{
			luabind::call_function<void>(sLuaMgr.getL(), "DBS_dropFail");
		}
	}
}
//--------------------------------------------------------------
void DriftBottleSystem::pickBottle()
{
	if (mDbCMgr != NULL)
	{
		if (mPickCount != 0)
		{
			mIspick = 1;
			mDbCMgr->reqPickBottle();
			luabind::call_function<void>(sLuaMgr.getL(), "DBS_playPickAnimation");
		}
		else
		{
			luabind::call_function<void>(sLuaMgr.getL(), "DBS_pickFail");
		}
	}
}
//--------------------------------------------------------------
void DriftBottleSystem::deleteBottle(uint del_id,int own_list)
{
	if (mDbMgr != NULL)
	{
		mDbMgr->deleteDriftBottle(del_id,own_list);
	}
	if (mDbCMgr != NULL)
	{
		mDbCMgr->reqDelBottleLog(del_id);
	}
}
//--------------------------------------------------------------
void DriftBottleSystem::viewBottle()
{
	if (mDbCMgr != NULL)
	{
		//mDbCMgr->reqDropBottle();
	}

}
//---------------------------------------------------------------
DriftBottleMgr* DriftBottleSystem::getDriftBottleMgrPtr()
{
	assert(mDbMgr);
	return mDbMgr;
}
//---------------------------------------------------------------
void DriftBottleSystem::setDropCount(int count)
{
	mDropCount = count;
	luabind::call_function<void>(sLuaMgr.getL(), "DBS_setDropCountUI", mDropCount);
}
//---------------------------------------------------------------
void DriftBottleSystem::setPickCount(int count)
{
	mPickCount = count;
	luabind::call_function<void>(sLuaMgr.getL(), "DBS_setPickCountUI", mPickCount);
}
//---------------------------------------------------------------
void DriftBottleSystem::update()
{
	if (mIsdrop == 1)
	{
		bool isDrop = UIAnimationMgr::getSingleton().isPlaying("reng");
		if (!isDrop)
		{
			luabind::call_function<void>(sLuaMgr.getL(), "DBS_DropAnimationEnd");
			mIsdrop = 0;
		}
		else
		{
// 			mIsdrop = 1;
		}
	}
	if (mIspick == 1)
	{
		bool isPick = UIAnimationMgr::getSingleton().isPlaying("lao");
		if (!isPick)
		{
			luabind::call_function<void>(sLuaMgr.getL(), "DBS_PickAnimationEnd");
			mIspick = 0;
		}
	}
}
//-----------------------------------------------------------------
std::string DriftBottleSystem::caculateTimeInterval(std::string sec_str)
{
	uint64 sec = mDbMgr->transformInsIdToUi64(sec_str);
	stime now = sTimekeeper.getTimeNow();
	struct tm tm_now;
	CommonHelper::translate(&tm_now,&now);
	uint64 now_sec = mktime(&tm_now);
	uint64 elapsedsecs = now_sec - sec;
	return mDbMgr->transformInsIdToStr(elapsedsecs);
}
//-----------------------------------------------------------------
void DriftBottleSystem::reqEqUserInfo(std::string req_acc)
{
	mDbCMgr->reqEqUserInfo(req_acc);
}