#include "pch.h"
#include "ActItemNew.h"

ActItemNew::ActItemNew(void)
{
	m_actType = ACT_NEW;
}

ActItemNew::~ActItemNew(void)
{
}

ActType ActItemNew::Undo()
{
	m_winClone.DelFromParent();

	return m_actType;
}

ActType ActItemNew::Redo()
{
	m_winClone.AddToParent();

	return m_actType;
}