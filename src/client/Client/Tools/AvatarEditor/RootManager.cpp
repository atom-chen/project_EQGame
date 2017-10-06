#include "AvatarEditorPCH.h"

#include "RootManager.h"
#include "GraphicsSystem.h"
#include "EQMainSingletonManager.h"
#include "OgreRootEx.h"

//-----------------------------------------------------------------------
template<> RootManager* Ogre::Singleton<RootManager>::ms_Singleton = 0;

//-----------------------------------------------------------------------
RootManager::RootManager()
{
	new EQMainSingletonManager();

	new GraphicsSystem("../Log/AvatarEditor.Ogre.log");
}
//-----------------------------------------------------------------------
RootManager::~RootManager()
{
	delete GraphicsSystem::getSingletonPtr();

	delete EQMainSingletonManager::getSingletonPtr();

	delete OgreRootEx::getSingletonPtr();
}
