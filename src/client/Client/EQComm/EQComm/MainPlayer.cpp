#include "EQCommPCH.h"
#include "MainPlayer.h"
#include "Player.h"
#include "Avatar.h"
#include "GameObj.h"
#include "EQGameMgr.h"
#include "EQMsgManager.h"
#include "EQOgreSys.h"
#include "GameState.h"
#include "Variable.h"
#include "ObjMgr.h"
#include "EQCamera.h"
#include "EQWndProc.h"
#include "EQGameSceneMgr.h"
#include "SceneManager.h"
#include "HouseScene.h"
#include "SearchSystem/NavSearchSystem.h"
#include "FitmentEvent.h"
#include "FitmentInteractMgr.h"
#include "TerrainData.h"
#include "CursorMgr.h"
#include "NPC.h"
#include "BodyActionMgr.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
//-------------------------------------------------------------------
template<> MainPlayer* Ogre::Singleton<MainPlayer>::ms_Singleton = 0;


//--------------------------------------------------------------------
void MainPlayer::init()
{
	Player::init();

	GameObj::NORMAL_WALK_SPEED = VariableSystem::getSingleton().GetAs_Int("WalkSpeed");

	Ogre::SceneManager* sm = EQOgreSys::getSingleton().getSceneManager();
	mTargetNode = sm->getRootSceneNode()->createChildSceneNode("showTarget.mesh_testtarget");

	//箭头
	Ogre::Entity* ent = sm->createEntity("showTarget.mesh", "showTarget.mesh");
	Ogre::SceneNode* node = mTargetNode->createChildSceneNode();
	node->attachObject(ent);
	//ent->setCastShadows(true);

	//箭头动画
	Ogre::Animation* anim = sm->createAnimation("TargetArrow", 1);
	anim->setInterpolationMode(Ogre::Animation::IM_LINEAR);
	Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, node);
	Ogre::TransformKeyFrame* key;
	key = track->createNodeKeyFrame(0);
	key->setTranslate(Ogre::Vector3(0,0,0));
	key->setScale(Ogre::Vector3(0.8,0.8,0.8));
	key = track->createNodeKeyFrame(0.5);
	key->setTranslate(Ogre::Vector3(0,25,0));
	key->setScale(Ogre::Vector3(0.8,0.8,0.8));
	key = track->createNodeKeyFrame(1);
	key->setTranslate(Ogre::Vector3(0,0,0));
	key->setScale(Ogre::Vector3(0.8,0.8,0.8));

	mArrowAnimState = sm->createAnimationState("TargetArrow");
	mArrowAnimState->setEnabled(true);

	//光圈
	node = mTargetNode->createChildSceneNode();
	mTargetEffect = sm->createManualObject("TargetCircle");
	mTargetEffect->setCastShadows(false);
	node->attachObject(mTargetEffect);

	//光圈模型
	int radius = 36;
	mTargetEffect->begin("tar001", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	mTargetEffect->getSection(0)->getMaterial()->setReceiveShadows(false);
	mTargetEffect->position(-radius, 1, -radius);
	mTargetEffect->textureCoord(0, 0);
	mTargetEffect->position(-radius, 1, radius);
	mTargetEffect->textureCoord(0, 1);
	mTargetEffect->position(radius, 1, radius);
	mTargetEffect->textureCoord(1, 1);
	mTargetEffect->position(radius, 1, -radius);
	mTargetEffect->textureCoord(1, 0);
	mTargetEffect->quad(0,1,2,3);
	mTargetEffect->end();

	//光圈动画
	anim = sm->createAnimation("TargetCircle", 1.0);
	anim->setInterpolationMode(Ogre::Animation::IM_SPLINE);
	track = anim->createNodeTrack(0, node);
	key = track->createNodeKeyFrame(0);
	key = track->createNodeKeyFrame(0.5);
	key->setScale(Ogre::Vector3(0.2,0.2,0.2));
	key = track->createNodeKeyFrame(1);
	key->setScale(Ogre::Vector3(1,1,1));

	mCircleAnimState = sm->createAnimationState("TargetCircle");
	mCircleAnimState->setEnabled(true);

	mTargetNode->setVisible(false);
}
//--------------------------------------------------------------------
MainPlayer::MainPlayer(void):
mTargetNode(0),
mTargetEffect(NULL),
mCircleAnimState(NULL),
mArrowAnimState(NULL),
mListener(NULL),
mAutoUpDown(true),
EQInputEventListener(epfMainplayer),
mCurDestPos(0,0,0),
mMoveForWhat(MOVE_FOR_NONE)
{
	setType(ObjMgr::GOT_MAINPLAYER);
	sInputMgr.addListener(this);
}

