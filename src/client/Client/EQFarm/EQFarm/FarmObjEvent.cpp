#include "EQFarmPCH.h"
#include "FarmObjEvent.h"

#include "EnumDef.h"
#include "FarmTable.h"
#include "Timekeeper.h"
#include "GraphicsTable.h"
#include "FunnyFarm.h"
//#include "FarmScene.h"
#include "CommonHelper.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "EQLog.h"
#include "CommFunc.h"
#include "DynamicUIMgr.h"
#include "InputMgr.h"
#include "ObjMgr.h"

//#define CROP_DIED 0

void reset_farm_crop_data(farm_crop_data& data)
{
	data.tempid = 0;
	data.health = 0;
	//std::vector<int> curr_damage;
	{
		data.start_time.year = 0;
		data.start_time.month = 0;
		data.start_time.day = 0;
		data.start_time.hour = 0;
		data.start_time.minute = 0;
		data.start_time.second = 0;
	}
	data.pick_num = 0;
	data.remain = 0;
	data.output = 0;
	data.stage = 0;
	data.land_type = 0;
	data.picked = 0;
	//std::vector<int> stage_time;
	data.aberrance_fruit = 0;
}

CropsCtrl::CropsCtrl()
:curStage(0)
,remainsecs(0)
,index(0)
{
	reset_farm_crop_data(crop);
}
//------------------------------------------------------------------------------
void CropsCtrl::update()
{
	if (crop.tempid > 0)
	{
		int oldstage = curStage;

		_getCropStageWithRemain();

		if(oldstage != curStage)
			_cropModelDisplay();
	}
	//else
	//{
	//	mFarmScene->removePlant(index);
	//	mFarmScene->removeGrass(index);
	//}

}
//------------------------------------------------------------------------------
void CropsCtrl::startup(FarmScene* farmScene)
{
	//clearLand(farmScene);
	mFarmScene = farmScene;

	if ( NORMAl == crop.land_type )
		mFarmScene->changeLand(index,LS_NORMAL);
	else if( RED == crop.land_type )
		mFarmScene->changeLand(index,LS_RED);

	if (crop.tempid > 0)
	{
		_getCropStageWithRemain();
		_cropModelDisplay();
	}
	//else
	//{
	//	mFarmScene->removePlant(index);
	//	mFarmScene->removeGrass(index);
	//	//¡Ÿ ±≥Ê‘÷
	//	removePest(index);
	//}

	//mFarmScene->removeSignExpand(index);
}
//------------------------------------------------------------------------------
int CropsCtrl::_getCropStageWithRemain()
{
	if(crop.stage == fcs_die)
		curStage = fcs_die;

	if(curStage == fcs_die)
		return fcs_die;

	stime start = crop.start_time,
		now  = sTimekeeper.getTimeNow();

	int elapsedsecs;
	elapsedsecs = CommonHelper::diffTime(&start, &now);

	int &stage = curStage,
		&remain= remainsecs;

	for (; stage<fcs_die; stage++)
	{	
		//const SeedRow* sr = sFarmTableMgr.getSeedRow(crop.tempid);

		int stagetimer = 0;
		for (int i=0;i<stage;i++)
		{
			stagetimer = this->crop.stage_time[i];//sr->stageTimer[i];
		}

		remain =  stagetimer - elapsedsecs;
		if (stage >= fcs_picking)
		{
			//mFarmScene->removeGrass(index);
			//¡Ÿ ±≥Ê‘÷
			//sFunnyFarm.removePest(index);
			//mFarmScene->changeLand(index,LS_NORMAL);
		}
		if( remain > 0)
			return 1;
	}

	return fcs_die;
}
//------------------------------------------------------------------------------
void CropsCtrl::_cropModelDisplay()
{
	string	mesh,
		material;
	_getCropModelCfg(crop.tempid,curStage-1,mesh,material);
	//if(strcmp(mesh.c_str(),landMesh.c_str()) != 0 
	//	|| strcmp(material.c_str(),landMtrl.c_str()) != 0)
	{
		mFarmScene->seedPlant(index, mesh, material);
		landMesh = mesh;
		landMtrl = material;
	}
	
	mFarmScene->removeGrass(index);
	int count = hasState(fms_grass);
	LOGDBG(FormatStr("ctrl %d: hasState grass: %d.",index,count));
	for (int i=0; i<count && curStage < fcs_picking; i++)
	{
		mFarmScene->seedGrass(index);
	}

	if (hasState(fms_drought) != 0  && curStage < fcs_picking)
	{
		if( NORMAl == crop.land_type)
			mFarmScene->changeLand(index,LS_DROUGHT_NORMAL);
		else if( RED == crop.land_type )
			mFarmScene->changeLand(index,LS_DROUGHT_RED);
	}
	else
	{
		if( NORMAl == crop.land_type )
			mFarmScene->changeLand(index,LS_NORMAL);
		else if( RED == crop.land_type )
			mFarmScene->changeLand(index, LS_RED);
	}

	//¡Ÿ ±≥Ê‘÷
	if (hasState(fms_pest) != 0 && curStage < fcs_picking)
	{
		putPest(index,hasState(fms_pest));
	}
	else
	{
		sFunnyFarm.removePest(index);
	}
	//ø…’™–ßπ˚
	if(!sFarmObjEvent.getIsPickUI())
	{
		if (curStage == fcs_picking)
		{
			if (crop.picked == 0)
			{
				setPickUI(index);
			}
		}
		else
		{
			sFunnyFarm.removePickUI(index);
		}
	}
}
//------------------------------------------------------------------------------
void CropsCtrl::_getCropModelCfg(int tempid, int stage, string& mesh, string& material)
{
	const PlantDisplayRow* row = sFarmTableMgr.getPlantDisplayRow(tempid);
	const FarmDisplayRow* farmrow = sFarmTableMgr.getFarmDisplayRow(
		row->farmdisplayid[stage]);

	const ModelDisplayRow* modelrow = GraphicsTableManager::getSingleton().getModelDisplayRow(farmrow->modelDisplayid);

	if( modelrow == NULL )
	{
		LOGERR("Can't find 'ModelDisplayRow modelDisplayid' " + Ogre::StringConverter::toString(farmrow->modelDisplayid));
		return;
	}
	mesh		= modelrow->mesh;
	material	= modelrow->mat;
}


