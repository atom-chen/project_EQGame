/******************************************************************************
* ����: Y3
* ʱ��: 2010-09-21
* ����: ��������Ѱ·
		�ο���ַ: http://blianchen.blog.163.com/
******************************************************************************/
#ifndef SSystem_NavMesh_H
#define SSystem_NavMesh_H

#include "Cell.h"
#include "BinaryHeap.h"

namespace SearchSystem
{
	class NavMesh
	{
		struct WayPoint;
	public:
		float	mEpsilon;

	public:
		NavMesh() : mEpsilon(0.000001f) {}

		//���
		void clear();

		void setCells(std::vector<Cell>* cells);

		/// �ҳ����������ڵ�������. ����㷨����Ҫ�Ż���. TODO:�Ժ���˵
		Cell* findClosestCell(const Vector2f& pt);
		
		/// Ѱ·
		bool findPath(const Vector2f& startPointPx, const Vector2f& endPointPx);

		/// ��ȡ·��
		void getPath(std::vector<Vector2f>& path);


	private:
		bool buildPath(Cell& startCell, const Vector2f& startPos, Cell& endCell, const Vector2f& endPos);
		/// ���ݾ����������η���·����(��һ���սǵ㷨)
		void _getPath(std::vector<Vector2f>& path, const Vector2f& s, const Vector2f& e);

		/// ��һ���յ�
		WayPoint getFurthestWayPoint(WayPoint wayPoint, std::vector<Cell*>& cellPath, int& startIndex, const Vector2f& e);

		/// ����ֱ��ͨ��
		bool lineThrough(Line2D& line, Cell& startCell, Cell& endCell);

	private:
		/// Path finding session ID. This Identifies each pathfinding session
		/// so we do not need to clear out old data in the cells from previous sessions.
		/// ò��ֻ�������ж�cell�Ƿ���ʹ�ù��ı��
		static int				mPathSessionId;

		std::vector<Cell>*		mCells;

		CBinaryHeap				mOpenList;
		std::vector<int>		mCloseList;

		Vector2f				mStartPos;
		Vector2f				mEndPos;

		bool					mDirectThrough;
	};
}

#endif