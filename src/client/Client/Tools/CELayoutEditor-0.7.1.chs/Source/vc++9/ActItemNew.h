/******************************************************************************
* 作者: xj
* 时间: 2010-10-29
* 描述: 新建窗体动作项
******************************************************************************/
#pragma once

#ifndef __ActItemNew_H
#define __ActItemNew_H

#include "ActItem.h"
#include "ActWindowClone.h"

class ActItemNew : public ActItem
{
protected:
	//窗体副本对象
	ActWindowClone m_winClone;

public:
	//获取窗体副本对象
	ActWindowClone &GetWinClone()
	{
		return m_winClone;
	}

public:
	//撤消
	virtual ActType Undo();
	//重做
	virtual ActType Redo();

public:
	ActItemNew(void);
	virtual ~ActItemNew(void);
};

#endif