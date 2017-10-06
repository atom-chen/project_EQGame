#include "EQMainPCH.h"

#include "SystemGcs.h"
#include "ModelManagerGcs.h"
#include "CollisionTools.h"
#include "ParticleMgr.h"
#include "AmbientMgr.h"

//-----------------------------------------------------------------------
template<> SystemGcs* Ogre::Singleton<SystemGcs>::ms_Singleton = 0;
//-----------------------------------------------------------------------
SystemGcs::SystemGcs() :
mSceneMgr(NULL),
mCollisionTools(NULL),
mCap(NULL)
{
	mModelMgr = new ModelManagerGcs();
	mParticleMgr = new ParticleMgr();
}
//-----------------------------------------------------------------------
SystemGcs::~SystemGcs()
{
	delete mModelMgr;
	delete mParticleMgr;
	if(mCollisionTools)
		delete mCollisionTools;
}
//-----------------------------------------------------------------------
void SystemGcs::setSceneManager( Ogre::SceneManager* mgr )
{
	mSceneMgr = mgr;
	mCollisionTools = new MOC::CollisionTools(mgr);
	ParticleMgr::getSingleton().init(mgr);
	AmbientMgr::getSingleton().init(mgr);
}
//-----------------------------------------------------------------------