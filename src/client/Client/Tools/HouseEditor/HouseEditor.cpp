// HouseEditor.cpp : 定义应用程序的类行为。
//

#include "HouseEditorPCH.h"
#include "HouseEditor.h"
#include "MainFrm.h"

#include "RootManager.h"
#include "ExceptionHandler.h"
#include "EQMain.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHouseEditorApp

BEGIN_MESSAGE_MAP(CHouseEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CHouseEditorApp::OnAppAbout)
END_MESSAGE_MAP()

CHouseEditorApp::CHouseEditorApp() 
	: m_goingTime(0)
{
}

// 唯一的一个 CHouseEditorApp 对象

CHouseEditorApp theApp;


BOOL CHouseEditorApp::InitInstance()
{
	return EQTryThisCall<BOOL>( this, &CHouseEditorApp::_InitInstance );
}

BOOL CHouseEditorApp::_InitInstance()
{
	//_crtBreakAlloc = 945633;

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

		RootManager::getSingleton().initScene( pFrame->getView().m_hWnd );	

		pFrame->UpdateWindow();
		// 仅当具有后缀时才调用 DragAcceptFiles
		//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	}

	return TRUE;
}



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CHouseEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}




int CHouseEditorApp::Run()
{
	return EQTryThisCall<BOOL>( this, &CHouseEditorApp::_Run );
}

int CHouseEditorApp::_Run()
{	
	return CWinApp::Run();
}

int CHouseEditorApp::ExitInstance()
{
	::EQLogManager::getLogger().getConsole().hide();
	//===============================================================
	int rc = CWinApp::ExitInstance();
	//===============================================================

	delete RootManager::getSingletonPtr();

	return rc;
}


BOOL CHouseEditorApp::PumpMessage()
{
	// TODO: Add your specialized code here and/or call the base class
	const float iv = 1.0f/100.0f;

	MSG msg;
	while ( !::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) )
	{
		float currTime = clock()/1000.0f;
		if ( currTime - m_goingTime > iv )
		{
			m_goingTime = currTime;
			if ( m_pMainWnd )
				g_MainFrame->getView().Invalidate();

			break;
		}
		else
			Sleep(1);
	}

	return CWinApp::PumpMessage();
}
