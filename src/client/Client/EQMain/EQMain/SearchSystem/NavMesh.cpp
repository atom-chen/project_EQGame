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
	//�ٵ���������, ���Ч��, ��ȻON_Line������Ҳ��ò��ϸ�
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

		//����Ҫ�Ż�
		Cell* startCell = findClosestCell(startPos);
		Cell* endCell = findClosestCell(endPos);

		if (startCell == NULL || endCell == NULL)
			return false;

		//
		mStartPos = startPos;
		mEndPos = endPos;
		
		//
		mDirectThrough = false;
		if (startCell == endCell)												//��ͬһ����������
		{
			mDirectThrough = true;
			return true;
		}
		else if (lineThrough(Line2D(startPos, endPos), *startCell, *endCell))	//���ֱ�߿���ͨ��
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
		//��ʼ������
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

		bool foundPath = false;		//�Ƿ��ҵ�·��
		while(mOpenList.GetTop() != NULL)
		{
			// 1. �ѵ�ǰ�ڵ�ӿ����б�ɾ��, ���뵽����б�
			HeapPair hp = *mOpenList.GetTop();
			mOpenList.DeleteTop(true);
			Cell& currNode = (*mCells)[hp.Dat];
			currNode.isOpen = false;	//����������ᵼ��һЩ���ڵļ���
			mCloseList.push_back(currNode.getIndex());

			//·������ͬһ����������
			if (&currNode == &startCell)
			{
				foundPath = true;
				break;
			}

			// 2. �Ե�ǰ�ڵ����ڵ�ÿһ���ڵ�����ִ�����²���:
			Cell *adjacentTmpPt = NULL;
			//�����ڽ�������
			int adjacentId;
			for (int i = 0; i < 3; ++i)
			{
				adjacentId = currNode.getLink(i);
				// 3. ��������ڽڵ㲻��ͨ�л��߸����ڽڵ��Ѿ��ڷ���б���,
				//    ��ʲô����Ҳ��ִ��,����������һ���ڵ�;
				if ( adjacentId < 0 )
					continue;
				else
					adjacentTmpPt = &(*mCells)[adjacentId];
				assert(adjacentTmpPt);
				Cell& adjacentTmp = *adjacentTmpPt;

				if(adjacentTmp.sessionId != mPathSessionId)
				{
					// 4. ��������ڽڵ㲻�ڿ����б���,�򽫸ýڵ���ӵ������б���, 
					//    ���������ڽڵ�ĸ��ڵ���Ϊ��ǰ�ڵ�,ͬʱ��������ڽڵ��G��Fֵ;
					adjacentTmp.sessionId = mPathSessionId;
					adjacentTmp.parent = currNode.getIndex();
					adjacentTmp.isOpen = true;

					// �����ǰ��, ԭ����computeHeuristic�㷨����, ��Ҫʹ�������
					// remember the side this caller is entering from
					adjacentTmp.setAndGetArrivalWall(currNode.getIndex());

					//ԭ�����봦������ֵ, Ӧ������bug, �����޸�����.   --Y3
					//H,G��Fֵ
					adjacentTmp.computeHeuristic(startPos);
					//adjacentTmp.g = currNode.g + adjacentTmp.wallDistance[abs( i- currNode.arrivalWall)];	//��ȫ�������ⲽ�ڸ���? gֵ�������������ĵ��ֱ�߾�����ܸ���  --Y3
					//Gֵ�Ĵ���ʽ��Ϊ���������ĵ�֮��ľ���!! <<�������񲻶�, ���ֻ���Ӧ����������2�������е�������ĺ��� --Y3
					if ( currNode.parent == -1 )	//���ڵ�
					{
						assert( currNode.getIndex() == endCell.getIndex() );
						adjacentTmp.g = currNode.g + ptEndCellLen[i]; //���ڸ��ڵ�û��2��������, ���������gֵΪ�㵽�����ߵľ���
					}
					else
					{
						//����ȡ2�����ߵ��е�
						int aw = currNode.arrivalWall;
						assert(i != aw && aw != -1);
						int otherSide = Triangle::getOtherSide((Triangle::SIDE)aw, (Triangle::SIDE)i);
						adjacentTmp.g = currNode.g + currNode.getDistance(otherSide);
					}
					adjacentTmp.f = adjacentTmp.g + adjacentTmp.h;

					//���뿪���б�����
					HeapPair hp;
					hp.Dat = adjacentTmp.getIndex();
					hp.Key.f = adjacentTmp.f;
					mOpenList.Insert(&hp, true);

				}
				else
				{
					// 5. ��������ڽڵ��ڿ����б���, 
					//    ���ж������ɵ�ǰ�ڵ㵽������ڽڵ��Gֵ�Ƿ�С��ԭ�������Gֵ,
					//    ��С��,�򽫸����ڽڵ�ĸ��ڵ���Ϊ��ǰ�ڵ�,���������ø����ڽڵ��G��Fֵ
					if ( adjacentTmp.isOpen )	//�Ѿ���open list��
					{
						// ԭ��������fֵ���Ƚ�, ��Ϊgֵ�Ƚ�, ��ʵ����hֵ��һ�µ�, ����Ч����һ����.  --Y3
						//int g = currNode.g + adjacentTmp.wallDistance[abs( i- currNode.arrivalWall)];
						float g = 0;
						if ( currNode.parent == -1 )	//���ڵ�
						{
							assert( currNode.getIndex() == endCell.getIndex() );
							g = currNode.g + ptEndCellLen[i];
						}
						else
						{
							int aw = currNode.arrivalWall;
							assert(i != aw && aw != -1);	//���������������
							int otherSide = Triangle::getOtherSide((Triangle::SIDE)aw, (Triangle::SIDE)i);
							g = currNode.g + currNode.getDistance(otherSide);
						}

						if ( g < adjacentTmp.g)
						{
							adjacentTmp.g = g;		//ԭ���Ĵ���������.  --Y3
							adjacentTmp.f = adjacentTmp.g + adjacentTmp.h;
							adjacentTmp.parent = currNode.getIndex();

							//��������Ҫ���¶���ѵ�, ԭ���Ĵ���û����!   --Y3
							HeapPair hp;
							hp.Dat = adjacentTmp.getIndex();
							hp.Key.f = adjacentTmp.f;
							mOpenList.Update(mOpenList.GetDataIndex(hp.Dat), &hp, true);

							// remember the side this caller is entering from
							adjacentTmp.setAndGetArrivalWall(currNode.getIndex());
						}
					}
					else	//�Ѿ��� close list��
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
			//��ʼ��
			path.push_back(s);
			//������յ���ͬһ��������
			if (cellPath.size() == 1)
			{
				path.push_back(e);
				return;
			}

			if(true)
			{
				//��ȡ·��
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


			//�����������100, ��2���Ľڵ���Ŀ, �ͰѶ��ڵĿռ�ɾ��
			if (path.capacity() > 100 && path.capacity() > 2*path.size() )
				std::vector<Vector2f>().swap(path);
		}
	}
	//-------------------------------------------------------------------------
	//����ʵ��ƫ��յ��Ч��, Ч������
	//extern float CarmSqrt(float x);
	//Vector2f fun(Cell& cell, bool A, float range)
	//{
	//	//����������߶ξ���Ϊ0���ж�
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
		Vector2f startPt = wayPoint.postion; //��ǰ����·����
		Cell* cell = wayPoint.cell;
		Cell* lastCell = cell;
		//var startIndex:int = cellPath.indexOf(cell);	//��ʼ·�����ڵ���������
		Line2D outSide = cell->getSide((Triangle::SIDE)cell->arrivalWall);	//·�����������еĴ�����
		Vector2f lastPtA = outSide.EndPointA();
		Vector2f lastPtB = outSide.EndPointB();
		Line2D lastLineA(startPt, lastPtA);
		Line2D lastLineB(startPt, lastPtB);
		//Ҫ���Եĵ�
		Vector2f testPtA, testPtB;

		int lastIndex = startIndex;

		//�������ߵ�һ���ǿ����Ż���. ������cell�����ߵ�����, ����ֱ��Ҫnormalһ��
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

					//·��
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
						//����ֱ��
						lastLineA.SentEndPointB(lastPtA);
					}
				}
			}

			if ( lastPtB != testPtB )
			{
				if (fastClassifyPoint(lastLineA, testPtB, mEpsilon) == Line2D::RIGHT_SIDE)
				{
					//·����
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
						//����ֱ��
						lastLineB.SentEndPointB(lastPtB);
					}
				}
			}
		}


		startIndex = cellPath.size()-1;
		return WayPoint(cellPath[cellPath.size()-1], e);	//�յ�
	}
	//-------------------------------------------------------------------------
	bool NavMesh::lineThrough(Line2D& line, Cell& startCell, Cell& endCell)
	{
		//����ֻ�����߶��ཻ�γɵĴ��������. �ڱ߽�����¿��ܲ�׼ȷ
		//�������Ϊ��������������(�����߶ζ˵��ཻ�жϴ���), �������߶��غ��������Щ�߽�����,�����뿼��.
		//A�����ҪstartCell��, B�������endCell��

		//ͬһ��cell, ��ɹ�
		if ( startCell.getIndex() == endCell.getIndex() )
			return true;

		int throughNo;		//�ӵڼ����ߴ���

		throughNo = -1;
		for ( int i = 0; i < 3; ++i )
		{
			Line2D side = startCell.getSide(i);
			if (line.Intersection(side, NULL) == Line2D::SEGMENTS_INTERSECT)	//�߶��ཻ, ����ֱ���ཻ
			{
				throughNo = i;
				break;
			}
		}
		if (throughNo == -1)	//�����Ա��, ���ܰ�����������, �߶��غϵ����
			return false;
		// �赲���߽߱�
		if ( startCell.getLink(throughNo) == -1 )
			return false;

		//
		Cell* lastCell = &startCell;
		while(true)
		{
			Cell& cell = (*mCells)[lastCell->getLink(throughNo)];
			if ( cell.getIndex() == endCell.getIndex() )	//�����ͬһ��cell, ˵���ҵ�
				return true;

			int inNo = cell.getIndexByLink(lastCell->getIndex());
			throughNo = -1;
			for ( int i = 0; i < 3; ++i )	//�жϴ�����2��, �����ཻ
			{
				if ( i == inNo )
					continue;

				Line2D side = cell.getSide(i);
				if (line.Intersection(side, NULL) == Line2D::SEGMENTS_INTERSECT)	//�߶��ཻ
				{
					throughNo = i;
					break;
				}
			}
			if (throughNo == -1)	//�����Ա��
				return false;
			if ( cell.getLink(throughNo) == -1 )	//�赲���߽߱�
				return false;

			lastCell = &cell;
		}

		return false;
	}
}