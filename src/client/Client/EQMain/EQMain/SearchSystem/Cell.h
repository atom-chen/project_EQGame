/******************************************************************************
* 作者: Y3
* 时间: 2010-09-19
* 描述: 
		参考网址: http://blianchen.blog.163.com/
******************************************************************************/
#ifndef SSystem_Cell_H
#define SSystem_Cell_H

#include "Triangle.h"

namespace SearchSystem
{
	class Cell : public Triangle
	{
	public:
		float		f;
		float		h;
		float		g;

		bool		isOpen;
		int			parent;
		int			sessionId;

		Vector2f	center;

		int			arrivalWall;	//!< the side we arrived through.
		//Vector2f	wallMidpoint[3];//!< 每个边的中点
		//float		wallDistance[3];//!< the distances between each wall midpoint of sides (0-1, 1-2, 2-0)

	public:
		Cell(const Vector2f& va, const Vector2f& vb, const Vector2f& vc, int index);
		Cell(const Vector2f& va, const Vector2f& vb, const Vector2f& vc, int index, int link[3]);

		/// 检查并设置当前三角型与cellB的连接关系（方法会同时设置cellB与该三角型的连接）
		void checkAndLink(Cell& cellB);

		/// 获取关联的三角形索引
		int getLink(int i) {return mLinks[i];}

		/// 获取连接三角形的索引, 确保link必须是有效值
		int getIndexByLink(int link);

		/// 点到3线中点的距离
		void lengthPointToMidLine(Vector2f pt, float outLen[3]);

		/// 设置距离
		void setDistance(int i, float d) {mDistance[i] = d;}

		/// 获取距离
		float getDistance(int i) {return mDistance[i];}

		/// 记录路径从上一个节点进入该节点的边（如果从终点开始寻路即为穿出边）. index为上个节点的索引
		int setAndGetArrivalWall(int index);

		/// 计算估价（h）  Compute the A* Heuristic for this cell given a Goal point
		void computeHeuristic(const Vector2f& goal);


		int getIndex() {return mIndex;}

	protected:
		/// 获得两个点的相邻三角型. 首先必须这2个点在三角形上, 才返回true
		bool requestLink(const Vector2f& pA, const Vector2f& pB, int cellIdx);

	private:
		/// 不允许拷贝
		Cell();

	protected:
		int			mIndex;			//!< 索引值
		int			mLinks[3];		//!< 与该三角型连接的三角型索引， -1表示改边没有连接
		float		mDistance[3];	//!< 距离.
		Vector2f	mMidPoint[3];	//!< 中点

	};
}


#endif