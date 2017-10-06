#include "HouseEditorPCH.h"

#include "HeightAction.h"
#include "HitIndicator.h"
#include "TerrainSelections.h"
#include "ModifyTerrainHeightOperator.h"
#include "OperatorManager.h"
#include "RootManager.h"

#include "BrushSelectorDlg.h"
#include "MfcInputEventMgr.h"
#include "SceneManager.h"


//////////////////////////////////////////////////////////////////////////
HeightAction::~HeightAction()
{
	g_CBrushSelectorDlg->unselectTerrainModifyCtl();
}
//-----------------------------------------------------------------------
void HeightAction::_onActive(bool active)
{
	if (!active)
	{
		RootManager::getSingleton().getHitIndicator("JunctionPoints")->hide();
	}
}
//-----------------------------------------------------------------------
void HeightAction::doFinish(JunctionSelection* origin, bool canceled)
{
	assert(origin);

	if (canceled)
	{
		origin->apply();
	}
	else
	{
		if (!origin->empty())
		{
			std::auto_ptr<ModifyTerrainHeightOperator> op(new ModifyTerrainHeightOperator());

			const JunctionSelection::JunctionMap& junctions = origin->getJunctions();
			JunctionSelection::JunctionMap::const_iterator it;
			for (it = junctions.begin(); it != junctions.end(); ++it)
			{
				const JunctionSelection::Junction& junction = it->second;
				Real newHeight = EQSceneManager::getSingleton().getTerrainData()->getHeight(junction.x, junction.z);
				Real oldHeight = junction.height;
				if (newHeight != oldHeight)
				{
					op->add(junction.x, junction.z, oldHeight, newHeight);
				}
			}

			if (!op->empty())
				RootManager::getSingleton().getOperatorManager()->addOperator(op.release());
		}
	}
}

//-----------------------------------------------------------------------
void HeightAction::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		RootManager::getSingleton().clearActiveAction();
		processed = true;
	}
}
