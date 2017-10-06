/******************************************************************************
* ����: xj
* ʱ��: 2010-10-29
* ����: ɾ�����嶯����
******************************************************************************/
#pragma once

#ifndef __ActItemDel_H
#define __ActItemDel_H

#include "ActItem.h"
#include "ActWindowClone.h"

class ActItemDel : public ActItem
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
	ActItemDel(void);
	virtual ~ActItemDel(void);
};

#endif