/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 Mr.Q All rights reserved.

	filename:   CommonHelper.h
	created:    Mr.Q 2009-11-15 0:06
	purpose:    通用函数
==============================================================================*/
#pragma once
#include <Ogre.h>
#include <Windows.h>
/*==============================================================================
	purpose: CommonHelper
	created: Mr.Q 2009-11-15 0:07
  =============================================================================*/
struct stime;

namespace CommonHelper
{
    // 投射线操作
	Ogre::Ray ProjectionLine(Ogre::Real x, Ogre::Real y);

    // 屏幕坐标转化为3D坐标
    Ogre::Vector3 ScreenPointTo3D(const Ogre::Vector2 &point);

	// 3D坐标转化为屏幕坐标
	bool Vector3ToScreenPoint(const Ogre::Vector3& pos, int& x, int& y);

    // delete_object() can free memory used as any type. it is especially
    // useful for the deletion of elements in a linked-list such
    // as in the following snippet:
    //
    //		std::list<whatever*> list;
    //		std::for_each(list.begin(), list.end(), delete_object());
    struct delete_object
    {
        template <typename T>
            void operator()(T *ptr)
        { 
            delete ptr; 
        }
    };

    // ToString() converts takes a parameter of any given type
    // and converts it into a string
    // it's usefulness is in the conversion of numeric data
    // into a string such as in the following:
    //
    //		std::string test = "Age: " + ToString(22);
    template <typename T>
        std::string ToString(const T item)
    {
        std::stringstream a;
        a << item;
        return a.str();
    }

	//下面这一组函数能利用 Win32 本身提供的 API 简单地进行 UTF-8、UTF-16、MBCS 字符串之间的转换
	//要注意的是，这些转换不是无损的，因为很明显的例子就是，一些用变长的 UTF-8 所能表示的字符，用 UTF-16 不一定能表示。
	PWCHAR mbcs_to_utf16(PCHAR src);
	PCHAR utf16_to_mbcs(PWCHAR src);
	PWCHAR utf8_to_utf16(PCHAR src);
	PCHAR utf16_to_utf8(PWCHAR src);
	PCHAR mbcs_to_utf8(PCHAR src);
	PCHAR utf8_to_mbcs(PCHAR src);

	void translate(struct tm *when, stime *date);

	// 计算两个时间的时间差: int dt = t2 - t1;
	int diffTime(stime *t1, stime *t2);
}