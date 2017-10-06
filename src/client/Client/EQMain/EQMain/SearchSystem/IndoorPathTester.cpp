#include "EQMainPCH.h"
#include "IndoorPathTester.h"
#include "IndoorPath.h"
#include "../HouseScene.h"
#include "../SceneManager.h"
#include "../CommFunc.h"
#include "../EQLog.h"

static std::vector<const HouseSceneGrid *> getAllGrids(HouseScene * hs)
{
	std::vector<const HouseSceneGrid *> grids;
	typedef const std::map<int, HouseSceneFloor*> TFloors; 
	for (TFloors::const_iterator it = hs->getFloors().begin();
		it != hs->getFloors().end(); ++it)
	{
		const HouseSceneFloor * flr = it->second;

		for (HouseSceneFloor::GridMap::const_iterator iGrid = flr->getGridMap().begin();
			iGrid != flr->getGridMap().end(); ++iGrid)
		{
			grids.push_back(iGrid->second);
		}		
	}

	return grids;
}

//-----------------------------------------------------------------------
void IndoorPathTester::run()
{
	HouseScene * hs = EQSceneManager::getSingleton().getHouseScene();

	std::queue<Ogre::Vector3> path;

	std::vector<const HouseSceneGrid *> grids = getAllGrids(hs);
	int startTick = GetTickCount();
	LOGMSG("开始对当前房间所有路径进行测试");
	for (size_t x = 0; x < grids.size(); ++x)
	{
		const HouseSceneGrid * startGrid = grids[x];
		const Ogre::Vector3 startPos = startGrid->getSceneNode()->_getDerivedPosition();

		for (size_t y = 0; y < grids.size(); ++y)
		{
			const HouseSceneGrid * endGrid = grids[y];
			const Ogre::Vector3 endPos = endGrid->getSceneNode()->_getDerivedPosition();
			IndoorPath::findPath(startPos, 
								endPos, 
								path, hs);
		}
		LOGMSG(FormatStr("测试进度 %d/%d", x, grids.size()));
	}
	LOGMSG("测试完成");
	int endTick = GetTickCount();

	int findPathPerSecond = grids.size() * grids.size() / ((endTick - startTick) / 1000);
	LOGMSG(FormatStr("平均每秒完成%d次寻路", findPathPerSecond));
}
