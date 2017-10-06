#include "EQCommPCH.h"

#include "EQWndProc.h"
//#include "OgreOIS.h"
#include "AppListener.h"
#include "EQGameMgr.h"
#include "EQCamera.h"
#include "MainPlayer.h"
#include "Variable.h"
#include "EQOIS.h"
#include "EQGameSceneMgr.h"
#include "EQOgreSys.h"
#include "ConfigOption.h"
#include "GiftSystem.h"
#include "CommFunc.h"

#include <CEGUIBase.h>
#include <CEGUIString.h>
#include <CEGUISystem.h>
#include "shellapi.h"
#include "resource.h"

#pragma comment(lib, "Imm32.lib")

#define MWM_SHOWTASK (WM_USER + 100)
#define IDM_PMENU_EXIT 12345

//extern bool g_bActive;
//-------------------------------------------------------------------------------
template<> EQWndProc* Ogre::Singleton<EQWndProc>::ms_Singleton = 0;

//--------------------------------------------------------------------------------
int releaseMemory(void*)
{
	if(VariableSystem::getSingleton().GetAs_Int("AutoReleaseMemory") == 1)
		::SetProcessWorkingSetSize(::GetCurrentProcess(), -1, -1);
	return 0;
}
//--------------------------------------------------------------------------------
#if defined( _DEBUG )							
//调试版本
const char * APP_TITLE = "EQHome Debug Version 0.5.0("__TIME__ " " __DATE__")";
#elif defined( _EVALUATION )				
//功能演示版本
const char * APP_TITLE = "EQHome Evaluation Version  0.5.0("__TIME__ " " __DATE__")";
#else			
//发布版本
const char * APP_TITLE = "EQHome Release Version 0.5.0("__TIME__ " " __DATE__")";
#endif
//--------------------------------------------------------------------------------
static const LPCTSTR	APP_NAME				= "EQClient";
static const int		MIN_WINDOW_WIDTH		= 480;
static const int		MIN_WINDOW_HEIGH		= 320;

LRESULT CALLBACK WndProc( HWND	 hWnd, 
						 UINT	 msg, 
						 WPARAM wParam, 
						 LPARAM lParam );
