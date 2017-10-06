/******************************************************************************
* 作者: Y3
* 时间: 2009-10-22
* 描述: Ogre及其图形系统包装的单件类
******************************************************************************/
#ifndef SystemGcs_H
#define SystemGcs_H

#include "EQMainPrerequisites.h"

class ModelManagerGcs;
class VideoCardCapabilities;
class ParticleMgr;
namespace MOC { class CollisionTools; }

class SystemGcs : public Ogre::Singleton<SystemGcs>
{
public:
	SystemGcs();
	~SystemGcs();

public:
	Ogre::Root* getRoot() { return Ogre::Root::getSingletonPtr(); }

	void setSceneManager( Ogre::SceneManager* mgr );
	Ogre::SceneManager* getSceneManager() { return mSceneMgr; }

	void setVideoCardCapabilities(VideoCardCapabilities* cap) { mCap = cap; }
	VideoCardCapabilities* getVideoCardCapabilities() { return mCap; }

protected:
	Ogre::SceneManager*			mSceneMgr;
	ModelManagerGcs*			mModelMgr;
	MOC::CollisionTools*		mCollisionTools;
	VideoCardCapabilities*		mCap;
	ParticleMgr*				mParticleMgr;
};


#endif
