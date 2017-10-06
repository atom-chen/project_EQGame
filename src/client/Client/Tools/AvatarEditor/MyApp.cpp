// AvatarEditor.cpp : ����Ӧ�ó��������Ϊ��
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

// Ψһ��һ�� CMyApp ����

CMyApp theApp;
// CMyApp

BEGIN_MESSAGE_MAP(CMyApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMyApp::OnAppAbout)
END_MESSAGE_MAP()


// CMyApp ����

CMyApp::CMyApp() 
	: m_goingTime(0)
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}





// CMyApp ��ʼ��

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
		CRect rect;
		pFrame->m_wnd3DView.GetClientRect(&rect);
		GraphicsSystem::getSingleton().setup(pFrame->m_wnd3DView.m_hWnd, rect.Width(), rect.Height());
		pFrame->setPage( 1 );

		pFrame->UpdateWindow();
		// �������к�׺ʱ�ŵ��� DragAcceptFiles
		//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	}
	

	return TRUE;
}


// CMyApp ��Ϣ�������



void CMyApp::OnAppAbout()
{	
	CDialog aboutDlg(IDD_ABOUTBOX);
	aboutDlg.DoModal();
}


// CMyApp ��Ϣ�������

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

