#include "HouseEditorPCH.h"
#include "GridInfoFlagModifyOperator.h"
#include "HitIndicator.h"

#include "Terrain.h"
#include "SceneManager.h"
//#include "SceneInfo.h"
#include "RootManager.h"


GridInfoFlagModifyOperator::GridInfoFlagModifyOperator()
: mInfos(),
mIndicatorName(""), mGroupName("")
{
}
//-----------------------------------------------------------------------
GridInfoFlagModifyOperator::~GridInfoFlagModifyOperator()
{
}
//-----------------------------------------------------------------------
void GridInfoFlagModifyOperator::add(int x, int z, uint oldFlipFlag, uint newFlipFlag)
{
	assert(EQSceneManager::getSingleton().getTerrainData()->isValidGrid(x, z));

	if (mInfos.empty())
	{
		mMinX = mMaxX = x;
		mMinZ = mMaxZ = z;
	}
	else
	{
		if (mMinX > x)
			mMinX = x;
		else if (mMaxX < x)
			mMaxX = x;

		if (mMinZ > z)
			mMinZ = z;
		else if (mMaxZ < z)
			mMaxZ = z;
	}

	Info info;
	info.x = x;
	info.z = z;
	info.oldFlipFlag = oldFlipFlag;
	info.newFlipFlag = newFlipFlag;

	mInfos.push_back(info);
}
//-----------------------------------------------------------------------
bool GridInfoFlagModifyOperator::empty(void) const
{
	return mInfos.empty();
}
//-----------------------------------------------------------------------
const std::string& GridInfoFlagModifyOperator::getGroupName(void) const
{
	static const std::string groupName = mGroupName;
	return groupName;
}
//-----------------------------------------------------------------------
std::string GridInfoFlagModifyOperator::getDescription(void) const
{
	// TODO:
	return "";
}
//-----------------------------------------------------------------------
std::string GridInfoFlagModifyOperator::getHelp(void) const
{
	// TODO
	return "";
}
//-----------------------------------------------------------------------
void GridInfoFlagModifyOperator::undo(void)
{
	apply(&Info::oldFlipFlag);
}
//-----------------------------------------------------------------------
void GridInfoFlagModifyOperator::redo(void)
{
	apply(&Info::newFlipFlag);
}

//-----------------------------------------------------------------------
void GridInfoFlagModifyOperator::apply(uint Info::* flipFlag)
{
	if (!mInfos.empty())
	{
		for (InfoList::const_iterator i = mInfos.begin(); i != mInfos.end(); ++i)
		{
			const Info& info = *i;
			assert(EQSceneManager::getSingleton().getTerrainData()->isValidGrid(info.x, info.z));

			TerrainData::GridInfo gridinfo = EQSceneManager::getSingleton().getTerrainData()->getGridInfo(info.x, info.z);
			gridinfo.flags = info.*flipFlag;

			EQSceneManager::getSingleton().getTerrainData()->setGridInfo(info.x, info.z, gridinfo);
		}
		EQSceneManager::getSingleton().getTerrain()->notifyGridInfoModified(mMinX, mMinZ, mMaxX+1, mMaxZ+1);
	}
	RootManager::getSingleton().getHitIndicator(mIndicatorName)->refresh();
}

//////////////////////////////////////////////////////////////////////////
FlipTerrainDiagonalOperator::FlipTerrainDiagonalOperator()
{
	mIndicatorName = "FlipDiagonalGrids";
	mGroupName = "FlipDiagonal";
}

//////////////////////////////////////////////////////////////////////////
DisableDecalOperator::DisableDecalOperator()
{
	mIndicatorName = "DisableDecalGrids";
	mGroupName = "DisableDecal";
}
