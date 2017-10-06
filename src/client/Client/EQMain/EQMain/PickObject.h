/******************************************************************************
* 作者: Y3
* 时间: 2009-07-13
* 描述: 场景中可拾取物件的接口类
******************************************************************************/
#ifndef PickObject_H
#define PickObject_H

#include "EQMainPrerequisites.h"

#define FarmPlantUnit_PickObjectUnit "FarmPlantUnit::PickObjectUnit"

class PickObject
{
public:
	virtual ~PickObject() {}

	virtual const std::string& getClassName() = 0;
};

#endif
