#include "pch.h"
#include "ActItemMove.h"

ActItemMove::ActItemMove()
{
	m_actType = ACT_MOVE;
}

ActItemMove::ActItemMove( NRects &nrects )
{
	m_actType = ACT_MOVE;

	m_windows.UpdateNRects(nrects);
}

ActItemMove::~ActItemMove(void)
{
	m_windows.Clear();

}

ActType ActItemMove::Undo()
{
	Do();

	return m_actType;
}

ActType ActItemMove::Redo()
{
	Do();

	return m_actType;
}

void ActItemMove::Do()
{
	NRects bakNrects;

	m_windows.OuputRects(bakNrects);
	m_windows.UpdateAreas();
	m_windows.UpdateRects(bakNrects);

}