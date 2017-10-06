#include "EQMainPCH.h"

#include "HSCManager.h"

//-----------------------------------------------------------------------
template<> HSCManager* Ogre::Singleton<HSCManager>::ms_Singleton = 0;
//-----------------------------------------------------------------------
HSCManager::HSCManager() :
mNum(0)
{
}
//-----------------------------------------------------------------------
HSCManager::~HSCManager()
{
	if ( !mHouseSceneComponentMap.empty() )
	{
		LOGERR("Call HSCManager::~HSCManager! HSCManager has " + 
			Ogre::StringConverter::toString(mHouseSceneComponentMap.size()) + " member left!");
	}

	HouseSceneComponentMap::iterator it;
	for ( it = mHouseSceneComponentMap.begin(); it != mHouseSceneComponentMap.end(); ++it )
	{
		delete it->second;
	}
	mHouseSceneComponentMap.clear();
}
//-----------------------------------------------------------------------
HouseSceneComponent* HSCManager::createHouseSceneComponent()
{
	HouseSceneComponent* hsc = new HouseSceneComponent(++mNum);
	mHouseSceneComponentMap.insert(HouseSceneComponentMap::value_type(hsc->getUid(), hsc));
	return hsc;
}
//-----------------------------------------------------------------------
void HSCManager::destroyHouseSceneComponent(uint uid)
{
	HouseSceneComponentMap::iterator it = mHouseSceneComponentMap.find(uid);

	if ( it != mHouseSceneComponentMap.end() )
	{
		delete it->second;
		mHouseSceneComponentMap.erase(it);
	}
}
//-----------------------------------------------------------------------
void HSCManager::destroyHouseSceneComponent(HouseSceneComponent* hsc)
{
	destroyHouseSceneComponent(hsc->getUid());
}
//-----------------------------------------------------------------------
bool HSCManager::hasHouseSceneComponent(uint uid) const
{
	return mHouseSceneComponentMap.find(uid) != mHouseSceneComponentMap.end();
}
//-----------------------------------------------------------------------
HouseSceneComponent* HSCManager::getHouseSceneComponent(uint uid)
{
	if ( hasHouseSceneComponent(uid) )
	{
		return mHouseSceneComponentMap[uid];
	}

	EQ_EXCEPT("Can't find HouseSceneComponent '" + 
		Ogre::StringConverter::toString(uid) + "'!", 
		"HSCManager::getHouseSceneComponent");

	return NULL;
}
//-----------------------------------------------------------------------
//HouseSceneComponent* HSCManager::getHouseSceneComponent(const ModelGcs * m)
//{
//	for ( HouseSceneComponentMap::const_iterator it = mHouseSceneComponentMap.begin();
//		it != mHouseSceneComponentMap.end();
//		++it )
//	{
//		if ( m == it->second->getModel() )
//		{
//			return it->second;
//		}
//	}
//
//	return NULL;
//}
//-----------------------------------------------------------------------
