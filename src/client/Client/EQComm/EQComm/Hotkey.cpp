#include "EQCommPCH.h"
#include "Hotkey.h"
#include "AppListener.h"
#include "EQLog.h"
#include "InputMgr.h"
#include "EQOgreSys.h"
#include "CEGUI.h"
#include "EQWndProc.h"
#include "EQGameMgr.h"
#include "EQCamera.h"
#include "CommFunc.h"
#include "GUIManager.h"
#include "EQOIS.h"
#include "GameState.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include "EditboxSearcher.h"
#include "QQWeibo.h"
#include "MainPlayer.h"
#include "EQGameSceneMgr.h"
#include "MicroBlog.h"
#include "Variable.h"
//------------------------------------------------------------------------------
Hotkey::Hotkey(void)
:EQInputEventListener(epfHotkey)
{
	sInputMgr.addListener(this);
	mHotkeys[KC_RETURN]	= false;
	mHotkeys[KC_R]		= false;
	mHotkeys[KC_F1]		= false;
	mHotkeys[KC_F2]		= false;
	mHotkeys[KC_F3]		= false;
	mHotkeys[KC_F4]		= false;
	mHotkeys[KC_F9]		= false;
	mHotkeys[KC_LSHIFT] = false;
	mHotkeys[KC_RSHIFT] = false;
	mHotkeys[KC_SysRq]	= false;
}
//------------------------------------------------------------------------------
Hotkey::~Hotkey(void){}
//------------------------------------------------------------------------------
Hotkey& Hotkey::getSingleton()
{
	static Hotkey singleton;
	return singleton;
}
//------------------------------------------------------------------------------
bool Hotkey::isKeydown(int key)
{
	if (mHotkeys.find(key) == mHotkeys.end())
	{
		//LOGERR("没有注册这个按键！");
		return false;
	}
	else
		return mHotkeys[key];
}

//------------------------------------------------------------------------------
void Hotkey::KeyUp( UINT& flags, UINT nChar, UINT nRepCnt)
{
	if((flags&epfUI) != 0) 
		return;

	if (mHotkeys.find(nChar) != mHotkeys.end())
	{
		switch (nChar)
		{
		case KC_RETURN:
			{
				// 激活输入框
				EditboxSearcher::activeTextbox();
			}
			return;
		case KC_R:
			{
				if (sStateMgr.inMainGame())
				{
					luabind::call_function<void>(sLuaMgr.getL(), "SpeedPrivateChat_RKeyDown");	// 快速私聊(hezhr)
				}
			}
			return;
		case KC_SysRq:
			{
				MicroBlogManager::getSingleton().fullScreenShot(GetGameUserDir()+"Screenshots/");	// 截全屏(2011/6/28 add by hezhr)
			}
			return;
		}

		//不发布的放这里
		if(VariableSystem::getSingleton().GetAs_Int("DebugHotkey"))
		{
			switch (nChar)
			{
			case KC_F1:
				{
					AppListener::getSingleton().showStatPanel( !AppListener::getSingleton().isStatPanelShown() );
				}
				return;
			case KC_F2:
				{
					//EQGameSceneMgr::getSingleton().setWeatherType( EQGameSceneMgr::WEATHER_RAIN );
				}
				return;
			case KC_F3:
				{
					if (sStateMgr.inMainGame())
					{
						MainPlayer::getSingleton().getMainModel()->addParticle(100);
					}
				}
				return;
			case KC_F4:
				{
					CEGUI::System::getSingleton().executeScriptFile("AssistantMsg.lua");
				}
				return;
			case KC_F9:
				{
					if (sStateMgr.inMainGame())
					{
						luabind::call_function<void>(sLuaMgr.getL(), "Player_ShowHide");	// 角色隐藏(2011/6/20 add by hezhr)
					}
				}
				return;
			}
		}

		mHotkeys[nChar] = false;
	}
}
//------------------------------------------------------------------------------
void Hotkey::KeyDown( UINT& flags, UINT nChar, UINT nRepCnt)
{
	//if((flags&epfUI) != 0) 
	//	return;

	if (mHotkeys.find(nChar) == mHotkeys.end())
		//LOGERR("没有注册这个按键！");
		;
	else
		mHotkeys[nChar] = true;
}

void Hotkey::setKeyFlag( int key, bool flag )
{
	if (mHotkeys.find(key) == mHotkeys.end())
		return;

	mHotkeys[key] = flag;

}
//------------------------------------------------------------------------------