//////////////////////////////////////////////////////////////////////////
FarmObjEvent::FarmObjEvent(void)
	: mObjOnEvent(NULL)
	, mIsSetPickUI(false)
	, EQInputEventListener(epfCursor)
{
	sInputMgr.addListener(this);
}

FarmObjEvent::~FarmObjEvent(void)
{
}
//-----------------------------------------------------------------------
void FarmObjEvent::onMovein(FarmPlantUnit::PickObjectUnit* objunit,
							Ogre::ColourValue& highlight)
{
	if(GameStateMgr::getSingleton().getState() != GS_FunnyFarm)
		return;

	mObjOnEvent = objunit;
	ModelGcs* gcs = 0;

	gcs = mObjOnEvent->getCreator()->getLand();
	if (gcs != NULL)
		gcs->setHighLight(highlight);

	for(int i=0; i<GRASS_MAX; i++)
	{
		gcs = mObjOnEvent->getCreator()->getGrass(i);
		if (gcs != NULL)
			gcs->setHighLight(highlight);
	}
	gcs = mObjOnEvent->getCreator()->getPlant();
	if (gcs != NULL)
		gcs->setHighLight(highlight);

	mIndex = mObjOnEvent->getCreator()->getIndex();
	if(mIndex >= sFunnyFarm.mCropNum)
		return;
	const CropsCtrl& ctrl = sFunnyFarm.getCropCtrl(mIndex);
	const Ogre::Vector3& pos = mObjOnEvent->getCreator()->getPos();

	int x,y;
	if(!CommonHelper::Vector3ToScreenPoint(pos, x, y))
		return;

	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTip_setCoord", mIndex,x, y , pos.x, pos.y, pos.z);
	if (ctrl.hasCrop())
	{
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTip_showCropStaUI");
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTip_getCropStaParam",ctrl);

	}
	
}
//-----------------------------------------------------------------------
void FarmObjEvent::onMoveout(FarmPlantUnit::PickObjectUnit* objunit)
{
	if(GameStateMgr::getSingleton().getState() != GS_FunnyFarm)
		return;
	if (objunit == mObjOnEvent)
	{
		ModelGcs* gcs = 0;

		gcs = mObjOnEvent->getCreator()->getLand();
		if (gcs != NULL)
			gcs->clearHighLight();

		for(int i=0; i<GRASS_MAX; i++)
		{
			gcs = mObjOnEvent->getCreator()->getGrass(i);
			if (gcs != NULL)
				gcs->clearHighLight();
		}

		gcs = mObjOnEvent->getCreator()->getPlant();
		if (gcs != NULL)
			gcs->clearHighLight();

		mObjOnEvent = NULL;
	}
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FarmTip_removeCropSta");

}
//-----------------------------------------------------------------------
void FarmObjEvent::onClicked(FarmPlantUnit::PickObjectUnit* objunit)
{
	if(GameStateMgr::getSingleton().getState() != GS_FunnyFarm)
		return;
	mObjOnEvent = objunit;
	mIndex = mObjOnEvent->getCreator()->getIndex();
	luabind::call_function<void>(sLuaMgr.getL(), "FarmTip_set_ClickLand_Id",mIndex);
	sFunnyFarm.playMouseAnimate();
	sFunnyFarm.onClickedUnit(mObjOnEvent);
}
//-----------------------------------------------------------------------
void FarmObjEvent::onMoveinFarmObj(FarmObject *farmObj, Ogre::ColourValue &highlight)
{
	ModelGcs* mgcs = farmObj->getModel();
	std::string objName = farmObj->getName();
	if (objName == "doghouse")
	{
		mgcs->setHighLight(highlight);
	}
}
//-----------------------------------------------------------------------
void FarmObjEvent::onMoveoutFarmObj(FarmObject* farmObj)
{
	ModelGcs* mgcs = farmObj->getModel();
	std::string objName = farmObj->getName();
	if (objName == "doghouse")
	{
		mgcs->clearHighLight();
	}
}
//-----------------------------------------------------------------------
void FarmObjEvent::onClickFarmObj(FarmObject* farmObj)
{
	std::string farmObjName = farmObj->getName();
	if (farmObjName == "doghouse")
	{
		if (sObjMgr.isMainPlayer(sFunnyFarm.mAccount))
		{
			stime elapsedTime;
			BOOL hasDogFood = 0;
			int elapsedSecs = sFunnyFarm.getSelfDogFoodRemainSecs();
			if (elapsedSecs > 0)
			{
				hasDogFood = 1;
				elapsedTime.second = elapsedSecs % SECONDS_PER_MINUE;
				elapsedTime.minute = (elapsedSecs % SECONDS_PER_HOUR) / SECONDS_PER_MINUE;
				elapsedTime.hour   = (elapsedSecs % SECONDS_PER_DAY) / SECONDS_PER_HOUR;
				elapsedTime.day    = elapsedSecs / SECONDS_PER_DAY;
			}
			else
			{
				hasDogFood = 0;
				elapsedTime.second = 0;
				elapsedTime.minute = 0;
				elapsedTime.hour   = 0;
				elapsedTime.day    = 0;
			}
			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "DogNFood_setDogFoodRemainData",
																				elapsedTime.day,
																				elapsedTime.hour,
																				elapsedTime.minute,
																				elapsedTime.second,
																				hasDogFood);
		}
	}

}
//-----------------------------------------------------------------------
bool FarmObjEvent::hasObjOnEvent()
{
	return mObjOnEvent != NULL;
}
//-----------------------------------------------------------------------
void CropsCtrl::putPest(int index,int num)
{
	const Ogre::Vector3& pos = mFarmScene->getLandUintPos(index);
	//Y÷µ¡Ÿ ±–¥À¿
	//DynamicUIMgr::getSingleton().createFarmPaoPao(index,pos.x,30,pos.z,"","Button3","ImageButton6_Normal");
	if (num == 1)
	{
		DynamicUIMgr::getSingleton().createFarmHeadup(index,pos,EFHT_INSECTPEST,"Button3","ImageButton6_Normal");
	}
	if (num == 2)
	{
		DynamicUIMgr::getSingleton().createFarmHeadup(index,pos,EFHT_INSECTPEST,"Button3","ImageButton6_Normal",
																				"Button3","ImageButton6_Normal");
	}
	if (num == 3)
	{
		DynamicUIMgr::getSingleton().createFarmHeadup(index,pos,EFHT_INSECTPEST,"Button3","ImageButton6_Normal",
																				"Button3","ImageButton6_Normal",
																				"Button3","ImageButton6_Normal");
	}
}
//
void CropsCtrl::setPickUI(int index)
{
	Ogre::Vector3 pos = mFarmScene->getLandUintPos(index) + Ogre::Vector3(0,65,0);
	DynamicUIMgr::getSingleton().createFarmHeadup(index,pos,EFHT_PICKABLE,	"Button3","ImageButton8_Pushed",
																			"common1","Farm_h_01",
																			"common1","Farm_h_02");
	sFarmObjEvent.setIsPickUI(true);
}
//-----------------------------------------------------------------------
//
void FarmObjEvent::setIsPickUI(bool ispickui)
{
	mIsSetPickUI = ispickui;
}
//
//void CropsCtrl::removePest(int index)
//{
//	std::string win = "DynamicUI_PestPaoPao_";
//	CEGUI::WindowManager::getSingleton().getWindow("FarmmainUI")->removeChildWindow(win + Ogre::StringConverter::toString(index).c_str());
//	//CEGUI::WindowManager::getSingleton().destroyWindow(win + Ogre::StringConverter::toString(index).c_str());
//
//}
void FarmObjEvent::LButtonDown(UINT &flags, POINT point)
{

}
//
void FarmObjEvent::LButtonUp(UINT &flags, POINT point)
	{ 
		if ((flags & epfUI) != 0)
			return;

		if (sInputMgr.mLMouseDraged)
		{
			return;
		}

		if (sInputMgr.mLMousedown)
		{
			if (sStateMgr.isFunnyFarm())
			{
				sFunnyFarm.setFarmCurState(sFunnyFarm.getAction());
			}
		}
	}
//
void FarmObjEvent::MouseMove(UINT &flags, POINT delta, POINT point)
	{
		if (sStateMgr.isFunnyFarm())
		{
			sFunnyFarm._updatePos(point);
			if ((flags & epfUI) != 0)
			{
				sFunnyFarm.hideMouseAnimateWnd();
			}
			else
			{
				sFunnyFarm.setFarmCurState(sFunnyFarm.getAction());
			}
		}
	}