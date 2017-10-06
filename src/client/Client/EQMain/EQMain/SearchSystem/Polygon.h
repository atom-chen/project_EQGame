/******************************************************************************
* 作者: Y3
* 时间: 2010-09-15
* 描述: 简单多边形
******************************************************************************/
#ifndef SSystem_Polygon_H
#define SSystem_Polygon_H

#include "Line2D.h"

namespace SearchSystem
{
	class Polygon
	{
	public:
		void addVertex(const Vector2f& v) {mVertexs.push_back(v);}

		const std::vector<Vector2f>& getVertexs() const {return mVertexs;}

		bool isCW() const;

		bool isCCW() const;

		void CW();

		void CCW();

	private:
		std::vector<Vector2f> mVertexs;
	};
}

#endif
