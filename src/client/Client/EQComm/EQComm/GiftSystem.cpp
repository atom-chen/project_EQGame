#include "EQCommPCH.h"
#include "GiftSystem.h"
#include "ModelManagerGcs.h"
#include "CommFuncGcs.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "LogicTable.h"
#include "luabind/luabind/luabind.hpp"
#include "LuaManager.h"
#include "HouseScene.h"
#include "SceneManager.h"
#include "LogicTable.h"
#include "GraphicsTable.h"
#include "MainPlayer.h"
#include "GiftLogManager.h"
#include "CommFunc.h"
#include "FlashDlg.h"
#include "EQWndProc.h"
#include "StringHandle.h"
//------------------------------------------------
void GiftDate::set(uint year, uint month, uint day)
{
	// 判断有效性
	_year = year;
	_month = month;
	_day = day;
}
//------------------------------------------------
void GiftInfo::reset()
{
	*this = GiftInfo();

	//
	storageIndex = -1;
}

/**************************************************
					--Gift Class--
 *************************************************/

Gift::Gift(const GiftInfo &giftInfo)
:mInfo(giftInfo)
,mIndex(0)
,mModel(NULL)
{
	mPickObjectUnit.setCreator(this);
	mPickObjectProxy.setObject(&mPickObjectUnit);	
}
//------------------------------------------------
Gift::~Gift()
{
	// 此处无需调用ModelManagerGcs::getSingleton().destroyModel(mModel);
	// 移除mModel。 ModelManagerGcs自己会移除。如果此处调用ModelManagerGcs::getSingleton().destroyModel(mModel)
	// 而此时ModelManagerGcs已经析构。会导致异常
}
//------------------------------------------------
void Gift::reset()
{
	if( NULL != mModel )
	{
		ModelManagerGcs::getSingleton().destroyModel(mModel);
		mModel = NULL;
	}

	mInfo.reset();
}
//------------------------------------------------
void Gift::setModel(ModelGcs* model)
{
	if(NULL == model)
		return;

	mModel = model;
	// 设置选取代理
	mModel->setPickObjectProxy(&mPickObjectProxy);
}
//------------------------------------------------
const std::string& Gift::PickObjectUnit::getClassName()
{
	static std::string name = "Gift::PickObjectUnit";
	return name;
}
//------------------------------------------------
void Gift::setPosition()
{
	_setPosition(mInfo.floorNum, mInfo.gridX, mInfo.gridZ);
}
//------------------------------------------------
void Gift::_setPosition(const int floorNum, const int gridX, const int gridZ)
{
	HouseScene * hs = EQSceneManager::getSingleton().getHouseScene();

	if(NULL == hs)
		return;

	HouseSceneGrid * grid = hs->getGrid(floorNum, gridX, gridZ);

	if(NULL == grid)
		return;
	
	Ogre::Vector3 position = grid->getSceneNode()->getPosition();
	_setPosition(position);
}
//------------------------------------------------
void Gift::_setPosition(const Ogre::Vector3& position)
{
	if( NULL == mModel )
		return;

	mModel->getMainNode()->setPosition(position);
}
//-----------------------------------------------

/**************************************************
				  --GiftSystem Class--
 *************************************************/
template<> GiftSystem* Ogre::Singleton<GiftSystem>::ms_Singleton = NULL;

