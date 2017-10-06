#include "HouseEditorPCH.h"
#include "RaiseHeightAction.h"

#include "RootManager.h"
#include "SceneManager.h"

//////////////////////////////////////////////////////////////////////////
RaiseHeightAction* RaiseHeightAction::create()
{
	return new RaiseHeightAction();
}

RaiseHeightAction::RaiseHeightAction()
{
}

const std::string& RaiseHeightAction::getName(void) const
{
	static const std::string name = "RaiseHeightAction";
	return name;
}

void RaiseHeightAction::_prepareUpdate(const JunctionSelection& selection, Real seconds)
{
	//¼ÓºÅ!!
	mAdjust = + RootManager::getSingleton()._getHeightAdjustSpeed() * seconds;
}

Real RaiseHeightAction::_computeHeight(const JunctionSelection::Junction& junction, Real seconds)
{
	return EQSceneManager::getSingleton().getTerrainData()->getHeight(junction.x, junction.z) + mAdjust * junction.weight;
}


