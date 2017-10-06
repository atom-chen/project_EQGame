#include "EQCommPCH.h"

#include <OgreCamera.h>
#include <OgreRoot.h>

#include "EQCamera.h"
#include "Variable.h"
#include "EQWndProc.h"
#include "InputMgr.h"
#include "MainPlayer.h"
#include "SceneManager.h"
#include "TerrainData.h"
#include "CommFunc.h"
#include "OISKeyboard.h"
#include "EQOIS.h"
#include "EQGameSceneMgr.h"
#include "LuaManager.h"
#include "GameState.h"
#include "FitmentEvent.h"
#include "FitmentInteractMgr.h"
#include "luabind/luabind/luabind.hpp"

EQCamera::EQCamera()
: mOgreCam(0)
,EQInputEventListener(epfCameraFly)
{
	reset();
}

void EQCamera::reset()
{
	mLookAt			= Ogre::Vector3(0,0,0);
	mNextLookAt		= mLookAt;
	mMoveDirection	= mLookAt;
	mPitch			= 0;
	mYaw			= 0;
	mDistance		= 0;
	mNextDistance	= 0;
	mTargetDist		= 0;
	mZoomSpeed		= 0;
	mMove			= false;
	mZooming		= false;
	mFollowMe		= true;
	mRbtmMoved		= false;
	mRbtmMoveStarted = false;
	mLeftArrowKeyDown = false;
	mRightArrowKeyDown = false;
}
//--------------------------------------------------------------------
void EQCamera::initial(Ogre::Camera* cam)
{
	mOgreCam	= cam;
	Ogre::Vector3 campos = cam->getPosition();

	printf("\n Camera Pos x= %f y= %f z= %f \n",campos.x,campos.y,campos.z);

	mPitchMin	= VariableSystem::getSingleton().GetAs_Int("Pitch_MIN");
	mPitchMax	= VariableSystem::getSingleton().GetAs_Int("Pitch_MAX");
	mYawMin		= VariableSystem::getSingleton().GetAs_Int("Yaw_MIN");
	mYawMax		= VariableSystem::getSingleton().GetAs_Int("Yaw_MAX");
	mZoomMin	= VariableSystem::getSingleton().GetAs_Int("Zoom_MIN");
	mZoomMax	= VariableSystem::getSingleton().GetAs_Int("Zoom_MAX");
	mLookAtMin	= VariableSystem::getSingleton().GetAs_Int("LookAt_MIN");
	mLookAtMax	= VariableSystem::getSingleton().GetAs_Int("LookAt_MAX");
	mMoveSpeed  = VariableSystem::getSingleton().GetAs_Int("CamMoveSpeed");
	mZoomSpeed	= VariableSystem::getSingleton().GetAs_Int("CamZoomSpeed");
	mRotSpeed	= VariableSystem::getSingleton().GetAs_Int("CamRotSpeed");
}
//---------------------------------------------------------------------
void EQCamera::lookAt(const Ogre::Vector3& pos)
{
	mLookAt = pos;
	_reCalculate();
}
//----------------------------------------------------------------------
void EQCamera::resetCamMovSpeed(int times)
{
	mMoveSpeed  = VariableSystem::getSingleton().GetAs_Int("CamMoveSpeed") * times;
}
//----------------------------------------------------------------------
void EQCamera::pitch(float degree)
{
	while(degree >= 360.0f)
		degree -= 360.0f;

// 	while(degree < 0 )
// 		/*degree += 360.0f;*/
// 		degree = 0.0f;

	mPitch = degree;

	_reCalculate();
}
//---------------------------------------------------------------------
void EQCamera::yaw(float degree)
{
	while(degree >= 360.0f)
		degree -= 360.0f;

	while(degree < 0 )
		degree += 360.0f;

	mYaw = degree;

	_reCalculate();
}
////---------------------------------------------------------------------
//void EQCamera::yaw(float yaw)
//{
//	mYaw = yaw;
//	_reCalculate();
//}
//---------------------------------------------------------------------
void EQCamera::resetYaw()
{
	mYaw = 0;
	_reCalculate();
}
//--------------------------------------------------------------------
void EQCamera::zoom(int distance)
{
	if (distance <= 0)
	{
		distance = 1;
	}

	mDistance = distance;
	_reCalculate();
}
void EQCamera::linearZoom(int tardistance)
{
	mNextDistance = tardistance;
}
//---------------------------------------------------------------------
void EQCamera::_viewLimit()
{
	int		pitmin = mPitchMin,
			pitmax = mPitchMax;

	mPitch = mPitch > pitmax ? pitmax : mPitch;
	mPitch = mPitch < pitmin ? pitmin : mPitch;

	pitmin = mYawMin;
	pitmax = mYawMax;

	mYaw = mYaw > pitmax ? pitmax : mYaw;
	mYaw = mYaw < pitmin ? pitmin : mYaw;

	mDistance = mDistance > mZoomMax ? mZoomMax : mDistance;
	mDistance = mDistance < mZoomMin ? mZoomMin : mDistance;

	mLookAt.x = mLookAt.x <= mLookAtMin.x ? mLookAtMin.x : mLookAt.x;
	mLookAt.x = mLookAt.x >= mLookAtMax.x ? mLookAtMax.x : mLookAt.x;

	mLookAt.z = mLookAt.z <= mLookAtMin.z ? mLookAtMin.z : mLookAt.z;
	mLookAt.z = mLookAt.z >= mLookAtMax.z ? mLookAtMax.z : mLookAt.z;
}
//------------------------------------------------------------------------------
void EQCamera::setNextLookAt(Ogre::Vector3 lk)
{
	mNextLookAt = lk;

	mNextLookAt.x = mNextLookAt.x <= mLookAtMin.x ? mLookAtMin.x : mNextLookAt.x;
	mNextLookAt.x = mNextLookAt.x >= mLookAtMax.x ? mLookAtMax.x : mNextLookAt.x;

	mNextLookAt.z = mNextLookAt.z <= mLookAtMin.z ? mLookAtMin.z : mNextLookAt.z;
	mNextLookAt.z = mNextLookAt.z >= mLookAtMax.z ? mLookAtMax.z : mNextLookAt.z;
	 
	mMoveDirection = mNextLookAt - mLookAt;
	mMoveDirection.normalise();
	mMoveDirection = mMoveDirection * mMoveSpeed;

	mTargetDist = mLookAt.distance(mNextLookAt) ;

	mMove = true;
}
//
void EQCamera::followMe()
{
	Ogre::Vector3 pos = sMainPlayer.getPos();
	float yDistance = VariableSystem::getSingleton().GetAs_Float("Camera_Stand_Y_Pos");
	float y_Sit_Distance = VariableSystem::getSingleton().GetAs_Float("Camera_Sit_Y_Pos");
	float y_Lie_Distance = VariableSystem::getSingleton().GetAs_Float("Camera_Lie_Y_Pos");
	if (sMainPlayer.getState() == GOS_ANIMATION && sMainPlayer.getActionState() == BodyActionMgr::AS_SIT)
	{
		pos.y = pos.y + y_Sit_Distance;
		setNextLookAt(pos);
		return;
	}
	if (sMainPlayer.getState() == GOS_ANIMATION && sMainPlayer.getActionState() == BodyActionMgr::AS_LIE)
	{
		pos.y = pos.y + y_Lie_Distance;
		setNextLookAt(pos);
		return;
	}
	if (sMainPlayer.getState() == GOS_ANIMATION && sMainPlayer.getActionState() == BodyActionMgr::AS_LIESIT)
	{
		pos.y = pos.y + y_Sit_Distance;
		setNextLookAt(pos);
		return;
	}
	if (sMainPlayer.getState() == GOS_ANIMATION && sMainPlayer.getActionState() == BodyActionMgr::AS_SITLIE)
	{
		pos.y = pos.y + y_Lie_Distance;
		setNextLookAt(pos);
		return;
	}
	else if (sMainPlayer.getState() == GOS_SIT_IDLE)
	{
		pos.y = pos.y + y_Sit_Distance;
	}
	else if (sMainPlayer.getState() == GOS_ANIMATION && sMainPlayer.getCurrentAnimation() == "sit_lookaround")
	{
		pos.y = pos.y + y_Sit_Distance;
	}
	else if (sMainPlayer.getState() == GOS_LIE_IDLE)
	{
		pos.y = pos.y + y_Lie_Distance;
	}
	else if (sMainPlayer.getState() == GOS_ANIMATION && sMainPlayer.getActionState() == BodyActionMgr::AS_SITLOOKAROUND)
	{
		pos.y = pos.y + y_Sit_Distance;
	}
	else
	{
		pos.y = pos.y + yDistance;
	}
	lookAt(pos);
}
//-------------------------------------------------------------------------------
void EQCamera::updateLookAt(float elapsedTime)
{
	if (mZooming)
	{
		if (mNextDistance < mDistance)
		{
			mDistance = mDistance - elapsedTime * mZoomSpeed;
			if (mDistance <= mNextDistance)
			{
				mZooming = false;
			}
			else
			{
				_reCalculate();
			}
		}
		else
		{
			mDistance = mDistance + elapsedTime * mZoomSpeed;
			if (mDistance >= mNextDistance)
			{
				mZooming = false;
			}
			else
			{
				_reCalculate();
			}

		}
	}

	if (!sStateMgr.isFunnyFarm())
	{
		if (getFollowMe())
			followMe();
	}
	
// 	float moveSpeedFactor = fabs((float)mNextDistance) / 500;
// 	moveSpeedFactor *= moveSpeedFactor;
// 	float moveLen = mZoomSpeed * elapsedTime * moveSpeedFactor;
// 	if ( moveLen < fabs((float)mNextDistance))
// 	{
// 		mDistance = mDistance + mNextDistance * elapsedTime * moveSpeedFactor;
// 	}
// 	else
// 	{
// 		mZooming = false;
// 	}

	if(!mMove)
		return;

	float	dis		= mLookAt.distance(mNextLookAt),
			moveSpeedFactor = dis / 500; //mLookAtMax * 6;
	moveSpeedFactor *= moveSpeedFactor;

#define MIN_FACTOR 1.2
	moveSpeedFactor = moveSpeedFactor < MIN_FACTOR ? MIN_FACTOR : moveSpeedFactor;

	float moveLen = mMoveSpeed * elapsedTime * moveSpeedFactor;

	if(  dis <=  moveLen)
	{
		mLookAt = ( mNextLookAt );
		mOgreCam->lookAt(mLookAt);
		mMove = false;
	}
	else if(dis > moveLen)
	{
		mLookAt = (mLookAt + mMoveDirection * elapsedTime * moveSpeedFactor);
		mOgreCam->lookAt(mLookAt);
	}

	_reCalculate();
}
//---------------------------------------------------------------------------------
void EQCamera::moveLookAt(float screenDeltaX, float screenDeltaY)
{
	if( (abs(screenDeltaX)) > (abs(screenDeltaY)))
		screenDeltaY = 0;
	else
		screenDeltaX = 0;


	float deltax = 0,
		  deltaz = 0;

	float	y	= ::sin(Ogre::Math::DegreesToRadians (mPitch)),
		r_xy	= ::cos(Ogre::Math::DegreesToRadians (mPitch)),
		x		= -r_xy * ::sin(Ogre::Math::DegreesToRadians (mYaw)),
		z		= -r_xy * ::cos(Ogre::Math::DegreesToRadians (mYaw));

#define MOVE_SPEED_FACTOR 150

	mLookAt.x += screenDeltaY * mMoveSpeed * x / MOVE_SPEED_FACTOR;
	mLookAt.z += screenDeltaY * mMoveSpeed * z / MOVE_SPEED_FACTOR;

#define RADIAN_90 3.1415926 / 2

	x		= -r_xy * ::sin(Ogre::Math::DegreesToRadians (mYaw) + RADIAN_90),
	z		= -r_xy * ::cos(Ogre::Math::DegreesToRadians (mYaw) + RADIAN_90);

	mLookAt.x += screenDeltaX * mMoveSpeed * x / MOVE_SPEED_FACTOR;
	mLookAt.z += screenDeltaX * mMoveSpeed * z / MOVE_SPEED_FACTOR;


	//printf("\n Camera ratio xz= %f \n",ratioXZ);

	_reCalculate();
}
//---------------------------------------------------------------------
void EQCamera::_reCalculate()
{
	_viewLimit();

	//计算位置()
	float	y	= mDistance * ::sin(Ogre::Math::DegreesToRadians (mPitch)),
		r_xy	= mDistance * ::cos(Ogre::Math::DegreesToRadians (mPitch)),
		x		= -r_xy * ::sin(Ogre::Math::DegreesToRadians (mYaw)),
		z		= -r_xy * ::cos(Ogre::Math::DegreesToRadians (mYaw));

	Ogre::Vector3 vEyePos(	mLookAt.x + x, 
		mLookAt.y + y,
		mLookAt.z + z);
	if (EQGameSceneMgr::getSingleton().getSceneType() != EQGameSceneMgr::SCENE_COMMON 
		|| EQGameSceneMgr::getSingleton().getSceneType() != EQGameSceneMgr::SCENE_FARM
		|| EQGameSceneMgr::getSingleton().getSceneType() != EQGameSceneMgr::SCENE_NONE)
	{
		if (vEyePos.y < 5)
		{
			vEyePos.y = 5;
		}
	}

	// add by Y3
	TerrainData* terr = EQSceneManager::getSingleton().getTerrainData();
	if ( terr && !terr->isEmpty() )
	{
		Ogre::Vector3 dir(x, y, z);
		dir.normalise();
		Ogre::Ray ray(mLookAt, dir);
		Ogre::Vector3 newpos;
		if ( terr->getIntersects(ray, newpos ) )
		{
			if ( (vEyePos - mLookAt).squaredLength() > (newpos - mLookAt).squaredLength() + 400.0f )
				vEyePos = newpos;
		}
		if (sStateMgr.isFunnyFarm())
		{
			vEyePos.y += 20; //始终抬高20
		}
		else if (vEyePos.y < 10)
		{
			vEyePos.y += 10; //始终抬高10

		}
// 		LOGERR(Ogre::StringConverter::toString(vEyePos));
	}
	mOgreCam->setPosition(vEyePos);
	mOgreCam->lookAt(mLookAt);
// 	LOGERR(Ogre::StringConverter::toString(vEyePos));
// 	LOGERR(Ogre::StringConverter::toString(mLookAt));
// 	LOGERR(Ogre::StringConverter::toString(mLookAt.x));
// 	LOGERR(Ogre::StringConverter::toString(mLookAt.y));
// 	LOGERR(Ogre::StringConverter::toString(mLookAt.z));

}
//-------------------------------------------------------------------------------
bool EQCamera::_intersects(POINT mouse, Ogre::Vector3& out)
{
	Ogre::Ray ray = mOgreCam->getCameraToViewportRay(
		(Ogre::Real)mouse.x / EQWndProc::getSingleton().getWndWidth(), 
		(Ogre::Real)mouse.y / EQWndProc::getSingleton().getWndHeight());


	std::pair<bool,Ogre::Real> inters;
	inters =  ray.intersects(Ogre::Plane (Ogre::Vector3::UNIT_Y, 0));

	if( false == inters.first)
		return false;

	out = ray.getPoint(inters.second);

	return true;
}
//-------------------------------------------------------------------------------
void EQCamera::MouseWheel( UINT& flags, POINT point, short zDelta )
{ 
	//if (GameStateMgr::getSingleton().getState() 
	//	!= GS_MAINGAME)
	//	return;
	if( (flags & epfUI) != 0)
		return;
	
	mZooming = true;
// 	LOGERR(Ogre::StringConverter::toString(mPitch));
// 	LOGERR(Ogre::StringConverter::toString(mYaw));
	//if(CLuaManager::getSingleton().getL())
	//	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "OnZoomView");
// 	linearZoom(getZoom() - zDelta);
	float zStep = VariableSystem::getSingleton().GetAs_Float("Zoom_Step");
	if (zDelta>= 0)
	{
		linearZoom( getZoom() - zStep);
	}
	else
	{
		linearZoom( getZoom() + zStep);
	}
	
}
void EQCamera::LButtonUp( UINT& flags, POINT point )
{
	// 		if (getFollowMe() == true)
	// 			return;

	// 		if( (flags & epfUI) != 0
	// 			|| (flags & epfDragItem)!= 0)
	// 			return;
	// 
	// 		mRbtmMoveStarted = false;
	// 
	// 		if (mRbtmMoved)
	// 		{
	// 			mRbtmMoved = false;
	// 			return;
	// 		}
}
void EQCamera::RButtonUp( UINT& flags, POINT point ) 
{
	if (getFollowMe() == true)
		return;

	if( (flags & epfUI) != 0
		|| (flags & epfDragItem)!= 0)
		return;

	mRbtmMoveStarted = false;

	if (mRbtmMoved)
	{
		mRbtmMoved = false;
		//return;
	}

	if (sStateMgr.isFunnyFarm() || sFitMgr.isFiting() || sEQGameSceneMgr.getSceneType() == EQGameSceneMgr::SCENE_HOUSE_PREVIEW/* || FitmentInteractMgr::getSingleton().isInteracting()*/)
	{
		Ogre::Vector3 ptIntersect;
		if(_intersects(point,ptIntersect))
		{
			ptIntersect.y = 120; //临时增加高度
			setNextLookAt(ptIntersect);
		}
	}
}
//--------------------------------------------------------------
void EQCamera::MButtonDown( UINT& flags, POINT point )
{
	if( (flags & epfUI) != 0)
		return;
	mZooming = false;

}
void EQCamera::MouseMove( UINT& flags, POINT delta, POINT point )
{ 
	//LOGDBG(FormatStr("delta x = %d, delta y = %d.", delta.x, delta.y));
	if (sInputMgr.mLMousedown/*mRBtmdown*/)//mMidmousedown)//相机控制
	{	
		
		flags |= epfCameraFly;
		static DWORD start = GetTickCount();

		if(! mRbtmMoveStarted)
		{
			mRbtmMoveStarted = true;
			start = GetTickCount();
		}

		int camtime = VariableSystem::getSingleton()
			.GetAs_Int("CamRotNeedofTime");

		if ( (int)(GetTickCount() - start) < camtime )
			return;

		//主游戏普通模式
		float pixes = VariableSystem::getSingleton()
			.GetAs_Int("CamRotNeedPixes");
		float camspeed = VariableSystem::getSingleton()
			.GetAs_Float("CamRotSpeed");

		if( abs(delta.x) > abs(delta.y) )
		{
			if (abs(point.x - sInputMgr.mMouseLastPoint.x) < pixes)
				return;
			sInputMgr.mLMouseDraged = true;
			int del = (delta.x) * camspeed;
			if (del == 0)
			{
				del = delta.x < 0 ? -1 : 1;
			}

			yaw( getYaw() - del );
		}
		else
		{
			if (abs(point.y - sInputMgr.mMouseLastPoint.y) < pixes)
				 return;
			sInputMgr.mLMouseDraged = true;
			int del = (delta.y) * camspeed;
			if ( del == 0 )
			{
				del = delta.y < 0 ? -1 : 1;
			}

			pitch( getPitch() + del);
		}

		mRbtmMoved = true;
	}
	else if(sInputMgr.mLMouseDraged)//左键拖场景
	{
		moveLookAt(delta.x,delta.y);
	}

	EQGameSceneMgr::getSingleton().setWeatherPos(mOgreCam->getPosition());
}
//-------------------------------------------------------------------------------
void EQCamera::Update(float timElapsed)
{
	if (!EQOIS::getSingleton().isKeyDown(OIS::KC_LEFT))
	{
		mLeftArrowKeyDown = false;
	}
	if (!EQOIS::getSingleton().isKeyDown(OIS::KC_RIGHT))
	{
		mRightArrowKeyDown = false;
	}

	//
	if ( mLeftArrowKeyDown )
	{
		float camspeed = VariableSystem::getSingleton()
			.GetAs_Float("CamRotSpeed2");
		yaw( getYaw() - timElapsed*camspeed );
	} 
	else if ( mRightArrowKeyDown )
	{
		float camspeed = VariableSystem::getSingleton()
			.GetAs_Float("CamRotSpeed2");
		yaw( getYaw() + timElapsed*camspeed );
	}
}
//-------------------------------------------------------------------------------
void EQCamera::KeyDown( UINT& flags, UINT nChar, UINT nRepCnt )
{
	if ( epfUI && flags )
		return;

	switch (nChar)
	{
	case OIS::KC_LEFT:
		{
			mLeftArrowKeyDown = true;
		}
		break;
	case OIS::KC_RIGHT:
		{
			mRightArrowKeyDown = true;
		}
		break;
	}
}