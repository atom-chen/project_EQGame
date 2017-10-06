#include "EQFarmPCH.h"
#include "CEGUI.h"
#include "FunnyFarm.h"
#include "SceneManager.h"
#include "GraphicsTable.h"
#include "MessageManager.h"
#include "GameEventMgr.h"
#include "FarmScene.h"
#include "MainPlayer.h"
#include "luabind/luabind.hpp"
#include "LuaManager.h"
#include "Timekeeper.h"
#include "EnumDef.h"
#include "ObjMgr.h"
#include "FarmTable.h"
#include "ItemTable.h"
#include "EQGameSceneMgr.h"
#include "CEGUIEx/CEGUIAnimate.h"
#include "CEGUIEx/CEGUIAnimateBox.h"
#include "DynamicUIMgr.h"
#include "CommonHelper.h"
#include "EQLog.h"
#include "Ogre.h"
#include "RecycleResource.h"

//------------------------------------------------------------------------------
void FunnyFarm::init()
{
	setAction(CS_FARM_NORMAL);
	//初始化农场界面
	_initFarmUI();
	_addMouseAnimateWnd();
	//mFarmScene = EQSceneManager::getSingleton().loadFarmScene();
	mCropsStartPos = 0;
}
//------------------------------------------------------------------------------
void FunnyFarm::_initFarmUI()
{
	CEGUI::Window* farmGUIRoot = CEGUI::WindowManager::getSingleton().getWindow("UIRoot");
	CEGUI::System::getSingleton().setGUISheet(farmGUIRoot);
	farmGUIRoot->addChildWindow("FarmmainUI");
	CEGUI::System::getSingleton().setDefaultMouseCursor("common1","FarmMouse01");
}
//------------------------------------------------------------------------------
void FunnyFarm::update()
{	
	Fupdate();
	DWORD now = GetTickCount();
	static DWORD lastTime = 0;
	static DWORD lastPop = 0;
	DWORD elapsed = 0;
	if (sObjMgr.isMainPlayer(mAccount))
	{
		elapsed = 550;
	}
	else
	{
		elapsed = 550;
	}

	if (now - lastPop > elapsed)
	{
		if (popNomoneyPickInfo())
			lastPop = now;
	}
	if (now - lastTime > elapsed)
	{
		if(popAllPickInfo())
			lastTime = now;
		else
			if (mIsOneKeyHavSta)
			{
				GameEventMgr::getSingleton().fireEvent("END_ALLPICK");
				mIsOneKeyHavSta = false;
			}
	}
	
	_updateCrops();
}
void FunnyFarm::Fupdate()
{	
	//农场泡泡动画缓冲
	DWORD now = GetTickCount();
	static DWORD lastFarm = 0;
	DWORD ela = 0;

	if (sObjMgr.isMainPlayer(mAccount))
	{
		ela = 250;
	}
	else
	{
		ela = 500;
	}
	if(now - lastFarm > ela)
	{
		if(DynamicUIMgr::getSingleton()._popFarmPaoPao())
			lastFarm = now;
	}

}
//------------------------------------------------------------------------------
void FunnyFarm::_updateCrops()
{
	if (mCropsStartPos > 0)
	{
		CropsCtrlListIter iter = mCropsCtrl.begin();
		for (;iter!=mCropsCtrl.end();iter++)
		{
			(*iter).update();
		}
	}
}
//
void FunnyFarm::removePest(int index)
{
	std::string win = "DynamicUI_PestHeadup_";
	std::string win1 = win + Ogre::StringConverter::toString(index).c_str();
	if (CEGUI::WindowManager::getSingleton().isWindowPresent(win1))
	{
		DynamicUIMgr::getSingleton().destroyWindow(win1);
	}
}
//------------------------------------------------------------------------------
void FunnyFarm::removePickUI(int index)
{
	std::string win = "DynamicUI_PickHeadup_";
	std::string win1 = win + Ogre::StringConverter::toString(index).c_str();
	if (CEGUI::WindowManager::getSingleton().isWindowPresent(win1))
	{
		DynamicUIMgr::getSingleton().destroyWindow(win1);
	}
	sFarmObjEvent.setIsPickUI(false);
}
//----------------------------------------------------------------------------
void FunnyFarm::removeFarmPaoPao(int index)
{
	std::string win = "DynamicUI_Farm_";
	std::string win1 = win + Ogre::StringConverter::toString(index).c_str();
	if (CEGUI::WindowManager::getSingleton().isWindowPresent(win1))
	{
		DynamicUIMgr::getSingleton().destroyWindow(win1);
	}
}
//------------------------------------------------------------------------------
void FunnyFarm::removeHavestUI(int index)
{
	std::string w = "DynamicUI_FarmHarvest_";
	std::string w1 = w + Ogre::StringConverter::toString(index).c_str();
	if (CEGUI::WindowManager::getSingleton().isWindowPresent(w1))
	{
		DynamicUIMgr::getSingleton().destroyWindow(w1);
	}
}
//------------------------------------------------------------------------------
void FunnyFarm::removeDexpUI(int index)
{
	std::string win = "DynamicUI_FarmDexp_";
	std::string win1 = win + Ogre::StringConverter::toString(index).c_str();
	if (CEGUI::WindowManager::getSingleton().isWindowPresent(win1))
	{
		DynamicUIMgr::getSingleton().destroyWindow(win1);
	}
}
//------------------------------------------------------------------------------
void FunnyFarm::clearLand(FarmScene* farmScene,int index)
{
	farmScene->changeLand(index,LS_WASTE);

	{
		farmScene->removePlant(index);
		farmScene->removeGrass(index);
		//临时虫灾
		removePest(index);
		removePickUI(index);
		if (!sObjMgr.isMainPlayer(mAccount))
		{
 			removeFarmPaoPao(index);
			removeHavestUI(index);
	 		removeDexpUI(index);
		}
	}

	farmScene->removeSignExpand(index);
}
//------------------------------------------------------------------------------
void FunnyFarm::_startupPlantUnits()
{
	mCropsStartPos = GetTickCount();
	CropsCtrlListIter iter = mCropsCtrl.begin();
	int idx = 0;

	for(;idx < mFarmScene->getLandSize();idx++)
	{
		clearLand(mFarmScene,idx);
	}

	idx = 0;iter = mCropsCtrl.begin();
	for (;iter!=mCropsCtrl.end() && idx < mCropNum;iter++,idx++)
	{
		(*iter).startup(mFarmScene);
		//mFarmScene->removeSignExpand(idx);
	}
	//扩展标识
	int signIdx = idx;
	if(signIdx < mFarmScene->getLandSize()
		&& (sObjMgr.isMainPlayer(mAccount)))
	{
		mFarmScene->signExpand(signIdx);
	}
}
//------------------------------------------------------------------------------
void FunnyFarm::notiFarmData()
{
	bool hasDrought,
		 hasPest,
		 hasGrass,
		 //hasHavest,
		 hasGain;

	if (sStateMgr.getState() != GS_FunnyFarm)
	{
		sEQGameSceneMgr.changeScene(EQGameSceneMgr::SCENE_FARM,10000,10000);
		sEventMgr.fireEvent("FIRST_ENTER_FARM");
		mFarmScene = EQSceneManager::getSingleton().getFarmScene();
	}
	notify_farm_data data;
	data.decode(sMsgMgr.getRecebuff());

	//农场基本数据
	mAccount			= data.data.account;
	mLev				= data.data.level;
	//mPet				= data.data.pet;
	mExp				= data.data.exp;
	mCommPetFoodPeriod = data.data.pet_food.period_date;
	//农作物数据更新
	mCropNum		= data.data.crops.size();
	mCropsCtrl.clear();
	mCropsCtrl.resize(mFarmScene->getLandSize());
	for (int i=0; i<mCropNum; i++)
	{
		mCropsCtrl[i].crop = data.data.crops[i];
		mCropsCtrl[i].index= i;
		mCropsCtrl[i].curStage = 1;
	}
	//
	if (0 == mpushFlag)
	{
		bool hasFlag = hasDamage(mCropsCtrl,data.data.crops,hasDrought,hasPest,hasGrass,hasGain);
	}

	if(sObjMgr.isMainPlayer(data.data.account))
	{
		mSelfFarmInfo.mSaccount = data.data.account;
		mSelfFarmInfo.mSusername = data.data.account;//sMainPlayer.getName();
		mSelfFarmInfo.mSlev	= data.data.level;
		mSelfFarmInfo.mSexp = data.data.exp;
		//mSelfFarmInfo.mSpet = data.data.pet;
		mSelfFarmInfo.mSpetFood = data.data.pet_food;
		mSelfFarmInfo.mSdeco = data.data.decoration;
		saveCurDec_tolua(data.data.decoration);
		setSelfFarmState(mCropsCtrl,data.data.crops);
	}
	if(mLastAcc != mAccount)
	{
		while (!mNomoneyPickData.empty())
		{
			mNomoneyPickData.pop();
		}
	}
	for (int i=0; i<mCropNum; i++)
	{
		if (data.data.crops[i].pick_num != 0)
		{
			setHarvestDymanicUI(i,data.data.crops[i].tempid,data.data.crops[i].pick_num);
		}
//		else
//		{
//			if (mClickHavest && getAction() == CS_FARM_HARVEST)
//			{
//				if(data.data.crops[i].picked == 1)
//				{
//					setHintDymanicUI(i, 0,-1);
//				}
//				mClickHavest = false;
//			}
//		}
	}
	//启动农作物
	_startupPlantUnits();
	//装饰物
	mDecoration.setFarmScene(mFarmScene);
	mDecoration.setServerData(&data.data.decoration);
	mDecoration.updateObjects();
	mLastAcc = mAccount;
	sEventMgr.fireEvent("UPDATE_UI_FARM_DATA");

	/// 回收资源
	RecycleResource();
}
//
bool FunnyFarm::isPlot(int index)
{
	return (index < mCropNum + 1);
}
//
bool FunnyFarm::hasCrop(int index)
{
	return ((index < mCropNum + 1)
		&& (mCropsCtrl[index-1].crop.tempid > 0 ));
}
//
bool FunnyFarm::canPick(int index)
{
	bool ret = false;

	if (index < (int)mCropsCtrl.size() + 1)
		ret = mCropsCtrl[index-1].curStage >= CS_PICK;

	return ret;
}
//------------------------------------------------------------------------------
void FunnyFarm::assartPlot(int index)
{
	if(index == mCropNum + 1)
	{
		if(sObjMgr.isMainPlayer(mAccount))
		{
			this->reqAssartPlot();
		}
	}
}
//------------------------------------------------------------------------------
void FunnyFarm::onClickedUnit(FarmPlantUnit::PickObjectUnit* objunit)
{
	uint index = objunit->getCreator()->getIndex() + 1;
	setPushFlag(1);
	assartPlot(index);

	switch (mAction)
	{
	case CS_FARM_PUTBUG:
		if(!mIsOneKeyHavSta)
			if (hasCrop(index))
				reqPutPest(mAccount,index);
		break;
	case CS_FARM_PUTGRASS:
		if(!mIsOneKeyHavSta)
			if (hasCrop(index))
				reqPutGrass(mAccount,index);
		break;
	case CS_FARM_SEEDING:
		if(!mIsOneKeyHavSta)
			if (isPlot(index))
			{
				_reqSowAction(mAccount,index);
				m_curOperateType = CS_FARM_SEEDING;
			}
		break;
	case CS_FARM_MOVE:
		if(!mIsOneKeyHavSta)
		break;
	case CS_FARM_TILL:
		{
			if(!mIsOneKeyHavSta)
				if (isPlot(index))
				{
					if (hasCrop(index))
					{
						if ( mCropsCtrl[index-1].curStage == fcs_die)
						{
							reqHoeing(index);
							m_curOperateType = CS_FARM_TILL;
						}
						else//对未枯死的作物铲除发出消息提示
						{
							luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTip_popUpRootUI");
						}
					}
					else//对空地铲除发出消息提示
					{
						luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTip_popEmptyLandUI");
					}

				}
		}
		break;
	case CS_FARM_WATER:
		if(!mIsOneKeyHavSta)
			if (isPlot(index))
				if (hasCrop(index))
				{
					reqWater(mAccount,index);
					m_curOperateType = CS_FARM_WATER;
				}
				else//对空地操作发出消息提示
				{
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTip_popEmptyLandUI");
				}
		break;
	case CS_FARM_WEED:
		if(!mIsOneKeyHavSta)
			if (isPlot(index))
				if (hasCrop(index) )
				{
					reqWeed(mAccount,index);
					m_curOperateType = CS_FARM_WEED;
				}
				else//对空地操作发出消息提示
				{
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTip_popEmptyLandUI");
				}
		break;
	case CS_FARM_PICKBUG:
		if(!mIsOneKeyHavSta)
			if (isPlot(index))
				if (hasCrop(index) )
				{
					reqWorn(mAccount,index);
					m_curOperateType = CS_FARM_PICKBUG;
				}
				else//对空地操作发出消息提示
				{
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTip_popEmptyLandUI");
				}
		break;
	case CS_FARM_HARVEST:
		{
			if(!mIsOneKeyHavSta)
				if (isPlot(index))
				{
					if(canPick(index))
					{
						reqPickCrop(mAccount,index);
						m_curOperateType = CS_FARM_HARVEST;
	//					mClickHavest = true;
						//setHarvestDymanicUI(index);
					}
					else//对空地操作发出消息提示
					{
	//					mClickHavest = false;
						luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTip_popEmptyLandUI");
					}
				}
		}
		break;
	case CS_FARM_ONEKEYHARVEST:
		{
			if(!mIsOneKeyHavSta)
				reqPickAllCrops(mAccount);
			//if(canPick(index))
// 				for(uint i=0; i<mCropsCtrl.size();i++)
// 				{
// 					if(canPick(i+1))
// 					{
// 						reqPickCrop(mAccount,i+1);
// 						//setHarvestDymanicUI(i+1);
// 					}
// 				}
		}
		break;
	case CS_FARM_USE_PROP:
		{
			if(!mIsOneKeyHavSta)
				if (isPlot(index))
					_reqUseProp(index);
		}
		break;
	default:
		return;
	}
}
//------------------------------------------------------------------------------
void FunnyFarm::cleanup()
{
	for (int i=0; i<mCropNum; i++)
	{
		removePest(i);
		removePickUI(i);
	}
	mMouseAnimateWnd->setVisible(false);

	CEGUI::System::getSingleton().setDefaultMouseCursor("MouseArrow", "MouseArrow");

	EQSceneManager::getSingleton().destroyFarmScene();
}
//------------------------------------------------------------------------------
void FunnyFarm::setFarmCurState(int sta, std::string imageset /* =  */, std::string icon /* = */)
{
	setAction(sta);
	std::string set =  "common1",
		image = "FarmMouse01";
	switch (sta)
	{
	case CS_FARM_NORMAL:
		hideMouseAnimateWnd();
		break;
	case CS_FARM_MOVE:
		hideMouseAnimateWnd();
		break;
	case CS_FARM_TILL:
		getMouseAniInfoFromXml(MA_TILL,46,73);//(MA_TILL,40,80);
		break;
	case CS_FARM_WATER:
		getMouseAniInfoFromXml(MA_WATER);
		break;
	case CS_FARM_WEED:
		getMouseAniInfoFromXml(MA_WEED);
		break;
	case CS_FARM_PICKBUG:
		getMouseAniInfoFromXml(MA_PICKBUG);
		break;
	case CS_FARM_HARVEST:
		if(sObjMgr.isMainPlayer(mAccount))
		{
			getMouseAniInfoFromXml(MA_HARVEST);
		}
		else
		{
			getMouseAniInfoFromXml(MA_STEAL);
		}
		break;
	case CS_FARM_ONEKEYHARVEST:
		getMouseAniInfoFromXml(MA_ONEKEYHARVEST);
		break;
	case CS_FARM_SEEDING:
		if (!mSeedingImgset.empty() && !mSeedingIcon.empty())
		{
// 			CEGUI::System::getSingleton().setDefaultMouseCursor((CEGUI::utf8*)mSeedingImgset.c_str(), 
// 																(CEGUI::utf8*)mSeedingIcon.c_str());
			_addAnimateImg(1,0,mSeedingImgset,mSeedingIcon);
		}
		break;
	case CS_FARM_PUTBUG:
		getMouseAniInfoFromXml(MA_PUTBUG,25,65);
		break;
	case CS_FARM_PUTGRASS:
		getMouseAniInfoFromXml(MA_PUTGRASS,30,60);
		break;
	case CS_FARM_STEAL:
		break;
	case CS_FARM_USE_PROP:
		{
			getMouseAniInfoFromXml(MA_USE_PROP,30,60);
		}
		break;
	default:
		return;
	}
	CEGUI::System::getSingleton().setDefaultMouseCursor(set, image);
}

