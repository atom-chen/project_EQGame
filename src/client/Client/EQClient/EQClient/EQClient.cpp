#include "EQClientPCH.h"
#include <windowsx.h>
#include "GameMgr.h"
#include "EQWndProc.h"
#include "Variable.h"
//eqmain
#include "ExceptionHandler.h"
#include <io.h>

#include "CEGUI.h"
// int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
#ifdef _DEBUG
// 	#include "vld.h"
//#define new   new(_NORMAL_BLOCK, __FILE__,__LINE__)
#endif


//
std::string				g_User;
std::string				g_Pwd;
bool					g_bAppStarted			= false;
bool					g_bAppQuit				= false;

int IdleProcess(LONGLONG freq, int inv, int inv1);
void ParseArgv(LPTSTR lpCmdLine, bool& autoRun);

int getCEGUIWindowCount()
{
	using namespace CEGUI;

	int cnt = 0;
	WindowManager::WindowIterator wit = WindowManager::getSingleton().getIterator();
	while(!wit.isAtEnd())
	{
		cnt ++;

		wit++;
	}

	return cnt;
}

//---------------------------------------------------------------------------
std::string getUser()
{
	return g_User;
}
//---------------------------------------------------------------------------
std::string getPwd()
{
	return g_Pwd;
}


//---------------------------------------------------------------------------
int APIENTRY _tWinMain( HINSTANCE hInstance, 
					  HINSTANCE hPrevInstance, 
					  LPTSTR lpCmdLine, 
					  int nCmdShow );
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	HINSTANCE instance = GetInstanceModule(NULL);
	_tWinMain(instance , NULL , NULL , 1);
}
//---------------------------------------------------------------------------
int APIENTRY /*_t*/EQWinMain (HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{
// 	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
// 
// 	tmpFlag = tmpFlag | _CRTDBG_LEAK_CHECK_DF;
// 
// 	_CrtSetDbgFlag( tmpFlag );
#ifdef _DEBUG
	long bk = -1; 
	_crtBreakAlloc = bk;
#endif

	bool ar = false;
	ParseArgv(lpCmdLine, ar);

#ifndef _DEBUG
	bool canrun = false;
	if ( (_access( "FuckingAutoRun", 0 )) == 0 )
		canrun = true;

	if ( !canrun && ar )
	{
		canrun = true;
	}

	if ( !canrun )
	{
		MessageBox(NULL, "请从EQUpdate.exe运行游戏!", "EQ家园", 0);
		return 0;
	}
#endif


	{
		//LOGMSG( "startup..." );
		//游戏管理器
		new GameMgr;	
		// 游戏初始化
		if( !GameMgr::getSingleton().init( hInstance ) )
		{
			DestroyWindow( EQWndProc::getSingleton().getHWnd() );
			return 0;
		}

		ShowCursor(true);
		g_bAppStarted = true;

		SetThreadAffinityMask(GetCurrentThread(), 1);
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		int fr = VariableSystem::getSingleton().GetAs_Int("FrameRate");
		int inv = 0;
		if ( fr != 0 )
			inv = 1000 / fr;
		int fr1 = VariableSystem::getSingleton().GetAs_Int("BackgroundRate");
		int inv1 = 0;
		if ( fr1 != 0 )
			inv1 = 1000 / fr1;

		//timeBeginPeriod(1);

		// 游戏循环
		while( !g_bAppQuit )
		{
			MSG msg;

			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				if( msg.message == WM_QUIT )
					break;

				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				// 检查用户输入 
				// 更新游戏窗口
				if (!GameMgr::getSingleton().update(true))
				{
					// 隐藏控制台
					::EQLogManager::getLogger().getConsole().hide();

					::PostQuitMessage(0);
				}
				else
				{
					IdleProcess(freq.QuadPart, inv, inv1);
				}
			}
		}

		char sstr[64] = {0};
		sprintf(sstr, "Total window count:%d", getCEGUIWindowCount());
		LOGMSG(sstr);
		//timeEndPeriod(1);

		GameMgr::getSingleton().cleanup();
		delete GameMgr::getSingletonPtr();
	}


	return 0;
}

//---------------------------------------------------------------------------
int APIENTRY /*_t*/WinMain (HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{

	// 设置进程异常捕获函数, 程序运行时该设置可能被其它程序修改
	//	考虑到有可能多线程, 这个方法仍保留
	ExceptionHandler::startHandleException();

	int ret = 0;
	__try
	{
		// 这里包含游戏所有代码 
		ret = EQWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	}
	__except(ExceptionHandler::showStackAndDump(GetExceptionInformation()))
	{
	}
	return ret;
}

//---------------------------------------------------------------------------
int IdleProcessImpl(LONGLONG freq, LONGLONG& last, int inv)
{
	int st = 0;

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	LONGLONG curr = li.QuadPart;

	LONGLONG x = (curr - last)*1000;	//放大1000倍
	x /= freq;
	if ( x < inv )
	{
		st = inv - x;
		Sleep(st);
		QueryPerformanceCounter(&li);
		curr = li.QuadPart;
	}

	last = curr;

	return st;
}
//---------------------------------------------------------------------------
int IdleProcess(LONGLONG freq, int inv, int inv1)
{
	int st = 0;
	static int wndStatus = 0;

	//如果最小化, 就保持在5帧数
	if ( IsIconic(EQWndProc::getSingleton().getHWnd()) )
	{
		static LONGLONG last = 0;

		st = IdleProcessImpl(freq, last, 200);

		wndStatus = 1;
	}
	else if (EQWndProc::getSingleton().getHWnd() != GetForegroundWindow())
	{
		if ( inv1 != 0 )
		{
			//窗口不在最前面
			static LONGLONG last = 0;
			st = IdleProcessImpl(freq, last, inv1);
		}

		if(wndStatus != 2)
			releaseMemory(NULL);
		wndStatus = 2;
	}
	else
	{
		if ( inv != 0 )
		{
			static LONGLONG last = 0;

			st = IdleProcessImpl(freq, last, inv);
		}

		wndStatus = 3;
	}

	return st;
}
//---------------------------------------------------------------------------
void ParseArgv(LPTSTR lpCmdLine, bool& autoRun)
{
	//debug这里会空
	if ( lpCmdLine == NULL )
		return;

	autoRun = false;

	std::vector<std::string> strv = Ogre::StringUtil::split(lpCmdLine);
	for ( uint i = 0; i < strv.size(); ++i )
	{
		if ( strv[i] == "-u" && i+2 < strv.size() )
		{
			g_User = strv[i+1];
			g_Pwd = strv[i+2];
			i += 2;
			continue;
		}
		else if ( strv[i] == "-FuckingAutoRun" )
		{
			autoRun = true;
			continue;
		}
	}

}