/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 Mr.Q All rights reserved.

	filename:   CommonHelper.h
	created:    Mr.Q 2009-11-15 0:06
	purpose:    ͨ�ú���
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
    // Ͷ���߲���
	Ogre::Ray ProjectionLine(Ogre::Real x, Ogre::Real y);

    // ��Ļ����ת��Ϊ3D����
    Ogre::Vector3 ScreenPointTo3D(const Ogre::Vector2 &point);

	// 3D����ת��Ϊ��Ļ����
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

	//������һ�麯�������� Win32 �����ṩ�� API �򵥵ؽ��� UTF-8��UTF-16��MBCS �ַ���֮���ת��
	//Ҫע����ǣ���Щת����������ģ���Ϊ�����Ե����Ӿ��ǣ�һЩ�ñ䳤�� UTF-8 ���ܱ�ʾ���ַ����� UTF-16 ��һ���ܱ�ʾ��
	PWCHAR mbcs_to_utf16(PCHAR src);
	PCHAR utf16_to_mbcs(PWCHAR src);
	PWCHAR utf8_to_utf16(PCHAR src);
	PCHAR utf16_to_utf8(PWCHAR src);
	PCHAR mbcs_to_utf8(PCHAR src);
	PCHAR utf8_to_mbcs(PCHAR src);

	void translate(struct tm *when, stime *date);

	// ��������ʱ���ʱ���: int dt = t2 - t1;
	int diffTime(stime *t1, stime *t2);
}