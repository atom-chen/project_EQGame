#include "EQCommPCH.h"
#include "GameObj.h"
#include "Avatar.h"
#include "GameObj.h"
#include "EQGameMgr.h"
#include "EQMsgManager.h"
#include "HouseScene.h"
#include "SearchSystem/NavSearchSystem.h"
#include "SceneManager.h"
#include "EQGameSceneMgr.h"
#include "TerrainData.h"
#include "EQOgreSys.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "BodyActionMgr.h"
#include "SearchSystem/IndoorPath.h"
#include "FitmentInteractMgr.h"


float GameObj::NORMAL_WALK_SPEED = 200;
float GameObj::NORMAL_RUN_SPEED = 400;

GameObj::GameObj(void)
: mAvatar(0)
, mTargetWalk(Ogre::Vector3::ZERO)
, mTargetDist(0)
, mWalkDirection(Ogre::Vector3::ZERO)
, mMainNode(0)
, mMainModel(0)
, mMoveStart(false)
, mScenename("")
, mModelHeight(0)
, mEffect(NULL)
, mAnimState(NULL)
,mStandTime(0)
,mpBodyActionMgr(new BodyActionMgr())
,mMoveType(GOMT_WALK)
{
	mProxy.setObject(this);
}
//
void GameObj::reset()
{
	mMoveStart = false;
	mTargetDist = 0;
	mMainNode->resetOrientation();
}
//--------------------------------------------------------------------
void GameObj::cleanup()
{
	if(mAvatar)
		delete mAvatar;

	delSelectedEffect();
}
//---------------------------------------------------------------------
GameObj::~GameObj(void)
{
	if (mpBodyActionMgr != NULL)
	{
		delete mpBodyActionMgr;
	}
}
//---------------------------------------------------------------------
const std::string& GameObj::getClassName()
{
	static const std::string name = "GameObj";
	return name;
}
//  [4/28/2010 huangdj1]
void GameObj::updateModel()
{
	mMainModel = mAvatar->getModel();
	mMainModel->setPickObjectProxy(&mProxy);
	mMainNode  = mMainModel->getMainNode();
	mMainModel->enableAnimation(true);
}
//--------------------------------------------------------------------
void GameObj::createAvatar(int id)
{
	if(mAvatar)
		return;

	mAvatar = new Avatar(id);

	updateModel();

	setPos(Ogre::Vector3(0,0,0));
	setState(GOS_IDLE);
	setVisible(true);
}
//--------------------------------------------------------------------
void GameObj::setPos(Ogre::Vector3& v)
{
	mMainNode->setPosition(v);
	_setPos(v);
}
//--------------------------------------------------------------------
void GameObj::moveTo(Ogre::Vector3& dest)
{
	mMoveStart  = true;
	mTargetWalk  = dest;

	_setDirectionLength(dest);

	mTargetDist = mBaseData.pos.distance(mTargetWalk) ;
}
//--------------------------------------------------------------------
void GameObj::setState(GameObjState state,std::string anima)
{
// 	if (state != GOS_IDLE)
	{
		_setState(state);
	}
	//std::string anima = "";
	switch (state)
	{
	case GOS_WALK:
		anima = "Walk";
		mMainModel->modifyAnimationRate(NORMAL_WALK_SPEED/120);
		break;
	case GOS_RUN:
		anima = "run";
		mMainModel->modifyAnimationRate(150.0f/120);
		break;
	case GOS_IDLE:
		//anima = "Stand";
		anima = mCurIdleActionName;
		mMainModel->modifyAnimationRate(1.0f);
		break;
	case GOS_STAND_IDLE:
		anima = luabind::call_function<std::string>(sLuaMgr.getL(), "Action_randSelectIdleAction");
		mCurIdleActionName = anima;
		mStandTime = GetTickCount();
		mMainModel->modifyAnimationRate(1.0f);
		break;
	case GOS_USEFURNI_INTERVAL:
		anima = "Stand";
		mMainModel->modifyAnimationRate(1.0f);
		break;
	case GOS_SIT_IDLE:
		anima = "sit_1";
		break;
	case GOS_LIE_IDLE:
		anima = "lie_1";
		break;
	case GOS_ANIMATION:
		//anima = "Stand001";
		//mMainModel->_getAnimationStateSet()->
		//mMainModel->modifyAnimationRate(1.0f);
		//break;
		/**
		*/
		setActionState(anima);
		forwardSetState(anima);
		break;
	}
	mMainModel->setDefAnimation(anima);
}
//------------------------------------------------------------------------------
void GameObj::forwardSetState(std::string& anima)
{
	if (anima == "Sit" || anima == "sit_lookaround" || anima == "lie_sit")
	{
		anima = "sit_1";
	}
	else if(anima == "Lie" || anima == "sit_lie")
	{
		anima = "lie_1";
	}
	else
	{
		anima = "Stand";
	}
}
//------------------------------------------------------------------------------
float GameObj::getYaw(void)
{
	return mMainNode->getOrientation().getYaw().valueDegrees();
}
//------------------------------------------------------------------------------
void GameObj::stopAnimation()
{
	playAnima("");
	setActionState(0);
 	//setState(mBaseData.state);
// 	setState(GOS_IDLE);
}
//-------------------------------------------------------
DWORD GameObj::getAnimationLength(const std::string & ani)
{
	return mMainModel->getAnimationLength(ani);
}
//------------------------------------------------------------------------------
void GameObj::playAnima(std::string ani,bool loop)
{
	mMoveStart = false;
	mTargetWalk = getPos();
	mMainModel->playAnimation(ani,loop);
}
//------------------------------------------------------------------------------
std::string	GameObj::getCurrentAnima()
{
	return mMainModel->getCurrAnimationName();
}
//------------------------------------------------------------------------------
void GameObj::setDefAnimation(const std::string& ani)
{
	mMainModel->setDefAnimation(ani);
}
//------------------------------------------------------------------------------
void GameObj::_setDirectionLength(Ogre::Vector3& tarPos)
{
	mWalkDirection = tarPos - mBaseData.pos;
	mWalkDirection.normalise();
// 	mWalkDirection = mWalkDirection * NORMAL_WALK_SPEED;

	Ogre::Vector3 vec = mWalkDirection;
	vec.y = 0;
	mMainNode->setDirection(vec, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_X);
}
//------------------------------------------------------------------------------
void GameObj::_walk()
{
	if (mMoveType == GOMT_RUN/*luabind::call_function<bool>(sLuaMgr.getL(), "MainUI_getIsRunFlag" )*/)
	{
		if(getState() != GOS_RUN)
			setState(GOS_RUN);
	}
	else
	{
		if(getState() != GOS_WALK)
			setState(GOS_WALK);
	}

	Ogre::Vector3 pos = mBaseData.pos + (mWalkDirection  * NORMAL_WALK_SPEED) * EQGameMgr::getSingleton().getTimElapsed();

	EQGameSceneMgr::eSceneType curType = EQGameSceneMgr::getSingleton().getSceneType();
	if ( curType == EQGameSceneMgr::SCENE_COMMON )	//贴地面
		//现在不按地形高度，统一为0
		pos.y = 0;
		//pos.y = EQSceneManager::getSingleton().getTerrainData()->getHeightAt(pos.x, pos.z);

	setPos(pos);


	mTargetDist -= NORMAL_WALK_SPEED * EQGameMgr::getSingleton().getTimElapsed();

}
//------------------------------------------------------------------------------
bool GameObj::update()
{
	if(mAnimState)
		mAnimState->addTime(EQGameMgr::getSingleton().getTimElapsed());
	mpBodyActionMgr->update();
	return updateMove();
}
//------------------------------------------------------------------------------
bool GameObj::updateMove()
{
	if (mMoveType == GOMT_WALK)
	{
		NORMAL_WALK_SPEED = 150;
	}
	else
	{
		NORMAL_WALK_SPEED = 400;
	}
	if(!mMoveStart)
	{
		return false;
	}

	if( mTargetDist <= NORMAL_WALK_SPEED * EQGameMgr::getSingleton().getTimElapsed() )
	{
		if(true == _moveStep())
		{	
			return true;//false;都要發停止請求
		}
		else
		{
			setPos( mTargetWalk );
			setState(GOS_STAND_IDLE);
			mMoveStart = false;

			return true;
		}
	}
	else if(mTargetDist > NORMAL_WALK_SPEED * EQGameMgr::getSingleton().getTimElapsed())
	{
 		_walk();
		return false;
	}
	return false;
}
//------------------------------------------------------------------------------
bool GameObj::_moveStep()
{
	if(!mPathqueue.empty())
	{
		EQGameSceneMgr::eSceneType curType = EQGameSceneMgr::getSingleton().getSceneType();
		if(curType == EQGameSceneMgr::SCENE_HOUSE)
		{
			//在房屋内的寻路切换楼上楼下会出现瞬移. 所以要区别对待
			//目前不考虑连续瞬移的情况, 这种情况应该让策划避免
			HouseScene * sce = EQSceneManager::getSingleton().getHouseScene();
			assert(sce);

			Ogre::Vector3 p1 = mPathqueue.front();
			mPathqueue.pop();

			HouseSceneGrid* gc = sce->gridByPosition(getPos());
			HouseSceneGrid* g1 = sce->gridByPosition(p1);

			if ( gc && g1 )	//2个网格都能找到
			{
				int fc = gc->getCreator()->getFloorNO();
				int f1 = g1->getCreator()->getFloorNO();
				if (fc == f1)	//同一个楼层
				{
					moveTo(p1);
					return true;
				}
				else			//不同楼层
				{
					setPos(p1);	//瞬移
					stuffedBear();	// 瞬移后，可能需要相关的操作(2011/6/13 add by hezhr)
					if(!mPathqueue.empty())		//有目标点, 设置下个目标点. tip:如果下个目标点还是不同楼层, 则角色会浮空行走, 这个需要策划避免
					{
						moveTo(mPathqueue.front());
						mPathqueue.pop();
						return true;
					}
					else						//如果没目标点, false
					{
						mTargetWalk = p1;	// 设置角色停止时的位置(2011/7/6 add by hezhr)
						return false;
					}
				}
			}
			else	//如果是错误位置, 或者浮空状态, 导致gc或者个g1是空
			{
				moveTo(p1);
				return true;
			}
		}
		else	//原来的情况
		{
			moveTo(mPathqueue.front());
			mPathqueue.pop();
			return true;
		}
		
	}
	return false;
}
//------------------------------------------------------------------------------
bool GameObj::setPath(std::queue<Ogre::Vector3> &path)
{
	if(!path.empty())
	{
		mPathqueue = path;
		return _moveStep();	// (2011/7/6 modify by hezhr)
	}

	return false;
}
//------------------------------------------------------------------------------
void GameObj::equipOn( uint slot, uint equipDisplayID, uint holdSlot )
{
	mAvatar->equipOn(slot, equipDisplayID, holdSlot);
}
//------------------------------------------------------------------------------
void GameObj::takeOff( uint slot )
{
	mAvatar->takeOff(slot);
}
//------------------------------------------------------------------------------
void GameObj::stopWalk(bool ifNotifySev)
{ 
	//while(!mPathqueue.empty()) 
	//	mPathqueue.pop();
	
	mTargetDist = 0.f;
	mTargetWalk = getPos();
	mMoveStart = false;
	
	// 停止走路动画	
	if (FitmentInteractMgr::getSingleton().isPreparing())
	{
		this->setState(GOS_USEFURNI_INTERVAL);
	}
	else
		this->setState(GOS_STAND_IDLE);

	if(ifNotifySev)
		this->reqStopWalk();
}