GiftSystem::GiftSystem()
:mAutoGiftIndex(0)
,mIsLoadBoxData(false)
,mIsLoadCardData(false)
,mCurPickGift(NULL)
,mDisplayFlashDlg(NULL)
,mFlashHwnd(NULL)
{
	// 预分配空间
	mBuffer.reserve(50);
}
//-----------------------------------------------
GiftSystem::~GiftSystem()
{
	GiftMap::iterator iter = mGiftMap.begin();

	while(mGiftMap.end() != iter)
	{
		delete iter->second;
		iter++;
	}

	mGiftMap.clear();

	// 缓存列表
	GiftBuffer::iterator iterBuffer = mBuffer.begin();

	while(mBuffer.end() != iterBuffer)
	{
		delete (*iterBuffer);
		iterBuffer++;
	}

	mBuffer.clear();

	// 清除礼物窗口
	if(NULL != mDisplayFlashDlg)
	{
		if(mDisplayFlashDlg->IsWindow())
			mDisplayFlashDlg->DestroyWindow();
	
		delete mDisplayFlashDlg;
	}

	DeleteObject(mFlashHwnd);
}
//-----------------------------------------------
Gift* GiftSystem::createGift(const GiftInfo &info)
{	
	Gift* giftPtr = _reqGiftBuffer();

	if(NULL == giftPtr)
	{
		giftPtr = new Gift(info);
	}
	else
	{
		giftPtr->setGiftInfo(info);
	}

	giftPtr->setIndex(mAutoGiftIndex);
	mGiftMap.insert(GiftMap::value_type(mAutoGiftIndex++, giftPtr));

	return giftPtr;
}
//-----------------------------------------------
void GiftSystem::deleteGift(const uint32 giftId)
{
	GiftMap::iterator iter = mGiftMap.find(giftId);

	if(mGiftMap.end() != iter)
	{	
		_recycleGift(iter->second);
		mGiftMap.erase(iter);
	}
	else
	{
		return;
	}
}
//-----------------------------------------------
void GiftSystem::deleteAllGift()
{
	GiftMap::iterator iter = mGiftMap.begin();

	while(mGiftMap.end() != iter)
	{
		_recycleGift(iter->second);
		iter = mGiftMap.erase(iter);
	}
}
//-----------------------------------------------
void GiftSystem::putGift( Gift* gift) const
{
	if(NULL == gift)
		return;

	uint boxId = gift->getGiftInfo().boxId;
	const GiftBoxRow* giftBoxRow = LogicTableManager::getSingleton().getGiftBoxRow(boxId);

	if(NULL == giftBoxRow)
		return;

	const ModelDisplayRow* displayRow = GraphicsTableManager::getSingleton().getModelDisplayRow(giftBoxRow->modelId);

	if(NULL == displayRow)
		return;

	SimpleCreateModelRule rule(displayRow->mesh, displayRow->mat, displayRow->particle);
	ModelGcs * model = ModelManagerGcs::getSingleton().createModel(&rule);

	assert(NULL != model);
	gift->setModel(model);

	gift->setPosition();
}
//-----------------------------------------------
void GiftSystem::clickGift(Gift* gift)
{
	if(NULL == gift)
		return;

	reqStartGiftService(PickUp);
	mCurPickGift = gift;
}
//-----------------------------------------------
void GiftSystem::cleanCurMakingGiftInfo()
{
	mCurMakingGiftInfo.reset();
}
//-----------------------------------------------
void GiftSystem::loadGiftBoxData()
{
	// 只加载一次
	if( false == mIsLoadBoxData )
	{
		const GiftBoxTable table = LogicTableManager::getSingleton().getGiftBoxTable();

		GiftBoxTable::const_iterator iter = table.begin();

		for(uint index = 0; iter != table.end(); iter++, index++)
		{
			//调用LUA函数
			luabind::call_function<void>(sLuaMgr.getL(), "LoadGiftBoxData", iter->second, index, table.size());
		}

		mIsLoadBoxData = true;
	}
}
//-----------------------------------------------
void GiftSystem::loadGiftCardData()
{
	if( false == mIsLoadCardData )
	{
		const GiftCardTable table = LogicTableManager::getSingleton().getGiftCardTable();

		GiftCardTable::const_iterator iter = table.begin();

		for(uint index = 0; iter != table.end(); iter++, index++)
		{
			//调用LUA函数
			luabind::call_function<void>(sLuaMgr.getL(), "LoadGiftCardData", iter->second, index, table.size());
		}

		mIsLoadCardData = true;
	}
}
//-----------------------------------------------
void GiftSystem::setDIYGiftInfo(std::string giftame, uint cardId, uint boxId, int storageGolbaIndex, std::string greetings, std::string giverName)
{
	mCurMakingGiftInfo.name = giftame;
	mCurMakingGiftInfo.boxId = boxId;
	mCurMakingGiftInfo.cardId = cardId;
	mCurMakingGiftInfo.storageIndex = storageGolbaIndex;
	mCurMakingGiftInfo.giverName = giverName;
	mCurMakingGiftInfo.greetings = sStringHandle.stringFilter(const_cast<char*>(greetings.c_str()),0);
}
//-----------------------------------------------
void GiftSystem::recGiftData(const std::vector<gift_box> &giftData)
{
	deleteAllGift();

	bool hasMyGift = false;
	std::vector<gift_box>::const_iterator iter = giftData.begin();
	for(; iter != giftData.end(); iter++)
	{
		GiftInfo info; 
		converDataToGiftInfo((*iter), info);

		if (iter->receiver == MainPlayer::getSingleton().getAccount())
			hasMyGift = true;

		Gift* gift = createGift(info);
		putGift(gift);
	}

	
	if (hasMyGift)
	{
		// 运行新手教学任务(第一次在家中收到礼物)
		MainPlayer::getSingleton().getTask().startNewerTeachTask(15002);		
	}
}
//-----------------------------------------------
void GiftSystem::startGiftService(const GiftServiceType type)
{
	switch(type)
	{
	case Pack:
		{
			luabind::call_function<void>(sLuaMgr.getL(), "OpenGiveGiftUI");
		}
		break;
		
	case PickUp:
		{
			_pickUpGift();
		}
		break;

	case Put:
		{
			luabind::call_function<void>(sLuaMgr.getL(), "OpenPutGiftUI");
		}
		break;

	case GetExpressGift:
		{
			reqReciveExpressGift();
		}
		break;

	case ReplyMailGift:
		{
			luabind::call_function<void>(sLuaMgr.getL(), "DoReplyMailGift");
		}
		break;

	default:return;
	}
	return;

}
//-----------------------------------------------
void GiftSystem::setGiftItem(int storageGolbaIndex)
{// 在脚本中已检查了参数的有效性
	const ItemGrid* itemGrid = MainPlayer::getSingleton().getItemGridByGlobaIdx(storageGolbaIndex,  ItemPack::PT_WAREHOUSE);

	if(NULL == itemGrid)
		return;

	mCurMakingGiftInfo.itemInstanceId = itemGrid->mItem->getInstanceID();
	mCurMakingGiftInfo.itemOverlay = itemGrid->mOverlay;
	mCurMakingGiftInfo.itemTemplateId = MainPlayer::getSingleton().getItemIDByGlobaIdx(storageGolbaIndex,  ItemPack::PT_WAREHOUSE);
}

