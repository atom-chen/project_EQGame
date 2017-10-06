#include "EQCommPCH.h"
//#include "ClientShell.h"
#include "GUIManager.h"
#include "CommonHelper.h"
#include "EQOgreSys.h"
#include "ChatSystem.h"
#include "GameState.h"
#include "CommFunc.h"
#include "Variable.h"

#define GUI_GUIMANAGER_CONFIG "MainGUI.config"
/*==============================================================================
purpose: CGUIManager	
created: Mr.Q 2009:8:10 22:00
=============================================================================*/
CGUIManager* CGUIManager::m_pInstance = NULL;
CGUIManager::CGUIManager()
:m_pGUISystem(NULL),
m_pGUIRenderer(NULL),
m_pDebugOverlay(NULL)
{
}
//---------------------------------------------------------------------------------------------
CGUIManager::~CGUIManager()
{
	if (m_pGUISystem)
	{
		delete m_pGUISystem;

		m_pGUISystem = NULL;
	}

	if (m_pGUIRenderer)
	{
		delete m_pGUIRenderer;

		m_pGUIRenderer = NULL;
	}
}
//---------------------------------------------------------------------------------------------
void CGUIManager::Create(Ogre::RenderWindow* window, 
					     Ogre::uint8 queue_id  /* = Ogre::RENDER_QUEUE_OVERLAY */, 
					     bool post_queue	   /* = false */, 
					     CEGUI::uint max_quads /* = 0 */,
					     Ogre::SceneManager* scene_manager /*= NULL*/,
						 std::string log_file /*= "CEGUI.log"*/)
{
	// setup GUI system
	m_pGUIRenderer = new CEGUI::OgreCEGUIRenderer(window, queue_id, post_queue, max_quads, scene_manager);
	m_pGUISystem = new CEGUI::System(m_pGUIRenderer, 0, 0, 0, "", log_file.c_str());

	m_pGUIRenderer->setRecycleTextureListener(this);
	m_pGUIRenderer->enableRecycleTexture(VariableSystem::getSingleton().GetAs_Int("GUIRecycleEnable") != 0);
	m_pGUIRenderer->setRecycleTextureInterval(VariableSystem::getSingleton().GetAs_Int("GUIRecycleIntervalTime")*1000);
}
//---------------------------------------------------------------------------------------------
CGUIManager* CGUIManager::getSingletonPtr(void)
{
	if (!m_pInstance)
		m_pInstance = new CGUIManager();

	return m_pInstance;
}
//---------------------------------------------------------------------------------------------
CGUIManager& CGUIManager::getSingleton(void)
{  
	if (!m_pInstance)
		m_pInstance = new CGUIManager();

	return (*m_pInstance);
}
//---------------------------------------------------------------------------------------------
bool CGUIManager::Initialize(std::string log_file /*= "CEGUI.log"*/)
{
	Create(
		EQOgreSys::getSingleton().getRenderWindow()
		, Ogre::RENDER_QUEUE_OVERLAY, false, 0, 
		EQOgreSys::getSingleton().getSceneManager(),
		log_file
		);
	// set the default resource groups to be used
	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	return true;
}
//---------------------------------------------------------------------------------------------
void CGUIManager::OnRender()
{
	if (!m_pGUISystem)
		return;

	m_pGUISystem->renderGUI();
}
//---------------------------------------------------------------------------------------------
void CGUIManager::Terminate()
{
	if (m_pInstance)
	{
		delete m_pInstance;

		m_pInstance = NULL;
	}
}
//---------------------------------------------------------------------------------------------
CEGUI::MouseButton CGUIManager::convertOISMouseButtonToCegui(int buttonID)
{
	switch (buttonID)
	{
	case 0: return CEGUI::LeftButton;
	case 1: return CEGUI::RightButton;
	case 2:	return CEGUI::MiddleButton;
	case 3: return CEGUI::X1Button;
	default: return CEGUI::LeftButton;
	}
}
//---------------------------------------------------------------------------------------------
bool CGUIManager::OnMouseMoved(const OIS::MouseEvent &Event)
{
// 	CEGUI::Window* move_win = CEGUI::System::getSingleton().getWindowContainingMouse();
// 	if (move_win != NULL)
// 	{
// 		LOGERR(move_win->getName().c_str());
// 	}
	return CEGUI::System::getSingleton().injectMouseMove(Event.state.X.rel, 
														Event.state.Y.rel);
}
//---------------------------------------------------------------------------------------------
bool CGUIManager::OnMousePressed(const OIS::MouseEvent &Event, 
								 OIS::MouseButtonID eID)
{
	return CEGUI::System::getSingleton().injectMouseButtonDown(
								convertOISMouseButtonToCegui(eID));
}
//---------------------------------------------------------------------------------------------
bool CGUIManager::OnMouseReleased(const OIS::MouseEvent &Event, 
								  OIS::MouseButtonID eID)
{
	return CEGUI::System::getSingleton().injectMouseButtonUp(
								convertOISMouseButtonToCegui(eID));
}
//---------------------------------------------------------------------------------------------
void CGUIManager::OnUpdate()
{
	UpdateDebugStats();
}
//---------------------------------------------------------------------------------------------
void CGUIManager::ShowDebugOverlay()
{
	if (!m_pDebugOverlay)
		return;

	m_pDebugOverlay->show();
}
//---------------------------------------------------------------------------------------------
void CGUIManager::HideDebugOverlay()
{
	if (!m_pDebugOverlay)
		return;

	m_pDebugOverlay->hide();
}
//---------------------------------------------------------------------------------------------
void CGUIManager::UpdateDebugStats()
{
}
//---------------------------------------------------------------------------------------------
void CGUIManager::WindowResized(Ogre::RenderWindow* rw)
{
	m_pGUIRenderer->setDisplaySize(CEGUI::Size(static_cast<float>(rw->getWidth()),
											static_cast<float>(rw->getHeight())));
}
//---------------------------------------------------------------------------------------------
void CGUIManager::SetText(const char* szWindows, const char* szText)
{
	if(false == CEGUI::WindowManager::getSingleton().isWindowPresent(szWindows))
		return;

	CEGUI::Window *pWindow = CEGUI::WindowManager::getSingleton().getWindow(szWindows);
	pWindow->setText((CEGUI::utf8*)szText);

	std::string sWinName(szWindows);
	std::string sWinText(szText);

	const int nOnlyEnMaxLen = 8;
	const int nOnlyChsMaxLen = 4;
	const int nBothEnAndChsMaxLen = 6;

	if(sWinText.length() > nOnlyChsMaxLen &&
		(0 == sWinName.find("farm_friendlist") || 
		0 == sWinName.find("freindlist/acc")))
	{
		int strMaxLen = 0;
		int checkResult = checkIsCharAlpha(szText);

		if (checkResult == 0)//全字母
		{
			strMaxLen = nOnlyEnMaxLen;
		}
		else if(checkResult == 1)//全中文
		{
			strMaxLen = nOnlyChsMaxLen;
		}
		else if(checkResult == 2)//混合
		{
			strMaxLen = nBothEnAndChsMaxLen;
		}

		const CEGUI::String &szTxt = pWindow->getText();

		if((int)szTxt.length() > strMaxLen)
		{
			pWindow->setText(szTxt.substr(0,strMaxLen) + "...");
		}
	}
}
//---------------------------------------------------------------------------------------------
//void CGUIManager::SetChatText(const char* dest, /*const CEGUI::String& src*/
//							  const char* src)
//{
//	//CEGUI::Window*pDest = CEGUI::WindowManager::getSingleton().getWindow(dest);
//	//CEGUI::Window*pSrc = CEGUI::WindowManager::getSingleton().getWindow(src);
//
//	//if (!pDest | !pSrc)
//	//return;
//	//
//	////pDest->setText(src);
//	////return;
//	////
//	//static CEGUI::String buf = "";
//
//	//CEGUI::String text = pSrc->getText();
//	//text += "\n";
//	//buf += text;
//
//	//pDest->setText(buf);
//
//	ChatSystem::getSingleton().requestTalk(dest,src);
//}

