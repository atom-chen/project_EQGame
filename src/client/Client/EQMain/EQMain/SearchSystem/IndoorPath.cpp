#include "EQMainPCH.h"
#include "IndoorPath.h"
#include "AStarPath.h"
#include "../HouseScene.h"

//-----------------------------------------------------------------------
// 判断能否通过两个格子
static bool _canPass(int x1, int z1, int x2, int z2, 
						const HouseSceneFloor * floor, const HouseSceneGrid * firstGrid)
{
	const HouseSceneGrid * a = floor->getGrid(x1, z1);
	const HouseSceneGrid * b = floor->getGrid(x2, z2);

	// 如果包含空格子, 肯定是无法直线穿越的
	if (!(a && b))
		return false;

	// 起始格，无条件通过
	if (a == firstGrid || b == firstGrid)
		return true;

	// 如果有阻挡, 也是无法直线穿越的
	if (a->isBlocked()|| b->isBlocked())
		return false;

	// 如果不同房间, 也是无法直线穿越的
	if (a->getRoomID() != b->getRoomID())
		return false;

   // 如果中间有墙, 也是无法直线穿越的
	int face = 0;
	if (a->isNeighborGrid(b, face))
	{
		if (a->getWallFace(face))
		{
			return false;
		}
	}	

	return true;
}

// 基本原理
// 直线斜率公式:k = (z2 - z1) / (x2 - x1) 
// 所以有
//        z = z1 + (x - x1) * k;  当x 值知道时，这个可用来求z
//        x = x1 + (z - z1) / k;  当z 值知道时，这个可用来求x

