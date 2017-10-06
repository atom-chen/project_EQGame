// AvatarEditor.cpp : 定义应用程序的类行为。
//

#include "AvatarEditorPCH.h"
#include "MyApp.h"
#include "MainFrm.h"

#include "RootManager.h"
#include "ExceptionHandler.h"
#include "EQMain.h"
#include "Avatar.h"
#include "HouseSceneComponent.h"
#include "HSCManager.h"
#include "ItemPack.h"
#include "EQLog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif




HouseSceneComponent * g_HouseComp = NULL;


Avatar * g_Avatar = NULL;
ItemPack * g_ItemPack = NULL;

// 唯一的一个 CMyApp 对象

CMyApp theApp;
// CMyApp

BEGIN_MESSAGE_MAP(CMyApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMyApp::OnAppAbout)
END_MESSAGE_MAP()


// CMyApp 构造

CMyApp::CMyApp() 
	: m_goingTime(0)
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}





// CMyApp 初始化

BOOL CMyApp::InitInstance()
{
	return EQTryThisCall<BOOL>( this, &CMyApp::_InitInstance );
}

BOOL CMyApp::_InitInstance()
{
	{
		EQConsole::getSingleton().open();

		new RootManager();

				
		//=========================================================
		// 如果一个运行在 Windows XP 上的应用程序清单指定要
		// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
		//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(InitCtrls);
		// 将它设置为包括所有要在应用程序中使用的
		// 公共控件类。
		InitCtrls.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx(&InitCtrls);

		CWinApp::InitInstance();

		// 标准初始化
		// 如果未使用这些功能并希望减小
		// 最终可执行文件的大小，则应移除下列
		// 不需要的特定初始化例程
		// 更改用于存储设置的注册表项
		// TODO: 应适当修改该字符串，
		// 例如修改为公司或组织名
		SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
		// 若要创建主窗口，此代码将创建新的框架窗口
		// 对象，然后将其设置为应用程序的主窗口对象
		CMainFrame* pFrame = new CMainFrame;
		if (!pFrame)
			return FALSE;
		m_pMainWnd = pFrame;
		// 创建并加载框架及其资源
		pFrame->LoadFrame(IDR_MAINFRAME,
			WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
			NULL);

		
		pFrame->SetIcon( LoadIcon(IDR_MAINFRAME), false );


		// 唯一的一个窗口已初始化，因此显示它并对其进行更新

		pFrame->ShowWindow(SW_SHOW);
		CRect rect;
		pFrame->m_wnd3DView.GetClientRect(&rect);
		GraphicsSystem::getSingleton().setup(pFrame->m_wnd3DView.m_hWnd, rect.Width(), rect.Height());
		pFrame->setPage( 1 );

		pFrame->UpdateWindow();
		// 仅当具有后缀时才调用 DragAcceptFiles
		//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	}
	

	return TRUE;
}


// CMyApp 消息处理程序



void CMyApp::OnAppAbout()
{	
	CDialog aboutDlg(IDD_ABOUTBOX);
	aboutDlg.DoModal();
}


// CMyApp 消息处理程序

int CMyApp::Run()
{
	return EQTryThisCall<BOOL>( this, &CMyApp::_Run );
}

int CMyApp::_Run()
{
	return CWinApp::Run();
}

int CMyApp::ExitInstance()
{
	::EQLogManager::getLogger().getConsole().hide();
	
	
	//===============================================================
	int rc = CWinApp::ExitInstance();
	//===============================================================

	if ( g_HouseComp )
	{
		HSCManager::getSingleton().destroyHouseSceneComponent( g_HouseComp );
		g_HouseComp = NULL;
	}

	if ( g_Avatar )
	{
		delete g_Avatar;
		g_Avatar = NULL;
	}

	if ( g_ItemPack )
	{
		ItemPackLogic::clearItemPack(*g_ItemPack);
		delete g_ItemPack;
		g_ItemPack = NULL;
	}

	delete RootManager::getSingletonPtr();

	return rc;
}


BOOL CMyApp::PumpMessage()
{	
	const float iv = 1.0f/100.0f;

	MSG msg;
	while ( !::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) )
	{
		float currTime = clock()/1000.0f;
		if ( currTime - m_goingTime > iv )
		{
			m_goingTime = currTime;
			if ( m_pMainWnd )
			{
				((CMainFrame*)m_pMainWnd)->m_wnd3DView.Invalidate();
			}
			break;
		}
		else
			Sleep(1);
	}

	
	return CWinApp::PumpMessage();
}

