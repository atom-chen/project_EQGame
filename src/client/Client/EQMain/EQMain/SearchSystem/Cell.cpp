#include "Cell.h"


namespace SearchSystem
{
	//-------------------------------------------------------------------------
	// http://www.cfannet.com/bbs/dispbbs.asp?boardID=11&ID=151&page=2
	//
	// Carmack在QUAKE3中使用的计算平方根的函数.
	// 误差自己初步测试下, 大概在2%
	// 这个函数不具备移植性, 因为有神奇的magic number. 存储格式在其它系统是不一致的, 比如mac
	//
	float CarmSqrt(float x){
		union{
			int intPart;
			float floatPart;
		} convertor;
		union{
			int intPart;
			float floatPart;
		} convertor2;
		convertor.floatPart = x;
		convertor2.floatPart = x;
		convertor.intPart = 0x1FBCF800 + (convertor.intPart >> 1);
		convertor2.intPart = 0x5f3759df - (convertor2.intPart >> 1);
		return 0.5f*(convertor.floatPart + (x * convertor2.floatPart));
	}
	//-------------------------------------------------------------------------
	Cell::Cell(const Vector2f& va, const Vector2f& vb, const Vector2f& vc, int index)
		: Triangle(va, vb, vc)
		, mIndex(index)
	{
		sessionId = 0;
		mMidPoint[0] = getSide(0).GetMidPoint();
		mMidPoint[1] = getSide(1).GetMidPoint();
		mMidPoint[2] = getSide(2).GetMidPoint();

		mLinks[0] = -1;
		mLinks[1] = -1;
		mLinks[2] = -1;
		mDistance[0] = 0.0f;
		mDistance[1] = 0.0f;
		mDistance[2] = 0.0f;
		arrivalWall = -1;

		//计算中心点
		center = getCenter();
	}
	//-------------------------------------------------------------------------
	Cell::Cell(const Vector2f& va, const Vector2f& vb, const Vector2f& vc, int index, int link[3])
		: Triangle(va, vb, vc)
		, mIndex(index)
	{
		sessionId = 0;
		mMidPoint[0] = getSide(0).GetMidPoint();
		mMidPoint[1] = getSide(1).GetMidPoint();
		mMidPoint[2] = getSide(2).GetMidPoint();

		mLinks[0] = link[0];
		mLinks[1] = link[1];
		mLinks[2] = link[2];
		mDistance[0] = 0.0f;
		mDistance[1] = 0.0f;
		mDistance[2] = 0.0f;
		arrivalWall = -1;

		//计算中心点
		center = getCenter();
	}
	//-------------------------------------------------------------------------
	bool Cell::requestLink(const Vector2f& pA, const Vector2f& pB, int cellIdx)
	{
		if (mPointA.equals(pA)) {
			if (mPointB.equals(pB)) {
				mLinks[SIDE_AB] = cellIdx;
				return (true);
			} else if (mPointC.equals(pB)) {
				mLinks[SIDE_CA] = cellIdx;
				return (true);
			}
		} else if (mPointB.equals(pA)) {
			if (mPointA.equals(pB)) {
				mLinks[SIDE_AB] = cellIdx;
				return (true);
			} else if (mPointC.equals(pB)) {
				mLinks[SIDE_BC] = cellIdx;
				return (true);
			}
		} else if (mPointC.equals(pA)) {
			if (mPointA.equals(pB)) {
				mLinks[SIDE_CA] = cellIdx;
				return (true);
			} else if (mPointB.equals(pB)) {
				mLinks[SIDE_BC] = cellIdx;
				return (true);
			}
		}

		// we are not adjacent to the calling cell
		return (false);
	}
	//-------------------------------------------------------------------------
	void Cell::checkAndLink(Cell& cellB)
	{
		if (mLinks[SIDE_AB] == -1 && cellB.requestLink(mPointA, mPointB, mIndex)) {
			mLinks[SIDE_AB] = cellB.mIndex;
		} else if (mLinks[SIDE_BC] == -1 && cellB.requestLink(mPointB, mPointC, mIndex)) {
			mLinks[SIDE_BC] = cellB.mIndex;
		} else if (mLinks[SIDE_CA] == -1 && cellB.requestLink(mPointC, mPointA, mIndex)) {
			mLinks[SIDE_CA] = cellB.mIndex;
		}
	}
	//-------------------------------------------------------------------------
	int Cell::getIndexByLink(int link)
	{
		assert( link > 0 );
		for ( int i = 0; i < 3; ++i )
		{
			if ( mLinks[i] == link )
				return i;
		}

		assert(0);
		return 0;
	}
	//-------------------------------------------------------------------------
	void Cell::lengthPointToMidLine(Vector2f pt, float outLen[3])
	{
		for ( int i = 0; i < 3; ++i )
		{
			Vector2f mp = mMidPoint[i];
			outLen[i] = CarmSqrt((pt-mp).squaredLength());
		}
	}
	//-------------------------------------------------------------------------
	int Cell::setAndGetArrivalWall(int index)
	{
		if (index == mLinks[0]) {
			arrivalWall = 0;
			return 0;
		} else if (index == mLinks[1]) {
			arrivalWall = 1;
			return 1;
		} else if (index == mLinks[2]) {
			arrivalWall = 2;
			return 2;
		}
		return -1;
	}
	//-------------------------------------------------------------------------
	void Cell::computeHeuristic(const Vector2f& goal)
	{
		//float XDelta = fabs(goal.x - center.x);
		//float YDelta = fabs(goal.y - center.y);

		//h = XDelta + YDelta;

		//入射边到目标的距离, 
		assert( arrivalWall >= 0 && arrivalWall < 3 );
#if 0
		//使用了快速平方根, 如果效率有问题,就换成其它方式
		h = CarmSqrt((mMidPoint[arrivalWall] - goal).squaredLength());
#else
		h = fabs(mMidPoint[arrivalWall].x - goal.x) + fabs(mMidPoint[arrivalWall].y - goal.y);
#endif
	}
}