#include "EQCommPCH.h"
#include "EQOIS.h"
#include "EQWndProc.h"
#include "GUIManager.h"
#include "EQOgreSys.h"
#include "InputMgr.h"
#include "GameState.h"
//#include "CursorMgr.h"
#include "EditboxSearcher.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"

//extern bool g_bAppQuit;
//-------------------------------------------------------------------------
template<> EQOIS* Ogre::Singleton<EQOIS>::ms_Singleton = 0;
//------------------------------------------------------------------------
EQOIS::EQOIS(void)
		:
		mInputManager(0)
		,mKeyboard(0)
		,mMouse(0)
		,mActiveElapsed(0)
		,bSkip(false)
{
}

EQOIS::~EQOIS(void)
{
}
//-------------------------------------------------------------------------
void EQOIS::cleanup()
{
	if (mInputManager)
	{
		mInputManager->destroyInputObject(mKeyboard);
		mInputManager->destroyInputObject(mMouse);

		OIS::InputManager::destroyInputSystem( mInputManager );
	}
}
//-------------------------------------------------------------------------
void EQOIS::initial(size_t hWnd)
{
	Ogre::RenderWindow* rw = EQOgreSys::getSingleton().getRenderWindow();

	OIS::ParamList paramList;
	paramList.insert(OIS::ParamList::value_type("WINDOW", 
									Ogre::StringConverter::toString(hWnd)));

	size_t windowHnd = 0;

	std::ostringstream windowHndStr;
	EQOgreSys::getSingleton().getRenderWindow()->getCustomAttribute("WINDOW", &windowHnd);

	windowHndStr << (unsigned int)windowHnd;

	paramList.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	paramList.insert(std::make_pair(std::string("w32_mouse"), 
						std::string("DISCL_FOREGROUND")));
	paramList.insert(std::make_pair(std::string("w32_mouse"), 
						std::string("DISCL_NONEXCLUSIVE")));

	paramList.insert(std::make_pair(std::string("w32_keyboard"), 
						std::string("DISCL_FOREGROUND")));
	paramList.insert(std::make_pair(std::string("w32_keyboard"), 
						std::string("DISCL_NONEXCLUSIVE")));

	mInputManager = OIS::InputManager::createInputSystem( paramList );

	if (mInputManager)
	{
		mKeyboard = static_cast<OIS::Keyboard*>(
						mInputManager->createInputObject(OIS::OISKeyboard, true));

		mMouse = static_cast<OIS::Mouse*>(
					mInputManager->createInputObject(OIS::OISMouse, true));
	}

	// Set initial mouse clipping size
	//windowResized(rw);
	CGUIManager::getSingleton().WindowResized(rw);

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	// Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(rw, this);

	unsigned int	width, 
					height, 
					depth;
	int				left, top;
	rw->getMetrics(width, height, depth, left, top);

	mMouse->getMouseState().width = EQWndProc::getSingleton().getWndWidth();
	mMouse->getMouseState().height = EQWndProc::getSingleton().getWndHeight();
}
//
bool mouseOver()
{
	bool	bMouseOver=true;
	POINT	pt;
	RECT	rc;

	GetCursorPos(&pt);
	GetClientRect(EQWndProc::getSingleton().getHWnd(), &rc);
	MapWindowPoints(EQWndProc::getSingleton().getHWnd(), 
					NULL, (LPPOINT)&rc, 2);

	if(/*bCaptured || */(PtInRect(&rc, pt) 
		&& WindowFromPoint(pt)==EQWndProc::getSingleton().getHWnd()))
		bMouseOver=true;
	else
		bMouseOver=false;
	return bMouseOver;
}
//extern bool	g_bActive;
bool onFocus()
{
	return (mouseOver() && sEQWndProc.isActive());
}
//------------------------------------------------------------------------
void EQOIS::update(float timElapsed)
{
	if (!sEQWndProc.isActive())
	{
		mActiveElapsed = 0;
		return;
	}
	if(mActiveElapsed < 1)
		mActiveElapsed+=timElapsed;

	mKeyboard->capture();

	if (onFocus())
	{
		mMouse->capture();
		const OIS::MouseState& st = mMouse->getMouseState();
		if (mMouse->getMouseState().Z.rel != 0)
		{
			POINT p;
			p.x = st.X.abs;
			p.y = st.Y.abs;
			sInputMgr.MouseWheel(epfNone,p,st.Z.rel);
		}
		sInputMgr.update(timElapsed);
	}
	else
	{
		sInputMgr.lostFocus();
	}
}
//----------------------------------------------------------------
bool EQOIS::mouseMoved( const OIS::MouseEvent &arg )
{
	bool done;
	done = CGUIManager::getSingleton().OnMouseMoved(arg);
	
	if(	   arg.state.X.abs > 0
		&&(arg.state.Y.abs > 0)
		&&(arg.state.X.abs < EQWndProc::getSingleton().getWndWidth())  
		&&(arg.state.Y.abs < EQWndProc::getSingleton().getWndHeight()))
	{
		CEGUI::MouseCursor::getSingleton().setPosition(
			CEGUI::Point(arg.state.X.abs,arg.state.Y.abs));
	}

	POINT delta,mouse;
	delta.x = arg.state.X.rel;
	delta.y = arg.state.Y.rel;
	mouse.x = arg.state.X.abs;
	mouse.y = arg.state.Y.abs;

	sInputMgr.MouseMove(done?epfUI:epfNone, delta, mouse );
	return true;
}
//extern bool g_bActive;
//----------------------------------------------------------------
bool EQOIS::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(mActiveElapsed < 1)
		return true;

	if(    arg.state.X.abs == 0 || arg.state.Y.abs == 0
		|| arg.state.X.abs == EQWndProc::getSingleton().getWndWidth() 
		|| arg.state.Y.abs == EQWndProc::getSingleton().getWndHeight())
		return true;

	bool done;
	if(  (done = CGUIManager::getSingleton().OnMousePressed(arg, id)) == true )
		return true;

	if (sStateMgr.gaming() && !sStateMgr.isFunnyFarm())
	{
		luabind::call_function<void>(sLuaMgr.getL(), "Action_closeActionUI");
		CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot")->removeChildWindow("furniMenu");
		CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot")->removeChildWindow("circleMenu");
		CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot")->removeChildWindow("friendlist_manage");
		CEGUI::WindowManager::getSingleton().getWindow("DFwnd_MainRoot")->removeChildWindow("gmfriend_manage");
	}
 	POINT mouse;
 	OIS::MouseState ms = arg.state;

 	mouse.x = ms.X.abs;
 	mouse.y = ms.Y.abs;
 
 	switch(id)
 	{
 	case OIS::MB_Left:
		sInputMgr.LButtonDown(done?epfUI:epfNone, mouse);
 		break;
 	case OIS::MB_Right:
		sInputMgr.RButtonDown(done?epfUI:epfNone, mouse);
 		break;
 	case OIS::MB_Middle:
		sInputMgr.MButtonDown(done?epfUI:epfNone, mouse);
 		break;
 	}

	return true;
}

