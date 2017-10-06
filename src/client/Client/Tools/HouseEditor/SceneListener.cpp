#include "HouseEditorPCH.h"

#include "SceneListener.h"

void SceneListener::onSceneReset(void)
{
}

void SceneListener::onTerrainHeightChanged(const std::vector<TerrainInfo>& terrainInfo)
{
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// listener π‹¿Ì∆˜
//
template<> SceneListenerManager* Ogre::Singleton<SceneListenerManager>::ms_Singleton = 0;


SceneListenerManager::SceneListenerManager(void)
: mListeners()
{
}

SceneListenerManager::~SceneListenerManager()
{
}

void SceneListenerManager::addSceneListener(SceneListener* listener)
{
	mListeners.push_back(listener);
}

void SceneListenerManager::removeSceneListener(SceneListener* listener)
{
	mListeners.remove(listener);
}

void SceneListenerManager::_fireSceneReset(SceneListener* exclude)
{
	for (Listeners::const_iterator it = mListeners.begin(); it != mListeners.end(); ++it)
	{
		SceneListener* listener = *it;
		if (listener != exclude)
			listener->onSceneReset();
	}
}

void SceneListenerManager::_fireTerrainHeightChanged(const std::vector<TerrainInfo>& terrainInfo,SceneListener* exclude)
{
	for (Listeners::const_iterator it = mListeners.begin(); it != mListeners.end(); ++it)
	{
		SceneListener* listener = *it;
		if (listener != exclude)
			listener->onTerrainHeightChanged(terrainInfo);
	}
}

