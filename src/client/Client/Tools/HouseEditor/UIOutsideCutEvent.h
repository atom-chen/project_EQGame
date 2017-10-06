/******************************************************************************
* 作者: Y3
* 时间: 2009-07-12
* 描述: 测试室外物件剔除的对应事件
******************************************************************************/
#ifndef UIOutsideCutEvent_H
#define UIOutsideCutEvent_H

#include "MfcInputEvent.h"

#include "SceneInfo.h"

class OutsideCutEvent: public MfcInputEvent
{
public:
	OutsideCutEvent();
	~OutsideCutEvent();

	static OutsideCutEvent * create();
};

#endif
