/*********************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: 游戏管理
*		
**********************************************************************/
#ifndef EQGameMgr_H
#define EQGameMgr_H


#include <Windows.h>
#include "EQMainPrerequisites.h"

class EQCamera;
class AvatarRTT;
class FurnitureRTT;
class OtherPlayerRTT;
class BeautyRTT;
class EQLogin;
class EQMsgManager;

class EQGameMgr : public Ogre::Singleton<EQGameMgr>
{
public:	
	class InitError : public std::runtime_error 
	{
	public:
		InitError( const char* what ) :
		  std::runtime_error( what )
		  {}
	};

	EQGameMgr():mAppStarted(0) {}
	virtual ~EQGameMgr(void) {}

	virtual bool init(HINSTANCE h) { return true; }
	virtual bool update(bool active) { return true; }
	virtual void cleanup() {}

	virtual EQMsgManager* getEQMsgManager() { return NULL; }

	bool ifAppStarted(){return mAppStarted;}
	void setTimElapsed(Ogre::Real elapsed){mTimElapsed = elapsed;}
	Ogre::Real getTimElapsed(){ return mTimElapsed; }

	EQCamera* getMainCamera(){return mMainCamera;}
	AvatarRTT* getRTT(){return mAvatarRTT;}
	FurnitureRTT* getFurnitureRTT() { return mFurnitureRTT; }
	OtherPlayerRTT* getOtherPlayerRTT(){return mOtherPlayerRTT;}
	BeautyRTT* getBeautyRTT(){return mBeautyRTT;}
	EQLogin* getLogin() { return mLogin; }

	static EQMsgManager* getMsgManager() { return EQGameMgr::getSingleton().getEQMsgManager(); }

protected:
	bool mAppStarted;
	HWND mMainWnd;
	Ogre::Real mTimElapsed;
	EQCamera* mMainCamera;
	AvatarRTT* mAvatarRTT;
	OtherPlayerRTT* mOtherPlayerRTT;
	FurnitureRTT* mFurnitureRTT;
	BeautyRTT* mBeautyRTT;
	EQLogin* mLogin;

};
#define sGameMgr EQGameMgr::getSingleton()
#endif