//--------------------------------------------------------------------------------
HWND EQWndProc::createWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_GAME);
	wcex.hCursor		= LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground	= (HBRUSH)NULL; 
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= VariableSystem::getSingleton().GetAs_String("APP_NAME").c_str();//APP_NAME;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	if( !RegisterClassEx( &wcex ) )
		return FALSE;

	const int	borderWidth		= GetSystemMetrics(SM_CXFRAME),
				borderHeight	= GetSystemMetrics(SM_CYFRAME),
				titleHeight		= GetSystemMetrics(SM_CYCAPTION);

	std::string cfgDir = GetGameUserDir() + "Config/";
	ForceDir(cfgDir);
	std::string cfgFile = cfgDir + VariableSystem::getSingleton().GetAs_String("CONFIG_FILE");
	EQOgreSys::getSingleton().getConfigOption()->load(cfgFile.c_str());
	mWndWidth = EQOgreSys::getSingleton().getConfigOption()->getWidth();
	mWndHeight = EQOgreSys::getSingleton().getConfigOption()->getHeight();

	LONG style;
	int x,y,actWidth,actHeight;
	if(EQOgreSys::getSingleton().getConfigOption()->isFullScreen())
	{
		//这样写为啥还会有标题栏啊，8明白哇
// 		style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
// 		style &= ~(WS_CAPTION);
		style = WS_POPUP;
		actWidth = mWndWidth;
		actHeight = mWndHeight;
		x = 0;
		y = 0;
	}
	else
	{
		style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		actWidth = EQOgreSys::getSingleton().getConfigOption()->getWidthWithBorder();
		actHeight = EQOgreSys::getSingleton().getConfigOption()->getHeightWithBorder();
		x = (GetSystemMetrics(SM_CXFULLSCREEN) - actWidth) / 2;
		y = (GetSystemMetrics(SM_CYFULLSCREEN) - actHeight) / 2;
	}

	//窗口居中
	mHWnd = CreateWindow(	 VariableSystem::getSingleton().GetAs_String("APP_NAME").c_str(),
		VariableSystem::getSingleton().GetAs_String("APP_NAME").c_str(),
		style,
		x,
		y,
		actWidth,
		actHeight,
		NULL,
		NULL,
		hInstance,
		0 );

	ShowWindow( mHWnd, true );
	UpdateWindow( mHWnd );

	mHMenu = CreatePopupMenu();
	AppendMenu(mHMenu,MF_STRING,IDM_PMENU_EXIT,"退出(&X)");
	AddNotifyIcon(mHWnd, hInstance, IDI_SMALL, VariableSystem::getSingleton().GetAs_String("APP_NAME").c_str());

	return mHWnd;
}
//
VOID KeepWindowFOV(RECT* pRect, UINT dwChanging, UINT dwAnchor);
bool injectChar(CEGUI::utf32 code_point);
//-------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND	 hWnd, 
								 UINT	 msg, 
								 WPARAM  wParam, 
								 LPARAM  lParam )
{
	static bool hasClickMinimizeBox = false;
	//
	if (!EQGameMgr::getSingleton().ifAppStarted())
	{
		if( WM_PAINT == msg )
		{
			RECT rect;
			GetClientRect( hWnd, &rect );
			FillRect( GetDC( hWnd ), &rect, (HBRUSH)( COLOR_WINDOW+1) );
		}
		return DefWindowProc( hWnd, msg, wParam, lParam );
	}

	//window窗口系统消息
	switch (msg) 
	{
	case WM_ACTIVATE:
		{
			switch(wParam)
			{
			case WA_INACTIVE:
				{
					sEQWndProc.setActive(false);
				}
				break;
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				{
					sEQWndProc.setActive(true);
				}
				break;
			}
		}
		break;
	//设置光标
	case WM_SETCURSOR:
		{
			if(sEQWndProc.isActive() && LOWORD(lParam)==HTCLIENT/*bHideMouse*/) 
			{
				SetCursor(NULL);
				if (!(sEQGameSceneMgr.hasScene() && sEQGameSceneMgr.getIsLoading()))
				{
					CEGUI::MouseCursor::getSingleton().show();
				}
			}
			else
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				CEGUI::MouseCursor::getSingleton().hide();
			}
		}
		break;
	//窗口大小发生改变
	case WM_SIZE:
		{
			//最小化
			if( SIZE_MINIMIZED == wParam )
			{
				sEQWndProc.setActive(false);
			}
			//最大化
			else if( SIZE_MAXIMIZED == wParam )
			{
				sEQWndProc.setActive(true);
			}
			//恢复窗口
			else if( SIZE_RESTORED == wParam )
			{
				sEQWndProc.setActive(true);
			}
		}
		break;

	case WM_MOVE:
		{
			if(NULL != GiftSystem::getSingletonPtr())
			{
				GiftSystem::getSingleton().moveDisplayFlashWin();
			}
		}
		break;

	//case 0x020A: // WM_MOUSEWHEEL:
	//	{
	//		CEGUI::System::getSingleton().injectMouseWheelChange(
	//			static_cast<float>((short)HIWORD(wParam)) / static_cast<float>(120));
	//	}
	//	break;
	//case WM_MOVE:
	//	break;
		//用户拖动窗口开始
	//case WM_ENTERSIZEMOVE:
	//	{
	//		//m_bRenderingPaused = true;
	//	}
	//	break;
		//用户拖动中
	//case WM_SIZING:
	//	{
			//RECT* pRect = (RECT*)lParam;

			//switch(wParam)
			//{
			//case WMSZ_RIGHT:
			//case WMSZ_BOTTOMRIGHT:
			//	{
			//		KeepWindowFOV(pRect, WMSZ_RIGHT, WMSZ_TOPLEFT);
			//	}
			//	break;
			//case WMSZ_LEFT:
			//case WMSZ_BOTTOMLEFT:
			//	{
			//		KeepWindowFOV(pRect, WMSZ_LEFT, WMSZ_TOPRIGHT);
			//	}
			//	break;

			//case WMSZ_TOP:
			//case WMSZ_TOPRIGHT:
			//	{
			//		KeepWindowFOV(pRect, WMSZ_TOP, WMSZ_BOTTOMLEFT);
			//	}
			//	break;

			//case WMSZ_BOTTOM:
			//	{
			//		KeepWindowFOV(pRect, WMSZ_BOTTOM, WMSZ_TOPLEFT);
			//	}
			//	break;

			//case WMSZ_TOPLEFT:
			//	{
			//		KeepWindowFOV(pRect, WMSZ_TOP, WMSZ_BOTTOMRIGHT);
			//	}
			//	break;
			//}
			//return TRUE;
		//}
		//break;
		//用户拖动窗口边缘结束
	//case WM_EXITSIZEMOVE:
	//	{
			////设置
			////if(s_pVariableSystem)
			//{
			//	RECT rect;
			//	GetClientRect(hWnd, &rect);
			//	EQWndProc::getSingleton().setWndSize(
			//								rect.right-rect.left
			//								,rect.bottom-rect.top);

			//	OIS::Mouse* mouse = EQOIS::getSingleton().getMouse();

			//	mouse->getMouseState().width = EQWndProc::getSingleton().getWndWidth();
			//	mouse->getMouseState().height = EQWndProc::getSingleton().getWndHeight();
			//	//char szTemp[MAX_PATH];
			//	//_snprintf(szTemp, MAX_PATH, "%d,%d", 
			//	// rect.right-rect.left, rect.bottom-rect.top);
			//	//CEGUI::System::getSingleton().handleDisplaySizeChange()
			//}
		//}
		//break;
	case WM_NCHITTEST:
		{
			LRESULT rtn = DefWindowProc( hWnd, msg, wParam, lParam );

			if(HTMINBUTTON == rtn)
				hasClickMinimizeBox = true;
			else
				hasClickMinimizeBox = false;

			return rtn;
		}
		break;
	case WM_SYSCOMMAND:
		{
			switch(wParam)
			{
			case SC_MINIMIZE:
				if(hasClickMinimizeBox)
				{
					ShowWindow(sEQWndProc.getHWnd(), SW_HIDE);
					hasClickMinimizeBox = false;
					break;
				}
			default:
				return DefWindowProc( hWnd, msg, wParam, lParam );
			}
		}
		break;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDM_PMENU_EXIT:
				PostQuitMessage(0);
				break;
			}
		}
		break;
	case MWM_SHOWTASK:
		{
			switch(lParam)
			{
			case WM_LBUTTONDOWN:
				{
					if(IsWindowVisible(sEQWndProc.getHWnd()))
					{
						ShowWindow(sEQWndProc.getHWnd(), SW_HIDE);
					}
					else
					{
						ShowWindow(sEQWndProc.getHWnd(), SW_SHOWNORMAL);
						SwitchToThisWindow(sEQWndProc.getHWnd(), TRUE);
						sEQWndProc.setActive(true);
					}
				}
				break;
			case WM_RBUTTONDOWN:
				{
					POINT pt;
					GetCursorPos(&pt);
					SetForegroundWindow(hWnd);
					TrackPopupMenu(sEQWndProc.getHMenu(), TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
				}
				break;
			}
		}
		break;
		
	case WM_KEYDOWN:
		{
			bool isImeInput = (wParam == ImmGetVirtualKey(hWnd));
			//
			EQWndProc::getSingleton().setImeInput(isImeInput);
			if (!isImeInput)
			{
				// 输入栏无法连续输入问题
				// 将来这种需求如果很多，应将OIS改为非缓冲，每帧处理一下
				if (wParam == VK_BACK)
					CEGUI::System::getSingleton().injectKeyDown(OIS::KC_BACK);
				else if (wParam == VK_DELETE)
					CEGUI::System::getSingleton().injectKeyDown(OIS::KC_DELETE);
				else if (wParam == VK_LEFT)
					CEGUI::System::getSingleton().injectKeyDown(OIS::KC_LEFT);
				else if (wParam == VK_RIGHT)
					CEGUI::System::getSingleton().injectKeyDown(OIS::KC_RIGHT);
			}
		}
		break;
	case WM_SYSKEYDOWN:
		{
			//F10是window默认的菜单快捷键，不会触发WM_KEYDOWN，要在这里特殊处理
			if(wParam == VK_F10)
				sEQWndProc.setActive(true);
		}
		break;
	case WM_CHAR:
		injectChar((CEGUI::utf32)wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}

	return true;
}
//
#define MIN_CHINESE 161

