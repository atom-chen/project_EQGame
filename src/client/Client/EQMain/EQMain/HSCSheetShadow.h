/******************************************************************************
* ����: Y3
* ʱ��: 2010-12-17
* ����: ���ݲ�������Ƭ��Ӱ
******************************************************************************/
#ifndef HSCSheetShadow_H
#define HSCSheetShadow_H

#include "EQMainPrerequisites.h"

#include "AABBBox.h"


/** ���ݲ�������Ƭ��Ӱ, �򵥵�˵�����ڼҾ�����Ū��һ�źںڵĶ���
*/
class HSCSheetShadow
{
public:
	/// ������Ӱ���. 
	static Ogre::ManualObject* create(const Int2AABBBox& box);
};


#endif
