#include "EQMainPCH.h"

#include "HouseResManager.h"

//-----------------------------------------------------------------------
template<> HouseResManager* Ogre::Singleton<HouseResManager>::ms_Singleton = 0;
//-----------------------------------------------------------------------
HouseResManager::HouseResManager()
{
	mResourceType = "HouseRes";

	Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}
//-----------------------------------------------------------------------
HouseResManager::~HouseResManager()
{
	Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}
//-----------------------------------------------------------------------
HouseResPtr HouseResManager::createManual( const Ogre::String& name, const Ogre::String& groupName, 
										  Ogre::ManualResourceLoader* loader )
{
	// Don't try to get existing, create should fail if already exists
	return create(name, groupName, true, loader);
}
//-----------------------------------------------------------------------
Ogre::Resource* HouseResManager::createImpl(const Ogre::String& name, Ogre::ResourceHandle handle, 
											const Ogre::String& group, bool isManual,Ogre:: ManualResourceLoader* loader, 
											const Ogre::NameValuePairList* createParams)
{
	// no use for createParams here
#pragma push_macro("new")
#undef new
	Ogre::Resource* res = OGRE_NEW HouseRes(this, name, handle, group, isManual, loader);
#pragma pop_macro("new")
	return res;
}

