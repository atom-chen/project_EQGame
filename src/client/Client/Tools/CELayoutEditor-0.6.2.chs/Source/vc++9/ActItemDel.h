/******************************************************************************
* 作者: xj
* 时间: 2010-10-29
* 描述: 删除窗体动作项
******************************************************************************/
#pragma once

#ifndef __ActItemDel_H
#define __ActItemDel_H

#include "ActItem.h"
#include "ActWindowClone.h"

class ActItemDel : public ActItem
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
	ActItemDel(void);
	virtual ~ActItemDel(void);
};

#endif