bool injectChar(CEGUI::utf32 code_point)
{
	//CEGUI::utf32 code_point = (CEGUI::utf32)wParam;
	//#ifndef UNICODE
	static char s_tempChar[3] = "";
	static wchar_t s_tempWchar[2] = L"";
	static bool s_flag = false;
	unsigned char uch = (unsigned char)code_point;
	if( uch >= MIN_CHINESE )
	{
		if( !s_flag )
		{
			s_tempChar[0] = (char)uch; //第一个字节
			s_flag = true;
			return true;
		}
		else/* if( uch >= MIN_CHINESE )*/
		{
			s_tempChar[1] = (char)uch; //第二个字节
			s_flag = false;
			MultiByteToWideChar( 0, 0, s_tempChar, 2, s_tempWchar, 1); //转成宽字节
			s_tempWchar[1] = L'\0';
			CEGUI::utf32 code = (CEGUI::utf32)s_tempWchar[0];
			return CEGUI::System::getSingleton().injectChar( code );
		}
		//else
		//{
		//	return CEGUI::System::getSingleton().injectChar(code_point);
		//}
	}
	else
	{
		s_flag = false;
		return CEGUI::System::getSingleton().injectChar(code_point);
	}
	//#else
	//	return CEGUI::System::getSingleton().injectChar(code_point );
	//#endif
}
//保持窗口比例
VOID KeepWindowFOV(RECT* pRect, UINT dwChanging, UINT dwAnchor)
{
	RECT rectTemp;
	CopyRect(&rectTemp, pRect);

	if(WMSZ_LEFT == dwChanging || WMSZ_RIGHT == dwChanging)
	{
		//宽不变
		rectTemp.bottom = rectTemp.top + (INT)((rectTemp.right-rectTemp.left)*0.75);
	}
	else if(WMSZ_TOP == dwChanging || WMSZ_BOTTOM == dwChanging)
	{
		//高不变
		rectTemp.right = rectTemp.left + (INT)((rectTemp.bottom-rectTemp.top)/0.75);
	}
	else
	{
		//根据比例自动调节
		FLOAT fFov = (FLOAT)(rectTemp.bottom-rectTemp.top)/(FLOAT)(rectTemp.right-rectTemp.left);
		if(fFov > 0.75)
		{
			rectTemp.bottom = rectTemp.top + (INT)((rectTemp.right-rectTemp.left)*0.75);
		}
		else
		{
			rectTemp.right = rectTemp.left + (INT)((rectTemp.bottom-rectTemp.top)/0.75);
		}
	}

	AdjustWindowRect(&rectTemp, WS_OVERLAPPEDWINDOW, FALSE);

	switch(dwAnchor)
	{
	case WMSZ_TOPLEFT:
		{
			pRect->right = pRect->left + (rectTemp.right-rectTemp.left);
			pRect->bottom = pRect->top + (rectTemp.bottom-rectTemp.top);
		}
		break;

	case WMSZ_TOPRIGHT:
		{
			pRect->left = pRect->right - (rectTemp.right-rectTemp.left);
			pRect->bottom = pRect->top + (rectTemp.bottom-rectTemp.top);
		}
		break;

	case WMSZ_BOTTOMLEFT:
		{
			pRect->right = pRect->left + (rectTemp.right-rectTemp.left);
			pRect->top = pRect->bottom - (rectTemp.bottom-rectTemp.top);
		}
		break;

	case WMSZ_BOTTOMRIGHT:
		{
			pRect->left = pRect->right - (rectTemp.right-rectTemp.left);
			pRect->top = pRect->bottom - (rectTemp.bottom-rectTemp.top);
		}
		break;
	}
}

void EQWndProc::AddNotifyIcon(HWND hWnd, HINSTANCE hInst, UINT uId, const char *tip)
{
	NOTIFYICONDATA nid = {0};

	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = uId;
	nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	nid.uCallbackMessage = MWM_SHOWTASK;
	nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(uId));
	strcpy(nid.szTip, tip);
	Shell_NotifyIcon(NIM_ADD, &nid);
}

void EQWndProc::DelNotifyIcon(HWND hWnd, UINT uId)
{
	NOTIFYICONDATA nid = {0};

	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = uId;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

void EQWndProc::cleanup()
{
	DestroyMenu(mHMenu);
	DelNotifyIcon(mHWnd, IDI_SMALL);
}