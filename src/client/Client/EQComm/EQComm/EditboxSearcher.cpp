#include "EQCommPCH.h"
#include "EditboxSearcher.h"
#include "CEGUIWindow.h"

//------------------------------------------------------------------------------
bool EditboxSearcher::canInput(CEGUI::Window * win)
{
	// ��Ϊ��
	if (!win)
		return false;

	// �ɼ�
	if (!win->isVisible())
		return false;

	// ������
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
// ���������(�򵥵ݹ����)
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
// �ݹ��������� (�Ȳ�����, �ٲ�childs, �ٲ�������)
// ���� pSkip: ����ĳ������
CEGUI::Window * EditboxSearcher::searchTextbox(CEGUI::Window * win, CEGUI::Window * pSkip)
{
	if (!win)
		return NULL;

	// �ȿ������Ƿ���������
	if (canInput(win))
	{
		return win;
	}		

	// ��������Ӵ���
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

	// ����������
	return searchTextbox(win->getParent(), win);
}
//------------------------------------------------------------------------------
// ȡ��ǰ����Ĵ���
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
