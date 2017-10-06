/******************************************************************************
* ����: xj
* ʱ��: 2010-11-1
* ����: ���������������
******************************************************************************/

#pragma once

#ifndef __ActManager_H
#define __ActManager_H

#include "ActItem.h"
#include <stack>

//�������������ʱ���¼�֪ͨ�ӿ�
//�κ������ڴ˵���ע���¼��󽫻���������������ʱ�õ�֪ͨ
class ActChangeHandler
{
public:
	virtual void StackChanged() = 0;
};

class ActManager
{
public:
	//����������ջ����
	typedef std::stack<ActItem *> ACTSTACK;

protected:
	//����ջ
	ACTSTACK m_undoStack;
	//����ջ
	ACTSTACK m_redoStack;
	//����֪ͨ�¼���ֻ֧��һ���¼�ע��
	ActChangeHandler *m_event;

public:
	//�жϳ���ջ�Ƿ�Ϊ��
	bool IsUndoStackEmpty()
	{
		return m_undoStack.empty();
	}
	//��ȡ����ջ����
	ACTSTACK &GetUndoStack()
	{
		return m_undoStack;
	}
	//�ж�����ջ�Ƿ�Ϊ��
	bool IsRedoStackEmpty()
	{
		return m_redoStack.empty();
	}
	//��ȡ����ջ����
	ACTSTACK &GetRedoStack()
	{
		return m_redoStack;
	}
	//��ȡ����֪ͨ�¼�
	ActChangeHandler *GetEvent()
	{
		return m_event;
	}
	//ע�ᶯ��֪ͨ�¼�
	void RegisterEvent(ActChangeHandler *evnt)
	{
		m_event = evnt;
	}

public:
	//��������ջ
	void PushToUndo(ActItem *item);
	//����
	ActType Undo();
	//����
	ActType Redo();
	//�������������ջ
	void Clear();
	//�������ջ
	void ClearUndoStack();
	//�������ջ
	void ClearRedoStack();

public:
	ActManager(void);
	virtual ~ActManager(void);
};

#endif