//---------------------------------------------------------------------
void CGUIManager::setSysMsgText(std::string sysMsg)
{
	ChatSystem::getSingleton().setSysMsgText(sysMsg);
}

void CGUIManager::setTooltipProperty(const std::string szWindow, const std::string szText)
{
	CEGUI::Window*pWindow = CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)szWindow.c_str());

	if (!pWindow)
		return;

	pWindow->setProperty("Tooltip", (CEGUI::utf8*)szText.c_str());
}
//
int CGUIManager::checkIsCharAlpha(const char* checkStr)
{
	BOOL lastChkRes = FALSE;
	BOOL chkRes =FALSE;
	bool firstDoLoop = true;
	int ret  = 0;
	for ( ;(*checkStr) != '\0'; checkStr++)
	{
		lastChkRes = chkRes;
		chkRes = IsCharAlpha((*checkStr));		
		if (chkRes == lastChkRes && chkRes == TRUE)
		{
			ret = 0;
		}
		else if (chkRes == lastChkRes && chkRes == FALSE)
		{
			ret = 1;
		}
		else
		{
			ret = 2;
			if (!firstDoLoop)
			{
				break;
			}
		}
		if (firstDoLoop)
		{
			firstDoLoop = false;
		}
	}
	return ret;
}
//----------------------------------------------------------------
std::string CGUIManager::returnValidString(std::string fullStr)
{
	int checkResult = 0;
	int strMaxLen = 0;

	checkResult = checkIsCharAlpha(fullStr.c_str());
	if (checkResult == 0)//全字母
	{
		strMaxLen = 6;
	}
	else if(checkResult == 1)//全中文
	{
		strMaxLen = 4;
	}
	else if(checkResult == 2)//混合
	{
		strMaxLen = 4;
	}

	const unsigned char* full_char = (const unsigned char*)(fullStr.c_str());
	int full_len = fullStr.length();
	unsigned char un_char;
	int str_count=0;
	while (full_len--)
	{
		un_char = *full_char++;
		++str_count;

		if (un_char < 0x80)
		{
		}
		else if (un_char < 0xE0)
		{
			--full_len;
			++full_char;
		}
		else if (un_char < 0xF0)
		{
			full_len -= 2;
			full_char += 2;
		}
		else
		{
			full_len -= 2;
			full_char += 3;
		}

	}

	std::string part_string;
	if (str_count > strMaxLen)
	{
		for (int i = 0; i < strMaxLen; i++)
		{
			std::string first_string = fullStr.substr(0,1) ;
			if((unsigned char)first_string[0] > 127) 
			{
				part_string += fullStr.substr(0,3);
				fullStr = fullStr.substr(3);

			}
			else
			{
				part_string += fullStr.substr(0,1);
				fullStr = fullStr.substr(1);
			}
		}

		std::string end_string("...");
		std::string to_end_utf_string = GB2312ToUTF8(end_string.c_str());
		std::string final_utf_string = part_string + to_end_utf_string;
		return final_utf_string;
	}
	else
	{
		return fullStr;
	}
}
//----------------------------------------------------------------
void CGUIManager::recycle(long useTime, size_t totalCount, size_t totalSize, size_t recyCount, size_t recySize)
{
	LOGDBG("GUI recycle texture. Use time:" + Ogre::StringConverter::toString(useTime) +
		"  Total count:" + Ogre::StringConverter::toString(totalCount) +
		"  Total size:" + Ogre::StringConverter::toString(totalSize) +
		"  Recycle count:" + Ogre::StringConverter::toString(recyCount) +
		"  Recycle size:" + Ogre::StringConverter::toString(recySize) 
		);
}