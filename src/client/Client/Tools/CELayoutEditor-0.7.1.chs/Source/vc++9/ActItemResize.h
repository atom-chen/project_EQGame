/******************************************************************************
* ����: xj
* ʱ��: 2010-10-29
* ����: ���Ĵ����С������
******************************************************************************/
#pragma once

#ifndef __ActItemResize_H
#define __ActItemResize_H

#include "ActItem.h"
#include "ActWindows.h"

class ActItemResize : public ActItem
{
protected:
	//����ѡ�񼯱���
	ActWindows m_windows;

protected:
	void Do();

public:
	//����
	virtual ActType Undo();
	//����
	virtual ActType Redo();

public:
	ActItemResize(void);
	ActItemResize(NRects &nrects);
	virtual ~ActItemResize(void);
};

#endif