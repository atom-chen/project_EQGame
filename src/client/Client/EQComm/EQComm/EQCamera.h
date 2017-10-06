/*********************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: EQCamera
*		游戏摄像机控制
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
		导航（绕着LookAt）,注意这里并不是Node绕自身的Yaw
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
		相机视角限制，这里根据
		配置表来控制
	*/
	void		_viewLimit();

	bool		_intersects(POINT mouse, Ogre::Vector3& out);

protected:
	bool				mFollowMe;
	//lookat 看点	
	//pitch  倾斜	
	//yaw    导航（绕着LookAt）	
	//       距离
	Ogre::Vector3		mLookAt;
	float				mPitch;
	float				mYaw;
	int					mDistance;
	int					mNextDistance;
	//Ogre
	Ogre::Camera*		mOgreCam;

	//相机移动相关
	Ogre::Vector3		mNextLookAt,
						mMoveDirection;
	float				mTargetDist;
	bool				mMove;
	bool				mZooming;
	bool				mRbtmMoved,mRbtmMoveStarted;

	//相机配置参数相关
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

	//左右键按下
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

