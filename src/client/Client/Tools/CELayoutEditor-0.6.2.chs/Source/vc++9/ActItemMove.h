/******************************************************************************
* ����: xj
* ʱ��: 2010-10-29
* ����: �ƶ����嶯����
******************************************************************************/
#pragma once

#ifndef __ActItemMove_H
#define __ActItemMove_H

#include "ActItem.h"
#include "ActWindows.h"

class ActItemMove : public ActItem
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
	ActItemMove(void);
	ActItemMove(NRects &nrects);
	virtual ~ActItemMove(void);
};

#endif