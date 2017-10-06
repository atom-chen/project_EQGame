#include "HouseEditorPCH.h"
#include "SmoothHeightAction.h"

#include "RootManager.h"
#include "SceneManager.h"


//////////////////////////////////////////////////////////////////////////
SmoothHeightAction* SmoothHeightAction::create()
{
	return new SmoothHeightAction();
}

SmoothHeightAction::SmoothHeightAction()
{
}

const std::string& SmoothHeightAction::getName(void) const
{
	static const std::string name = "SmoothHeightAction";
	return name;
}

void SmoothHeightAction::_onBegin(const Ogre::Vector2& pt)
{
	TimeBasedHeightAction::_onBegin(pt);

	computeAverageHeight();
}

void SmoothHeightAction::_onDrag(const Ogre::Vector2& pt)
{
	TimeBasedHeightAction::_onDrag(pt);

	computeAverageHeight();
}

void SmoothHeightAction::computeAverageHeight(void)
{
	Real totalHeight = 0;
	Real totalWeight = 0;
	JunctionSelection* selection = static_cast<JunctionSelection*>(
		RootManager::getSingleton()._getSelection("JunctionSelection"));
	const JunctionSelection::JunctionMap& junctions = selection->getJunctions();
	JunctionSelection::JunctionMap::const_iterator it;
	for (it = junctions.begin(); it != junctions.end(); ++it)
	{
		const JunctionSelection::Junction& junction = it->second;
		totalWeight += junction.weight;
		totalHeight += EQSceneManager::getSingleton().getTerrainData()->getHeight(junction.x, junction.z) * junction.weight;
	}
	mAverageHeight = totalWeight ? totalHeight / totalWeight : 0;
}

Real SmoothHeightAction::_computeHeight(const JunctionSelection::Junction& junction, Real seconds)
{
	Real height = EQSceneManager::getSingleton().getTerrainData()->getHeight(junction.x, junction.z);
	Real diff = mAverageHeight - height;
	Real secondsRequest = Ogre::Math::Abs(diff * junction.weight / RootManager::getSingleton()._getHeightAdjustSpeed());
	if (secondsRequest < seconds)
		return mAverageHeight;
	else
		return height + diff * seconds / secondsRequest;
}


