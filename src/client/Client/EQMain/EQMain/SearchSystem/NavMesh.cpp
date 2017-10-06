#include "NavMesh.h"


namespace SearchSystem
{
	//-------------------------------------------------------------------------
	struct NavMesh::WayPoint
	{
		Vector2f postion;
		Cell* cell;

		WayPoint(Cell* c, const Vector2f& p)
			: cell(c), postion(p)
		{

		}
	};
	//-------------------------------------------------------------------------
	int NavMesh::mPathSessionId = 0;
	//-------------------------------------------------------------------------
	//少掉开方操作, 提高效率, 当然ON_Line的意义也变得不严格
	int fastClassifyPoint(const Line2D& line, const Vector2f& pt, float eps )
	{
		Vector2f vn = line.EndPointB() - line.EndPointA();
		float OldYValue = vn.y;
		vn.y = vn.x;
		vn.x = -OldYValue;

		Vector2f v = pt - line.EndPointA();
		float d = v.dotProduct(vn);

		if (d > eps)
		{
			return  Line2D::RIGHT_SIDE;
		}
		else if (d < -eps)
		{
			return  Line2D::LEFT_SIDE;
		}
		else
			return Line2D::ON_LINE;
	}
	//-------------------------------------------------------------------------
	void NavMesh::clear()
	{
		mOpenList.Destroy();
		mCloseList.clear();
	}
	//-------------------------------------------------------------------------
	void NavMesh::setCells(std::vector<Cell>* cells)
	{
		mCells = cells;

		mCloseList.reserve(mCells->size());
	}
	//-------------------------------------------------------------------------
	Cell* NavMesh::findClosestCell(const Vector2f& pt)
	{
		for (size_t i = 0; i < mCells->size(); ++i)
		{
			if( (*mCells)[i].isPointIn(pt) )
				return &(*mCells)[i];
		}
		return NULL;
	}
	//-------------------------------------------------------------------------
	bool NavMesh::findPath(const Vector2f& startPos, const Vector2f& endPos)
	{
		mPathSessionId++;

		//这里要优化
		Cell* startCell = findClosestCell(startPos);
		Cell* endCell = findClosestCell(endPos);

		if (startCell == NULL || endCell == NULL)
			return false;

		//
		mStartPos = startPos;
		mEndPos = endPos;
		
		//
		mDirectThrough = false;
		if (startCell == endCell)												//在同一个三角形内
		{
			mDirectThrough = true;
			return true;
		}
		else if (lineThrough(Line2D(startPos, endPos), *startCell, *endCell))	//如果直线可以通过
		{
			mDirectThrough = true;
			return true;
		}
		else
			return buildPath(*startCell, startPos, *endCell, endPos);
	}
	//-------------------------------------------------------------------------
	bool NavMesh::buildPath(Cell& startCell, const Vector2f& startPos, Cell& endCell, const Vector2f& endPos)
	{
		//初始化数据
		mCloseList.clear();
		mOpenList.Create(mCells->size());

		//
		endCell.g = 0;
		endCell.f = 0;
		endCell.h = 0;
		endCell.isOpen = false;
		endCell.parent = -1;
		endCell.sessionId = mPathSessionId;
		endCell.arrivalWall = -1;

		float ptEndCellLen[3];
		endCell.lengthPointToMidLine(endPos, ptEndCellLen);

		HeapPair ehp;
		ehp.Dat = endCell.getIndex();
		ehp.Key.f = endCell.f;
		mOpenList.Insert(&ehp, true);

		bool foundPath = false;		//是否找到路径
		while(mOpenList.GetTop() != NULL)
		{
			// 1. 把当前节点从开放列表删除, 加入到封闭列表
			HeapPair hp = *mOpenList.GetTop();
			mOpenList.DeleteTop(true);
			Cell& currNode = (*mCells)[hp.Dat];
			currNode.isOpen = false;	//不设置这个会导致一些多于的计算
			mCloseList.push_back(currNode.getIndex());

			//路径是在同一个三角形内
			if (&currNode == &startCell)
			{
				foundPath = true;
				break;
			}

			// 2. 对当前节点相邻的每一个节点依次执行以下步骤:
			Cell *adjacentTmpPt = NULL;
			//所有邻接三角型
			int adjacentId;
			for (int i = 0; i < 3; ++i)
			{
				adjacentId = currNode.getLink(i);
				// 3. 如果该相邻节点不可通行或者该相邻节点已经在封闭列表中,
				//    则什么操作也不执行,继续检验下一个节点;
				if ( adjacentId < 0 )
					continue;
				else
					adjacentTmpPt = &(*mCells)[adjacentId];
				assert(adjacentTmpPt);
				Cell& adjacentTmp = *adjacentTmpPt;

				if(adjacentTmp.sessionId != mPathSessionId)
				{
					// 4. 如果该相邻节点不在开放列表中,则将该节点添加到开放列表中, 
					//    并将该相邻节点的父节点设为当前节点,同时保存该相邻节点的G和F值;
					adjacentTmp.sessionId = mPathSessionId;
					adjacentTmp.parent = currNode.getIndex();
					adjacentTmp.isOpen = true;

					// 这句提前了, 原因是computeHeuristic算法边了, 需要使用入射边
					// remember the side this caller is entering from
					adjacentTmp.setAndGetArrivalWall(currNode.getIndex());

					//原来代码处理评估值, 应该是有bug, 所以修改了下.   --Y3
					//H,G和F值
					adjacentTmp.computeHeuristic(startPos);
					//adjacentTmp.g = currNode.g + adjacentTmp.wallDistance[abs( i- currNode.arrivalWall)];	//完全看不懂这步在干嘛? g值改用三角形中心点的直线距离可能更好  --Y3
					//G值的处理方式改为三角形中心点之间的距离!! <<这样好像不对, 发现还是应该用三角形2穿出边中点距离来的合适 --Y3
					if ( currNode.parent == -1 )	//根节点
					{
						assert( currNode.getIndex() == endCell.getIndex() );
						adjacentTmp.g = currNode.g + ptEndCellLen[i]; //由于根节点没有2个穿出边, 所以这个的g值为点到穿出边的距离
					}
					else
					{
						//这里取2穿出边的中点
						int aw = currNode.arrivalWall;
						assert(i != aw && aw != -1);
						int otherSide = Triangle::getOtherSide((Triangle::SIDE)aw, (Triangle::SIDE)i);
						adjacentTmp.g = currNode.g + currNode.getDistance(otherSide);
					}
					adjacentTmp.f = adjacentTmp.g + adjacentTmp.h;

					//放入开放列表并排序
					HeapPair hp;
					hp.Dat = adjacentTmp.getIndex();
					hp.Key.f = adjacentTmp.f;
					mOpenList.Insert(&hp, true);

				}
				else
				{
					// 5. 如果该相邻节点在开放列表中, 
					//    则判断若经由当前节点到达该相邻节点的G值是否小于原来保存的G值,
					//    若小于,则将该相邻节点的父节点设为当前节点,并重新设置该相邻节点的G和F值
					if ( adjacentTmp.isOpen )	//已经在open list中
					{
						// 原来代码是f值做比较, 改为g值比较, 其实由于h值是一致的, 所以效果是一样的.  --Y3
						//int g = currNode.g + adjacentTmp.wallDistance[abs( i- currNode.arrivalWall)];
						float g = 0;
						if ( currNode.parent == -1 )	//根节点
						{
							assert( currNode.getIndex() == endCell.getIndex() );
							g = currNode.g + ptEndCellLen[i];
						}
						else
						{
							int aw = currNode.arrivalWall;
							assert(i != aw && aw != -1);	//不会有折情况产生
							int otherSide = Triangle::getOtherSide((Triangle::SIDE)aw, (Triangle::SIDE)i);
							g = currNode.g + currNode.getDistance(otherSide);
						}

						if ( g < adjacentTmp.g)
						{
							adjacentTmp.g = g;		//原来的代码这句错了.  --Y3
							adjacentTmp.f = adjacentTmp.g + adjacentTmp.h;
							adjacentTmp.parent = currNode.getIndex();

							//这里是需要更新二叉堆的, 原来的代码没更新!   --Y3
							HeapPair hp;
							hp.Dat = adjacentTmp.getIndex();
							hp.Key.f = adjacentTmp.f;
							mOpenList.Update(mOpenList.GetDataIndex(hp.Dat), &hp, true);

							// remember the side this caller is entering from
							adjacentTmp.setAndGetArrivalWall(currNode.getIndex());
						}
					}
					else	//已经在 close list中
					{
						continue;
					}
				}
			}
		}

		return foundPath;
	}
	//-------------------------------------------------------------------------
	void NavMesh::getPath(std::vector<Vector2f>& path)
	{
		return _getPath(path, mStartPos, mEndPos);
	}
	//-------------------------------------------------------------------------
	void NavMesh::_getPath(std::vector<Vector2f>& path, const Vector2f& s, const Vector2f& e)
	{
		if ( mDirectThrough )
		{
			path.reserve(2);
			path.push_back(s);
			path.push_back(e);
		}
		else
		{
			std::vector<Cell*> cellPath;
			assert(mCloseList.size()>0);
			int ix = mCloseList[mCloseList.size()-1];
			Cell* st = &(*mCells)[ix];

			int c = 0;
			while(true)
			{
				c++;
				if ( st->parent == -1 )
					break;
				st = &(*mCells)[st->parent];
			}
			cellPath.reserve(c);

			st = &(*mCells)[ix];
			while(true)
			{
				cellPath.push_back(st);

				if ( st->parent == -1 )
					break;
				st = &(*mCells)[st->parent];
			}

			//
			assert(cellPath.size()>0);
			path.reserve(cellPath.size()+2);
			//开始点
			path.push_back(s);
			//起点与终点在同一三角形中
			if (cellPath.size() == 1)
			{
				path.push_back(e);
				return;
			}

			if(true)
			{
				//获取路点
				WayPoint wayPoint(cellPath[0], s);
				int index = 0;
				while (wayPoint.postion != e)
				{
					wayPoint = getFurthestWayPoint(wayPoint, cellPath, index, e);
					path.push_back(wayPoint.postion);
				}
			}
			else
			{
				for ( size_t i = 0; i < cellPath.size(); ++i )
					path.push_back(cellPath[i]->center);
			}


			//如果容量大于100, 且2倍的节点数目, 就把多于的空间删除
			if (path.capacity() > 100 && path.capacity() > 2*path.size() )
				std::vector<Vector2f>().swap(path);
		}
	}
	//-------------------------------------------------------------------------
	//可以实现偏离拐点的效果, 效果不好
	//extern float CarmSqrt(float x);
	//Vector2f fun(Cell& cell, bool A, float range)
	//{
	//	//这里忽略了线段距离为0的判断
	//	Line2D l = cell.getSide((Triangle::SIDE)cell.arrivalWall);
	//	assert(l.EndPointA() != l.EndPointB());

