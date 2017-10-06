#include "EQCommPCH.h"

#include "PickMgr.h"
#include "CollisionTools.h"
#include "SystemGcs.h"
#include "HSCManager.h"
#include "FitmentInteractMgr.h"
#include "Variable.h"
#include "ObjMgr.h"
#include "ChatSystem.h"
#include "GameEventMgr.h"
#include "NPC.h"
#include "ObjMgr.h"
#include "GameState.h"
#include "EQWndProc.h"
#include "EQGameMgr.h"
#include "EQCamera.h"
#include "CommonHelper.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "PickObjectProxy.h"
#include "Avatar.h"
//#include "FarmObjEvent.h"
#include "FarmScene.h"
#include "EQGameSceneMgr.h"

//-----------------------------------------------------------------------
template<> PickMgr* Ogre::Singleton<PickMgr>::ms_Singleton = 0;
//-----------------------------------------------------------------------
PickMgr::PickMgr() :
EQInputEventListener(epfPick)
{
	sInputMgr.addListener(this);
}
//-----------------------------------------------------------------------
PickMgr::~PickMgr()
{
}
//-----------------------------------------------------------------------
void PickMgr::init()
{
	mHighLight = Ogre::ColourValue(VariableSystem::getSingleton().GetAs_Float("PickHLColourR"),
		VariableSystem::getSingleton().GetAs_Float("PickHLColourG"),
		VariableSystem::getSingleton().GetAs_Float("PickHLColourB"));
}
//-----------------------------------------------------------------------
bool PickMgr::pick(const Ogre::Ray &ray, Ogre::uint32 flag)
{
	if(sEQGameSceneMgr.getSceneType() == EQGameSceneMgr::SCENE_HOUSE_PREVIEW)
		return false;

	mLastName = mPickName;
	Ogre::MovableObject* pick=NULL;
	bool ret =  MOC::CollisionTools::getSingleton().raycast(ray, mPoint, pick, mDistance, flag);
	if ( ret )
		mPickName = pick->getName();
	else
		mPickName = "";
	return ret;
}
//-----------------------------------------------------------------------
void PickMgr::movein()
{
	if(mLastName == mPickName)
		return;
	else
		moveout();

	Ogre::MovableObject* mo = getPickObject();
	PickObject* po = PickObjectProxy::pickUpObject(mo);
	if ( po == NULL )
		return;

	if ( po->getClassName() == "HouseSceneComponent" )
	{
		HouseSceneComponent* com = static_cast<HouseSceneComponent*>(po);
		//房屋部件
		if(com->isFromFrame())
		{
			//框架
			return;
		}
		else
		{
			//家具
			if(FitmentInteractMgr::getSingleton().canClick(com))
				com->getModel()->setSelfIllum(mHighLight);
			return;
		}
	}
	else if ( po->getClassName() == "GameObj" )
	{
		//人物
		GameObj* obj = static_cast<GameObj*>(po);
		if ( obj->getAvatar() && obj->getAvatar()->getModel() )
			obj->getAvatar()->getModel()->setSelfIllum(mHighLight);
	}
	else if ( po->getClassName() == FarmPlantUnit_PickObjectUnit)
	{
		//FarmPlantUnit::PickObjectUnit *farmObjUnit 
		//	= static_cast<FarmPlantUnit::PickObjectUnit*>(po);
		//sFarmObjEvent.onMovein(farmObjUnit,mHighLight);
	}

}
//-----------------------------------------------------------------------
void PickMgr::moveout()
{
	if(mLastName.empty())
		return;

	Ogre::MovableObject* mo = getLastPickObject();
	PickObject* po = PickObjectProxy::pickUpObject(mo);
	if ( po == NULL )
		return;

	if ( po->getClassName() == "HouseSceneComponent" )
	{
		HouseSceneComponent* com = static_cast<HouseSceneComponent*>(po);
		//房屋部件
		if(com->isFromFrame())
		{
			//框架
			return;
		}
		else
		{
			//家具
			if(FitmentInteractMgr::getSingleton().canClick(com))
				com->getModel()->clearSelfIllum();
			return;
		}
	}
	else if ( po->getClassName() == "GameObj" )
	{
		GameObj* obj = static_cast<GameObj*>(po);
		if ( obj->getAvatar() && obj->getAvatar()->getModel() )
			obj->getAvatar()->getModel()->clearSelfIllum();
	}
	else if ( po->getClassName() == FarmPlantUnit_PickObjectUnit)
	{
		//FarmPlantUnit::PickObjectUnit *farmObjUnit 
		//	= static_cast<FarmPlantUnit::PickObjectUnit*>(po);
		//sFarmObjEvent.onMoveout(farmObjUnit);
	}

}
//-----------------------------------------------------------------------
bool PickMgr::click()
{
	MainPlayer::getSingleton().clearTarget();

	if(mPickName.empty())
		return false;

	Ogre::MovableObject* mo = getPickObject();
	PickObject* po = PickObjectProxy::pickUpObject(mo);
	if ( po == NULL )
		return false;

	if ( po->getClassName() == "HouseSceneComponent" )
	{
		HouseSceneComponent* com = static_cast<HouseSceneComponent*>(po);
		//房屋部件
		if(com->isFromFrame())
		{
			//框架
			return false;
		}
		else
		{
			//家具
			if(FitmentInteractMgr::getSingleton().canClick(com))
				FitmentInteractMgr::getSingleton().pickFitment(com);
			return true;
		}
	}
	else if ( po->getClassName() == "GameObj" )
	{
		GameObj* obj = static_cast<GameObj*>(po);
		return clickGameObj(obj);
	}
	else if ( po->getClassName() == FarmPlantUnit_PickObjectUnit)
	{
		//FarmPlantUnit::PickObjectUnit *farmObjUnit 
		//	= static_cast<FarmPlantUnit::PickObjectUnit*>(po);
		//sFarmObjEvent.onClicked(farmObjUnit);
	}

	return false;
}
////-----------------------------------------------------------------------
//bool PickMgr::isPickObject(ModelGcs* model)
//{
//	if(!mPick || !model)
//		return false;
//
//	ModelGcs* pmodel = dynamic_cast<ModelGcs*>(mPick->getUserObject());
//	if(!pmodel)
//		return false;
//
//	if(pmodel == model)
//		return true;
//	else
//		return false;
//}
//-----------------------------------------------------------------------
Ogre::MovableObject* PickMgr::getPickObject()
{
	if ( mPickName.empty() )
		return NULL;

	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();
	if ( mgr->hasEntity(mPickName) )
		return mgr->getEntity(mPickName);
	else
	{
		mPickName = "";
		return NULL;
	}
}
//-----------------------------------------------------------------------
Ogre::MovableObject* PickMgr::getLastPickObject()
{
	if ( mLastName.empty() )
		return NULL;

	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();
	if ( mgr->hasEntity(mLastName) )
		return mgr->getEntity(mLastName);
	else
	{
		mLastName = "";
		return NULL;
	}
}
//-----------------------------------------------------------------------
bool PickMgr::clickGameObj(GameObj* obj)
{
	//GameObj
	int x,y;
	if(obj)
	{
		ObjMgr::getSingleton().setPickObj(obj);
		switch(obj->getType())
		{
		case ObjMgr::GOT_NPC:
			{
				NPC *npc = (NPC*)(obj);
				npc->onClickObj();

				MainPlayer::getSingleton().setTarget(npc->getName(), ObjMgr::GOT_NPC);
			}
			break;
		case ObjMgr::GOT_PLAYER:
			{
				string acc = ((Player*)(obj))->getAccount();
				string name = ((Player*)(obj))->getName();
				ChatSystem::getSingleton().setTalktoAccount(acc);
				ChatSystem::getSingleton().setTalktoName(name);

				//  [7/21/2010 huangdj1]
				Ogre::Vector3 headup = obj->getPos() + Ogre::Vector3(0,obj->getModelHeight(),0);
				if(!CommonHelper::Vector3ToScreenPoint(headup, x, y))
					return NULL;
				luabind::call_function<void>(CLuaManager::getSingleton().getL(), "setDisplayPos", x, y);

				MainPlayer::getSingleton().setTarget(acc, ObjMgr::GOT_PLAYER);
			}
			break;
		}

		GameEventMgr::getSingleton().fireEvent("ON_CLICKED_GAMEOBJ");
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------
void PickMgr::LButtonUp	( UINT& flags, POINT point ) 
{ 
	if(!sStateMgr.gaming())
		return;// false;
	if(sInputMgr.mLMouseDraged)
		return;

	if((flags & epfUI) != 0
		|| (flags & epfDragItem)!=0)
		return;

	Ogre::Camera* cam = GameMgr::getSingleton().getMainCamera()->getOgreCam();

	Ogre::Ray ray = cam->getCameraToViewportRay(
		(Ogre::Real)point.x / EQWndProc::getSingleton().getWndWidth(), 
		(Ogre::Real)point.y / EQWndProc::getSingleton().getWndHeight());

	if(PickMgr::getSingleton().pick(ray))
	{
		if(PickMgr::getSingleton().click())
		{
			flags |= epfPick;//标识事件处理过了
			return;
		}
	}

	return;
}
//-----------------------------------------------------------------------
void PickMgr::MouseMove	( UINT& flags, POINT delta, POINT point ) 
{ 
	if(GameStateMgr::getSingleton().getState() == GS_LOGIN)
		return;

	if((flags & epfUI) != 0)//ui没接收到消息才进行pick
		return;

	Ogre::Camera* cam = GameMgr::getSingleton().getMainCamera()->getOgreCam();

	Ogre::Ray ray = cam->getCameraToViewportRay(
		(Ogre::Real)point.x / EQWndProc::getSingleton().getWndWidth(), 
		(Ogre::Real)point.y / EQWndProc::getSingleton().getWndHeight());

	if(PickMgr::getSingleton().pick(ray))
		PickMgr::getSingleton().movein();
	else
		PickMgr::getSingleton().moveout();
}
