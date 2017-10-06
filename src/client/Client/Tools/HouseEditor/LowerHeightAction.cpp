#include "HouseEditorPCH.h"

#include "LowerHeightAction.h"
#include "RootManager.h"
#include "SceneManager.h"


//////////////////////////////////////////////////////////////////////////
LowerHeightAction* LowerHeightAction::create()
{
	return new LowerHeightAction();
}

LowerHeightAction::LowerHeightAction()
{
}

const std::string& LowerHeightAction::getName(void) const
{
	static const std::string name = "LowerHeightAction";
	return name;
}

void LowerHeightAction::_prepareUpdate(const JunctionSelection& selection, Real seconds)
{
	mAdjust = - RootManager::getSingleton()._getHeightAdjustSpeed() * seconds;
}

Real LowerHeightAction::_computeHeight(const JunctionSelection::Junction& junction, Real seconds)
{
	return EQSceneManager::getSingleton().getTerrainData()->getHeight(junction.x, junction.z) + mAdjust * junction.weight;
}