MainPlayer::~MainPlayer(void)
{
}
//--------------------------------------------------------------------
void MainPlayer::moveTo(Ogre::Vector3& dest, MainPlayerListener* listener)
{
	setListener(listener);
	if(mListener)
		mListener->moveBegin();

	//取消寻路
	//stopWalk();

	GameObj::moveTo(dest);
	//reqStartWalk(dest);
	showTargetPath(dest);

}
//--------------------------------------------------------------------
//bool MainPlayer::update()
//{
////	if( GameStateMgr::getSingleton().getState() != GS_MAINGAME)
////		return false;
////#define WALKING 0
////	if( Player::update() != WALKING)//不在走路了
////	{
////		reqStopWalk();
////
////		if(mListener)
////			mListener->moveEnd();
////	}
////	else if(getState() == GOS_WALK)//在走路在
////	{
////		static DWORD oldtime	= GetTickCount();
////		DWORD timenow			= GetTickCount();
////#define SYN_POS_TIMER 500
////		if( timenow - oldtime > SYN_POS_TIMER )
////		{
////			 syncPosToServer();
////			 oldtime = timenow;
////		}
////	}
////
////	//目标动画
////	_TargetPoint();
//	updateMove();
//	return true;
//}
//--------------------------------------------------------------------
bool MainPlayer::update()
{
	if (getState() == GOS_STAND_IDLE
		&& !FitmentInteractMgr::getSingleton().isPreparing()
		&& !FitmentInteractMgr::getSingleton().isInteracting())
	{
		DWORD time = GetTickCount(); 
// 		static DWORD time1 = GetTickCount();
		if (mStandTime != 0 && time - mStandTime >= 5000)
		{
			setState(GOS_IDLE);
			mStandTime = 0;
// 			time1 = time;
		}
	}
	bool ret = Player::update();
	//LOGMSG(mPlayerBD.account + " " + Ogre::StringConverter::toString(getState()) + " " + Ogre::StringConverter::toString(getActionState()));
	bool GotoUIOpenedFlag = luabind::call_function<bool>(sLuaMgr.getL(), "Goto_getUIOpenedFlag");
	bool HouseSalesUIOpenedFlag = luabind::call_function<bool>(sLuaMgr.getL(), "HouseSales_getUIOpenedFlag");
	bool PworkUIOpenedFlag = luabind::call_function<bool>(sLuaMgr.getL(), "PWork_getUIOpenedFlag");
	bool PchooseworkUIOpenedFlag = luabind::call_function<bool>(sLuaMgr.getL(), "PWork_getchooseWorkUIOpenedFlag");
	bool BeautyStartFlag = luabind::call_function<bool>(sLuaMgr.getL(), "Beauty_getStartFlag");
	bool DomesticServiceFlag = luabind::call_function<bool>(sLuaMgr.getL(), "getDomesticServiceFlag");

	if ( !FitmentInteractMgr::getSingleton().isPreparing()
		&& !FitmentInteractMgr::getSingleton().isInteracting()
		&& !sEQGameSceneMgr.getIsLoading()
		&& !sFitMgr.isFiting()
		&& !GotoUIOpenedFlag
		&& !HouseSalesUIOpenedFlag
		&& !PworkUIOpenedFlag
		&& !PchooseworkUIOpenedFlag
		&& !BeautyStartFlag
		&& !DomesticServiceFlag
		&& sEQGameSceneMgr.getSceneType() != EQGameSceneMgr::SCENE_HOUSE_PREVIEW
		&& sEQGameSceneMgr.getSceneType() != EQGameSceneMgr::SCENE_NONE
		&& sEQGameSceneMgr.getSceneType() != EQGameSceneMgr::SCENE_FARM)
	{
		if(!ret)
		{
			DWORD now = GetTickCount(); 
			static DWORD last_time = GetTickCount(); 
			float time_interval = VariableSystem::getSingleton().GetAs_Float("RandPlayActionTime");
			if ( now - last_time >= time_interval)
			{
				if (getState() == GOS_SIT_IDLE)
				{
					//mpBodyActionMgr->reqStartBodyAction("sit_lookaround","",false);
					luabind::call_function<void>(sLuaMgr.getL(), "Action_randPlaySitAnima");
				}
				else if (getState() == GOS_IDLE)
				{
					luabind::call_function<void>(sLuaMgr.getL(), "Action_randPlayStandAnima");
				}
				last_time = now;
			}

			return false;
		}
		else
			return true;
	}
	else
		return ret;
}
//--------------------------------------------------------------------
bool MainPlayer::updateMove()
{
	if( GameStateMgr::getSingleton().getState() != GS_MAINGAME)
		return false;
	
	float cam_zoom = EQGameMgr::getSingleton().getMainCamera()->getZoom();
	float zDistance = VariableSystem::getSingleton().GetAs_Float("Zoom_hideObjDistance");
	if (cam_zoom < zDistance)
	{
		setVisible(false);
		setHeadUpVisible(false);
	}
	else
	{
		if (sEQGameSceneMgr.getSceneType() != EQGameSceneMgr::SCENE_HOUSE_PREVIEW)
		{
			setVisible(true);		// 此处关系到室内角色的隐藏(hezhr)
			setHeadUpVisible(true);
		}
	}
	//
// 	DWORD now = GetTickCount(); 
// 	static DWORD last_time = GetTickCount(); 
	if (mMoveType == GOMT_RUN/*luabind::call_function<bool>(sLuaMgr.getL(), "MainUI_getIsRunFlag" )*/)
	{
		NORMAL_WALK_SPEED = 400;
	}
	else
	{
		NORMAL_WALK_SPEED = 150;
	}
	if(!mMoveStart)
	{
		return false;
	}

	if( mTargetDist <= NORMAL_WALK_SPEED 
		* EQGameMgr::getSingleton().getTimElapsed() )
	{
		if(false == _moveStep())
		{
			setPos( mTargetWalk );
			//setState(GOS_STAND_IDLE);
			mMoveStart = false;
			if(mTargetName!="")
			{
				GameObj* obj = sObjMgr.findObj((ObjMgr::GAMEOBJ_TYPE)mTargetType, mTargetName);
				if(obj
					&& mTargetType == ObjMgr::GOT_NPC)
				{
					((NPC*)obj)->onClickObj();
				}
			}
			reqStopWalk();
		}
	}
	else if(mTargetDist > NORMAL_WALK_SPEED 
		* EQGameMgr::getSingleton().getTimElapsed())
	{
		_walk();

		static DWORD oldtime	= GetTickCount();
		DWORD timenow			= GetTickCount();
#define SYN_POS_TIMER 500
		if( timenow - oldtime > SYN_POS_TIMER )
		{
			syncPosToServer();
			oldtime = timenow;
		}
		//
		_checkTeleport();
		//
		GameObj* obj = (ObjMgr::getSingleton().getPickObj());
		if(obj 
			&& obj->getType() == ObjMgr::GOT_NPC)
		{
			Ogre::Real dis  = obj->getPos().distance(getPos());
			float npcdis = VariableSystem::getSingleton().GetAs_Float("NpcDistance");
			if(dis > npcdis)
			{
				luabind::call_function<void>(sLuaMgr.getL(), "DBS_exitDBS");
				((NPC*)obj)->closeDia();
				((NPC*)obj)->resetOrient();
			}
		}
	}

	//目标动画
	_TargetPoint();

	return true;
}
//-----------------------------------------------------------------
void MainPlayer::_checkTeleport()
{
	int tid = 0;
	if( (tid = EQGameSceneMgr::getSingleton().checkTeleportID()) != 0 )
	{
		stopWalk(true);
		uint copy_id = luabind::call_function<uint>(sLuaMgr.getL(), "Goto_getCopyID");
		req_teleport req;
		req.id = tid;
		req.copy_id = copy_id;
		sGameMgr.getEQMsgManager()->Send(req);
		luabind::call_function<uint>(sLuaMgr.getL(), "Goto_setCopyID",0);
	}
}
//-----------------------------------------------------------------
void MainPlayer::setCeateEquip(uint a,uint b,uint c,uint d,uint e,uint f,uint g,uint h)
{
	mCreateEquip.curHatEquip	= a;
	mCreateEquip.curClothEquip	= b;
	mCreateEquip.curGlassesEquip	= c;
	mCreateEquip.curTrouserEquip	= d;
	mCreateEquip.curBackEquip	= e;
	mCreateEquip.curShoesEquip	= f;
	mCreateEquip.curHandEquip	= g;
	mCreateEquip.curTrinketEquip	= h;

}
//-----------------------------------------------------------------
bool MainPlayer::setPath(std::queue<Ogre::Vector3> &path, MainPlayerListener* listener)
{
	setListener(listener);
	if(mListener)
		mListener->moveBegin();

	bool haspath = GameObj::setPath(path);

	if(!haspath)
	{
		if(mListener)
			mListener->moveEnd();
	}

	return haspath;
}
//-------------------------------------------------------
void MainPlayer::setListener(MainPlayerListener* listener)
{
	if(mListener)
	{
		mListener->exit();
		delete mListener;
		mListener = NULL;
	}

	if(listener)
		mListener = listener;
}
//-------------------------------------------------------
void MainPlayer::reqStartWalk(Ogre::Vector3& dest)
{
	Ogre::Vector3 pos = FitmentInteractMgr::getSingleton().isInteracting() ? 
							FitmentInteractMgr::getSingleton().getInteractPos() : getPos();
	//mpBodyActionMgr->reqEndBodyAction();
// 	LOGMSG(Ogre::StringConverter::toString(BodyActionMgr::getSingleton().getActionState()));
	setActionState(0);
	stopAnimation();
	sGameMgr.getEQMsgManager()->StartWalkRequest(pos, dest,mMoveType);
}
//-------------------------------------------------------
void MainPlayer::reqContinueWalk()
{
	sGameMgr.getEQMsgManager()->StartWalkRequest(getPos(), mCurDestPos,mMoveType);
}
//-------------------------------------------------------
void MainPlayer::reqStopWalk()
{
	sGameMgr.getEQMsgManager()->StopWalkRequest( getPos() );
	//sGameMgr.getEQMsgManager()->MoveSyncPosition( getPos() );
}
//-------------------------------------------------------
void MainPlayer::syncPosToServer()
{
	sGameMgr.getEQMsgManager()->MoveSyncPosition( getPos() );
}
//-------------------------------------------------------
void MainPlayer::reqPlayBigExpression(int id)
{
	req_chat_big_expression req;
	req.exp_id = id;
	sGameMgr.getEQMsgManager()->Send(req);
}
//-------------------------------------------------------
void MainPlayer::_TargetPoint()
{
 	if(mArrowAnimState && mCircleAnimState)
	{
 		mArrowAnimState->addTime(EQGameMgr::getSingleton().getTimElapsed());
		mCircleAnimState->addTime(EQGameMgr::getSingleton().getTimElapsed());
	}

	if( getPos() == mTargetWalk || sStateMgr.isFunnyFarm())
	{
		if(mTargetNode)
		{
			mTargetNode->setVisible(false);
		}
	}
}
//-------------------------------------------------------
void MainPlayer::LButtonUp	( UINT& flags, POINT point ) 
{ 
	if(!sStateMgr.inMainGame() || 
		sEQGameSceneMgr.getSceneType() == EQGameSceneMgr::SCENE_HOUSE_PREVIEW)
		return;
	if(sInputMgr.mLMouseDraged)
		return;
	if(sFitMgr.isFiting())
		return;
	if (	( flags & epfPick)!= 0		// pick处理过了？
		||	( flags & epfUI ) != 0		// ui ?
		||  ( flags & epfDragItem) != 0 //
		||  sCursorMgr.getSpliting()
		)
		return;

	//主游戏普通模式
	Ogre::Camera* cam = EQGameMgr::getSingleton().getMainCamera()->getOgreCam();

	Ogre::Ray ray = cam->getCameraToViewportRay(
		(Ogre::Real)point.x / EQWndProc::getSingleton().getWndWidth(), 
		(Ogre::Real)point.y / EQWndProc::getSingleton().getWndHeight());

	Ogre::Vector3 inster(0,0,0);

	//bool is;

	//is =  ray.intersects(Ogre::Plane (Ogre::Vector3::UNIT_Y, 0)).first;

	//if( false == is)
	//	return;

	//Ogre::Real rsec= ray.intersects(Ogre::Plane (Ogre::Vector3::UNIT_Y, 0)).second;


	//Ogre::Vector3 ptIntersect = ray.getPoint(rsec);

	// 选择寻路系统
	Ogre::Vector3 startPos = FitmentInteractMgr::getSingleton().isInteracting() ? 
		FitmentInteractMgr::getSingleton().getInteractPos() : getPos();

	EQGameSceneMgr::eSceneType curType = EQGameSceneMgr::getSingleton().getSceneType();
	if(curType == EQGameSceneMgr::SCENE_HOUSE)
	{
		//std::pair<bool, float> rt = ray.intersects(Ogre::Plane (Ogre::Vector3::UNIT_Y, 0));
		//if ( false == rt.first )
		//	return;
		//Ogre::Vector3 ptIntersect = ray.getPoint(rt.second);

		// 寻路 获得起始位置和终点位置的格子

		HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();
		Ogre::Vector3 endPos;
		HouseSceneGrid * endGrid = NULL;
		if(sce)
		{
			if (sce->rayGrid(ray, endGrid, endPos, false))
			{
				std::queue<Ogre::Vector3> path;
				findPath(startPos, endPos, path);

				if(!path.empty())
				{
					// disable by hjx 是否停止，由服务端判定
					//if (getState() == GOS_WALK)
					//	reqStopWalk();
					//reqStartWalk(ptIntersect);//moveTo(ptIntersect);
					mCurDestPos = path.back();
					reqStartWalk(mCurDestPos);
				}
			}
		}
		//setPath(path);
	}
	else if ( curType == EQGameSceneMgr::SCENE_COMMON )
	{
		Ogre::Vector3 pos;
		//改成和水平面碰撞
// 		if ( !EQSceneManager::getSingleton().getTerrainData()->getIntersects(ray, pos) )
// 			return;
		Ogre::Plane pla( Ogre::Vector3::UNIT_Y, 0 );
		std::pair<bool, float> rt = Ogre::Math::intersects(ray, pla);
		if ( rt.first )
			pos = ray.getPoint( rt.second );

		SearchSystem::Vector2f s(startPos.x, startPos.z);
		SearchSystem::Vector2f e(pos.x, pos.z);
		if (EQGameSceneMgr::getSingleton().getNavSearchSystem()->findPath(s,e))
		{
			mCurDestPos = pos;
			reqStartWalk(mCurDestPos);
		}
	}
	else	//现在这个条件应该不会发生了
	{
		std::pair<bool, float> rt = ray.intersects(Ogre::Plane (Ogre::Vector3::UNIT_Y, 0));
		if ( false == rt.first )
			return;
		Ogre::Vector3 ptIntersect = ray.getPoint(rt.second);

		// disable by hjx 是否停止，由服务端判定
		//if (getState() == GOS_WALK)
		//	reqStopWalk();
		mCurDestPos = ptIntersect;
		reqStartWalk(mCurDestPos);//moveTo(ptIntersect);
	}
	clearTarget();
}
//-------------------------------------------------------
void MainPlayer::enableAutoUpDown(const Ogre::Vector3& curr, const Ogre::Vector3& dest)
{
	if (EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_HOUSE)
	{
		if (sObjMgr.getPosFloorNO(curr)==sObjMgr.getPosFloorNO(dest) && !sObjMgr.isPosInStair(curr))
		{
			mAutoUpDown = true;
		}
		else
		{
			mAutoUpDown = false;
		}
	}
}
//-------------------------------------------------------
void MainPlayer::autoUpDown(void)
{
	if (EQGameSceneMgr::getSingleton().getSceneType() == EQGameSceneMgr::SCENE_HOUSE)
	{
		// 条件不满足，返回
		if (!sObjMgr.isPosInStair(getPos()) || !mAutoUpDown)
		{
			return;
		}
		// 自动上下楼
		switch (sObjMgr.getViewFloorNO())
		{
		case 1:		// 观察楼层为楼下
			{
				if (sObjMgr.isPosInViewFloor(getPos()))
				{
					mCurDestPos = Ogre::Vector3(120, 290, -280);
					sGameMgr.getEQMsgManager()->StartWalkRequest(getPos(), mCurDestPos,mMoveType);	// 上2楼
				}
				else
				{
					mCurDestPos = Ogre::Vector3(440, 0, -120);
					sGameMgr.getEQMsgManager()->StartWalkRequest(getPos(), mCurDestPos,mMoveType);	// 下1楼
				}
			}
			break;
		case 2:		// 观察楼层为楼上
			{
				if (sObjMgr.isPosInViewFloor(getPos()))
				{
					mCurDestPos = Ogre::Vector3(440, 0, -120);
					sGameMgr.getEQMsgManager()->StartWalkRequest(getPos(), mCurDestPos,mMoveType);	// 下1楼
				}
				else
				{
					mCurDestPos = Ogre::Vector3(120, 290, -280);
					sGameMgr.getEQMsgManager()->StartWalkRequest(getPos(), mCurDestPos,mMoveType);	// 上2楼
				}
			}
			break;
		default:
			break;
		}
	}
}
//-------------------------------------------------------
void MainPlayer::stuffedBear(void)
{
	switch (sObjMgr.getViewFloorNO())
	{
	case 1:		// 观察楼层为楼下
		if (!sObjMgr.isPosInViewFloor(getPos()))
		{
			EQSceneManager::getSingleton().getHouseScene()->upView();
			luabind::call_function<void>(sLuaMgr.getL(), "MainUI_setViewBtn", true);
		}
		break;
	case 2:		// 观察楼层为楼上
		if (!sObjMgr.isPosInViewFloor(getPos()))
		{
			EQSceneManager::getSingleton().getHouseScene()->downView();
			luabind::call_function<void>(sLuaMgr.getL(), "MainUI_setViewBtn", false);
		}
		break;
	default:
		break;
	}
	luabind::call_function<void>(sLuaMgr.getL(), "OnUpDownFloor");
}
//-------------------------------------------------------
void MainPlayer::showTargetPath(Ogre::Vector3& pos)
{
	if(mTargetNode)
	{
		mTargetNode->setPosition(pos);
		mTargetNode->setVisible(true);
	}
}

