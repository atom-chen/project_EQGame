#pragma once

#include <OIS.h>

#include <Ogre.h>
#include <OgreCEGUIRenderer.h>

#include <string>

#include <CEGUIBase.h>
#include <CEGUISystem.h>
#include <CEGUILogger.h>
#include <CEGUIWindow.h>
#include <CEGUIScheme.h>
#include <CEGUIImageset.h>	
#include <CEGUIScriptModule.h>
#include <CEGUISchemeManager.h>
#include <CEGUIWindowManager.h>
#include <falagard/CEGUIFalWidgetLookManager.h>

#include <elements/CEGUIListbox.h>
#include <elements/CEGUICheckbox.h>
#include <elements/CEGUICombobox.h>
#include <elements/CEGUIScrollbar.h>
#include <elements/CEGUIPushButton.h>
#include <elements/CEGUIScrollablePane.h>
#include <elements/CEGUIListboxTextItem.h>

/*==============================================================================
purpose: CGUIManager
created: Mr.Q 2009:8:10 21:59
=============================================================================*/
class CGUIManager : public CEGUI::OgreCEGUIRecycleTextureListener
{
public:
	CGUIManager();
	virtual ~CGUIManager();

	static CGUIManager& getSingleton(void);
	static CGUIManager* getSingletonPtr(void);
public:
	bool Initialize(std::string log_file = "CEGUI.log");
	void Terminate();

public:
	void Create(Ogre::RenderWindow* window,
					Ogre::uint8 queue_id		= Ogre::RENDER_QUEUE_OVERLAY,
					bool post_queue			= false, 
					CEGUI::uint max_quads	= 0, 
					Ogre::SceneManager* scene_manager = NULL,
					std::string log_file = "CEGUI.log");

	virtual void recycle(long useTime, size_t totalCount, size_t totalSize, size_t recyCount, size_t recySize);

	void OnUpdate();

	void OnRender();

	void WindowResized(Ogre::RenderWindow* rw);

	void ShowDebugOverlay();
	void HideDebugOverlay();
	void UpdateDebugStats();

	bool OnMouseReleased(const OIS::MouseEvent &Event, OIS::MouseButtonID eID);
	bool OnMousePressed(const OIS::MouseEvent &Event, OIS::MouseButtonID eID);
	bool OnMouseMoved(const OIS::MouseEvent &Event);

	void SetText(const char* szWindows, const char* szText);
	int checkIsCharAlpha(const char* checkStr);
	std::string returnValidString(std::string fullStr);
	void SetChatText(const char* dest, /*const CEGUI::String& src*/const char* src);

	//[5/12/2010 huangdj1]
	void setSysMsgText(std::string );
	//  [6/13/2010 huangdj1]
	void setTooltipProperty(const std::string szWindow, const std::string szText);

protected:
	CEGUI::MouseButton convertOISMouseButtonToCegui(int buttonID);
protected:
	CEGUI::System*							m_pGUISystem;
	CEGUI::OgreCEGUIRenderer*		m_pGUIRenderer;

	Ogre::Overlay*							m_pDebugOverlay;

	Ogre::String								m_DebugText;
private:
	static CGUIManager*					m_pInstance;
};