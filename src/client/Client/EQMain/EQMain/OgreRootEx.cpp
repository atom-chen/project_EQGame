#include "EQMainPCH.h"

#include "OgreRootEx.h"
#include "HouseResManager.h"
#include "Projector.h"
#include "CommFuncGcs.h"

//-----------------------------------------------------------------------
template<> OgreRootEx* Ogre::Singleton<OgreRootEx>::ms_Singleton = 0;
//-----------------------------------------------------------------------
OgreRootEx::OgreRootEx(const Ogre::String& pluginFileName, 
					   const Ogre::String& configFileName, const Ogre::String& logFileName )
{
#pragma push_macro("new")
#undef new
	mRoot = OGRE_NEW Ogre::Root(pluginFileName, configFileName, logFileName);

	mHouseResManager = OGRE_NEW HouseResManager();
	mProjectorFactory = OGRE_NEW ProjectorFactory();
#pragma pop_macro("new")

	mRoot->addMovableObjectFactory(mProjectorFactory);

	//
	Ogre::MeshManager::getSingleton().setListener(&g_EQMeshSerializerListener);
}
//-----------------------------------------------------------------------
OgreRootEx::~OgreRootEx()
{
	delete mProjectorFactory;

	delete mHouseResManager;

	OGRE_DELETE mRoot;
}