//-----------------------------------------------
Gift* GiftSystem::_reqGiftBuffer()
{
	if(true == mBuffer.empty())
		return NULL;
	else
	{
		Gift* gift = mBuffer.back();
		mBuffer.pop_back();

		return gift;
	}
}
//-----------------------------------------------
void GiftSystem::_addGiftBuffer(Gift *gift)
{
	if(NULL != gift)
	{
		mBuffer.push_back(gift);
	}
	else
	{
		return;
	}
}
//-----------------------------------------------
bool GiftSystem::_canPutGift(const int floorNum, const int gridX, const int gridZ) const
{
	GiftMap::const_iterator iter = mGiftMap.begin();

	while(mGiftMap.end() != iter)
	{
		const GiftInfo giftInfo = iter->second->getGiftInfo();
		
		if(giftInfo.floorNum == floorNum && giftInfo.gridX == gridX && giftInfo.gridZ == gridZ)
			return false;
	
		iter++;
	}

	return true;
}
//-----------------------------------------------
void  GiftSystem::_recycleGift(Gift* gift)
{
	if(NULL == gift)
		return;
	
	gift->reset();
	_addGiftBuffer(gift);
}
//-----------------------------------------------
void GiftSystem::_pickUpGift() const
{
	std::string account = MainPlayer::getSingleton().getAccount();
	// 玩家自己放置的礼物
	if(account == mCurPickGift->getGiftInfo().giverAcc || account == mCurPickGift->getGiftInfo().recipientAcc)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "SetDIYGiftUI"
									,mCurPickGift->getGiftInfo().giverName
									,mCurPickGift->getGiftInfo().greetings
									,mCurPickGift->getGiftInfo().itemTemplateId
									,mCurPickGift->getGiftInfo().effecTime
									,mCurPickGift->getGiftInfo().itemOverlay
									,mCurPickGift->getGiftInfo().cardId
									,true);
		
	}
	else
	{
		reqEndGiftService();
		return;
	}
}
//-----------------------------------------------
void GiftSystem::setMailGiftInfo(std::string reciveAcc,
								 std::string greetings,
								 uint cardId, 
								 int  storageGolbaIndex,
								 uint year,
								 uint month,
								 uint day)
{
	mCurMakingGiftInfo.recipientAcc = reciveAcc;
	mCurMakingGiftInfo.greetings = sStringHandle.stringFilter(const_cast<char*>(greetings.c_str()),0);
	mCurMakingGiftInfo.cardId = cardId;
	mCurMakingGiftInfo.storageIndex = storageGolbaIndex;
	mCurMakingGiftInfo.sendTime.set(year, month, day);
}
//-----------------------------------------------
void GiftSystem::reqReciveExpressGift() const
{
	uint64 instanceId = GiftLogManager::getSingleton().getCurGiftInstanceId();

	req_receive_gift_express req;
	req.inst_id = instanceId;
	req.page_count = GiftLogManager::PerPageLogCount;
	req.page_index = luabind::call_function<int>(sLuaMgr.getL(), "GetGiftLogCurPage");
	sGameMgr.getEQMsgManager()->Send(req);
}
//-----------------------------------------------
void GiftSystem::playGiftCardFlash(const char* flashUrl)
{
	if (NULL == flashUrl)
		return;

	if(NULL == mDisplayFlashDlg || !mDisplayFlashDlg->IsWindow())
	{
		mDisplayFlashDlg = new FlashDlg();
	}

	if(!IsWindow(mFlashHwnd))
		mFlashHwnd = mDisplayFlashDlg->Create(EQWndProc::getSingleton().getHWnd());

	moveDisplayFlashWin();

	mDisplayFlashDlg->ShowWindow(true);
	mDisplayFlashDlg->PlayFlash(flashUrl);
}
//-----------------------------------------------
void GiftSystem::moveDisplayFlashWin()
{
	if(NULL == mDisplayFlashDlg)
		return;

	RECT parentWinRect, flashWinRect;
	GetWindowRect(EQWndProc::getSingleton().getHWnd(), &parentWinRect);
	mDisplayFlashDlg->GetClientRect(&flashWinRect);
	

	long x = (parentWinRect.left + (parentWinRect.right - parentWinRect.left - (flashWinRect.right - flashWinRect.left))/2);
	long y = (parentWinRect.top + (parentWinRect.bottom - parentWinRect.top - (flashWinRect.bottom - flashWinRect.top))/2);

	mDisplayFlashDlg->SetWindowPos(HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE);
}

bool GiftSystem::hasGiftInGrid( HouseSceneGrid *grid )
{
	return !_canPutGift(grid->getCreator()->getFloorNO(), grid->getGridX(), grid->getGridZ());
}