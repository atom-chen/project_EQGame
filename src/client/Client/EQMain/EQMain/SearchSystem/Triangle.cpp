#include "Triangle.h"

namespace SearchSystem
{
	//-------------------------------------------------------------------------
	bool RealEqual( float a, float b, float tolerance )
	{
		if (fabs(b-a) <= tolerance)
			return true;
		else
			return false;
	}
	//-----------------------------------------------------------------------
	float Sign (float fValue)
	{
		if ( fValue > 0.0 )
			return 1.0;

		if ( fValue < 0.0 )
			return -1.0;

		return 0.0;
	}
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	Triangle::SIDE Triangle::getOtherSide(SIDE s0, SIDE s1)
	{
		assert(s0 != s1);

		int side[3] = {0};
		side[s0] = 1;
		side[s1] = 1;

		for(int i = 0; i < 3; ++i)
			if ( side[i] == 0 )
				return (SIDE)i;

		assert(0);
		return (SIDE)0;
	}
	//-------------------------------------------------------------------------
	Triangle::Triangle()
	{

	}
	//-------------------------------------------------------------------------
	Triangle::Triangle(const Vector2f& va, const Vector2f& vb, const Vector2f& vc)
		:  mPointA(va)
		, mPointB(vb)
		, mPointC(vc)
	{

	}
	//-------------------------------------------------------------------------
	void Triangle::setPoints(const Vector2f& va, const Vector2f& vb, const Vector2f& vc)
	{
		mPointA = va;
		mPointB = vb;
		mPointC = vc;
	}
	//-------------------------------------------------------------------------
	const Vector2f& Triangle::getVertex(int i) const
	{
		switch(i)
		{
		case 0:
			return mPointA;
		case 1:
			return mPointB;
		case 2:
			return mPointC;
		default:
			assert(0);
			return mPointA;
		}
	}
	//-------------------------------------------------------------------------
	void Triangle::setVertex(int i, const Vector2f& v)
	{
		switch(i)
		{
		case 0:
			mPointA = v;
			break;
		case 1:
			mPointB = v;
			break;
		case 2:
			mPointC = v;
			break;
		}
	}
	//-------------------------------------------------------------------------
	Line2D Triangle::getSide(SIDE side)
	{
		switch(side)
		{
		case SIDE_AB:
			return Line2D(mPointA, mPointB);
		case SIDE_BC:
			return Line2D(mPointB, mPointC);
		case SIDE_CA:
			return Line2D(mPointC, mPointA);
		default:
			assert(0);
			return Line2D();
		}
	}
	//-------------------------------------------------------------------------
	Line2D Triangle::getSide(int side)
	{
		return getSide((SIDE)side);
	}
	//-------------------------------------------------------------------------
	bool Triangle::isPointIn(const Vector2f& p, float epsilon)
	{
#if 0
		//这里会涉及到直线的开方,所以不用这个方式
		Line2D side[3];
		side[0] = getSide(SIDE_AB);
		side[1] = getSide(SIDE_BC);
		side[2] = getSide(SIDE_CA);

		// 点在所有边的右面, 或者左边, 这样就不在乎三角形的排序是cw,还是ccw
		int c0 = 0;
		int c1 = 0;
		for(int i = 0; i < 3; ++i )
		{
			Line2D::POINT_CLASSIFICATION ret = side[i].ClassifyPoint(v, epsilon);
			if (ret != Line2D::LEFT_SIDE)
				c0++;
			if (ret != Line2D::RIGHT_SIDE)
				c1++;
		}

		return (c0 == 3) || (c1 == 3);
#else
		//这段代码来自Ogre
		Vector2f& a = mPointA;
		Vector2f& b = mPointB;
		Vector2f& c = mPointC;

		// Winding must be consistent from all edges for point to be inside
		Vector2f v1, v2;
		float dot[3];
		bool zeroDot[3];

		v1 = b - a;
		v2 = p - a;

		// Note we don't care about normalisation here since sign is all we need
		// It means we don't have to worry about magnitude of cross products either
		dot[0] = v1.crossProduct(v2);
		zeroDot[0] = RealEqual(dot[0], 0.0f, epsilon);


		v1 = c - b;
		v2 = p - b;

		dot[1] = v1.crossProduct(v2);
		zeroDot[1] = RealEqual(dot[1], 0.0f, epsilon);

		// Compare signs (ignore colinear / coincident points)
		if(!zeroDot[0] && !zeroDot[1] 
		&& Sign(dot[0]) != Sign(dot[1]))
		{
			return false;
		}

		v1 = a - c;
		v2 = p - c;

		dot[2] = v1.crossProduct(v2);
		zeroDot[2] = RealEqual(dot[2], 0.0f, epsilon);
		// Compare signs (ignore colinear / coincident points)
		if((!zeroDot[0] && !zeroDot[2] 
		&& Sign(dot[0]) != Sign(dot[2])) ||
			(!zeroDot[1] && !zeroDot[2] 
		&& Sign(dot[1]) != Sign(dot[2])))
		{
			return false;
		}


		return true;
#endif
	}
	//-------------------------------------------------------------------------
	Vector2f Triangle::getCenter()
	{
		return (mPointA + mPointB + mPointC) / 3.0f;
	}
}