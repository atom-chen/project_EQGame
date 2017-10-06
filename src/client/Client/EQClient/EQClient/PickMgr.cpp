#include "EQClientPCH.h"

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
#include "GameMgr.h"
#include "EQCamera.h"
#include "CommonHelper.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "PickObjectProxy.h"
#include "Avatar.h"
#include "EQGameSceneMgr.h"
#include "GiftSystem.h"
#include "DynamicUIMgr.h"
#include "Variable.h"
#include "MagicBoxSystem.h"
#include "FitmentEvent.h"
#include "SceneManager.h"
#include "SceneEntityObject.h"
//-----------------------------------------------------------------------
template<> PickMgr* Ogre::Singleton<PickMgr>::ms_Singleton = 0;
//-----------------------------------------------------------------------
PickMgr::PickMgr() :
EQInputEventListener(epfPick),
mPickGarbageId(0),
mPickLastGarbageId(0)
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
	mComHighLight = Ogre::ColourValue(VariableSystem::getSingleton().GetAs_Float("PickComColourR"),
		VariableSystem::getSingleton().GetAs_Float("PickComColourG"),
		VariableSystem::getSingleton().GetAs_Float("PickComColourB"));

	mObjHighLight = Ogre::ColourValue(VariableSystem::getSingleton().GetAs_Float("PickObjColourR"),
		VariableSystem::getSingleton().GetAs_Float("PickObjColourG"),
		VariableSystem::getSingleton().GetAs_Float("PickObjColourB"));
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
	{
		mPickName = "";
	}

	return ret;
}
//-----------------------------------------------------------------------
bool PickMgr::pickGarbage(const Ogre::Ray &ray)
{
	if(sEQGameSceneMgr.getSceneType() == EQGameSceneMgr::SCENE_HOUSE_PREVIEW)
		return false;

	mPickLastGarbageId = mPickGarbageId;
	mPickGarbageId = 0;
	HouseScene *hs = EQSceneManager::getSingleton().getHouseScene();
	MagicBoxSystem *mbs = MagicBoxSystem::getSingletonPtr();

	if(NULL == hs || NULL == mbs)
		return false;

	HouseSceneGrid *hg = NULL;
	Ogre::Vector3 cp;
	if(false == hs->rayGrid(ray, hg, cp))
		return false;

	uint64 mboxId = mbs->getMagicBoxIdInGrid(hg);
	MagicBox *mbox = mbs->getMagicBox(mboxId);
	if(NULL == mbox)
		return false;

	if(mbt_garbage != mbox->getMagickBoxInfo().type)
		return false;

	mPickGarbageId = mboxId;
	return true;
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
				if (sFitMgr.isFiting())
				{
					sFitMgr.onMovein();
				}
				com->getModel()->setSelfIllum(mComHighLight);
			return;
		}
	}
	else if ( po->getClassName() == "GameObj" )
	{
		//人物
		GameObj* obj = static_cast<GameObj*>(po);
		//
		if (obj->getType() == ObjMgr::GOT_NPC)
		{
			static_cast<NPC*>(obj)->onMovein();
		}
		//
		if ( obj->getAvatar() && obj->getAvatar()->getModel() )
			obj->getAvatar()->getModel()->setSelfIllum(mObjHighLight);
	}
	else if ( po->getClassName() == FarmPlantUnit_PickObjectUnit)
	{
		//FarmPlantUnit::PickObjectUnit *farmObjUnit 
		//	= static_cast<FarmPlantUnit::PickObjectUnit*>(po);
		//sFarmObjEvent.onMovein(farmObjUnit,mComHighLight);
	}
	else if( "Gift::PickObjectUnit" == po->getClassName() )
	{// 礼物
		Gift::PickObjectUnit* pickUnit = static_cast<Gift::PickObjectUnit*>(po);
		Gift* gift = pickUnit->getCreator();
		gift->getModel()->setSelfIllum(mComHighLight);
	}
	//宝箱
	else if(MagicBox::PickObjectUnit::ClassName == po->getClassName())
	{
		MagicBox::PickObjectUnit *pickUnit = static_cast<MagicBox::PickObjectUnit*>(po);
		MagicBox *mbox = pickUnit->getCreator();

		if(mbt_magic_box == mbox->getMagickBoxInfo().type)
		{
			mbox->getModel()->setSelfIllum(mComHighLight);
			mbox->getModel()->playAnimation("Opening", false);
			mbox->getModel()->startParticle();
		}
	}
	else if(po->getClassName() == "SceneEntityObject")
	{
		SceneEntityObject* obj = static_cast<SceneEntityObject*>(po);
		if(FitmentInteractMgr::getSingleton().canClick(obj))
		{
			obj->setSelfIllum(mObjHighLight);
		}
	}
}

