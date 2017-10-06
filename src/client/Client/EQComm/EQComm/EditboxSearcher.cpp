#include "EQCommPCH.h"
#include "EditboxSearcher.h"
#include "CEGUIWindow.h"

//------------------------------------------------------------------------------
bool EditboxSearcher::canInput(CEGUI::Window * win)
{
	// 不为空
	if (!win)
		return false;

	// 可见
	if (!win->isVisible())
		return false;

	// 可输入
	if (win->testClassName("Editbox"))
	{
		return true;
	}
	else if (win->testClassName("MultiLineEditbox"))
	{
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------ 
// 查找输入框(简单递归查找)
CEGUI::Window * EditboxSearcher::simpleSearchTextbox(CEGUI::Window * win)
{
	if (!win)
		return NULL;

	for (size_t i = 0; i < win->getChildCount(); ++i)
	{
		CEGUI::Window * p = win->getChildAtIdx(i);
		if (canInput(p))
		{
			return p;
		}
		else
		{
			p = simpleSearchTextbox(p);
			if (p)
			{
				return p;
			}
		}
	}

	return NULL;
}
//------------------------------------------------------------------------------
// 递归查找输入框 (先查自已, 再查childs, 再查其它的)
// 参数 pSkip: 忽略某个窗口
CEGUI::Window * EditboxSearcher::searchTextbox(CEGUI::Window * win, CEGUI::Window * pSkip)
{
	if (!win)
		return NULL;

	// 先看自身是否满足条件
	if (canInput(win))
	{
		return win;
	}		

	// 找自身的子窗口
	for (size_t i = 0; i < win->getChildCount(); ++i)
	{
		CEGUI::Window * p = win->getChildAtIdx(i);
		if (p == pSkip)
			continue;

		if (canInput(p))
		{
			return p;
		}		

		p = simpleSearchTextbox(p);
		if (p)
			return p;
	}

	// 找其它窗口
	return searchTextbox(win->getParent(), win);
}
//------------------------------------------------------------------------------
// 取当前激活的窗口
CEGUI::Window * EditboxSearcher::findActivateWindow()
{
	CEGUI::Window * win = CEGUI::System::getSingleton().getGUISheet();
	if (win)
		return win->getActiveChild();
	return NULL;
}
//------------------------------------------------------------------------------
void EditboxSearcher::activeTextbox()
{
	CEGUI::Window * win = findActivateWindow();
	if (!canInput(win))
	{
		win = searchTextbox(CEGUI::System::getSingleton().getWindowContainingMouse(), NULL);
		if (win)
		{
			win->activate();
		}
	}
}