	//	if ( A )
	//	{
	//		Vector2f dir = l.EndPointB() - l.EndPointA();
	//		float sq = CarmSqrt(dir.squaredLength());
	//		float is = 1.0f/sq;
	//		dir.x *= is;
	//		dir.y *= is;
	//		float r = sq < range*2.0f ? sq*0.5f : range;
	//		return dir*r + l.EndPointA();
	//	}
	//	else
	//	{
	//		Vector2f dir = l.EndPointA() - l.EndPointB();
	//		float sq = CarmSqrt(dir.squaredLength());
	//		float is = 1.0f/sq;
	//		dir.x *= is;
	//		dir.y *= is;
	//		float r = sq < range*2.0f ? sq*0.5f : range;
	//		return dir*r + l.EndPointB();
	//	}
	//}
	//-------------------------------------------------------------------------
	NavMesh::WayPoint NavMesh::getFurthestWayPoint(WayPoint wayPoint, std::vector<Cell*>& cellPath, int& startIndex, const Vector2f& e)
	{
		Vector2f startPt = wayPoint.postion; //当前所在路径点
		Cell* cell = wayPoint.cell;
		Cell* lastCell = cell;
		//var startIndex:int = cellPath.indexOf(cell);	//开始路点所在的网格索引
		Line2D outSide = cell->getSide((Triangle::SIDE)cell->arrivalWall);	//路径线在网格中的穿出边
		Vector2f lastPtA = outSide.EndPointA();
		Vector2f lastPtB = outSide.EndPointB();
		Line2D lastLineA(startPt, lastPtA);
		Line2D lastLineB(startPt, lastPtB);
		//要测试的点
		Vector2f testPtA, testPtB;

		int lastIndex = startIndex;

		//计算在线的一侧是可以优化的. 考虑让cell保留边的数据, 否则直线要normal一次
		for ( int i = startIndex+1; i < (int)cellPath.size(); ++i )
		{
			cell = cellPath[i];
			if ( i == (int)cellPath.size() - 1 )
			{
				testPtA = e;
				testPtB = e;
			}
			else
			{
				assert(cell->arrivalWall>=0 && cell->arrivalWall <3);
				outSide = cell->getSide((Triangle::SIDE)cell->arrivalWall);
				testPtA = outSide.EndPointA();
				testPtB = outSide.EndPointB();
			}

			if ( lastPtA != testPtA )
			{
				if ( fastClassifyPoint(lastLineB, testPtA, mEpsilon) == Line2D::LEFT_SIDE )
				{

					//路点
					startIndex = lastIndex;
					//return WayPoint(lastCell, fun(*lastCell, false, 40.0f));
					return WayPoint(lastCell, lastPtB);
				}
				else
				{
					if (fastClassifyPoint(lastLineA, testPtA, mEpsilon) != Line2D::RIGHT_SIDE)
					{
						lastPtA = testPtA;
						lastIndex = i;
						lastCell = cell;
						//重设直线
						lastLineA.SentEndPointB(lastPtA);
					}
				}
			}

			if ( lastPtB != testPtB )
			{
				if (fastClassifyPoint(lastLineA, testPtB, mEpsilon) == Line2D::RIGHT_SIDE)
				{
					//路径点
					startIndex = lastIndex;
					//return WayPoint(lastCell, fun(*lastCell, true, 40.0f));
					return WayPoint(lastCell, lastPtA);
				} 
				else 
				{
					if (fastClassifyPoint(lastLineB, testPtB, mEpsilon) != Line2D::LEFT_SIDE) 
					{
						lastPtB = testPtB;
						lastIndex = i;
						lastCell = cell;
						//重设直线
						lastLineB.SentEndPointB(lastPtB);
					}
				}
			}
		}


		startIndex = cellPath.size()-1;
		return WayPoint(cellPath[cellPath.size()-1], e);	//终点
	}
	//-------------------------------------------------------------------------
	bool NavMesh::lineThrough(Line2D& line, Cell& startCell, Cell& endCell)
	{
		//这里只考虑线段相交形成的穿出边情况. 在边界情况下可能不准确
		//如果是因为浮点数精度问题(比如线段端点相交判断错误), 或者是线段重合问题等这些边界问题,将不与考虑.
		//A点必须要startCell里, B点必须在endCell里

		//同一个cell, 则成功
		if ( startCell.getIndex() == endCell.getIndex() )
			return true;

		int throughNo;		//从第几条边穿出

		throughNo = -1;
		for ( int i = 0; i < 3; ++i )
		{
			Line2D side = startCell.getSide(i);
			if (line.Intersection(side, NULL) == Line2D::SEGMENTS_INTERSECT)	//线段相交, 不是直线相交
			{
				throughNo = i;
				break;
			}
		}
		if (throughNo == -1)	//防御性编程, 可能包括精度问题, 线段重合等情况
			return false;
		// 阻挡或者边界
		if ( startCell.getLink(throughNo) == -1 )
			return false;

		//
		Cell* lastCell = &startCell;
		while(true)
		{
			Cell& cell = (*mCells)[lastCell->getLink(throughNo)];
			if ( cell.getIndex() == endCell.getIndex() )	//如果是同一个cell, 说明找到
				return true;

			int inNo = cell.getIndexByLink(lastCell->getIndex());
			throughNo = -1;
			for ( int i = 0; i < 3; ++i )	//判断穿出的2边, 哪条相交
			{
				if ( i == inNo )
					continue;

				Line2D side = cell.getSide(i);
				if (line.Intersection(side, NULL) == Line2D::SEGMENTS_INTERSECT)	//线段相交
				{
					throughNo = i;
					break;
				}
			}
			if (throughNo == -1)	//防御性编程
				return false;
			if ( cell.getLink(throughNo) == -1 )	//阻挡或者边界
				return false;

			lastCell = &cell;
		}

		return false;
	}
}