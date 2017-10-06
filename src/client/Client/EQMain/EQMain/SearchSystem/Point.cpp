#include "Point.h"

namespace SearchSystem
{
	Point::Point(const Point &point)
	{
		_x = point._x;
		_y = point._y;
		_z = point._z;
	}

	Point& Point::operator =(const Point &point)
	{// 不判断是否为同一点，几率不大
		_x = point._x;
		_y = point._y;
		_z = point._z;
		return *this;
	}
}