void PickMgr::moveinGarbage()
{
	if(mPickLastGarbageId == mPickGarbageId)
		return;
	else
		moveoutGarbage();

	MagicBox *mbox = MagicBoxSystem::getSingleton().getMagicBox(mPickGarbageId);
	if(NULL == mbox)
		return;

	mbox->getModel()->setSelfIllum(mComHighLight);
	//LOGERR("Garbage move in.");
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
				if (sFitMgr.isFiting())
				{
					sFitMgr.onMoveout();
				}

				com->getModel()->clearSelfIllum();
			return;
		}
	}
	else if ( po->getClassName() == "GameObj" )
	{
		GameObj* obj = static_cast<GameObj*>(po);
		//
		if (obj->getType() == ObjMgr::GOT_NPC)
		{
			static_cast<NPC*>(obj)->onMoveout();
		}
		//
		if ( obj->getAvatar() && obj->getAvatar()->getModel() )
			obj->getAvatar()->getModel()->clearSelfIllum();
	}
	else if ( po->getClassName() == FarmPlantUnit_PickObjectUnit)
	{
		//FarmPlantUnit::PickObjectUnit *farmObjUnit 
		//	= static_cast<FarmPlantUnit::PickObjectUnit*>(po);
		//sFarmObjEvent.onMoveout(farmObjUnit);
	}
	else if( "Gift::PickObjectUnit" == po->getClassName() )
	{// 礼物
		Gift::PickObjectUnit* pickUnit = static_cast<Gift::PickObjectUnit*>(po);
		Gift* gift = pickUnit->getCreator();
		
		gift->getModel()->clearSelfIllum();
	}
	//宝箱
	else if(MagicBox::PickObjectUnit::ClassName == po->getClassName())
	{
		MagicBox::PickObjectUnit *pickUnit = static_cast<MagicBox::PickObjectUnit*>(po);
		MagicBox *mbox = pickUnit->getCreator();

		if(mbt_magic_box == mbox->getMagickBoxInfo().type)
		{
			mbox->getModel()->clearSelfIllum();
			mbox->getModel()->playAnimation("Close", false);
			mbox->getModel()->stopParticle();
		}
	}
	else if(po->getClassName() == "SceneEntityObject")
	{
		SceneEntityObject* obj = static_cast<SceneEntityObject*>(po);
		if(FitmentInteractMgr::getSingleton().canClick(obj))
		{
			obj->clearSelfIllum();
		}
	}
}

void PickMgr::moveoutGarbage()
{
	if(0 == mPickLastGarbageId)
		return;

	MagicBox *mbox = MagicBoxSystem::getSingleton().getMagicBox(mPickLastGarbageId);
	if(NULL == mbox)
		return;

	mbox->getModel()->clearSelfIllum();
	//LOGERR("Garbage move out.");
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
	else if( "Gift::PickObjectUnit" == po->getClassName() )
	{// 礼物
		Gift::PickObjectUnit* pickUnit = static_cast<Gift::PickObjectUnit*>(po);
		Gift* gift = pickUnit->getCreator();
		GiftSystem::getSingleton().clickGift(gift);
		return true;
	}//宝箱
	else if(MagicBox::PickObjectUnit::ClassName == po->getClassName())
	{
		MagicBox::PickObjectUnit *pickUnit = static_cast<MagicBox::PickObjectUnit*>(po);
		MagicBox *mbox = pickUnit->getCreator();

		if(mbt_magic_box == mbox->getMagickBoxInfo().type)
		{
			MagicBoxSystem::getSingleton().clickMagicBox(mbox);
		}
		return true;
	}
	else if(po->getClassName() == "SceneEntityObject")
	{
		SceneEntityObject* obj = static_cast<SceneEntityObject*>(po);
		if(FitmentInteractMgr::getSingleton().canClick(obj))
		{
			FitmentInteractMgr::getSingleton().pickObject(obj);
			return true;
		}
	}

	return false;
}

bool PickMgr::clickGarbage()
{
	if(0 == mPickGarbageId)
		return false;

	MagicBox *mbox = MagicBoxSystem::getSingleton().getMagicBox(mPickGarbageId);
	if(NULL == mbox)
		return false;

	MagicBoxSystem::getSingleton().clickMagicBox(mbox);

	return true;
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
Ogre::MovableObject* PickMgr::getObject(std::string &name)
{
	if ( name.empty() )
		return NULL;

	Ogre::SceneManager* mgr = SystemGcs::getSingleton().getSceneManager();
	if ( mgr->hasEntity(name) )
		return mgr->getEntity(name);

	return NULL;
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
	if(obj)
	{
		Ogre::Real dis  = obj->getPos().distance(sMainPlayer.getPos());
		ObjMgr::getSingleton().setPickObj(obj);
		float npcdis = VariableSystem::getSingleton().GetAs_Float("NpcDistance");
		switch(obj->getType())
		{
		case ObjMgr::GOT_NPC:
			{
				//if(dis > npcdis)
				//	return true;

				NPC *npc = (NPC*)(obj);
				if( npc->onClickObj() )
					MainPlayer::getSingleton().setTarget(npc->getName(), ObjMgr::GOT_NPC);
				else
					return true;
			}
			break;
		case ObjMgr::GOT_PLAYER:
			{
				string acc = ((Player*)(obj))->getAccount();
				string name = ((Player*)(obj))->getName();
				ChatSystem::getSingleton().setTalktoAccount(acc);
				ChatSystem::getSingleton().setTalktoName(name);
				mPickPlayerName = acc;
				DynamicUIMgr::getSingleton().createCircleMenuDynamicInfo(obj,ObjMgr::GOT_PLAYER,"");
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

	if(pickGarbage(ray))
	{
		moveinGarbage();
		if(clickGarbage())
		{
			flags |= epfPick;//标识事件处理过了
			return;
		}
	}
	else
		moveoutGarbage();

	if(pick(ray))
	{
		movein();
		if(click())
		{
			flags |= epfPick;//标识事件处理过了
			return;
		}
	}
	else
		moveout();

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

	if(pickGarbage(ray))
		moveinGarbage();
	else
		moveoutGarbage();

	if(pick(ray))
		movein();
	else
		moveout();
}
//--------------------------------------------------------------------
void PickMgr::update()
{
		GameObj* p;
		p = ObjMgr::getSingleton().findObj(ObjMgr::GOT_PLAYER,getPickPlayerName());

		if(!p)
			CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot")->removeChildWindow("circleMenu");
}
