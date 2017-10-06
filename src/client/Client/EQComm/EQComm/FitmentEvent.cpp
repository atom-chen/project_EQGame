#include "EQCommPCH.h"

#include <OIS.h>

#include "FitmentEvent.h"
#include "LuaManager.h"
#include "GameEventMgr.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "MainPlayer.h"
#include "EQOgreSys.h"
//eqmain
#include "SceneManager.h"
#include "HouseScene.h"
#include "HSCManager.h"
#include "SystemGcs.h"
#include "Commfunc.h"
#include "PickMgr.h"
#include "NetPacket.h"
#include "PickMgr.h"
#include "PlayerHouse.h"
#include "EQGameSceneMgr.h"
#include "luabind/luabind/luabind.hpp"
#include "UIAnimationMgr.h"
#include "DragItem.h"
#include "CursorMgr.h"
#include "CommonHelper.h"

template<> FitmentEventMgr* Ogre::Singleton<FitmentEventMgr>::ms_Singleton = 0;



//-----------------------------------------------------------------------
FitmentEvent::FitmentEvent() 
	: mComp(NULL)
	, mFitmentCtrl( NULL )
	, mCompRep(NULL)
	, mCanLay(false)
	, mErr(HRV_FAIL)
	, mFace(0)
	, mGrid(NULL)
	, mOldGrid(NULL)
	, mMode( OpEditFitment )
	, mPickState(PS_NONE)
	, mCompID(0)
	, mBottom(0)
	, mOldBottom(0)
	, mCamera(NULL)
{
	HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();
	assert(sce);
	mFitmentCtrl = &sce->getFitmentCtrl();
	// 设为玩家模式
	mFitmentCtrl->setPutMode(HSCFitmentCtrl::pmPlayer);
	mCamera = EQOgreSys::getSingleton().getCamera();
	assert(mCamera);
}
//-----------------------------------------------------------------------
FitmentEvent::~FitmentEvent()
{
	undo();
}
//-----------------------------------------------------------------------
void FitmentEvent::setMouseCursor()
{
	sCursorMgr.setCurTarget(1);
	sCursorMgr.setCurTargetType(ObjMgr::GOT_FITMENT);
	sCursorMgr.setState(CursorMgr::CS_ON_FITMENT);
}
//-----------------------------------------------------------------------
void FitmentEvent::onMovein()
{
	// 保持更换墙壁地板模式下的鼠标状态(2011/7/29 add by hezhr)
	if (luabind::call_function<bool>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_setChangeWallFloorCursorState"))
	{
		return;
	}
	setMouseCursor();
}
//-----------------------------------------------------------------------
void FitmentEvent::onMoveout()
{
	if (mPickState == PS_NONE)
	{
		sCursorMgr.setCurTarget(0);
		sCursorMgr.setCurTargetType(0);
		sCursorMgr.backtoLastState();
	}
}
//-----------------------------------------------------------------------
void FitmentEvent::LButtonDown( POINT point )
{
	
}
//-----------------------------------------------------------------------
void FitmentEvent::pickup()
{
	if(mPickState == PS_ROTATE)
	{
		if(mCompRepInfo.template_id)
			FitmentEventMgr::getSingleton().addFitment(mCompRepInfo);

		mComp->take();
		mPickState = PS_PICKUP;

		setMouseCursor();
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_HideRotate");
		//luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_ShowRecycle", mPoint.x, mPoint.y);

		//mFitmentCtrl->rotEnd();
	}
}
//-----------------------------------------------------------------------
void FitmentEvent::putdown()
{
	if(mPickState == PS_ROTATE)
	{
		LButtonUp(mPoint);
	}
}
//-----------------------------------------------------------------------
void FitmentEvent::update()
{
	if(mPickState == PS_ROTATE)
	{
		int x,y;
		CommonHelper::Vector3ToScreenPoint(mRotatePos, x, y);
		luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_SetRotatePos", x, y);
	}
}
//-----------------------------------------------------------------------
void FitmentEvent::LButtonUp( POINT point )
{
	if(sEQGameSceneMgr.getSceneType() == EQGameSceneMgr::SCENE_HOUSE_PREVIEW)
		return;

	if(sInputMgr.mLMouseDraged)
		return;
	
	if ( mPickState != PS_NONE )
	{
		if ( !mComp )
			return;

		PlayerHouse* house = EQGameSceneMgr::getSingleton().getHouse();

		if(mPickState == PS_PICKUP)
		{
			Ogre::Ray ray = getRay(point);
			mFitmentCtrl->exec(ray);
			mPoint = point;

			if ( !mCanLay )
			{
				printf( HSCFitmentHelper::getFitmentErrorStr(mErr).c_str() );
				return;
			}

			// 处理替换操作
			if ( mCompRep )
			{
				mFace = mCompRep->getFace();
				mGrid = mCompRep->getAttachGrid();
				mBottom = mCompRep->getBottom();

				// 删除该位置上的旧物体
				//BeforeReplaceComponent( oldFitment = mComp, newFitment = mCompRep );
				PickObject* po = PickObjectProxy::pickUpObject(PickMgr::getSingleton().getPickObject());
				if ( po == mCompRep)
					PickMgr::getSingleton().clear();

				if(uint uid = house->isTempFurniture(mGrid, mBottom))
				{
					mCompRepInfo.template_id = house->fetchTempFurnitureRep(uid);
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_CartDel", uid);
				}
				else
				{
					mCompRepInfo.template_id = mCompRep->getHouseCompRow()->ID;
					HSCManager::getSingleton().destroyHouseSceneComponent( mCompRep );
				}
				mCompRepInfo.x			= mGrid->getGridX();
				mCompRepInfo.z			= mGrid->getGridZ();
				mCompRepInfo.height		= mBottom;
				mCompRepInfo.floor		= mGrid->getCreator()->getFloorNO();
				mCompRepInfo.face		= mFace;
				mCompRep = NULL;
			}

			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_HideRecycle");

			mRotatePos = mFitmentCtrl->getSceneNode()->_getDerivedPosition();
			int x,y;
			CommonHelper::Vector3ToScreenPoint(mRotatePos, x, y);
			luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_ShowRotate", x, y);

			//mFitmentCtrl->rotStart();
			mPickState = PS_ROTATE;
		}
		else if(mPickState == PS_ROTATE)
		{
			Ogre::Ray ray = getRay(mPoint);
			mFitmentCtrl->exec(ray);
			//mFitmentCtrl->rotEnd();
			mFitmentCtrl->detach();

			if(mMode == OpNewFitment)	//摆放新家具
			{
				if(luabind::call_function<bool>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_IsSelecting") == true) //从商城拉出来摆放的
				{
					// 家装模式商城的话，只是本地创建，购买时才通知服务端

					house_furniture fur;
					fur.template_id = mCompID;
					fur.x			= mGrid->getGridX();
					fur.z			= mGrid->getGridZ();
					fur.height		= mBottom;
					fur.floor		= mGrid->getCreator()->getFloorNO();
					fur.face		= mFace;

					uint uid = house->addTempFurniture(fur, mCompRepInfo);
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_AddShoppingCart", uid);
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "OnFitModeShopCenterPlaceFurniture");
				}
				else //从家装仓库里的家具拉出来摆放的
				{
					req_placed_furniture placed;
					placed.grid_index	= house->getCurUsingBagIdx();
					placed.x			= mGrid->getGridX();
					placed.z			= mGrid->getGridZ();
					placed.height		= mBottom;
					placed.floor		= mGrid->getCreator()->getFloorNO();
					placed.face			= mFace;

					sGameMgr.getEQMsgManager()->Send(placed);

					sDragItem.undo();
					int compType = 0;
					if(mComp->getHouseCompPropRow()->LightID != 0)
						compType = 1;
					else
						compType =0;
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "OnFitmodePlaceFurniture");
					luabind::call_function<void>(CLuaManager::getSingleton().getL(), "Fitmode_doPlayerGuideScript",compType);
				}
			}
			else	//移动重新已摆放好在场景中的家具
			{
				mComp->lay( mGrid, mFace, mBottom );
				if(uint uid = house->isTempFurniture(mOldGrid, mOldBottom))
				{
					house->moveTempFurniture(uid, mGrid->getCreator()->getFloorNO(), mGrid, mBottom, mFace);
					//家装模式下商城里的灯要默认开
					mComp->turnOnLight();
				}
				else
				{
					uint64 id = house->moveFit(mOldGrid, mGrid->getGridX(), mGrid->getGridZ(), mFace, mBottom, 
												mGrid->getCreator()->getFloorNO(), mOldBottom);
					if(id != 0)
					{
						req_move_furniture move;

						move.instance_id	= id;
						move.x				= mGrid->getGridX();
						move.z				= mGrid->getGridZ();
						move.height			= mBottom;
						move.floor			= mGrid->getCreator()->getFloorNO();
						move.face			= mFace;

						sGameMgr.getEQMsgManager()->Send(move);
					}
				}
			}

			sCursorMgr.setCurTarget(0);
			sCursorMgr.setCurTargetType(0);
			sCursorMgr.setState(CursorMgr::CS_NORMAL);

			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_HideRotate");
			mComp = NULL;
			mCanLay = false;
			mErr = HRV_FAIL;
			mPickState = PS_NONE;
		}
	}
	else
	{
		HouseSceneComponent * comp = getComponentByPoint(point);
		if ( comp )
		{
			// 家装模式下，更换房屋部件材料(2011/7/22 add by hezhr)
			if (luabind::call_function<bool>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_setChangeWallFloorCursorState"))
			{
				luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_changeRoomMaterial",
													(int)comp->getFloorNO(), (int)comp->getRoomNO(), (int)comp->getHouseCompPropRow()->PropID);
				return;
			}

			HouseReturnValue hrv = HSCLayLogic::canTake(comp);

			// 如果是可替换物件，只有在编辑器模式下才可拾取
			if ( comp->getHouseCompPropRow()->CanRep )
			{
				if ( this->mFitmentCtrl->getPutMode() != HSCFitmentCtrl::pmEditor )
					hrv = HRV_NOTAKE;
			}

			if ( hrv == HRV_OK )
			{
				// 拾取物件
				pickupFitment(comp, point);
				setMouseCursor();
				luabind::call_function<std::string>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_ShowRecycle", point.x, point.y);
			}
			else
			{
				//提示
				printf( HSCFitmentHelper::getFitmentErrorStr(hrv).c_str() ); 
			}
		}
	}
}//-----------------------------------------------------------------------
void FitmentEvent::RButtonUp( POINT point )
{
	if(mPickState == PS_PICKUP)
	{
		delFitment();
		mMode = OpEditFitment;

		sDragItem.undo();
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_CancelSelecting");
	}
}
//-----------------------------------------------------------------------
void FitmentEvent::MouseMove(UINT& flags, POINT delta, POINT point )
{
	if ((flags & epfUI) != 0)
	{
		sCursorMgr.setCurTarget(0);
		sCursorMgr.setCurTargetType(0);
		sCursorMgr.setState(CursorMgr::CS_NORMAL);
	}
	else
	{
		if ( !mComp )
		{
			luabind::call_function<bool>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_setChangeWallFloorCursorState");	// (2011/7/27 add by hezhr)
			return;
		}
		setMouseCursor();

		//屏蔽鼠标旋转
		if(mPickState != PS_ROTATE)
		{
			Ogre::Ray ray = getRay(point);	
			mFitmentCtrl->exec(ray);
		}
	}
}
//-----------------------------------------------------------------------
void FitmentEvent::KeyUp( UINT nChar, UINT nRepCnt )
{
	if ( OIS::KC_ESCAPE == nChar )
	{
		undo();

		sDragItem.undo();
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_CancelSelecting");
	}
	else if ( OIS::KC_DELETE == nChar )
	{
		delFitment();
		mMode = OpEditFitment;

		sDragItem.undo();
		luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_CancelSelecting");
	}
}
//-----------------------------------------------------------------------
Ogre::Ray FitmentEvent::getRay( POINT point )
{
	Ogre::Camera * cam = mCamera;

	int w = cam->getViewport()->getActualWidth();
	int h = cam->getViewport()->getActualHeight();

	Ogre::Ray ray = cam->getCameraToViewportRay((float)point.x/w, (float)point.y/h);

	return ray;
}
//-----------------------------------------------------------------------
HouseSceneComponent * FitmentEvent::getComponentByPoint( POINT point )
{
	HouseSceneComponent * pick = NULL;

	const Ogre::Ray kRay = getRay(point);


	if(PickMgr::getSingleton().pick(kRay))
	{
		PickObject* po = PickObjectProxy::pickUpObject(PickMgr::getSingleton().getPickObject());
		if ( po->getClassName() == "HouseSceneComponent" )
			pick = static_cast<HouseSceneComponent*>(po);
	}

	return pick;
}
//-----------------------------------------------------------------------
void FitmentEvent::notifyCanLay( HouseSceneGrid * grid, int face, int height, HouseSceneComponent * compRep )
{
	mCanLay = true;
	mGrid = grid;
	mFace = face;
	mBottom = height;
	mCompRep = compRep;
}
//-----------------------------------------------------------------------
void FitmentEvent::notifyForbidLay(HouseReturnValue hrv)
{
	mCanLay = false;
	mErr = hrv;
}
//-----------------------------------------------------------------------
void FitmentEvent::undo()
{
	mCanLay = false;
	mErr = HRV_FAIL;

	if ( mFitmentCtrl )
	{
		mFitmentCtrl->detach();
		mFitmentCtrl->setListener(NULL);
	}

	if ( mComp )
	{
		// 如果是新增物件，则删除物件; 如果是移动物件，则将物件放回原处		
		if ( OpNewFitment == mMode )
		{
			//BeforeDeleteComponent(mComp);
			PickObject* po = PickObjectProxy::pickUpObject(PickMgr::getSingleton().getPickObject());
			if ( po == mComp)
				PickMgr::getSingleton().clear();

			HSCManager::getSingleton().destroyHouseSceneComponent(mComp);
		}
		else
		{
			// 将物件放回原处
			mComp->lay( mOldGrid, mOldFace, mOldBottom );
		}

		mComp = NULL;
	}

	mMode = OpEditFitment;
	mPickState = PS_NONE;
}
//-----------------------------------------------------------------------
void FitmentEvent::delFitment()
{
	if ( mComp )
	{
		PlayerHouse* house = EQGameSceneMgr::getSingleton().getHouse();
		//删掉前处理，三种情况
		if(mMode == OpEditFitment)//回收
		{
			undo();

			if(uint uid = house->isTempFurniture(mOldGrid, mOldBottom))
			{
				house->delTempFurniture(uid);
				luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_DelShoppingCart", uid);
			}
			else
			{
				req_recovery_furniture recovery;

				recovery.furniture_id = house->getInstanceID(
					mOldGrid->getCreator()->getFloorNO(),
					mOldGrid->getGridX(),
					mOldGrid->getGridZ(),mOldBottom);

				if(recovery.furniture_id != 0)
					sGameMgr.getEQMsgManager()->Send(recovery);
			}
			luabind::call_function<void>(CLuaManager::getSingleton().getL(), "FitModeShopCenter_HideRecycle");
			return;
		}
	}
	mCanLay = false;
	mErr = HRV_FAIL;

	if ( mFitmentCtrl )
	{
		mFitmentCtrl->detach();
		mFitmentCtrl->setListener(NULL);
	}

	if ( mComp )
	{
		PickObject* po = PickObjectProxy::pickUpObject(PickMgr::getSingleton().getPickObject());
		if ( po == mComp)
			PickMgr::getSingleton().clear();

		HSCManager::getSingleton().destroyHouseSceneComponent(mComp);
		mComp = NULL;
	}

	mPickState = PS_NONE;
}
//-----------------------------------------------------------------------
void FitmentEvent::pickupFitment(HouseSceneComponent* comp, POINT point)
{
	assert(!mComp);
	mComp = comp;
	mOldGrid = comp->getAttachGrid();
	mOldFace = comp->getFace();
	mOldBottom = comp->getBottom();

	{
		//BeforeMoveComponent(comp);
		mComp->take();
	}

	mFitmentCtrl->attach(mComp);
	mFitmentCtrl->setListener(this);
	mFitmentCtrl->exec( getRay(point) );
	mMode = OpEditFitment;
	mPickState = PS_PICKUP;
	mPoint = point;
}
//-----------------------------------------------------------------------
void FitmentEvent::createNewFitment( uint compID )
{
	if ( 0 == compID )
		return;

	undo();

	mComp = HSCManager::getSingleton().createHouseSceneComponent();
	mComp->parseRes(compID);
	mComp->load();

	mComp->setResName( HSCFitmentHelper::fetchResName() );

	mFitmentCtrl->attach(mComp);
	mFitmentCtrl->setListener(this);
	
	mCompID = compID;
	mCompRepInfo.template_id = 0;

	// 设为新增模式
	mMode = OpNewFitment;
	mPickState = PS_PICKUP;
}
//-----------------------------------------------------------------------
void FitmentEvent::rotateFitment(float degree)
{
	if ( mComp )
		mFitmentCtrl->rotate(degree);
}
//-----------------------------------------------------------------------



