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
	{// ���ж��Ƿ�Ϊͬһ�㣬���ʲ���
		_x = point._x;
		_y = point._y;
		_z = point._z;
		return *this;
	}
}