//------------------------------------------------------------------
void FunnyFarm::setSeedingCur(int sta,std::string imageset, std::string icon)
{
	mSeedingImgset = imageset;
	mSeedingIcon = icon;
	setFarmCurState(sta,imageset,icon);
}
//------------------------------------------------------------------
void FunnyFarm:: _addMouseAnimateWnd()
{
	CEGUI::WindowManager& mgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* root = CEGUI::System::getSingleton().getGUISheet();
	LOGERR(root->getName().c_str());
	//动画控件容器窗口
	if (!mMouseAnimateWnd)
	{
		mMouseAnimateWnd = mgr.createWindow("EQHomeLook/StaticImage","MouseAnimateWnd");
	}
	mMouseAnimateWnd->setVisible(false);
	mMouseAnimateWnd->setMousePassThroughEnabled(true);
	mMouseAnimateWnd->setAlwaysOnTop(true);
	mMouseAnimateWnd->setProperty("FrameEnabled", "false");
	mMouseAnimateWnd->setProperty("BackgroundEnabled", "false");
	root->addChildWindow(mMouseAnimateWnd);
	//动画
	if (!mAniWnd)
	{
		mAniWnd = static_cast<CEGUI::AnimateBox*>(mgr.createWindow("EQHomeLook/AnimateBox"));
		mAniWnd->setMousePassThroughEnabled(true);
	}
	//添加动画控件
	mMouseAnimateWnd->addChildWindow(mAniWnd);
}
//------------------------------------------------------------------
void FunnyFarm::_updatePos(POINT posabs)
{
	if (mMouseAnimateWnd->isVisible())
	{
		mMouseAnimateWnd->setPosition(CEGUI::UVector2(
			CEGUI::UDim(0.f,(float)posabs.x),CEGUI::UDim(0.f,(float)posabs.y - 50)));
	}
}
//------------------------------------------------------------------
void FunnyFarm::hideMouseAnimateWnd()
{
	mMouseAnimateWnd->setVisible(false);
}
//------------------------------------------------------------------
void FunnyFarm::_addAnimateImg(int num,int time, std::string imgset,std::string imgName,int dx,int dy)
{
	CEGUI::Imageset* set = CEGUI::ImagesetManager::getSingleton().getImageset(imgset);
	float imgW,
		  imgH;
	if (time != 0)
	{	
		CEGUI::Image img = set->getImage(imgName + CEGUI::PropertyHelper::intToString(0).c_str()
												 + CEGUI::PropertyHelper::intToString(1).c_str());
		imgW = img.getWidth();
		imgH = img.getHeight();
		mMouseAnimate.initAnimate(time,imgW,imgH);
		for (int i = 1;i <= num;i++)
		{
			if (i >= 10)
			{
				mMouseAnimate.addAnimateFrame(imgset,imgName + CEGUI::PropertyHelper::intToString(i).c_str());
			}
			else
			{
				mMouseAnimate.addAnimateFrame(imgset,imgName 
												+  CEGUI::PropertyHelper::intToString(0).c_str() 
												+ CEGUI::PropertyHelper::intToString(i).c_str());
			}
		}
		mAniWnd->addAnimate(mMouseAnimate);
		mAniWnd->setSizeFunc(imgW, imgH);
		mMouseAnimateWnd->setSize(CEGUI::UVector2(
			CEGUI::UDim(0.f,imgW),CEGUI::UDim(0.f,imgH)));
		CEGUI::Point mp = CEGUI::MouseCursor::getSingleton().getPosition();
		mMouseAnimateWnd->setPosition(CEGUI::UVector2(
			CEGUI::UDim(0.f,mp.d_x - dx),CEGUI::UDim(0.f,mp.d_y - dy)));
		//mMouseAnimateWnd->removeProperty("Image");
	}
	else
	{	
		CEGUI::Image img = set->getImage(imgName);
		imgW = img.getWidth();
		imgH = img.getHeight();
		mMouseAnimate.initAnimate(1,imgW,imgH);
		mMouseAnimate.addAnimateFrame(imgset,imgName);
		mAniWnd->addAnimate(mMouseAnimate);
		mMouseAnimateWnd->setSize(CEGUI::UVector2(
			CEGUI::UDim(0.f,imgW),CEGUI::UDim(0.f,imgH)));
		CEGUI::Point mp = CEGUI::MouseCursor::getSingleton().getPosition();
		mMouseAnimateWnd->setPosition(CEGUI::UVector2(
			CEGUI::UDim(0.f,mp.d_x),CEGUI::UDim(0.f,mp.d_y - imgH)));
	}
	if (!sEQGameSceneMgr.getIsLoading())
	{
		mMouseAnimateWnd->setVisible(true);
		mAniWnd->setVisible(true);
	}
}
//------------------------------------------------------------------
void FunnyFarm::playMouseAnimate()
{
	mAniWnd->play();
}
//------------------------------------------------------------------
void FunnyFarm::getMouseAniInfoFromXml(int id,int dx,int dy)
{
	const MouseAnimateRow* mar = sFarmTableMgr.getMouseAnimateRow(id);
	_addAnimateImg(mar->frames,mar->time,mar->imgset,mar->icon,dx,dy);
}
//
bool FunnyFarm::hasDamage(CropsCtrlList& cropCtrl,std::vector<farm_crop_data>& data,bool& hasDrought,bool& hasPest,bool& hasGrass,bool& hasGain)
{
	for (int i=0; i<mCropNum; i++)
	{
		if (data[i].stage == fcs_picking && data[i].picked == 0)
		{
			setFarmCurState(CS_FARM_HARVEST);
			return true;
		}
	}
	for (int i=0; i<mCropNum; i++)
	{
		if (0 != cropCtrl[i].hasState(fms_pest))
		{
			setFarmCurState(CS_FARM_PICKBUG);
			return true;
		}
	}
	for (int i=0; i<mCropNum; i++)
	{
		if (0 != cropCtrl[i].hasState(fms_grass))
		{
			setFarmCurState(CS_FARM_WEED);
			return true;
		}
	}
	for (int i=0; i<mCropNum; i++)
	{
		if(0 != cropCtrl[i].hasState(fms_drought))
		{
			setFarmCurState(CS_FARM_WATER);
			return true;
		}
	}
	setFarmCurState(CS_FARM_NORMAL);
	return false;
}
//
bool FunnyFarm::setCurStaCur(bool hasPest, bool hasGrass,bool hasDrought)
{
	if (hasPest)
	{
		setFarmCurState(CS_FARM_PICKBUG);
	}
	if (hasGrass)
	{
		setFarmCurState(CS_FARM_WEED);
	}
	if (hasDrought)
	{
		setFarmCurState(CS_FARM_WATER);
	}
	return true;
}
//
void FunnyFarm::setPushFlag(int fg)
{
	mpushFlag = fg;
}
//
void FunnyFarm::setHarvestDymanicUI(int index,int templtID,int cropsnum)
{
	Ogre::Vector3 pos = mFarmScene->getLandUintPos(index);
	int num = cropsnum;//mCropsCtrl[index].getPickedNum();//getRemain();
	int id = templtID;//mCropsCtrl[index].getCropTpltID();
	const ItemRow *  itemRow =  ItemTableManager::getSingleton().getItemRow(id);
	std::string imgset = itemRow->imageset;
	std::string icon = itemRow->icon;
	luabind::call_function<void>(sLuaMgr.getL(), "FarmTip_setCanPickLandPos",pos.x,pos.y,pos.z);
// 	if (mCropsCtrl[index].curStage == fcs_picking && sObjMgr.isMainPlayer(mAccount))
// 	{
		DynamicUIMgr::getSingleton().createFarmHarvest(index,pos,EFPPT_DEFINE,num,imgset.c_str(),icon.c_str());
// 	}

}
void FunnyFarm::setHintDymanicUI(int index,int imageID,int lose_gold)
{
	Ogre::Vector3 pos = mFarmScene->getLandUintPos(index);
	int num = -lose_gold;
	int id = 0;
	std::string imgset = "Button3";
	std::string icon = "ImageButton35_Normal";
	// 	if (mCropsCtrl[index].curStage == fcs_picking && sObjMgr.isMainPlayer(mAccount))
	// 	{
	DynamicUIMgr::getSingleton().createFarmHarvest(index,pos,EFPPT_DEFINE,num,imgset.c_str(),icon.c_str());
	// 	}
}
//
bool FunnyFarm::setSelfFarmState(CropsCtrlList& cropCtrl,std::vector<farm_crop_data>& data)
{
	for (int i=0; i<mCropNum; i++)
	{
		if (data[i].stage == fcs_picking)
		{
			mSelfFarmInfo.mfarmState = fms_gain;
			return true;
		}
	}
	for (int i=0; i<mCropNum; i++)
	{
		if (0 != cropCtrl[i].hasState(fms_drought))
		{
			mSelfFarmInfo.mfarmState = fms_drought;
			return true;
		}
	}
	for (int i=0; i<mCropNum; i++)
	{
		if (0 != cropCtrl[i].hasState(fms_pest))
		{
			mSelfFarmInfo.mfarmState = fms_pest;
			return true;
		}
	}
	for (int i=0; i<mCropNum; i++)
	{
		if (0 != cropCtrl[i].hasState(fms_grass))
		{
			mSelfFarmInfo.mfarmState = fms_grass;
			return true;
		}
	}
	mSelfFarmInfo.mfarmState = fms_normal;
	return false;
}
//----------------------------------------------------------
void FunnyFarm::saveCurDec_tolua(farm_decoration dec)
{
	FarmDecID struct_DecID;
	std::vector<FarmDecID> tempDecID_vec;

	uint64 ins_bg_id = dec.background.instance_id;
	uint64 ins_bb_id = dec.billboard.instance_id;
	uint64 ins_bbc_id = dec.billboard_content.instance_id;
	uint64 ins_dog_id = dec.dog.instance_id;
	uint64 ins_doghouse_id = dec.doghouse.instance_id;
	uint64 ins_fence_id = dec.fence.instance_id;
	uint64 ins_house_id = dec.house.instance_id;
	uint64 ins_sc_id = dec.scarecrow.instance_id;
	uint64 ins_sp_id = dec.signpost.instance_id;

	char buffer[20];
	//1
	_ui64toa(ins_bg_id,buffer,10);
	std::string ins_bg_id_str(buffer);
	struct_DecID.ins_id = ins_bg_id_str;
	struct_DecID.tplt_id = dec.background.template_id;
	tempDecID_vec.push_back(struct_DecID);
	//2
	_ui64toa(ins_house_id,buffer,10);
	std::string ins_house_id_str(buffer);
	struct_DecID.ins_id = ins_house_id_str;
	struct_DecID.tplt_id = dec.house.template_id;
	tempDecID_vec.push_back(struct_DecID);
	//3
	_ui64toa(ins_doghouse_id,buffer,10);
	std::string ins_doghouse_id_str(buffer);
	struct_DecID.ins_id = ins_doghouse_id_str;
	struct_DecID.tplt_id = dec.doghouse.template_id;
	tempDecID_vec.push_back(struct_DecID);
	//4
	_ui64toa(ins_dog_id,buffer,10);
	std::string ins_dog_id_str(buffer);
	struct_DecID.ins_id = ins_dog_id_str;
	struct_DecID.tplt_id = dec.dog.template_id;
	tempDecID_vec.push_back(struct_DecID);
	//5
	_ui64toa(ins_sp_id,buffer,10);
	std::string ins_sp_id_str(buffer);
	struct_DecID.ins_id = ins_sp_id_str;
	struct_DecID.tplt_id = dec.signpost.template_id;
	tempDecID_vec.push_back(struct_DecID);
	//6
	_ui64toa(ins_fence_id,buffer,10);
	std::string ins_fence_id_str(buffer);
	struct_DecID.ins_id = ins_fence_id_str;
	struct_DecID.tplt_id = dec.fence.template_id;
	tempDecID_vec.push_back(struct_DecID);
	//7
	_ui64toa(ins_sc_id,buffer,10);
	std::string ins_sc_id_str(buffer);
	struct_DecID.ins_id = ins_sc_id_str;
	struct_DecID.tplt_id = dec.scarecrow.template_id;
	tempDecID_vec.push_back(struct_DecID);
	//8
	_ui64toa(ins_bb_id,buffer,10);
	std::string ins_bb_id_str(buffer);
	struct_DecID.ins_id = ins_bb_id_str;
	struct_DecID.tplt_id = dec.billboard.template_id;
	tempDecID_vec.push_back(struct_DecID);
	//9
	_ui64toa(ins_bbc_id,buffer,10);
	std::string ins_bbc_id_str(buffer);
	struct_DecID.ins_id = ins_bbc_id_str;
	struct_DecID.tplt_id = dec.billboard_content.template_id;
	tempDecID_vec.push_back(struct_DecID);
	
	for (size_t i = 0; i < tempDecID_vec.size(); i++)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "Dec_saveCurDec", (i+1), tempDecID_vec[i].ins_id, tempDecID_vec[i].tplt_id);
	}
}
//------------------------------------------------------------
int FunnyFarm::getSelfDogFoodRemainSecs()
{
	stime snow = sTimekeeper.getTimeNow();
	SelfFarmInfo sFi = getSelfFarmInfo();
	stime period = sFi.mSpetFood.period_date;

	int elapsedSecs = CommonHelper::diffTime(&snow,&period);

	return elapsedSecs;
}
//-------------------------------------------------------------
int FunnyFarm::getCommDogFoodRemainSecs()
{
	stime snow = sTimekeeper.getTimeNow();
	stime period = mCommPetFoodPeriod;

	int elapsedSecs = CommonHelper::diffTime(&snow,&period);

	return elapsedSecs;

}
//-------------------------------------------------------------
void FunnyFarm::insertAccountList(std::string account)
{
	m_accountList.push_back(account);
}