/******************************************************************************
* ����: ����
* ʱ��: 2011-08-16
* ����: A��Ѱ·�㷨
******************************************************************************/

#ifndef AStarPath_H
#define AStarPath_H


class AStarPathSearcher
{
public:
	template <typename TPos, typename TOutPath, typename TGetCostValue, typename TGetNearList>
	static bool run(TPos startPos, TPos endPos, TOutPath & outPath, 
						 TGetCostValue getCostValue,    // �ص�ȡGֵ  
						 TGetNearList getNearList)       // �ص�ȡ�����ĵ�
	{
		typedef AStarNode<TPos> TNode;
		typedef std::multiset<TNode *, OrderByFValue> TOpenList;
		typedef std::set<TNode *, OrderByPos> TNodeList;

		outPath.clear();

		AStarNodePool<TPos> nodePool; // �ڴ��
		TNodeList nodeList; // �ڵ��б�(��λ������)
		TOpenList openList; // �����б�(��Fֵ����)

		// �����׸��ڵ�
		TNode * curNode = nodePool.createNode(startPos, 0, 0, NULL); 
		openList.insert(curNode);
		nodeList.insert(curNode);

		TNode tmpNode;
		std::vector<TPos> nearList;  // �ܱߵ��б�
		for (; !openList.empty();)
		{
			// ȡ��Fֵ��С�Ľڵ�
			curNode = (*openList.begin());

			// ����ýڵ�ΪĿ��ڵ�, ����·�������Ѱ·
			if (curNode->mPos == endPos)
			{
				// ȡ��·��
				_getPath(curNode, outPath);
				return true;
			}
			
			// �ӿ����б���ɾ���ýڵ�
			openList.erase(openList.begin());
			
			// ��ɹر�״̬
			curNode->mIsOpened = false;

			// ȡ�ܱߵĵ�
			nearList.clear();
			getNearList(curNode->mPos, nearList);

			// ������Щ�ܱߵ�			
			for (size_t i = 0; i < nearList.size(); ++i)
			{
				_processNewPos(nearList[i], endPos, curNode, tmpNode, nodePool, nodeList, openList, getCostValue);
			}
		}
		return false;
	}

private:
	/* �ڵ�
	*/
	template <typename TPos>
	struct AStarNode
	{
	private:
		typedef AStarNode my_type;
	public:
		TPos mPos;    // λ����Ϣ(�����Ƕ�ά����㣬��ά����㣬Ҳ�����Ǹ���, Ҳ�����Ƕ����)
		int mG;       // ;����������ĵ�ֵ
		int mF;       // ��Ȩֵ F = G + H, Hָ���յ��Ȩֵ
		bool mIsOpened;     // �Ƿ���
		my_type * mParent;  // ���ڵ㣬�����������·����
	};

private:
	// ��Fֵ������
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
	// ��λ��������
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
	// �ڵ��(����һ�����ͷ�ȫ���ڴ�)
	template <typename TPos>
	class AStarNodePool
	{
	private:
		typedef AStarNode<TPos> TNode;
		enum {e_segment_size = 64 * 1024 / sizeof(TNode)}; // ÿ��64k
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
			p->mIsOpened = true;  // Ĭ�Ͽ���
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

		// ��ת·��
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
						TGetCostValue getCostValue   // �ص�ȡGֵ  
						)
	{
		tmpNode.mPos = newPos;
		// �ж�newPos�Ƿ�����nodeList��(ע: nodeList���������б��Լ��ر��б�)
		TNodeList::const_iterator iFind = nodeList.find(&tmpNode);
		if (iFind != nodeList.end()) // ����ҵ�
		{
			if ((*iFind)->mIsOpened) // ����ڿ����б���
			{						
				// �ж��Ƿ������Fֵ
				int newG = getCostValue(newPos, curNode->mPos) + curNode->mG;
				if (newG < (*iFind)->mG) // ������ĸ�С
				{
					// ȡ����ͬFֵ����
					std::pair<TOpenList::const_iterator, TOpenList::const_iterator> ab = openList.equal_range(*iFind);
					for (TOpenList::const_iterator iOpen = ab.first; iOpen != ab.second; ++iOpen)
					{
						if (*iOpen == *iFind) // �ҵ���Ӧ�ڵ�
						{
							// ���¿����б��Fֵ						
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
			// �����½ڵ�
			TNode * nd = nodePool.createNode(newPos,      // λ��
											getCostValue(newPos, curNode->mPos) + curNode->mG,  // Gֵ,
											getCostValue(newPos, endPos),         // Hֵ	
											curNode);                          // ���ڵ�

			// �����б�����½ڵ�
			openList.insert(nd);
			nodeList.insert(nd);
		}
	}
};


#endif


