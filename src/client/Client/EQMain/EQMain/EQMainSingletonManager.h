/******************************************************************************
* 作者: Y3
* 时间: 2009-10-22
* 描述: EQMain单件管理类
******************************************************************************/
#ifndef EQMainSingletonManager_H
#define EQMainSingletonManager_H

#include "EQMainPrerequisites.h"

/** 
@remarks 如果有必要, 增加创建图形的接口
*/
class EQMainSingletonManager : public Ogre::Singleton<EQMainSingletonManager>
{
public:
	EQMainSingletonManager();
	~EQMainSingletonManager();
};

#endif