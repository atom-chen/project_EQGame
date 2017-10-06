#include "HouseEditorPCH.h"

#include "CameraFlyEvent.h"
#include "GraphicsSystem.h"
#include "EQMain.h"
#include "SceneManager.h"
#include "SceneInfo.h"
#include "RootManager.h"
#include "MainFrm.h"

//-----------------------------------------------------------------------
static Ogre::Ray getRay( CPoint point )
{
	Ogre::Camera * cam = GraphicsSystem::getSingleton().getCamera();

	int w = cam->getViewport()->getActualWidth();
	int h = cam->getViewport()->getActualHeight();

	Ogre::Ray ray = cam->getCameraToViewportRay((float)point.x/w, (float)point.y/h);

	return ray;
}
//-----------------------------------------------------------------------
CameraFlyEvent::CameraFlyEvent( Ogre::Camera * cam ) 
	: MfcInputEvent( epCameraFly )
	, mCamera( cam )
	, mCamRate( 1.0f )
{
	assert( mCamera );
}
//-----------------------------------------------------------------------
CameraFlyEvent::~CameraFlyEvent()
{
}
//-----------------------------------------------------------------------
CameraFlyEvent * CameraFlyEvent::create()
{
	return new CameraFlyEvent( GraphicsSystem::getSingleton().getCamera() );
}
//-----------------------------------------------------------------------
void CameraFlyEvent::notifyCameraRateChange( float rate )
{
	mCamRate = rate;
}
//-----------------------------------------------------------------------
void CameraFlyEvent::MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed )
{
	if ( GetAsyncKeyState(VK_MBUTTON) & 0x8000 )
	{
		int w = mCamera->getViewport()->getActualWidth();
		int h = mCamera->getViewport()->getActualHeight();

		if ( GetAsyncKeyState(VK_LMENU) & 0x8000 || GetAsyncKeyState(VK_RMENU) & 0x8000 )
		{
			//旋转摄像机
			float factor = 0.005f;

			mCamera->yaw(Ogre::Radian(-factor*delta.x));
			mCamera->pitch(Ogre::Radian(-factor*delta.y));
		}
		else
		{
			//平移摄像机
			float factor = mCamRate*1000.0f;

			Ogre::Vector3 vec(-factor*delta.x/w, factor*delta.y/h, 0 );
			mCamera->move(mCamera->getOrientation()*vec);
		}

		processed = true;
	}

	if (!EQSceneManager::getSingleton().getSceneInfo()->isTerrainEmpty())
	{
		Ogre::Vector3 position;
		Ogre::Ray ray = getRay( point );
		bool intersected = RootManager::getSingleton().getTerrainIntersects(ray, position);
		if (intersected)
		{
			g_MainFrame->showPos( Ogre::StringConverter::toString(position) ); 
		}

	}
}
//-----------------------------------------------------------------------
void CameraFlyEvent::MouseWheel( UINT nFlags, short zDelta, CPoint pt, bool & processed )
{
	float factor = 50.0f;
	float zt = -factor*zDelta/120.0f;

	Ogre::Ray ray = mCamera->getCameraToViewportRay(0.5,0.5);

	mCamera->move(ray.getDirection()*zt);	


	if ( mCamera->getProjectionType() == Ogre::PT_ORTHOGRAPHIC )
	{
		Ogre::Vector3 v = mCamera->getPosition();
		mCamera->setOrthoWindowHeight(v.length());
	}


	processed = true;
}

