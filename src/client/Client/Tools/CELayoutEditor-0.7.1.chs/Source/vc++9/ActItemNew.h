/******************************************************************************
* ����: xj
* ʱ��: 2010-10-29
* ����: �½����嶯����
******************************************************************************/
#pragma once

#ifndef __ActItemNew_H
#define __ActItemNew_H

#include "ActItem.h"
#include "ActWindowClone.h"

class ActItemNew : public ActItem
{
protected:
	//���帱������
	ActWindowClone m_winClone;

public:
	//��ȡ���帱������
	ActWindowClone &GetWinClone()
	{
		return m_winClone;
	}

public:
	//����
	virtual ActType Undo();
	//����
	virtual ActType Redo();

public:
	ActItemNew(void);
	virtual ~ActItemNew(void);
};

#endif