/******************************************************************************
* 作者: Y3
* 时间: 2010-09-21
* 描述: 导航网格寻路
		参考网址: http://blianchen.blog.163.com/
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

		//清除
		void clear();

		void setCells(std::vector<Cell>* cells);

		/// 找出给定点所在的三角型. 这个算法是需要优化的. TODO:以后再说
		Cell* findClosestCell(const Vector2f& pt);
		
		/// 寻路
		bool findPath(const Vector2f& startPointPx, const Vector2f& endPointPx);

		/// 获取路径
		void getPath(std::vector<Vector2f>& path);


	private:
		bool buildPath(Cell& startCell, const Vector2f& startPos, Cell& endCell, const Vector2f& endPos);
		/// 根据经过的三角形返回路径点(下一个拐角点法)
		void _getPath(std::vector<Vector2f>& path, const Vector2f& s, const Vector2f& e);

		/// 下一个拐点
		WayPoint getFurthestWayPoint(WayPoint wayPoint, std::vector<Cell*>& cellPath, int& startIndex, const Vector2f& e);

		/// 可以直线通过
		bool lineThrough(Line2D& line, Cell& startCell, Cell& endCell);

	private:
		/// Path finding session ID. This Identifies each pathfinding session
		/// so we do not need to clear out old data in the cells from previous sessions.
		/// 貌似只是用来判定cell是否有使用过的标记
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