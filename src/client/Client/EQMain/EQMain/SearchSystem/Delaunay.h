/******************************************************************************
* 作者: Y3
* 时间: 2010-09-15
* 描述: Delaunay三角划分
*		参考网址: http://blianchen.blog.163.com/blog/static/131056299201037102315211/
******************************************************************************/
#ifndef SSystem_Delaunay_H
#define SSystem_Delaunay_H

#include "Triangle.h"
#include "Polygon.h"

namespace SearchSystem
{
	class Delaunay
	{
		class Circle;
		class Rectangle;
	public:
		Delaunay(float epsilon = 0.000001f)
			: mEpsilon(epsilon)
		{
		}

		const std::vector<Triangle>& createDelaunay(std::vector<Polygon>& polys, const Vector2f& edgeBegin, const Vector2f& edgeEnd);

		const std::vector<Triangle>& createDelaunay(Polygon& poly);

	private:
		/// 初始化数据
		void initData(const std::vector<Polygon>& polys, const Vector2f& edgeBegin, const Vector2f& edgeEnd);

		/// 获取初始外边界
		Line2D getInitOutEdge();

		/// 计算 DT 点
		std::pair<bool, Vector2f> findDT(const Line2D& line);

		/// 判断点vec是否为line的可见点
		bool isVisiblePointOfLine(const Vector2f vec, Line2D line);

		/// 点pa和pb是否可见(pa和pb构成的线段不与任何约束边相交，不包括顶点)
		bool isVisibleIn2Point(const Vector2f& pa, const Vector2f& pb);

		/// 返回三角形的外接圆
		Circle circumCircle(const Vector2f& p1, const Vector2f& p2, const Vector2f& p3);

		/// 返回圆的包围盒
		Rectangle circleBounds(const Circle& c);

		/// 返回顶角在o点，起始边为os，终止边为oe的夹角, 即∠soe (单位：弧度);  角度小于pi，返回正值;   角度大于pi，返回负值 
		float lineAngle(const Vector2f& s, const Vector2f& o, const Vector2f& e); 

		/// 判断线段是否是约束边. 返回线段的索引，如果没有找到，返回-1
		int indexOfVector(const Line2D& line, const std::vector<Line2D>& vectors);

		/// 计算
		void calc();

	private:
		float					mEpsilon;		//精度

		std::vector<Vector2f>	mVertexs;		//所有顶点列表, 前outEdgeVecNmu个为外边界顶点
		std::vector<Line2D>		mEdges;			//所有约束边

		//int						mOutEdgeVecNmu;	//区域外边界顶点数
		Line2D					mBeginLine;

		std::vector<Line2D>		mLines;			//线段堆栈

		std::vector<Triangle>	mTriangles;		//生成的Delaunay三角形

	};
}

#endif