//---------------------------------------------------------------------
//---------------------------------------------------------------------
FitmentEventMgr::FitmentEventMgr()
	: mFitMode(FMM_NONE)
	, mEvent(NULL) 
	, EQInputEventListener(epfFitEvent)
{
	sInputMgr.addListener(this);
}
//---------------------------------------------------------------------
FitmentEventMgr::~FitmentEventMgr()
{
	setFitMode(FMM_NONE);
}	
//---------------------------------------------------------------------
int FitmentEventMgr::getCurrFurID() 
{ 
	if ( mEvent && mEvent->getCurrentFitment() )
		return (int)mEvent->getCurrentFitment()->getUid();
	return -1;
}
//---------------------------------------------------------------------
void FitmentEventMgr::setFitMode(eFitmentMode fm)
{
	mFitMode = fm;
	if ( mFitMode == FMM_ADD || mFitMode == FMM_EDIT )
	{
		if ( !mEvent )
			mEvent = new FitmentEvent();
	}
	else
	{
		if ( mEvent )
		{
			delete mEvent;
			mEvent = NULL;
		}
	}
}
//--------------------------------------------------------------------------
void FitmentEventMgr::onMovein()
{
	if (mEvent )
		mEvent->onMovein();
}
//---------------------------------------------------------------------------
void FitmentEventMgr::onMoveout()
{
	if (mEvent )
		mEvent->onMoveout();
}
//--------------------------------------------------------------------------
void FitmentEventMgr::LButtonDown( UINT& flags, POINT point)
{
	if ((flags & epfCameraFly) ==0 && mEvent )
		mEvent->LButtonDown(point);
}
//--------------------------------------------------------------------------
void FitmentEventMgr::LButtonUp( UINT& flags, POINT point)
{
	if (
		(flags & epfUI )==0 
		&&
		(flags & epfDragItem)==0
		&& mEvent )
		mEvent->LButtonUp(point);
}
//--------------------------------------------------------------------------
void FitmentEventMgr::RButtonUp( UINT& flags, POINT point)
{
	if (
		(flags & epfUI )==0 
		&&
		(flags & epfDragItem)==0
		&& mEvent )
		mEvent->RButtonUp(point);
}
//--------------------------------------------------------------------------
void FitmentEventMgr::MouseMove( UINT& flags, POINT delta, POINT point)
{
	if ( mEvent )
		mEvent->MouseMove(flags,delta, point);
}
//--------------------------------------------------------------------------
void FitmentEventMgr::KeyUp( UINT& flags, UINT nChar, UINT nRepCnt)
{
	if ( mEvent )
		mEvent->KeyUp(nChar, nRepCnt);
}
//--------------------------------------------------------------------------
void FitmentEventMgr::setFitDro(int id)
{
	if ( !mEvent )
		return;

	if ( id > 0 )
		mEvent->createNewFitment(id);
	else
		mEvent->delFitment();
}
//--------------------------------------------------------------------------
uint FitmentEventMgr::addFitment(house_furniture& furni, bool turnOn)
{
	HouseScene* sce = EQSceneManager::getSingleton().getHouseScene();
	HouseSceneGrid * gr = sce->getGrid(furni.floor, furni.x, furni.z);
	if ( !gr )
	{
		LOGERR(FormatStr("FitmentEventMgr::addFitment: Can't find grid flr %d, x %d, z %d", furni.floor, furni.x, furni.z));
		return 0;
	}

	HouseSceneComponent* fit = HSCManager::getSingleton().createHouseSceneComponent();
	fit->parseRes(furni.template_id);
	fit->load();

	fit->setResName(HSCFitmentHelper::fetchResName());
	fit->getModel()->setVisible(true);
	
	fit->lay(gr, furni.face, furni.height);	
    //GameEventMgr::getSingleton().fireEvent(game_event[FIT_NEW_ADDED]);

	//很搓的做法，以后要改掉
	if(turnOn)
		fit->turnOnLight();
	else
		fit->turnOffLight();

	//如果有替换物件，删了
	if(fit->getHouseCompPropRow()->CanRep)
	{
		HouseSceneComponent* compRep=NULL;
		compRep = HSCLayLogic::findCanReplaceComponent( fit, gr );
		if(compRep)
		{
			HSCManager::getSingleton().destroyHouseSceneComponent( compRep );
		}
	}

	return fit->getUid();
}
//--------------------------------------------------------------------------
void FitmentEventMgr::delFitment(DWORD id)
{
	if(HSCManager::getSingleton().hasHouseSceneComponent(id))
	{
		HouseSceneComponent* fit = HSCManager::getSingleton().getHouseSceneComponent(id);
		if(mEvent)
		{
			if(fit==mEvent->mComp)
			{
				mEvent->mComp = NULL;
				if ( mEvent->mFitmentCtrl )
				{
					mEvent->mFitmentCtrl->detach();
					mEvent->mFitmentCtrl->setListener(NULL);
				}
			}
			if(fit==mEvent->mCompRep) 
			{
				mEvent->mCompRep = NULL;
			}
		}

		PickObject* po = PickObjectProxy::pickUpObject(PickMgr::getSingleton().getPickObject());
		if ( po == fit)
			PickMgr::getSingleton().clear();

		HSCManager::getSingleton().destroyHouseSceneComponent(id);
	}
}
//--------------------------------------------------------------------------
void FitmentEventMgr::playFitmentFX(uint id)
{
	HouseSceneComponent* fit = HSCManager::getSingleton().getHouseSceneComponent(id);
	if(fit)
		fit->getModel()->addStrokes();
}
//--------------------------------------------------------------------------
void FitmentEventMgr::delFitmentFX(uint id)
{
	HouseSceneComponent* fit = HSCManager::getSingleton().getHouseSceneComponent(id);
	if(fit)
		fit->getModel()->delStrokes();
}
//--------------------------------------------------------------------------
void FitmentEventMgr::rotateFitment(float degree)
{
	if(mEvent)
		mEvent->rotateFitment(degree);
}
//--------------------------------------------------------------------------
void FitmentEventMgr::Update(float timElapsed)
{
	if(mEvent)
		mEvent->update();
}