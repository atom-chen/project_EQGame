/******************************************************************************
* 作者: xj
* 时间: 2010-10-29
* 描述: 更改窗体大小动作项
******************************************************************************/
#pragma once

#ifndef __ActItemResize_H
#define __ActItemResize_H

#include "ActItem.h"
#include "ActWindows.h"

class ActItemResize : public ActItem
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
	ActItemResize(void);
	ActItemResize(NRects &nrects);
	virtual ~ActItemResize(void);
};

#endif