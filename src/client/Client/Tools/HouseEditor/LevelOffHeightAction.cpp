#include "HouseEditorPCH.h"
#include "LevelOffHeightAction.h"
#include "HitIndicator.h"
#include "TerrainSelections.h"

#include "RootManager.h"
#include "SceneManager.h"
#include "SceneListener.h"


//////////////////////////////////////////////////////////////////////////
LevelOffHeightAction* LevelOffHeightAction::create()
{
	return new LevelOffHeightAction();
}

LevelOffHeightAction::LevelOffHeightAction()
: mModifiedJunctions(NULL)
{
	mModifiedJunctions = new JunctionSelection();
}

LevelOffHeightAction::~LevelOffHeightAction()
{
	delete mModifiedJunctions;
}

const std::string& LevelOffHeightAction::getName(void) const
{
	static const std::string name = "LevelOffHeightAction";
	return name;
}

void LevelOffHeightAction::_doModify(const Ogre::Vector2& pt)
{
	Ogre::Vector3 position;
	bool intersected = RootManager::getSingleton().getTerrainIntersects(pt, position);
	if (intersected)
	{
		bool anyModified = false;

		JunctionSelection* selection = static_cast<JunctionSelection*>(
			RootManager::getSingleton()._getSelection("JunctionSelection"));
		RootManager::getSingleton()._buildSelection(selection, position.x, position.z);

		const JunctionSelection::JunctionMap& junctions = selection->getJunctions();
		JunctionSelection::JunctionMap::const_iterator it;
		std::vector<TerrainInfo> terrainInfo;
		for (it = junctions.begin(); it != junctions.end(); ++it)
		{
			const JunctionSelection::Junction& junction = it->second;
			Real newHeight = position.y;
			if (newHeight != junction.height)
			{
				anyModified = true;
				mModifiedJunctions->add(junction.x, junction.z, 1);

				TerrainInfo terrInfo;
				terrInfo.x = junction.x;
				terrInfo.z = junction.z;
				terrInfo.oldHeight = junction.height;
				terrInfo.nowHeight = newHeight;
				terrainInfo.push_back(terrInfo);

				EQSceneManager::getSingleton().getTerrainData()->setHeight(junction.x, junction.z, newHeight);
			}
		}

		SceneListenerManager::getSingleton()._fireTerrainHeightChanged(terrainInfo);

		if (anyModified)
		{
			selection->notifyModified();
		}

		RootManager::getSingleton().getHitIndicator("JunctionPoints")->refresh();
	}
	else
	{
		RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
	}

	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt);
}

void LevelOffHeightAction::_onMove(const Ogre::Vector2& pt)
{
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt);
}

void LevelOffHeightAction::_onBegin(const Ogre::Vector2& pt)
{
	mModifiedJunctions->reset();
	_doModify(pt);
}

void LevelOffHeightAction::_onDrag(const Ogre::Vector2& pt)
{
	_doModify(pt);
}

void LevelOffHeightAction::_onEnd(const Ogre::Vector2& pt, bool canceled)
{
	doFinish(mModifiedJunctions, canceled);
	mModifiedJunctions->reset();
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt);
}

