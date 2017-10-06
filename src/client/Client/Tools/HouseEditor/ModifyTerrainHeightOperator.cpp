#include "HouseEditorPCH.h"
#include "ModifyTerrainHeightOperator.h"
#include "HitIndicator.h"

#include "TerrainData.h"
#include "Terrain.h"
#include "SceneListener.h"
#include "SceneManager.h"
#include "SceneInfo.h"
#include "RootManager.h"


ModifyTerrainHeightOperator::ModifyTerrainHeightOperator()
: mInfos()
{
}
//-----------------------------------------------------------------------
ModifyTerrainHeightOperator::~ModifyTerrainHeightOperator()
{
}
//-----------------------------------------------------------------------
void ModifyTerrainHeightOperator::add(int x, int z, Real oldHeight, Real newHeight)
{
	assert(EQSceneManager::getSingleton().getTerrainData()->isValidJunction(x, z));

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
	info.oldHeight = oldHeight;
	info.newHeight = newHeight;

	mInfos.push_back(info);
}
//-----------------------------------------------------------------------
bool ModifyTerrainHeightOperator::empty(void) const
{
	return mInfos.empty();
}
//-----------------------------------------------------------------------
const std::string& ModifyTerrainHeightOperator::getGroupName(void) const
{
	static const std::string groupName = "TerrainHeight";
	return groupName;
}
//-----------------------------------------------------------------------
std::string ModifyTerrainHeightOperator::getDescription(void) const
{
	// TODO:
	return "";
}
//-----------------------------------------------------------------------
std::string ModifyTerrainHeightOperator::getHelp(void) const
{
	// TODO
	return "";
}
//-----------------------------------------------------------------------
void ModifyTerrainHeightOperator::undo(void)
{
	apply(&Info::oldHeight);
}
//-----------------------------------------------------------------------
void ModifyTerrainHeightOperator::redo(void)
{
	apply(&Info::newHeight);
}
//-----------------------------------------------------------------------
void ModifyTerrainHeightOperator::apply(Real Info::* height)
{
	if (!mInfos.empty())
	{
		std::vector<TerrainInfo> terrainInfo;
		for (InfoList::const_iterator i = mInfos.begin(); i != mInfos.end(); ++i)
		{
			const Info& info = *i;
			assert(EQSceneManager::getSingleton().getTerrainData()->isValidJunction(info.x, info.z));

			if( (info.*height) == info.newHeight)
			{
				TerrainInfo terrInfo;
				terrInfo.x = info.x;
				terrInfo.z = info.z;
				terrInfo.oldHeight = info.oldHeight;
				terrInfo.nowHeight = info.*height;
				terrainInfo.push_back(terrInfo);
			}
			else
			{
				TerrainInfo terrInfo;
				terrInfo.x = info.x;
				terrInfo.z = info.z;
				terrInfo.oldHeight = info.newHeight;
				terrInfo.nowHeight = info.*height;
				terrainInfo.push_back(terrInfo);
			}

			EQSceneManager::getSingleton().getTerrainData()->setHeight(info.x, info.z, info.*height);
		}

		SceneListenerManager::getSingleton()._fireTerrainHeightChanged(terrainInfo);
		EQSceneManager::getSingleton().getTerrain()->notifyHeightModified(mMinX, mMinZ, mMaxX+1, mMaxZ+1);
	}

	// redo或undo后刷新指示器的位置
	RootManager::getSingleton().getHitIndicator("JunctionPoints")->refresh();
}

