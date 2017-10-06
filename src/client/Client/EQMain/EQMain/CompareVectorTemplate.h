/******************************************************************************
* 作者: Y3
* 时间: 2009-12-19
* 描述: 比较矢量模板. 这些东西是给std::map 和 stdext::hash_map之类的容器当键值
******************************************************************************/
#ifndef CompareVectorTemplate_H
#define CompareVectorTemplate_H

#include "EQMainPrerequisites.h"


/** 二维比较矢量模板
*/
template <class T>
class CompareVector2Template
{
public:
	T x;
	T y;

	CompareVector2Template()
	{
	}

	CompareVector2Template( T a, T b)
	{
		x = a;
		y = b;
	}

	bool operator<( const CompareVector2Template<T>& r ) const
	{
		if ( x < r.x )
		{
			return true;
		}
		else if ( x == r.x)
		{
			if ( y < r.y )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	bool operator==( const CompareVector2Template<T>& r ) const
	{
		return( ( x == r.x) && ( y == r.y) );

	}
};



/** 三维比较矢量模板
*/
template <class T>
class CompareVector3Template
{
public:
	T x;
	T y;
	T z;

	CompareVector3Template( T a, T b, T c)
	{
		x = a;
		y = b;
		z = c;
	}

	bool operator<( const CompareVector3Template<T>& r ) const
	{
		if ( x < r.x)
		{
			return true;
		}
		else if ( x == r.x )
		{
			if ( y < r.y)
			{
				return true;
			}
			else if ( y == r.y )
			{
				if ( z < r.z)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else 
			{
				return false;
			}
		}
		else 
		{
			return false;
		}
	}

	bool operator==( const CompareVector3Template<T>& r) const
	{
		return( (x == r.x) && (y == r.y) && (z == r.z) );
	}
};


#endif

