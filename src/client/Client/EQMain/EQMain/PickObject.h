/******************************************************************************
* ����: Y3
* ʱ��: 2009-07-13
* ����: �����п�ʰȡ����Ľӿ���
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
