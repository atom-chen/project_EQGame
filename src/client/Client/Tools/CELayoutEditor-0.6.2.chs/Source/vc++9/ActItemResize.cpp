#include "pch.h"
#include "ActItemResize.h"

ActItemResize::ActItemResize(void)
{
	m_actType = ACT_RESIZE;
}

ActItemResize::ActItemResize( NRects &nrects )
{
	m_actType = ACT_RESIZE;

	m_windows.UpdateNRects(nrects);
}

ActItemResize::~ActItemResize(void)
{
	m_windows.Clear();
}

ActType ActItemResize::Undo()
{
	Do();

	return m_actType;
}

ActType ActItemResize::Redo()
{
	Do();

	return m_actType;
}

void ActItemResize::Do()
{
	NRects bakNrects;

	m_windows.OuputRects(bakNrects);
	m_windows.UpdateAreas();
	m_windows.UpdateRects(bakNrects);

}