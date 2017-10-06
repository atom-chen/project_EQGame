#include "EQMainPCH.h"

#include "EQMainSingletonManager.h"
#include "SystemGcs.h"
#include "DataTableCenter.h"
#include "SceneManager.h"
#include "HSCManager.h"
#include "ZoneLightMgr.h"
//-----------------------------------------------------------------------
template<> EQMainSingletonManager* Ogre::Singleton<EQMainSingletonManager>::ms_Singleton = 0;

//-----------------------------------------------------------------------
EQMainSingletonManager::EQMainSingletonManager()
{
	new SystemGcs();

	DataTableCenter::setup();

	new HSCManager();

	new EQSceneManager();

	new ZoneLightMgr();	
}
//-----------------------------------------------------------------------
EQMainSingletonManager::~EQMainSingletonManager()
{
	delete EQSceneManager::getSingletonPtr();

	delete ZoneLightMgr::getSingletonPtr();

	delete HSCManager::getSingletonPtr();

	DataTableCenter::shutdown();

	delete SystemGcs::getSingletonPtr();	
}