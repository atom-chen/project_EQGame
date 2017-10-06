// HouseEditor.cpp : ����Ӧ�ó��������Ϊ��
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

// Ψһ��һ�� CHouseEditorApp ����

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
		// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
		// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
		//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(InitCtrls);
		// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
		// �����ؼ��ࡣ
		InitCtrls.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx(&InitCtrls);

		CWinApp::InitInstance();

		// ��׼��ʼ��
		// ���δʹ����Щ���ܲ�ϣ����С
		// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
		// ����Ҫ���ض���ʼ������
		// �������ڴ洢���õ�ע�����
		// TODO: Ӧ�ʵ��޸ĸ��ַ�����
		// �����޸�Ϊ��˾����֯��
		SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
		// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
		// ����Ȼ��������ΪӦ�ó���������ڶ���
		CMainFrame* pFrame = new CMainFrame;
		if (!pFrame)
			return FALSE;
		m_pMainWnd = pFrame;
		// ���������ؿ�ܼ�����Դ
		pFrame->LoadFrame(IDR_MAINFRAME,
			WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
			NULL);

		
		pFrame->SetIcon( LoadIcon(IDR_MAINFRAME), false );
		// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
		pFrame->ShowWindow(SW_SHOW);

		RootManager::getSingleton().initScene( pFrame->getView().m_hWnd );	

		pFrame->UpdateWindow();
		// �������к�׺ʱ�ŵ��� DragAcceptFiles
		//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	}

	return TRUE;
}



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
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
