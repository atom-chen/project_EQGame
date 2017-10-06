#include "HouseEditorPCH.h"
#include "SetTerrainHeightAction.h"
#include "HitIndicator.h"
#include "TerrainSelections.h"

#include "RootManager.h"
#include "SceneManager.h"
#include "SceneListener.h"


//////////////////////////////////////////////////////////////////////////
SetTerrainHeightAction* SetTerrainHeightAction::create()
{
	return new SetTerrainHeightAction();
}

SetTerrainHeightAction::SetTerrainHeightAction()
:  mTerrainHeight(1000), mTerrainHeightIncrement(0)
{
	mModifiedJunctions = new JunctionSelection();
}

SetTerrainHeightAction::~SetTerrainHeightAction()
{
	if (mModifiedJunctions)
	{
		delete mModifiedJunctions;
		mModifiedJunctions = 0;
	}
}

const std::string& SetTerrainHeightAction::getName(void) const
{
	static const std::string name = "SetTerrainHeightAction";
	return name;
}

void SetTerrainHeightAction::_onMove(const Ogre::Vector2& pt)
{
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt);
}

void SetTerrainHeightAction::_onBegin(const Ogre::Vector2& pt)
{
	mModifiedJunctions->reset();
	mTerrainHeight += mTerrainHeightIncrement;
	_doModify(pt);
}

void SetTerrainHeightAction::_onDrag(const Ogre::Vector2& pt)
{
	_doModify(pt);
}

void SetTerrainHeightAction::_onEnd(const Ogre::Vector2& pt, bool canceled)
{
	doFinish(mModifiedJunctions, canceled);
	mModifiedJunctions->reset();
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt);
}

void SetTerrainHeightAction::_doModify(const Ogre::Vector2& pt)
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
			Real newHeight = mTerrainHeight;
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

void SetTerrainHeightAction::setParameter(const std::string& name, const std::string& value)
{
	if (name == "%TerrainHeight")
	{
		mTerrainHeight = Ogre::StringConverter::parseInt(value);
	} 
	else if (name == "%TerrainHeightIncrement")
	{
		mTerrainHeightIncrement = Ogre::StringConverter::parseInt(value);
	}
	else
	{
		Action::setParameter(name, value);
	}
}

std::string SetTerrainHeightAction::getParameter(const std::string& name) const
{
	if (name == "%TerrainHeight")
	{
		return Ogre::StringConverter::toString(mTerrainHeight);
	} 
	else if (name == "%TerrainHeightIncrement")
	{
		return Ogre::StringConverter::toString(mTerrainHeightIncrement);
	}

	return Action::getParameter(name);        
}