//----------------------------------------------------------------
bool EQOIS::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(arg.state.X.abs == 0 || arg.state.Y.abs == 0)
		return true;

	bool done = CGUIManager::getSingleton().OnMouseReleased(arg, id);

	//手动跳过
	if(bSkip)
	{
		done = true;
		bSkip = false;
	}

	POINT mouse;
	OIS::MouseState ms = arg.state;

	mouse.x = ms.X.abs;
	mouse.y = ms.Y.abs;

	switch(id)
	{
	case OIS::MB_Left:
		sInputMgr.LButtonUp(done?epfUI:epfNone, mouse);
		break;
	case OIS::MB_Right:
		sInputMgr.RButtonUp(done?epfUI:epfNone, mouse);
		break;
	case OIS::MB_Middle:
		sInputMgr.MButtonUp(done?epfUI:epfNone, mouse);
		break;
	}
	sInputMgr.resetFlag();
	return true;
}
//----------------------------------------------------------------
bool EQOIS::keyPressed( const OIS::KeyEvent &arg )
{
	//处理是否被输入法处理过
	if (EQWndProc::getSingleton().isImeInput())
		return true;


	// 跳过一些按键处理, 这些处理放在WndProc的WM_KEYDOWN
	// 将来这种需求如果很多，应将OIS改为非缓冲，每帧处理一下
	if (arg.key == OIS::KC_BACK || 
		arg.key == OIS::KC_DELETE || 
		arg.key == OIS::KC_LEFT || 
		arg.key == OIS::KC_RIGHT)
	{
		bool canInput = EditboxSearcher::canInput(EditboxSearcher::findActivateWindow());
		if ( canInput )
			return true;
	}

	bool done = CEGUI::System::getSingleton().injectKeyDown( arg.key );

	//if(sStateMgr.gaming())
	sInputMgr.KeyDown(done?epfUI:epfNone,UINT(arg.key));
	return true;
}

//----------------------------------------------------------------
bool EQOIS::keyReleased( const OIS::KeyEvent &arg )
{
	//主游戏普通模式
	bool done = CEGUI::System::getSingleton().injectKeyUp( arg.key );
	
	sInputMgr.KeyUp(done?epfUI:epfNone,UINT(arg.key));

	return true;
}
//----------------------------------------------------------------
bool EQOIS::isKeyDown( OIS::KeyCode key )
{
	assert( mKeyboard );
	return mKeyboard->isKeyDown(key);
}