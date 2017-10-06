#include "HouseEditorPCH.h"
#include "TerrainLayerOneEraserAction.h"
#include "HitIndicator.h"
#include "TerrainSelections.h"

#include "RootManager.h"
#include "SceneManager.h"
#include "SceneListener.h"

#include "BrushSelectorDlg.h"


TerrainLayerOneEraserAction* TerrainLayerOneEraserAction::create()
{
	return new TerrainLayerOneEraserAction();
}
//-----------------------------------------------------------------------
TerrainLayerOneEraserAction::TerrainLayerOneEraserAction()
{
	mCurrentGrids = new GridSelection();
	mHintModified = new GridSelection();
	mModifiedGrids = new GridSelection();
}
//-----------------------------------------------------------------------
TerrainLayerOneEraserAction::~TerrainLayerOneEraserAction()
{
	delete mCurrentGrids;
	delete mHintModified;
	delete mModifiedGrids;

	g_CBrushSelectorDlg->unselectTerrainModifyCtl();
}
//-----------------------------------------------------------------------
const std::string& TerrainLayerOneEraserAction::getName(void) const
{
	static const std::string name = "TerrainLayerOneEraserAction";
	return name;
}
//-----------------------------------------------------------------------
void TerrainLayerOneEraserAction::_buildHitIndicator(const Ogre::Vector2& pt)
{
	RootManager::getSingleton().getHitIndicator("IntersectGrids")->setHitPoint(pt);
	RootManager::getSingleton().getHitIndicator("IntersectPoint")->setHitPoint(pt);

	Ogre::Vector3 position;
	bool intersected = RootManager::getSingleton().getTerrainIntersects(pt, position);
	if (!intersected)
	{
		mCurrentGrids->reset();
		return;
	}

	RootManager::getSingleton()._buildSelection(mCurrentGrids, position.x, position.z);
}
//-----------------------------------------------------------------------
void TerrainLayerOneEraserAction::_onActive(bool active)
{
	if (!active)
	{
		mHintModified->apply();
		mHintModified->reset();
	}
}
//-----------------------------------------------------------------------
void TerrainLayerOneEraserAction::_onMove(const Ogre::Vector2& pt)
{
	mHintModified->apply();
	mHintModified->reset();
	_buildHitIndicator(pt);
	_doErase(mHintModified);
}
//-----------------------------------------------------------------------
void TerrainLayerOneEraserAction::_onBegin(const Ogre::Vector2& pt)
{
	mHintModified->apply();
	mHintModified->reset();
	mModifiedGrids->reset();
	_buildHitIndicator(pt);
	_doErase(mModifiedGrids);
}
//-----------------------------------------------------------------------
void TerrainLayerOneEraserAction::_onDrag(const Ogre::Vector2& pt)
{
	_buildHitIndicator(pt);
	_doErase(mModifiedGrids);
}
//-----------------------------------------------------------------------
void TerrainLayerOneEraserAction::_onEnd(const Ogre::Vector2& pt, bool canceled)
{
	/*if (canceled)
	{
	mModifiedGrids->apply();
	}*/
	doFinish(mModifiedGrids, canceled);
	mModifiedGrids->reset();
	_buildHitIndicator(pt);
}
//-----------------------------------------------------------------------
void TerrainLayerOneEraserAction::_doErase(GridSelection* modified)
{
	const GridSelection::GridMap& grids = mCurrentGrids->getGrids();

	bool anyModified = false;

	if (!grids.empty())
	{            
		for (GridSelection::GridMap::const_iterator it = grids.begin(); it != grids.end(); ++it)
		{
			const GridSelection::Grid& grid = it->second;
			TerrainData::GridInfo info = grid.info;

			if ( info.layers[1].pixmapId )
			{
				info.layers[1].pixmapId = 0;
				info.layers[1].orientation = 0;
			}

			if (info == grid.info)
				continue;

			anyModified = true;
			modified->add(grid.x, grid.z);
			EQSceneManager::getSingleton().getTerrainData()->setGridInfo(grid.x, grid.z, info);
		}

		if (anyModified)
		{
			mCurrentGrids->notifyModified();
		}
	}
}
//-----------------------------------------------------------------------
void TerrainLayerOneEraserAction::KeyDown( UINT nChar, UINT nRepCnt, UINT nFlags, bool & processed )
{
	if ( VK_ESCAPE == nChar )
	{
		RootManager::getSingleton().clearActiveAction();
		processed = true;
	}
}