//------------------------------------------------------------------------------
void GameObj::findPath(const Ogre::Vector3 &startPos, const Ogre::Vector3 &endPos, std::queue<Ogre::Vector3> &path)
{
	EQGameSceneMgr::eSceneType curType = EQGameSceneMgr::getSingleton().getSceneType();
	if(curType == EQGameSceneMgr::SCENE_HOUSE)
	{
		IndoorPath::findPath(startPos, endPos, path, EQSceneManager::getSingleton().getHouseScene());		
	}
	else if ( curType == EQGameSceneMgr::SCENE_COMMON )
	{
		SearchSystem::Vector2f s(startPos.x, startPos.z);
		SearchSystem::Vector2f e(endPos.x, endPos.z);
		if (!EQGameSceneMgr::getSingleton().getNavSearchSystem()->findPath(s,e))
			return;

		std::vector<SearchSystem::Vector2f> pa;
		EQGameSceneMgr::getSingleton().getNavSearchSystem()->getPath(pa);
		if ( pa.empty() )
			return;
		for ( size_t i = 1; i < pa.size(); ++i )	//起始的坐标不需要
		{
			Ogre::Vector3 v(pa[i].x, 0, pa[i].y);
			//现在不按地形高度，统一为0
			//v.y = EQSceneManager::getSingleton().getTerrainData()->getHeightAt(v.x, v.z);
			path.push(v);
		}
	}
	else	//应该没这情况
	{
		path.push(endPos);		
	}

}
//------------------------------------------------------------------------------
void GameObj::addSelectedEffect()
{
	if(!mEffect) 
	{
		mEffect = EQOgreSys::getSingleton().getSceneManager()->createManualObject("Selected");
		mEffect->setCastShadows(false);

		// ModelGcs::attachToLocator 没接口~~~就先这么做了
		Ogre::SceneNode* node = mMainNode->createChildSceneNode();
		node->attachObject(mEffect);

		// 大小先写死，以后统一改
		int radius = 48;
		mEffect->begin("tar001", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		mEffect->getSection(0)->getMaterial()->setReceiveShadows(false);
		mEffect->position(-radius, 1, -radius);
		mEffect->textureCoord(0, 0);
		mEffect->position(-radius, 1, radius);
		mEffect->textureCoord(0, 1);
		mEffect->position(radius, 1, radius);
		mEffect->textureCoord(1, 1);
		mEffect->position(radius, 1, -radius);
		mEffect->textureCoord(1, 0);
		mEffect->quad(0,1,2,3);
		mEffect->end();

		Ogre::Animation* anim = EQOgreSys::getSingleton().getSceneManager()->createAnimation("Selected", 1.5);
		anim->setInterpolationMode(Ogre::Animation::IM_SPLINE);
		Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, node);
		Ogre::TransformKeyFrame* key = track->createNodeKeyFrame(0);
		key = track->createNodeKeyFrame(0.75);
		key->setScale(Ogre::Vector3(0.6,0.6,0.6));
		key = track->createNodeKeyFrame(1.5);
		key->setScale(Ogre::Vector3(1,1,1));
		mAnimState = EQOgreSys::getSingleton().getSceneManager()->createAnimationState("Selected");
		mAnimState->setEnabled(true);
	}
}
//------------------------------------------------------------------------------
void GameObj::delSelectedEffect()
{
	if(mEffect)
	{
		EQOgreSys::getSingleton().getSceneManager()->destroyManualObject(mEffect);
		EQOgreSys::getSingleton().getSceneManager()->destroyAnimation("Selected");
		EQOgreSys::getSingleton().getSceneManager()->destroyAnimationState("Selected");
		mAnimState = NULL;
		mEffect = NULL;
	}
}
//------------------------------------------------------------------------------
void GameObj::setVisible(bool visible)
{
	if (mMainModel)
	{
		mMainModel->setVisible(visible);
	}
}
//------------------------------------------------------------------------------
Ogre::Real GameObj::getDistanceToTarget(const Ogre::Vector3& dest)
{
	return getPos().distance(dest);	
}
//-------------------------------------------------------------------------
void GameObj::setActionState(int sta)
{
	mCurActionState = sta;
	// 	LOGMSG(Ogre::StringConverter::toString(mCurActionState));
}
//--------------------------------------------------------------------------
void GameObj::setActionState(std::string anima)
{
	std::map<std::string,int>& actionState_map = mpBodyActionMgr->getActionStateMap();
	if (actionState_map.end() == actionState_map.find(anima))
	{
		LOGERR("Doesn't exsit " + anima + " state!");
		return;
	}

	setActionState(actionState_map[anima]);

}
//--------------------------------------------------------------------------
uint GameObj::addParticle(uint id)
{
	if(mMainModel) 
		return mMainModel->addParticle(id); 
	else
		return 0;
}
void GameObj::delParticleByHandle(uint handle)
{
	if(mMainModel) 
		mMainModel->delParticleByHandle(handle); 
}
void GameObj::delParticleByID(uint id)
{
	if(mMainModel) 
		mMainModel->delParticleByID(id); 
}
void GameObj::startParticle()
{
	if(mMainModel) 
		mMainModel->startParticle(); 
}
void GameObj::pauseParticle()
{
	if(mMainModel) 
		mMainModel->pauseParticle(); 
}
void GameObj::stopParticle()
{
	if(mMainModel) 
		mMainModel->stopParticle(); 
}
//--------------------------------------------------------------------------