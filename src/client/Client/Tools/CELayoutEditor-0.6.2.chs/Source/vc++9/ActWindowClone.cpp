#include "pch.h"
#include "ActWindowClone.h"
#include "ExceptionManager.h"

ActWindowClone::ActWindowClone(void)
{
	m_parent = "";
}

ActWindowClone::~ActWindowClone(void)
{
	Clear();
}

void ActWindowClone::Clear( WindowDatas &windows )
{
	for(WindowDatas::iterator itr = windows.begin(); itr != windows.end(); ++itr)
	{
		itr->properties.clear();

		Clear(itr->children);
	}

	windows.clear();

}

void ActWindowClone::Clear()
{
	Clear(m_windows);

}

void ActWindowClone::Clone( WindowData &data, CEGUI::Window *source )
{
	data.name = source->getName();
	data.type = source->getType();

	CEGUI::PropertySet::Iterator propertyIt = source->getPropertyIterator();

	while (!propertyIt.isAtEnd()) 
	{
		const CEGUI::String propertyName = propertyIt.getCurrentKey();
		
		data.properties.insert(std::make_pair(propertyName, source->getProperty(propertyName)));

		propertyIt++;
	}

}

void ActWindowClone::CloneRecursive( WindowData &data, CEGUI::Window *source )
{
	Clone( data, source );

	for( size_t i = 0; i < source->getChildCount(); ++i )
	{
		CEGUI::Window* child = source->getChildAtIdx(i);

		if (child->getName().find("__auto_") == CEGUI::String::npos && child->getName().find("__TabPane__"))
		{
			WindowData datac;

			data.children.push_back(datac);

			WindowDatas::iterator last = data.children.end();

			--last;

			CloneRecursive(*last, child);
		}
	}
}

void ActWindowClone::Clone( CEGUI::Window *source )
{
	if(0 == source)
		return;

	WindowData data;

	CloneRecursive(data, source);

	m_windows.push_back(data);

}

void ActWindowClone::DelFromParent()
{
	CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

	for(WindowDatas::iterator itr = m_windows.begin(); itr != m_windows.end(); ++itr)
	{
		winMgr.destroyWindow(itr->name);
	}

}

void ActWindowClone::AddToParent()
{
	if(0 == m_windows.size())
		return;

	CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

	CEGUI::Window *winParent = winMgr.getWindow(m_parent);

	if(winParent)
	{
		Create(winParent);
	}
	else
	{
		WindowDatas::iterator itr = m_windows.begin();

		winParent = Create(*itr, 0);

		if(0 == winParent)
			return;

		for(WindowDatas::iterator itrChilds = itr->children.begin(); itrChilds != itr->children.end(); ++itrChilds)
		{
			CreateRecursive( *itrChilds, winParent );
		}

		CEGUI::System::getSingleton().setGUISheet(winParent);
	}

}

void ActWindowClone::Create( CEGUI::Window *target )
{
	for(WindowDatas::iterator itr = m_windows.begin(); itr != m_windows.end(); ++itr)
	{
		CreateRecursive(*itr, target);
	}

}

CEGUI::Window *ActWindowClone::Create( WindowData &data, CEGUI::Window *target )
{
	CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();

	if(winMgr.isWindowPresent(data.name))
		return 0;

	CEGUI::Window *pNew = winMgr.createWindow (data.type, data.name);

	if(target)
		target->addChildWindow (pNew);

	for (Properties::const_iterator it = data.properties.begin();it != data.properties.end(); ++it)
	{
		if (it->second != "" && it->second != pNew->getProperty(it->first))
		{
			pNew->setProperty (it->first, it->second);					
		}
	}
	
	return pNew;

}

void ActWindowClone::CreateRecursive( WindowData &data, CEGUI::Window *target )
{
	CEGUI::Window* const pNew = Create( data, target );

	if(0 == pNew)
		return;

	for(WindowDatas::iterator itr = data.children.begin(); itr != data.children.end(); ++itr)
	{
		CreateRecursive( *itr, pNew );
	}

}