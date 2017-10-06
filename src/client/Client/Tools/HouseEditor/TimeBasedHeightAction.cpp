#include "HouseEditorPCH.h"
#include "TimeBasedHeightAction.h"
#include "HitIndicator.h"
#include "TerrainSelections.h"
#include "SceneManager.h"
#include "RootManager.h"
#include "SceneListener.h"


//////////////////////////////////////////////////////////////////////////

TimeBasedHeightAction::TimeBasedHeightAction()
: mModifiedJunctions(NULL)
{
	mModifiedJunctions = new JunctionSelection();
}

TimeBasedHeightAction::~TimeBasedHeightAction()
{
	delete mModifiedJunctions;
}

void TimeBasedHeightAction::_onMove(const Ogre::Vector2& pt)
{
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt);
}

void TimeBasedHeightAction::_onBegin(const Ogre::Vector2& pt)
{
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt);
	mModifiedJunctions->reset();
}

void TimeBasedHeightAction::_onDrag(const Ogre::Vector2& pt)
{
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
}

void TimeBasedHeightAction::_onUpdate(const Ogre::Vector2& pt, Real seconds)
{
	JunctionSelection* selection = static_cast<JunctionSelection*>(
		RootManager::getSingleton()._getSelection("JunctionSelection"));
	_prepareUpdate(*selection, seconds);

	const JunctionSelection::JunctionMap& junctions = selection->getJunctions();
	JunctionSelection::JunctionMap::const_iterator it;
	std::vector<TerrainInfo> terrainInfo;
	for (it = junctions.begin(); it != junctions.end(); ++it)
	{
		const JunctionSelection::Junction& junction = it->second;
		mModifiedJunctions->add(junction.x, junction.z, 1);
		Real newHeight = _computeHeight(junction, seconds);

		float oldHeight = EQSceneManager::getSingleton().getTerrainData()->getHeight(junction.x,junction.z);

		TerrainInfo terrInfo;
		terrInfo.x = junction.x;
		terrInfo.z = junction.z;
		terrInfo.oldHeight = junction.height;
		terrInfo.nowHeight = newHeight;
		terrainInfo.push_back(terrInfo);

		EQSceneManager::getSingleton().getTerrainData()->setHeight(junction.x, junction.z, newHeight);
	}   
	SceneListenerManager::getSingleton()._fireTerrainHeightChanged(terrainInfo);
	selection->notifyModified();
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->refresh();
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt);
}

void TimeBasedHeightAction::_onEnd(const Ogre::Vector2& pt, bool canceled)
{
	doFinish(mModifiedJunctions, canceled);
	mModifiedJunctions->reset();
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt);
}

void TimeBasedHeightAction::_prepareUpdate(const JunctionSelection& selection, Real seconds)
{
}

