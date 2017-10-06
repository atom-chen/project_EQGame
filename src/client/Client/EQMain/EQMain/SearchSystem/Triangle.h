/******************************************************************************
* 作者: Y3
* 时间: 2010-09-15
* 描述: 简单三角形
******************************************************************************/
#ifndef SSystem_Triangle_H
#define SSystem_Triangle_H

#include "Line2D.h"

namespace SearchSystem
{
	class Triangle
	{
	public:
		enum SIDE {
			SIDE_AB = 0,
			SIDE_BC,
			SIDE_CA,
		};

		static SIDE getOtherSide(SIDE s0, SIDE s1);

	public:
		Triangle();
		Triangle(const Vector2f& va, const Vector2f& vb, const Vector2f& vc);

		void setPoints(const Vector2f& va, const Vector2f& vb, const Vector2f& vc);

		const Vector2f& getPointA() const {return mPointA;}
		const Vector2f& getPointB() const {return mPointB;}
		const Vector2f& getPointC() const {return mPointC;}
		const Vector2f& getVertex(int i) const;
		void setVertex(int i, const Vector2f& v);

		Vector2f getCenter();
		Line2D getSide(SIDE side);
		Line2D getSide(int side);

		bool isPointIn(const Vector2f& v, float epsilon = 0.000001f);

	protected:
		Vector2f	mPointA;		//!< 顶点A
		Vector2f	mPointB;		//!< 顶点B
		Vector2f	mPointC;		//!< 顶点C
	};
}

#endif