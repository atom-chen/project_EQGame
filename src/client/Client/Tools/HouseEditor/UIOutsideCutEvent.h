/******************************************************************************
* ����: Y3
* ʱ��: 2009-07-12
* ����: ������������޳��Ķ�Ӧ�¼�
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
