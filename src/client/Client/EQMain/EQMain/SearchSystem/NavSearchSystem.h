/******************************************************************************
* 作者: Y3
* 时间: 2010-09-15
* 描述: 导航网格寻路系统
******************************************************************************/
#ifndef SSystem_NavSearchSystem_H
#define SSystem_NavSearchSystem_H

#include "SSystemCommon.h"
#include "NavMesh.h"
#include "Polygon.h"
#include "Cell.h"

#ifndef USING_NAVSAVELOAD
#define USING_NAVSAVELOAD
#endif

namespace SearchSystem
{
	/** 导航网格寻路系统
	*/
	class NavSearchSystem
	{
	public:
		/// 设置边界
		void setEdge(Vector2f edgeBegin, Vector2f edgeEnd);

		void build(std::vector<Polygon>& polys);

		void destroy();

#ifdef USING_NAVSAVELOAD
		void save(const std::string& filename);

		bool load(const std::string& filename);
#endif

		/// 寻路
		bool findPath(const Vector2f& startPointPx, const Vector2f& endPointPx);

		/// 获取路径
		void getPath(std::vector<Vector2f>& path);

	public:
		NavSearchSystem( float epsilon = 0.0001f );

		std::vector<Cell>& getCells() {return mCells;}

	private:
		float				mEpsilon;		//误差精度

		Vector2f			mEdgeBegin;
		Vector2f			mEdgeEnd;

		std::vector<Cell>	mCells;

		NavMesh				mNavMesh;
	};
}

#endif