/******************************************************************************
* 作者: Y3
* 时间: 2010-12-17
* 描述: 房屋部件的面片阴影
******************************************************************************/
#ifndef HSCSheetShadow_H
#define HSCSheetShadow_H

#include "EQMainPrerequisites.h"

#include "AABBBox.h"


/** 房屋部件的面片阴影, 简单的说就是在家具下面弄上一团黑黑的东西
*/
class HSCSheetShadow
{
public:
	/// 创建阴影物件. 
	static Ogre::ManualObject* create(const Int2AABBBox& box);
};


#endif
