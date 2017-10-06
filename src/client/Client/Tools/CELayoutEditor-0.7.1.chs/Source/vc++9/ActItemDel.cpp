#include "pch.h"
#include "ActItemDel.h"

ActItemDel::ActItemDel(void)
{
	m_actType = ACT_DEL;
}

ActItemDel::~ActItemDel(void)
{
}

ActType ActItemDel::Undo()
{
	m_winClone.AddToParent();

	return m_actType;
}

ActType ActItemDel::Redo()
{
	m_winClone.DelFromParent();

	return m_actType;
}
