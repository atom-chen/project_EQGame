#include "pch.h"
#include "ActWindows.h"

ActWindows::ActWindows(void)
{
}

ActWindows::~ActWindows(void)
{
}

void ActWindows::UpdateRects( NRects &nrects )
{
	for(NRects::iterator nrectItr = nrects.begin(); nrectItr != nrects.end(); ++nrectItr)
	{
		NRects::iterator findItr = m_winNRects.find(nrectItr->first);

		if(findItr == m_winNRects.end())
			continue;

		findItr->second.setPosition(nrectItr->second.getPosition());
		findItr->second.setSize(nrectItr->second.getSize());
	}

}

void ActWindows::UpdateAreas()
{
	UpdateAreas(m_winNRects);

}

void ActWindows::UpdateAreas( NRects &nrects )
{
	CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

	for(NRects::iterator nrectItr = nrects.begin(); nrectItr != nrects.end(); ++nrectItr)
	{
		if(!winMgr.isWindowPresent(nrectItr->first))
			continue;

		winMgr.getWindow(nrectItr->first)->setArea(nrectItr->second);
	}

}

void ActWindows::UpdateNRects( NRects &nrects )
{
	Clear();

	for(NRects::iterator nrectItr = nrects.begin(); nrectItr != nrects.end(); ++nrectItr)
	{
		m_winNRects.insert(NRects::value_type(nrectItr->first, nrectItr->second));
	}

}

void ActWindows::OuputRects( NRects &nrects )
{
	CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

	for(NRects::iterator nrectItr = m_winNRects.begin(); nrectItr != m_winNRects.end(); ++nrectItr)
	{
		if(!winMgr.isWindowPresent(nrectItr->first))
			continue;

		CEGUI::Window *win = winMgr.getWindow(nrectItr->first);

		nrects.insert(NRects::value_type(win->getName(), win->getArea()));
	}

}