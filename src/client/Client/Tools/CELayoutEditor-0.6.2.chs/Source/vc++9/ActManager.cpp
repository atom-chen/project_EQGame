#include "pch.h"
#include "ActManager.h"

//撤消列表最大值
#define MAX_UNDO_COUNT 1024

ActManager::ActManager(void)
{
	m_event = 0;
}

ActManager::~ActManager(void)
{
}

void ActManager::PushToUndo( ActItem *item )
{
	m_undoStack.push(item);

	ClearRedoStack();

	if(m_event)
		m_event->StackChanged();
}

ActType ActManager::Undo()
{
	if(m_undoStack.empty())
		return ACT_UNDEFINED;

	ActItem *item = m_undoStack.top();

	ActType rtn = item->Undo();

	m_redoStack.push(item);

	m_undoStack.pop();

	if(m_event)
		m_event->StackChanged();

	return rtn;
}

ActType ActManager::Redo()
{
	if(m_redoStack.empty())
		return ACT_UNDEFINED;

	ActItem *item = m_redoStack.top();

	ActType rtn = item->Redo();

	m_undoStack.push(item);

	m_redoStack.pop();

	if(m_event)
		m_event->StackChanged();

	return rtn;
}

void ActManager::Clear()
{
	ClearUndoStack();
	ClearRedoStack();
}

void ActManager::ClearUndoStack()
{
	if(0 == m_undoStack.size())
		return;

	while(false == m_undoStack.empty())
	{
		delete m_undoStack.top();
		m_undoStack.pop();
	}

	if(m_event)
		m_event->StackChanged();
}

void ActManager::ClearRedoStack()
{
	if(0 == m_redoStack.size())
		return;

	while(false == m_redoStack.empty())
	{
		delete m_redoStack.top();
		m_redoStack.pop();
	}

	if(m_event)
		m_event->StackChanged();
}