/******************************************************************************
* 作者: Y3
* 时间: 2009-12-10
* 描述: 摄像机飞翔对应的事件
******************************************************************************/
#ifndef CameraFlyEvent_H
#define CameraFlyEvent_H

#include "MfcInputEvent.h"

class CameraFlyEvent : public MfcInputEvent
{
public:
	explicit CameraFlyEvent( Ogre::Camera * cam );
	~CameraFlyEvent();

	static CameraFlyEvent * create();

	/** 通知相机移动速率发生改变
	*/
	void notifyCameraRateChange( float rate );
public:
	void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed );

	void MouseWheel( UINT nFlags, short zDelta, CPoint pt, bool & processed );

protected:
	Ogre::Camera *		mCamera;

	/// 摄像机移动速率
	float				mCamRate;
};

#endif