#include "EQMainPCH.h"
#include "IndoorPath.h"
#include "AStarPath.h"
#include "../HouseScene.h"

//-----------------------------------------------------------------------
// �ж��ܷ�ͨ����������
static bool _canPass(int x1, int z1, int x2, int z2, 
						const HouseSceneFloor * floor, const HouseSceneGrid * firstGrid)
{
	const HouseSceneGrid * a = floor->getGrid(x1, z1);
	const HouseSceneGrid * b = floor->getGrid(x2, z2);

	// ��������ո���, �϶����޷�ֱ�ߴ�Խ��
	if (!(a && b))
		return false;

	// ��ʼ��������ͨ��
	if (a == firstGrid || b == firstGrid)
		return true;

	// ������赲, Ҳ���޷�ֱ�ߴ�Խ��
	if (a->isBlocked()|| b->isBlocked())
		return false;

	// �����ͬ����, Ҳ���޷�ֱ�ߴ�Խ��
	if (a->getRoomID() != b->getRoomID())
		return false;

   // ����м���ǽ, Ҳ���޷�ֱ�ߴ�Խ��
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

// ����ԭ��
// ֱ��б�ʹ�ʽ:k = (z2 - z1) / (x2 - x1) 
// ������
//        z = z1 + (x - x1) * k;  ��x ֵ֪��ʱ�������������z
//        x = x1 + (z - z1) / k;  ��z ֵ֪��ʱ�������������x

// ���Ծ������ֱ��������ıߵĽ���, ����֪����Щ���ӱ�ֱ�ߴ���
//	 _____ /
//	|     |  
//	|    /|
//	|___/_|
//	   /	 
//	  /
//-----------------------------------------------------------------------
// �ж��ܷ�ֱ�ߴ�Խ(ֻ�ܶ�ͬһ��������ж�)
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

	// ��ʼ�����ڸ�
	const HouseSceneGrid * firstGrid = floor->getGrid(Ogre::Math::IFloor(x2), Ogre::Math::IFloor(z2));
	
	const float playerRadius = 5.0f / HOUSE_GRID_SIZE; // ��Ұ뾶(��λ����), ���ֵ̫���Ӱ�����

	float fBase = (x2 - x1);
	// ��ֹ��0
	if(Ogre::Math::Abs(fBase) < 1e-6f)
	{
		fBase = fBase > 0.0f ? 1e-6f : -(1e-6f);
	}
	// ���ֱ��б��
	const float k = (z2 - z1) / fBase;

	
	{// ͨ����X�ύ��ĵ�, ����������ĸ��� 
		// �յ��������뾶���Ƚ�׼ȷ
		int startX = x1 < x2 ? Ogre::Math::IFloor(x1 - playerRadius) : Ogre::Math::IFloor(x1 + playerRadius);
		// ���Ͳ���������뾶�ˣ���Ȼ��ʱ���߲�����
		int endX = x1 < x2 ? Ogre::Math::IFloor(x2) : Ogre::Math::IFloor(x2);

		if (endX < startX)
			std::swap(startX, endX);

		for (int x = startX + 1; x <= endX; x += 1)
		{
			float zFloat = z1 + (x - x1) * k;

			// �˵���������Ҫռ�ÿռ��, ���Էֳ�����������
			int z = Ogre::Math::IFloor(zFloat + playerRadius);
			if (!_canPass(x -1, z, x, z, floor, firstGrid)) 
				return false;

			z = Ogre::Math::IFloor(zFloat - playerRadius);
			if (!_canPass(x -1, z, x, z, floor, firstGrid)) 
				return false;
		}
	}

	{// ͨ����Z�ύ��ĵ�, ����������ĸ��� 
		// �յ��������뾶���Ƚ�׼ȷ
		int startZ = z1 < z2 ? Ogre::Math::IFloor(z1 - playerRadius) : Ogre::Math::IFloor(z1 + playerRadius);
		int endZ = z1 < z2 ? Ogre::Math::IFloor(z2) : Ogre::Math::IFloor(z2);

		if (endZ < startZ)
			std::swap(startZ, endZ);

		for (int z = startZ + 1; z <= endZ; z += 1)
		{
			float xFloat = x1 + (z - z1) / k;

			// �˵���������Ҫռ�ÿռ��, ���Էֳ�����������
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
// �Ż��������·��
template <typename TIter, typename TOutPath>
static void _processRoomPath(TIter beginRoom, TIter endRoom, 
						HouseScene * hs, const HouseSceneFloor * floor, 
						TOutPath & path)
{
	for (TIter a = beginRoom;;)
	{
		// ���·������3�㣬 ����
		if (endRoom - a < 3)
		{
			for (; a < endRoom; ++a)
			{
				path.push_back(a->second);
			}
			return;
		}

		// ��ֱ��·��
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
// �Ƴ�����ĸ���
static void _removeFatGrids(std::vector<const HouseSceneGrid *> & searchResult)
{
	// Ϊ��ֹ�����ֱ���жϣ�������ȥ����ͬ�����м��	
	std::vector<size_t> removeList;
	size_t idxA = 0;
	size_t idxB = 1;
	size_t idxC = 2;
	for (; idxC < searchResult.size(); ++idxA, ++idxB, ++idxC)
	{
		const HouseSceneGrid * a = searchResult[idxA];
		const HouseSceneGrid * b = searchResult[idxB];
		const HouseSceneGrid * c = searchResult[idxC];

		if (a->getRoomID() == b->getRoomID() && b->getRoomID() == c->getRoomID()) // ���ͬ����
		{
			if ((a->getGridX() - b->getGridX() == b->getGridX() - c->getGridX()) &&
				(a->getGridZ() - b->getGridZ() == b->getGridZ() - c->getGridZ())) // ���������ͬ
			{
				// ���Ҫȥ�����м��
				removeList.push_back(idxB);
			}
		}
	}			

	// ͷβҲȥ��
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
// �ص�����
// ���ܣ������һ���ƶ�����һ������Ȩֵ
class FunGetCostValue
{
public:
	int operator () (const HouseSceneGrid * src, const HouseSceneGrid * dest) const
	{
		// ��Ϊ�������ķ���Ѱ·������Ȩֱֵ���ø��Ӿ���abs(x - x0) + abs(y - y0) + abs(z - z0)����		
		int x = src->getGridX();
		int z = src->getGridZ();
		int y = src->getCreator()->getFloorHeight();		

		int xDest = dest->getGridX();
		int zDest = dest->getGridZ();
		int yDest = dest->getCreator()->getFloorHeight();		
		
		// Ȩֵ����(y������Ҫ����ɸ��ӵ�λ)
		return abs(x - xDest) + abs(z - zDest) + abs(y - yDest) / HOUSE_GRID_SIZE;	
	}
};

//-----------------------------------------------------------------------
// �ص�����
// ���ܣ��ռ����ڵķ��赲����
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

		// �ж��Ƿ�ͬ����, �Ƿ����赲, �Ƿ���ǽ
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

		// ȡ¥��
		const HouseSceneFloor * floor = src->getCreator();

		// �� 
		const HouseSceneGrid * left = _addToAroundList(x - 1, z, aroundList, src, floor);

		// �� 
		const HouseSceneGrid * right = _addToAroundList(x + 1, z, aroundList, src, floor);

		// �� 
		const HouseSceneGrid * up = _addToAroundList(x, z - 1, aroundList, src, floor);

		// �� 
		const HouseSceneGrid * down = _addToAroundList(x, z + 1, aroundList, src, floor);

		// ������ͨ��
		typedef TGridMap::const_iterator TIter;
		std::pair<TIter, TIter> ab = mGridMap.equal_range(src);
		for (TIter it = ab.first; it != ab.second; ++it)
		{
			if (!it->second->isBlocked())
				aroundList.push_back(it->second);
		}
 	}
private:
	const TGridMap & mGridMap; // ������ͨ��ӳ�䣬�Ա���ٲ�Ѱ
};

//-----------------------------------------------------------------------
void IndoorPath::findPath(const Ogre::Vector3 & startPos, const Ogre::Vector3 & endPos, 
		std::queue<Ogre::Vector3> & path, HouseScene * hs)
{
	// ���·��			
	for (; path.size() > 0;)
		path.pop();

	const HouseSceneGrid * startGrid = hs->gridByPosition(startPos); 
	const HouseSceneGrid * endGrid = endGrid = hs->gridByPosition(endPos);

	if (startGrid && endGrid)
	{
		typedef std::vector<const HouseSceneGrid *> TSearchResult;

		TSearchResult searchResult;

		// ������ͨ��ӳ�䣬�Ա���ٲ�Ѱ
		FunGetNearList::TGridMap gridMap;
		HouseScene::PairGridList::const_iterator it = hs->getPairGridList().begin();
		for ( ; it != hs->getPairGridList().end(); ++it)
		{
			gridMap.insert(std::make_pair(it->first, it->second));
			gridMap.insert(std::make_pair(it->second, it->first));
		}
		// �����ص�����
		FunGetNearList fnGetNearList(gridMap);
		FunGetCostValue fnGetCostValue;

		AStarPathSearcher::run(startGrid, endGrid, searchResult, 
								fnGetCostValue,    // �ص���һ���ƶ�����һ������Ȩֵ
								fnGetNearList      // �ص�ȡ�����ĵ�
								);

		if (!searchResult.empty()) // ����ҵ�·��
		{
			// �Ƴ�����ĸ���
			_removeFatGrids(searchResult);

			// ����ͷβ�㲢�Ǹ������ĵ㣬������Ҫ������֯��͸��ӵĹ�ϵ
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


			// ֱ�������Ż�
			std::vector<Ogre::Vector3> posArray;
			for (TRawPath::const_iterator beginRoom = rawPath.begin(); 
				beginRoom < rawPath.end(); )
			{
				// ȡ��һ������
				const HouseSceneFloor * curFloor = beginRoom->first->getCreator();
				int curRoom = beginRoom->first->getRoomID();
				TRawPath::const_iterator endRoom = beginRoom + 1;
				for (; endRoom < rawPath.end(); ++endRoom)
				{
					if (endRoom->first->getCreator() != curFloor || 
						endRoom->first->getRoomID() != curRoom)
						break;
				}

				// �Ż��������·��
				_processRoomPath(beginRoom, endRoom, hs, curFloor, posArray);

				beginRoom = endRoom;
			}

			// �������·��
			for (size_t i = 0; i < posArray.size(); ++i)
				path.push(posArray[i]);
		}
	}
}
