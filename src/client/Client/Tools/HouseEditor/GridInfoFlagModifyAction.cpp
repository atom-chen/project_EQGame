#include "HouseEditorPCH.h"
#include "GridInfoFlagModifyAction.h"
#include "HitIndicator.h"
#include "TerrainSelections.h"
#include "TerrainData.h"
#include "GridInfoFlagModifyOperator.h"

#include "RootManager.h"
#include "SceneManager.h"
#include "SceneListener.h"
#include "OperatorManager.h"

#include "BrushSelectorDlg.h"

//////////////////////////////////////////////////////////////////////////
    
GridInfoFlagModifyAction::GridInfoFlagModifyAction() :
mModifiedFlag(0),
mHitIndicatorName("")
{

}

GridInfoFlagModifyAction::~GridInfoFlagModifyAction()
{
	g_CBrushSelectorDlg->unselectTerrainModifyCtl();
}

//-----------------------------------------------------------------------
void GridInfoFlagModifyAction::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		RootManager::getSingleton().clearActiveAction();
		processed = true;
	}
}

void GridInfoFlagModifyAction::_onMove(const Ogre::Vector2& pt)
{
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
	RootManager::getSingleton().getHitIndicator(mHitIndicatorName)->setHitPoint(pt);
}

void GridInfoFlagModifyAction::_onBegin(const Ogre::Vector2& pt)
{
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->setHitPoint(pt);
	RootManager::getSingleton().getHitIndicator(mHitIndicatorName)->setHitPoint(pt);

	_modifyFlag(pt);
}

void GridInfoFlagModifyAction::_onEnd(const Ogre::Vector2& pt, bool canceled)
{     
	_doFinish(static_cast<GridSelection*>(RootManager::getSingleton()._getSelection("GridSelection")), canceled);
	RootManager::getSingleton().getHitIndicator(mHitIndicatorName)->setHitPoint(pt);
}

void GridInfoFlagModifyAction::_modifyFlag(const Ogre::Vector2& pt)
{
	GridSelection* selection = static_cast<GridSelection*>(
		RootManager::getSingleton()._getSelection("GridSelection"));

	Ogre::Vector3 position;
	bool intersected = RootManager::getSingleton().getTerrainIntersects(pt.x, pt.y, position);

	if (intersected)
		RootManager::getSingleton()._buildSelection(selection, position.x, position.z);

	const GridSelection::GridMap& grids = selection->getGrids();

	GridSelection::GridMap::const_iterator it;
	for (it = grids.begin(); it != grids.end(); ++it)
	{
		const GridSelection::Grid& grid = it->second;
		TerrainData::GridInfo info = grid.info;
		info.flags ^= mModifiedFlag;

		EQSceneManager::getSingleton().getTerrainData()->setGridInfo(grid.x, grid.z, info);
	}

	selection->notifyModified();    
}

void GridInfoFlagModifyAction::_doFinish(GridSelection* origin, bool canceled)
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
			if (!origin->empty())
			{
				std::auto_ptr<FlipTerrainDiagonalOperator> op(new FlipTerrainDiagonalOperator());

				const GridSelection::GridMap& grids = origin->getGrids();
				GridSelection::GridMap::const_iterator it;
				for (it = grids.begin(); it != grids.end(); ++it)
				{
					const GridSelection::Grid& grid = it->second;
					uint newFlipFlag = EQSceneManager::getSingleton().getTerrainData()->getGridInfo(grid.x, grid.z).flags;
					uint oldFlipFlag = grid.info.flags;
					if (newFlipFlag != oldFlipFlag)
					{
						op->add(grid.x, grid.z, oldFlipFlag, newFlipFlag);
					}
				}

				if (!op->empty())
					RootManager::getSingleton().getOperatorManager()->addOperator(op.release());
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
FlipDiagonalAction* FlipDiagonalAction::create()
{
	return new FlipDiagonalAction();
}

FlipDiagonalAction::FlipDiagonalAction() 
{
	mModifiedFlag = TerrainData::GridInfo::FlipDiagonal;
	mHitIndicatorName = "FlipDiagonalGrids";
}

const std::string & FlipDiagonalAction::getName() const 
{
	static const std::string name = "FlipDiagonalAction";
	return name;
}

//////////////////////////////////////////////////////////////////////////
DisableDecalAction::DisableDecalAction()
{
	mModifiedFlag = TerrainData::GridInfo::DisableDecal;
	mHitIndicatorName = "DisableDecalGrids";
}

const std::string & DisableDecalAction::getName() const 
{
	static const std::string name = "DisableDecalAction";
	return name;
}
