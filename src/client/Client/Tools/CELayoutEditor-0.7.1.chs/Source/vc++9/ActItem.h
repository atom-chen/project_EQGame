/******************************************************************************
* 作者: xj
* 时间: 2010-10-29
* 描述: 撤消动作项接口
******************************************************************************/

#pragma once

#ifndef __ActItem_H
#define __ActItem_H

//撤消动作类型
enum ActType
{
	ACT_UNDEFINED,//未定义类型，初使化用，为无效动作项。
	ACT_MOVE,//移动窗体动作项
	ACT_RESIZE,//调整窗体大小动作项
	ACT_NEW,//新建窗体动作项
	ACT_DEL//删除窗体动作项
};

class ActItem
{
protected:
	ActType m_actType;//动作类型

public:
	//获取动作类型
	ActType GetType()
	{
		return m_actType;
	}

public:
	//撤消动作接口，由派生的各种动作项实现。
	virtual ActType Undo() = 0;
	//重做动作接口，由派生的各种动作项实现。
	virtual ActType Redo() = 0;

public:
	ActItem(void) : m_actType(ACT_UNDEFINED) {};
	virtual ~ActItem(void) {};
};

#endif