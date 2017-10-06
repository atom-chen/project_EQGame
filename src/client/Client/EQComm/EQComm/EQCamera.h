/*********************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: EQCamera
*		��Ϸ���������
***********************************************************************/
#ifndef EQCamera_H
#define EQCamera_H

#include "EQMainPrerequisites.h"
#include "InputMgr.h"

namespace Ogre
{
	class Camera;
}

class EQCamera : public EQInputEventListener
{
public:
	EQCamera();
	virtual ~EQCamera(){}

	void		reset();
	void		initial(Ogre::Camera* cam);
	void		listenerEvt(){sInputMgr.addListener(this);}

	void		lookAt(const Ogre::Vector3& pos);
	void		pitch(float degree);
	/**
		����������LookAt��,ע�����ﲢ����Node�������Yaw
	 */
	void		yaw(float degree);
	void		resetYaw();
	void		zoom(int distance);
	void		linearZoom(int tardistance);

	Ogre::Vector3&getLookAt()		{ return mLookAt;}
	int			getPitch() const	{ return mPitch; }
	float		getYaw() const		{ return mYaw; }
	float		getZoom() const		{ return mDistance; }
	float       getLookAtX()   const {return mLookAt.x;}
	float       getLookAtY()   const {return mLookAt.y;}
	float       getLookAtZ()   const {return mLookAt.z;}

	void		setOgreCam(Ogre::Camera* cam){ mOgreCam = cam; }
	Ogre::Camera*getOgreCam()		{ return mOgreCam; }
	
	void		setNextLookAt(Ogre::Vector3 lk);
	void		updateLookAt(float elapsedTime);

	void		moveLookAt(float screenDeltaX, float screenDeltaY);

	void		followMe();

	virtual void MouseWheel( UINT& flags, POINT point, short zDelta );
	virtual void LButtonUp( UINT& flags, POINT point );
	virtual void RButtonUp( UINT& flags, POINT point ) ;
	virtual void MButtonDown( UINT& flags, POINT point );
	virtual void MouseMove( UINT& flags, POINT delta, POINT point );
	virtual void Update(float timElapsed);
	virtual void KeyDown( UINT& flags, UINT nChar, UINT nRepCnt );


private:
	void		_reCalculate();
	/**
		����ӽ����ƣ��������
		���ñ�������
	*/
	void		_viewLimit();

	bool		_intersects(POINT mouse, Ogre::Vector3& out);

protected:
	bool				mFollowMe;
	//lookat ����	
	//pitch  ��б	
	//yaw    ����������LookAt��	
	//       ����
	Ogre::Vector3		mLookAt;
	float				mPitch;
	float				mYaw;
	int					mDistance;
	int					mNextDistance;
	//Ogre
	Ogre::Camera*		mOgreCam;

	//����ƶ����
	Ogre::Vector3		mNextLookAt,
						mMoveDirection;
	float				mTargetDist;
	bool				mMove;
	bool				mZooming;
	bool				mRbtmMoved,mRbtmMoveStarted;

	//������ò������
	int					mPitchMin,
						mPitchMax,
						mYawMin,
						mYawMax,
						mZoomMin,
						mZoomMax,
						mMoveSpeed,
						mZoomSpeed,
						mRotSpeed;
	Ogre::Vector3		
						mLookAtMin,
						mLookAtMax;

	//���Ҽ�����
	bool				mLeftArrowKeyDown;
	bool				mRightArrowKeyDown;

	public:
		void resetCamMovSpeed(int times = 1);
		bool getFollowMe(){return mFollowMe;}
		int getPitchMin(){return mPitchMin;}
		int getPitchMax(){return mPitchMax;}
		int getYawMin(){return mYawMin;}
		int getYawMax(){return mYawMax;}
		int getZoomMin(){return mZoomMin;}
		int getZoomMax(){return mZoomMax;}
		Ogre::Vector3 getLookAtMin(){return mLookAtMin;}
		Ogre::Vector3 getLookAtMax(){return mLookAtMax;}

		void setFollowMe(bool f){mFollowMe = f;}
		void setPitchMin(int min){mPitchMin = min;_reCalculate();}
		void setPitchMax(int max){mPitchMax = max;_reCalculate();}
		void setYawMin(int min){mYawMin = min;_reCalculate();}
		void setYawMax(int max){mYawMax = max;_reCalculate();}
		void setZoomMin(int min){mZoomMin = min;_reCalculate();}
		void setZoomMax(int max){mZoomMax = max;_reCalculate();}
		void setLookAtMin(Ogre::Vector3 min){mLookAtMin = min;_reCalculate();}
		void setLookAtMax(Ogre::Vector3 max){mLookAtMax = max;_reCalculate();}
};

#endif

