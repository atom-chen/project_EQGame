/******************************************************************************
* ����: Y3
* ʱ��: 2009-12-10
* ����: ����������Ӧ���¼�
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

	/** ֪ͨ����ƶ����ʷ����ı�
	*/
	void notifyCameraRateChange( float rate );
public:
	void MouseMove( UINT nFlags, CPoint delta, CPoint point, bool & processed );

	void MouseWheel( UINT nFlags, short zDelta, CPoint pt, bool & processed );

protected:
	Ogre::Camera *		mCamera;

	/// ������ƶ�����
	float				mCamRate;
};

#endif