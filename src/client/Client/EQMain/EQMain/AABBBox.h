/******************************************************************************
* 作者: Y3
* 时间: 2009-20-30
* 描述: 类似Ogre的AxisAlignedBox. 类似bounding box
******************************************************************************/
#ifndef AABBBox_H
#define AABBBox_H

#include "EQMainPrerequisites.h"


/** 2维整数矢量的AABB box
*/
class Int2AABBBox
{
public:
	int	mXmin;
	int mYmin;
	int mXmax;
	int mYmax;
	bool mIsNull;

public:
	Int2AABBBox()
	{
		reset();
	}

	void reset()
	{
		mIsNull = true;
		mXmin = 0;
		mYmin = 0;
		mXmax = 0;
		mYmax = 0;
	}

	void merge( int x, int y )
	{
		makeFloor(x,y);
		makeCeil(x,y);
	}

	void merge( const Int2AABBBox& box )
	{
		if ( box.mIsNull )
			return;
		makeFloor(box.mXmin, box.mYmin);
		makeCeil(box.mXmax, box.mYmax);
	}

	bool intersects( int x, int y ) const
	{
		if ( mIsNull )
			return false;
		return (x>= mXmin && x<=mXmax && y>=mYmin && y<=mYmax);
	}

	bool intersects( const Int2AABBBox& box ) const
	{
		if ( mIsNull || box.mIsNull )
			return false;
		return (box.mXmin>= mXmin && box.mXmax<=mXmax && box.mYmin>=mYmin && box.mYmax<=mYmax);
	}

	void getSize( int& x, int& y ) const
	{
		if ( mIsNull )
		{
			x = 0;
			y = 0;
		}
		else
		{
			x = mXmax - mXmin + 1;	//注意这里
			y = mYmax - mYmin + 1;
		}
	}

	int volume() const
	{
		int x,y;
		getSize(x,y);
		return (x*y);
	}

	int linearIndex( int x, int y ) const
	{
		assert(intersects(x,y));
		return ((mYmax-mYmin+1)*(x-mXmin) + (y-mYmin));
	}

public:
	void makeCeil( int Mx, int My )
	{
		if ( mIsNull )
		{
			setExtents(Mx, My, Mx, My);
		}
		else
		{
			if ( Mx > mXmax ) mXmax = Mx;
			if ( My > mYmax ) mYmax = My;
		}
	}

	void makeFloor( int mx, int my )
	{
		if ( mIsNull )
		{
			setExtents(mx, my, mx, my);
		}
		else
		{
			if ( mx < mXmin ) mXmin = mx;
			if ( my < mYmin ) mYmin = my;
		}
	}

	void setExtents( int mx, int my, int Mx, int My )
	{
		assert( (mx <= Mx && my <= My) &&
			"The minimum corner of the box must be less than or equal to maximum corner" );
		mIsNull = false;
		mXmin = mx;
		mYmin = my;
		mXmax = Mx;
		mYmax = My;
	}

public:
	static void rotateQuarter( int &x, int &y, int qua, bool reverse = false)
	{
		int a = x, b = y;
		int face = qua;
		if ( reverse )
		{
			if ( qua == 1)
				face = 3;
			else if ( qua == 3 )
				face = 1;
		}

		if ( face == 0)
		{
		}
		else if ( face == 1 )
		{
			x = -b;
			y = a;
			return;
		}
		else if ( face == 2 )
		{
			x = -a;
			y = -b;
			return;
		}
		else if ( face == 3 )
		{
			x = b;
			y = -a;
		}
		else
		{
			assert(0);
		}
	}
};


#endif
