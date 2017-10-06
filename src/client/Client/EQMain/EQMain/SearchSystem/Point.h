/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 XiaoMing All rights reserved.

	filename: 	Point.h
	created:	D.W 2009-11-05 20:00
	purpose:	世界坐标类
==============================================================================*/
#ifndef SSystem_Point_H
#define SSystem_Point_H

#include "SSystemCommon.h"

namespace SearchSystem
{
	// 是否应分Point2 Point3
	class Point
	{
	public:
		Point()
			:_x(0)
			,_y(0)
			,_z(0)
		{}

		Point(int x, int y, int z)
			:_x(x)
			,_y(y)
			,_z(z)
		{}

		Point(const Point & point);

		void set(int x, int y, int z) { _x = x; _y = y; _z = z;}

		Point& operator=(const Point& point);
		bool operator==(const Point& destPoint)const
		{
			return (_x == destPoint._x && _y == destPoint._y &&_z == destPoint._z);
		}
		bool operator!=(const Point& destPoint)const
		{
			return !(*this == destPoint);
		}

	public:
		int _x;
		int _y;
		int _z;
	}; 

}

#endif