void MainPlayer::hideTargetPath()
{
	if(mTargetNode)
		mTargetNode->setVisible(false);
}

//---------------------------------------------------------------------------------
void MainPlayer::clearTarget()
{
	GameObj* obj = sObjMgr.findObj((ObjMgr::GAMEOBJ_TYPE)mTargetType, mTargetName);
	if(obj)
		obj->delSelectedEffect();

	mTargetName = "";
}
//---------------------------------------------------------------------------------
void MainPlayer::setTarget(std::string& name, uint type)
{
	//if(name == mTarget)
	//	return;

	clearTarget();

	GameObj* obj = sObjMgr.findObj((ObjMgr::GAMEOBJ_TYPE)type, name);
	if(obj)
	{
		obj->addSelectedEffect();
		mTargetName = name;
		mTargetType = type;
	}
}
//------------------------------------------------------------------------------
void MainPlayer::setFlowerCount(uint flower)
{
	mFlowerCount = flower;
}
//------------------------------------------------------------------------------
void MainPlayer::setDecoration(uint decoration)
{
	mDecoration = decoration;
}
//------------------------------------------------------------------------------
void MainPlayer::setFlowerToday(notify_house_flower_change& flower)
{
	if(flower.house_owner == getAccount())
	{
		mFlowerToday = flower.flower_count;
		luabind::call_function<void>(sLuaMgr.getL(), "updateFlowerEggToday");
	}
	else
	{
		luabind::call_function<void>(sLuaMgr.getL(), "Fne_setFlower", flower.flower_count);
	}
}
//------------------------------------------------------------------------------
void MainPlayer::setEggToday(notify_house_egg_change& egg)
{
	if(egg.house_owner == getAccount())
	{
		mEggToday = egg.egg_count;
		luabind::call_function<void>(sLuaMgr.getL(), "updateFlowerEggToday");
	}
	else
	{
		luabind::call_function<void>(sLuaMgr.getL(), "Fne_setEgg", egg.egg_count);
	}
}
//------------------------------------------------------------------------------
void MainPlayer::showFlowerLog(notify_flower_logs& log)
{
	luabind::call_function<void>(sLuaMgr.getL(), "FlowerEggLog_ClearFlowerLog");
	luabind::call_function<void>(sLuaMgr.getL(), "FlowerEggLog_OpenFlowerLog");
	for(uint i=0; i<log.logs.size(); i++)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "FlowerEggLog_InsertFlowerLog", log.logs[i].time.hour, log.logs[i].time.minute, log.logs[i].who);
	}
}
//------------------------------------------------------------------------------
void MainPlayer::showEggLog(notify_egg_logs& log)
{
	luabind::call_function<void>(sLuaMgr.getL(), "FlowerEggLog_ClearEggLog");
	luabind::call_function<void>(sLuaMgr.getL(), "FlowerEggLog_OpenEggLog");
	for(uint i=0; i<log.logs.size(); i++)
	{
		luabind::call_function<void>(sLuaMgr.getL(), "FlowerEggLog_InsertEggLog", log.logs[i].time.hour, log.logs[i].time.minute, log.logs[i].who);
	}
}
//------------------------------------------------------------------------------
void MainPlayer::showFne(notify_today_flower_egg_count& fne)
{
	luabind::call_function<void>(sLuaMgr.getL(), "Fne_Open", fne.flower_count, fne.egg_count);
}
//------------------------------------------------------------------------------
#include "OtherPlayerRTT.h"
#include "AvatarAdjuster.h"
void MainPlayer::showFlowerAni(notify_send_flower_success& data)
{
	const uint MAX_COUNT = 8;
	int equipArray[MAX_COUNT];

	for (size_t i = 0; i < data.body.size(); i++)
	{
		equipArray[i] = data.body[i].item_data.template_id;
		luabind::call_function<void>(sLuaMgr.getL(), "lua_getEquipParam",i, data.body[i].item_data.template_id);
	}
	int sex = EQGameMgr::getSingleton().getOtherPlayerRTT()->getAvatar()->getAdjuster()->getSex();
	if(sex != data.player.basic_data.sex)
	{
		EQGameMgr::getSingleton().getOtherPlayerRTT()->changeSex(data.player.basic_data.sex);
		EQGameMgr::getSingleton().getOtherPlayerRTT()->rttScale(0.9, 0.9, 0.9);
	}
	EQGameMgr::getSingleton().getOtherPlayerRTT()->syncPlayerEquip1(equipArray);
	EQGameMgr::getSingleton().getOtherPlayerRTT()->getRTTCamera()->lookAt(Ogre::Vector3(500, 50075, 500));

	OtherPlayerRTT* rtt = EQGameMgr::getSingleton().getOtherPlayerRTT();
	Avatar*avatar = rtt->getAvatar();
	AvatarAdjuster* adj = avatar->getAdjuster();

	adj->doChange(AvatarAdjuster::atHairType, data.player.basic_data.hair);
	adj->doChange(AvatarAdjuster::atHairColor, data.player.basic_data.hair_color);
	adj->doChange(AvatarAdjuster::atFace,data.player.basic_data.face);
	adj->doChange(AvatarAdjuster::atSkin, data.player.basic_data.skin_color);
	adj->doChange(AvatarAdjuster::atBeard, data.player.basic_data.beard);

	luabind::call_function<void>(sLuaMgr.getL(), "showFlowerAnimate");
}
//------------------------------------------------------------------------------
void MainPlayer::showEggAni(notify_send_egg_success& data)
{
	const uint MAX_COUNT = 8;
	int equipArray[MAX_COUNT];

	for (size_t i = 0; i < data.body.size(); i++)
	{
		equipArray[i] = data.body[i].item_data.template_id;
		luabind::call_function<void>(sLuaMgr.getL(), "lua_getEquipParam",i, data.body[i].item_data.template_id);
	}
	int sex = EQGameMgr::getSingleton().getOtherPlayerRTT()->getAvatar()->getAdjuster()->getSex();
	if(sex != data.player.basic_data.sex)
	{
		EQGameMgr::getSingleton().getOtherPlayerRTT()->changeSex(data.player.basic_data.sex);
		EQGameMgr::getSingleton().getOtherPlayerRTT()->rttScale(0.9, 0.9, 0.9);
	}
	EQGameMgr::getSingleton().getOtherPlayerRTT()->syncPlayerEquip1(equipArray);
	EQGameMgr::getSingleton().getOtherPlayerRTT()->getRTTCamera()->lookAt(Ogre::Vector3(500, 50075, 500));

	OtherPlayerRTT* rtt = EQGameMgr::getSingleton().getOtherPlayerRTT();
	Avatar*avatar = rtt->getAvatar();
	AvatarAdjuster* adj = avatar->getAdjuster();

	adj->doChange(AvatarAdjuster::atHairType, data.player.basic_data.hair);
	adj->doChange(AvatarAdjuster::atHairColor, data.player.basic_data.hair_color);
	adj->doChange(AvatarAdjuster::atFace,data.player.basic_data.face);
	adj->doChange(AvatarAdjuster::atSkin, data.player.basic_data.skin_color);
	adj->doChange(AvatarAdjuster::atBeard, data.player.basic_data.beard);

	luabind::call_function<void>(sLuaMgr.getL(), "showEggAnimate");
}
//------------------------------------------------------------------------------
void MainPlayer::resetDaily(notify_new_day_reset& reset)
{
	mFlowerToday = 0;
	mEggToday = 0;
	luabind::call_function<void>(sLuaMgr.getL(), "updateFlowerEggToday");
	luabind::call_function<void>(sLuaMgr.getL(), "FlowerEggLog_ClearFlowerLog");
	luabind::call_function<void>(sLuaMgr.getL(), "FlowerEggLog_ClearEggLog");
	luabind::call_function<void>(sLuaMgr.getL(), "Fne_setFlower", 0);
	luabind::call_function<void>(sLuaMgr.getL(), "Fne_setEgg", 0);
}
//------------------------------------------------------------------------------