// 所以就能算出直线与格子四边的交点, 进而知道哪些格子被直线穿过
//	 _____ /
//	|     |  
//	|    /|
//	|___/_|
//	   /	 
//	  /
//-----------------------------------------------------------------------
// 判断能否直线穿越(只能对同一房间进行判断)
static bool _canLineThrough(const Ogre::Vector3 & startPos, const Ogre::Vector3 & endPos,
				   HouseScene * hs, 
				   const HouseSceneFloor * floor)
{
	Ogre::Vector3 centerVec = hs->getSceneNode()->_getDerivedPosition();

	Ogre::Vector3 startVec = startPos - centerVec;
	Ogre::Vector3 endVec = endPos - centerVec;
	const float x1 = endVec.x / HOUSE_GRID_SIZE;
	const float z1 = endVec.z / HOUSE_GRID_SIZE;
	const float x2 = startVec.x / HOUSE_GRID_SIZE;
	const float z2 = startVec.z / HOUSE_GRID_SIZE;

	// 起始点所在格
	const HouseSceneGrid * firstGrid = floor->getGrid(Ogre::Math::IFloor(x2), Ogre::Math::IFloor(z2));
	
	const float playerRadius = 5.0f / HOUSE_GRID_SIZE; // 玩家半径(单位厘米), 这个值太大会影响操作

	float fBase = (x2 - x1);
	// 防止除0
	if(Ogre::Math::Abs(fBase) < 1e-6f)
	{
		fBase = fBase > 0.0f ? 1e-6f : -(1e-6f);
	}
	// 求出直线斜率
	const float k = (z2 - z1) / fBase;

	
	{// 通过与X轴交叉的点, 算出被穿过的格子 
		// 终点加上身体半径，比较准确
		int startX = x1 < x2 ? Ogre::Math::IFloor(x1 - playerRadius) : Ogre::Math::IFloor(x1 + playerRadius);
		// 起点就不考率身体半径了，不然有时会走不出来
		int endX = x1 < x2 ? Ogre::Math::IFloor(x2) : Ogre::Math::IFloor(x2);

		if (endX < startX)
			std::swap(startX, endX);

		for (int x = startX + 1; x <= endX; x += 1)
		{
			float zFloat = z1 + (x - x1) * k;

			// 人的身体是需要占用空间的, 所以分成两点来处理
			int z = Ogre::Math::IFloor(zFloat + playerRadius);
			if (!_canPass(x -1, z, x, z, floor, firstGrid)) 
				return false;

			z = Ogre::Math::IFloor(zFloat - playerRadius);
			if (!_canPass(x -1, z, x, z, floor, firstGrid)) 
				return false;
		}
	}

	{// 通过与Z轴交叉的点, 算出被穿过的格子 
		// 终点加上身体半径，比较准确
		int startZ = z1 < z2 ? Ogre::Math::IFloor(z1 - playerRadius) : Ogre::Math::IFloor(z1 + playerRadius);
		int endZ = z1 < z2 ? Ogre::Math::IFloor(z2) : Ogre::Math::IFloor(z2);

		if (endZ < startZ)
			std::swap(startZ, endZ);

		for (int z = startZ + 1; z <= endZ; z += 1)
		{
			float xFloat = x1 + (z - z1) / k;

			// 人的身体是需要占用空间的, 所以分成两点来处理
			int x = Ogre::Math::IFloor(xFloat + playerRadius);	
			if (!_canPass(x, z - 1, x, z, floor, firstGrid)) 
				return false;

			x = Ogre::Math::IFloor(xFloat - playerRadius);				
			if (!_canPass(x, z - 1, x, z, floor, firstGrid)) 
				return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------
// 优化这个房间路径
template <typename TIter, typename TOutPath>
static void _processRoomPath(TIter beginRoom, TIter endRoom, 
						HouseScene * hs, const HouseSceneFloor * floor, 
						TOutPath & path)
{
	for (TIter a = beginRoom;;)
	{
		// 如果路径不足3点， 返回
		if (endRoom - a < 3)
		{
			for (; a < endRoom; ++a)
			{
				path.push_back(a->second);
			}
			return;
		}

		// 找直线路径
		TIter b = a + 1;
		TIter c = b + 1;
		for (; c < endRoom; ++b, ++c)
		{
			if (!_canLineThrough(a->second, c->second, hs, floor))
				break;
		}

		path.push_back(a->second);

		a = b;
	}
}

//-----------------------------------------------------------------------
// 移除多余的格子
static void _removeFatGrids(std::vector<const HouseSceneGrid *> & searchResult)
{
	// 为防止过多的直线判断，这里先去掉相同方向中间点	
	std::vector<size_t> removeList;
	size_t idxA = 0;
	size_t idxB = 1;
	size_t idxC = 2;
	for (; idxC < searchResult.size(); ++idxA, ++idxB, ++idxC)
	{
		const HouseSceneGrid * a = searchResult[idxA];
		const HouseSceneGrid * b = searchResult[idxB];
		const HouseSceneGrid * c = searchResult[idxC];

		if (a->getRoomID() == b->getRoomID() && b->getRoomID() == c->getRoomID()) // 如果同房间
		{
			if ((a->getGridX() - b->getGridX() == b->getGridX() - c->getGridX()) &&
				(a->getGridZ() - b->getGridZ() == b->getGridZ() - c->getGridZ())) // 如果方向相同
			{
				// 标记要去掉的中间点
				removeList.push_back(idxB);
			}
		}
	}			

	// 头尾也去掉
	removeList.push_back(0);
	removeList.push_back(searchResult.size() - 1);

	std::vector<const HouseSceneGrid *> tmpArr;
	for (size_t i = 0; i < searchResult.size(); ++i)
	{
		if (std::find(removeList.begin(), removeList.end(), i) == removeList.end())
		{
			tmpArr.push_back(searchResult[i]);
		}
	}

	searchResult.swap(tmpArr);
}

//-----------------------------------------------------------------------
// 回调函数
// 功能：计算从一格移动到另一格所需权值
class FunGetCostValue
{
public:
	int operator () (const HouseSceneGrid * src, const HouseSceneGrid * dest) const
	{
		// 因为室内是四方向寻路，所以权值直接用格子距离abs(x - x0) + abs(y - y0) + abs(z - z0)来算		
		int x = src->getGridX();
		int z = src->getGridZ();
		int y = src->getCreator()->getFloorHeight();		

		int xDest = dest->getGridX();
		int zDest = dest->getGridZ();
		int yDest = dest->getCreator()->getFloorHeight();		
		
		// 权值计算(y方向需要换算成格子单位)
		return abs(x - xDest) + abs(z - zDest) + abs(y - yDest) / HOUSE_GRID_SIZE;	
	}
};

//-----------------------------------------------------------------------
// 回调函数
// 功能：收集相邻的非阻挡格子
class FunGetNearList
{
public:
	typedef std::multimap<const HouseSceneGrid *, const HouseSceneGrid *> TGridMap;
public:
	explicit FunGetNearList(const TGridMap & gridMap)
		: mGridMap(gridMap)
	{
	}
public:
	FunGetNearList(const FunGetNearList & r)
		: mGridMap(r.mGridMap)
	{
	}
private:
	const HouseSceneGrid * _addToAroundList(int x, int z,
											std::vector<const HouseSceneGrid *> & aroundList, 
											const HouseSceneGrid * src,
											const HouseSceneFloor * floor) const
	{		 
		const HouseSceneGrid * grid = floor->getGrid(x, z);

		// 判断是否同房间, 是否有阻挡, 是否有墙
		if (grid && !grid->isBlocked() 
			&& grid->getRoomID() == src->getRoomID())
		{
			int face = 0;
			if (grid->isNeighborGrid(src, face))
			{
				if (!grid->getWallFace(face))
				{
					aroundList.push_back(grid);
					return grid;
				}
			}
		}

		return NULL;
	}
public:
	void operator () (const HouseSceneGrid * src, std::vector<const HouseSceneGrid *> & aroundList) const
	{
		int x = src->getGridX();
		int z = src->getGridZ();

		// 取楼层
		const HouseSceneFloor * floor = src->getCreator();

		// 左 
		const HouseSceneGrid * left = _addToAroundList(x - 1, z, aroundList, src, floor);

		// 右 
		const HouseSceneGrid * right = _addToAroundList(x + 1, z, aroundList, src, floor);

		// 上 
		const HouseSceneGrid * up = _addToAroundList(x, z - 1, aroundList, src, floor);

		// 下 
		const HouseSceneGrid * down = _addToAroundList(x, z + 1, aroundList, src, floor);

		// 房间连通点
		typedef TGridMap::const_iterator TIter;
		std::pair<TIter, TIter> ab = mGridMap.equal_range(src);
		for (TIter it = ab.first; it != ab.second; ++it)
		{
			if (!it->second->isBlocked())
				aroundList.push_back(it->second);
		}
 	}
private:
	const TGridMap & mGridMap; // 保存连通点映射，以便快速查寻
};

//-----------------------------------------------------------------------
void IndoorPath::findPath(const Ogre::Vector3 & startPos, const Ogre::Vector3 & endPos, 
		std::queue<Ogre::Vector3> & path, HouseScene * hs)
{
	// 清空路径			
	for (; path.size() > 0;)
		path.pop();

	const HouseSceneGrid * startGrid = hs->gridByPosition(startPos); 
	const HouseSceneGrid * endGrid = endGrid = hs->gridByPosition(endPos);

	if (startGrid && endGrid)
	{
		typedef std::vector<const HouseSceneGrid *> TSearchResult;

		TSearchResult searchResult;

		// 保存连通点映射，以便快速查寻
		FunGetNearList::TGridMap gridMap;
		HouseScene::PairGridList::const_iterator it = hs->getPairGridList().begin();
		for ( ; it != hs->getPairGridList().end(); ++it)
		{
			gridMap.insert(std::make_pair(it->first, it->second));
			gridMap.insert(std::make_pair(it->second, it->first));
		}
		// 声明回调函数
		FunGetNearList fnGetNearList(gridMap);
		FunGetCostValue fnGetCostValue;

		AStarPathSearcher::run(startGrid, endGrid, searchResult, 
								fnGetCostValue,    // 回调从一格移动到另一格所需权值
								fnGetNearList      // 回调取附近的点
								);

		if (!searchResult.empty()) // 如果找到路径
		{
			// 移除多余的格子
			_removeFatGrids(searchResult);

			// 由于头尾点并非格子中心点，所以需要重新组织点和格子的关系
			typedef std::pair<const HouseSceneGrid *, Ogre::Vector3>  TPair;
			typedef std::vector<TPair> TRawPath;
			TRawPath rawPath;
			rawPath.push_back(TPair(startGrid, startPos));
			for (size_t i = 0; i < searchResult.size(); ++i)
			{
				Ogre::Vector3 pos = searchResult[i]->getSceneNode()->_getDerivedPosition();
				rawPath.push_back(TPair(searchResult[i], pos));
			}
			rawPath.push_back(TPair(endGrid, endPos));


			// 直线行走优化
			std::vector<Ogre::Vector3> posArray;
			for (TRawPath::const_iterator beginRoom = rawPath.begin(); 
				beginRoom < rawPath.end(); )
			{
				// 取得一个房间
				const HouseSceneFloor * curFloor = beginRoom->first->getCreator();
				int curRoom = beginRoom->first->getRoomID();
				TRawPath::const_iterator endRoom = beginRoom + 1;
				for (; endRoom < rawPath.end(); ++endRoom)
				{
					if (endRoom->first->getCreator() != curFloor || 
						endRoom->first->getRoomID() != curRoom)
						break;
				}

				// 优化这个房间路径
				_processRoomPath(beginRoom, endRoom, hs, curFloor, posArray);

				beginRoom = endRoom;
			}

			// 最终填充路径
			for (size_t i = 0; i < posArray.size(); ++i)
				path.push(posArray[i]);
		}
	}
}
