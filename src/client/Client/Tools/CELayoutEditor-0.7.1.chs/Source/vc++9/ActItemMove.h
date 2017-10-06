/******************************************************************************
* 作者: xj
* 时间: 2010-10-29
* 描述: 移动窗体动作项
******************************************************************************/
#pragma once

#ifndef __ActItemMove_H
#define __ActItemMove_H

#include "ActItem.h"
#include "ActWindows.h"

class ActItemMove : public ActItem
{
protected:
	//窗体选择集变量
	ActWindows m_windows;

protected:
	void Do();

public:
	//撤消
	virtual ActType Undo();
	//重做
	virtual ActType Redo();

public:
	ActItemMove(void);
	ActItemMove(NRects &nrects);
	virtual ~ActItemMove(void);
};

#endif