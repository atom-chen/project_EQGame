/******************************************************************************
* 作者: xj
* 时间: 2010-11-1
* 描述: 撤消动作项管理类
******************************************************************************/

#pragma once

#ifndef __ActManager_H
#define __ActManager_H

#include "ActItem.h"
#include <stack>

//撤消或重做变更时的事件通知接口
//任何派生于此的类注册事件后将会在做撤消或重做时得到通知
class ActChangeHandler
{
public:
	virtual void StackChanged() = 0;
};

class ActManager
{
public:
	//撤消动作项栈类型
	typedef std::stack<ActItem *> ACTSTACK;

protected:
	//撤消栈
	ACTSTACK m_undoStack;
	//重做栈
	ACTSTACK m_redoStack;
	//动作通知事件，只支持一个事件注册
	ActChangeHandler *m_event;

public:
	//判断撤消栈是否为空
	bool IsUndoStackEmpty()
	{
		return m_undoStack.empty();
	}
	//获取撤消栈引用
	ACTSTACK &GetUndoStack()
	{
		return m_undoStack;
	}
	//判断重做栈是否为空
	bool IsRedoStackEmpty()
	{
		return m_redoStack.empty();
	}
	//获取重做栈引用
	ACTSTACK &GetRedoStack()
	{
		return m_redoStack;
	}
	//获取动作通知事件
	ActChangeHandler *GetEvent()
	{
		return m_event;
	}
	//注册动作通知事件
	void RegisterEvent(ActChangeHandler *evnt)
	{
		m_event = evnt;
	}

public:
	//撤消项入栈
	void PushToUndo(ActItem *item);
	//撤消
	ActType Undo();
	//重做
	ActType Redo();
	//清除撤消和重做栈
	void Clear();
	//清除撤消栈
	void ClearUndoStack();
	//清除重做栈
	void ClearRedoStack();

public:
	ActManager(void);
	virtual ~ActManager(void);
};

#endif