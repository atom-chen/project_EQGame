/******************************************************************************
* 作者: 锦旭
* 时间: 2011-08-16
* 描述: A星寻路算法
******************************************************************************/

#ifndef AStarPath_H
#define AStarPath_H


class AStarPathSearcher
{
public:
	template <typename TPos, typename TOutPath, typename TGetCostValue, typename TGetNearList>
	static bool run(TPos startPos, TPos endPos, TOutPath & outPath, 
						 TGetCostValue getCostValue,    // 回调取G值  
						 TGetNearList getNearList)       // 回调取附近的点
	{
		typedef AStarNode<TPos> TNode;
		typedef std::multiset<TNode *, OrderByFValue> TOpenList;
		typedef std::set<TNode *, OrderByPos> TNodeList;

		outPath.clear();

		AStarNodePool<TPos> nodePool; // 内存池
		TNodeList nodeList; // 节点列表(按位置排序)
		TOpenList openList; // 开启列表(按F值排序)

		// 加入首个节点
		TNode * curNode = nodePool.createNode(startPos, 0, 0, NULL); 
		openList.insert(curNode);
		nodeList.insert(curNode);

		TNode tmpNode;
		std::vector<TPos> nearList;  // 周边点列表
		for (; !openList.empty();)
		{
			// 取出F值最小的节点
			curNode = (*openList.begin());

			// 如果该节点为目标节点, 返回路径，完成寻路
			if (curNode->mPos == endPos)
			{
				// 取得路径
				_getPath(curNode, outPath);
				return true;
			}
			
			// 从开启列表中删除该节点
			openList.erase(openList.begin());
			
			// 设成关闭状态
			curNode->mIsOpened = false;

			// 取周边的点
			nearList.clear();
			getNearList(curNode->mPos, nearList);

			// 遍历这些周边点			
			for (size_t i = 0; i < nearList.size(); ++i)
			{
				_processNewPos(nearList[i], endPos, curNode, tmpNode, nodePool, nodeList, openList, getCostValue);
			}
		}
		return false;
	}

private:
	/* 节点
	*/
	template <typename TPos>
	struct AStarNode
	{
	private:
		typedef AStarNode my_type;
	public:
		TPos mPos;    // 位置信息(可以是二维坐标点，三维坐标点，也可以是格子, 也可以是多边形)
		int mG;       // 途经点节所消耗的值
		int mF;       // 总权值 F = G + H, H指到终点的权值
		bool mIsOpened;     // 是否开启
		my_type * mParent;  // 父节点，最后用来生成路径的
	};

private:
	// 按F值排序函数
	class OrderByFValue
	{
	public:
		template <typename TNode>
		bool operator ()(TNode * a, TNode * b) const
		{
			return a->mF < b->mF;
		}
	};

private:
	// 按位置排序函数
	class OrderByPos
	{
	public:
		template <typename TNode>
		bool operator ()(TNode * a, TNode * b) const
		{
			return a->mPos < b->mPos;
		}
	};

private:
	// 节点池(最后会一次性释放全部内存)
	template <typename TPos>
	class AStarNodePool
	{
	private:
		typedef AStarNode<TPos> TNode;
		enum {e_segment_size = 64 * 1024 / sizeof(TNode)}; // 每段64k
	public:
		AStarNodePool()
			: mAllocCount(0)
		{
		}
	public:
		~AStarNodePool()
		{
			for (size_t i = 0; i < mSegments.size(); ++i)
			{
				TNode * pArr = mSegments[i];
				delete [] pArr;
			}
		}
	public:
		TNode * createNode(TPos pos, int g, int h, TNode * pParent)
		{
			TNode * p = newNode();

			p->mPos = pos;
			p->mG = g;
			p->mF = g + h;
			p->mIsOpened = true;  // 默认开启
			p->mParent = pParent;
			return p;
		}
	private:
		TNode * newNode()
		{
			size_t nSeg = mAllocCount / e_segment_size;
			for (; mSegments.size() <= nSeg;)
			{
				mSegments.push_back(new TNode[e_segment_size]);
			}

			size_t idx = mAllocCount % e_segment_size;

			++mAllocCount;
			return mSegments[nSeg] + idx;
		}
	private:
		AStarNodePool(const AStarNodePool &);
		void operator = (const AStarNodePool &);
	private:
		std::vector<TNode *> mSegments; 
		int mAllocCount;
	};
	
private:
	template <typename TNode, typename TOutPath>
	static void _getPath(const TNode * lastNode, TOutPath & outPath)
	{	
		for (const TNode * p = lastNode; p; p = p->mParent)
		{		
			outPath.push_back(p->mPos);
		}

		// 反转路径
		std::reverse(outPath.begin(), outPath.end());
	}
private:
	template <typename TNode, typename TPos, typename TNodePool, 
				typename TNodeList, typename TOpenList, typename TGetCostValue>
	static void _processNewPos(const TPos & newPos, const TPos & endPos, 
						TNode * curNode,
						TNode & tmpNode,	
						TNodePool & nodePool,
						TNodeList & nodeList, 
						TOpenList & openList,
						TGetCostValue getCostValue   // 回调取G值  
						)
	{
		tmpNode.mPos = newPos;
		// 判断newPos是否已在nodeList中(注: nodeList包含开启列表，以及关闭列表)
		TNodeList::const_iterator iFind = nodeList.find(&tmpNode);
		if (iFind != nodeList.end()) // 如果找到
		{
			if ((*iFind)->mIsOpened) // 如果在开启列表中
			{						
				// 判断是否需更新F值
				int newG = getCostValue(newPos, curNode->mPos) + curNode->mG;
				if (newG < (*iFind)->mG) // 如果消耗更小
				{
					// 取得相同F值区间
					std::pair<TOpenList::const_iterator, TOpenList::const_iterator> ab = openList.equal_range(*iFind);
					for (TOpenList::const_iterator iOpen = ab.first; iOpen != ab.second; ++iOpen)
					{
						if (*iOpen == *iFind) // 找到对应节点
						{
							// 更新开启列表的F值						
							openList.erase(iOpen);
							(*iFind)->mF -= ((*iFind)->mG - newG);
							(*iFind)->mG = newG;
							(*iFind)->mParent = curNode;
							openList.insert(*iFind);
							break;
						}
					}
				}
			}
		}
		else
		{
			// 创建新节点
			TNode * nd = nodePool.createNode(newPos,      // 位置
											getCostValue(newPos, curNode->mPos) + curNode->mG,  // G值,
											getCostValue(newPos, endPos),         // H值	
											curNode);                          // 父节点

			// 开启列表加入新节点
			openList.insert(nd);
			nodeList.insert(nd);
		}
	}
};


#endif


