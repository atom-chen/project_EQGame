// ErrorReport.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "ErrorReport.h"
#include "ErrorReportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CErrorReportApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()



CErrorReportApp::CErrorReportApp()
{
}


CErrorReportApp theApp;



BOOL CErrorReportApp::InitInstance()
{
	CWinApp::InitInstance();
	
	CErrorReportDlg dlg(this->m_lpCmdLine);
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	
	return FALSE;
}
