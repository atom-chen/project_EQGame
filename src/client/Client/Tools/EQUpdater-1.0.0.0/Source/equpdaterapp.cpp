/////////////////////////////////////////////////////////////////////////////
// Name:        equpdaterapp.cpp
// Purpose:     
// Author:      xj
// Modified by: 
// Created:     15/11/2010 09:25:37
// RCS-ID:      
// Copyright:   XJ-1
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "equpdaterapp.h"

////@begin XPM images
////@end XPM images


/*
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( EQUpdaterApp )
////@end implement app


/*
 * EQUpdaterApp type definition
 */

IMPLEMENT_CLASS( EQUpdaterApp, wxApp )


/*
 * EQUpdaterApp event table definition
 */

BEGIN_EVENT_TABLE( EQUpdaterApp, wxApp )

////@begin EQUpdaterApp event table entries
////@end EQUpdaterApp event table entries

END_EVENT_TABLE()


/*
 * Constructor for EQUpdaterApp
 */

EQUpdaterApp::EQUpdaterApp()
{
    Init();
}

/*
 * Member initialisation
 */

void EQUpdaterApp::Init()
{

}

/*
 * Initialisation for EQUpdaterApp
 */
bool EQUpdaterApp::Initialize(int& argc, wxChar **argv)
{
	m_para = NULL;
	m_user = NULL;
	m_pwd = NULL;
	m_checker = NULL;

	switch(argc)
	{
	case 4:
		m_para = argv[1];
		m_user = argv[2];
		m_pwd = argv[3];
		break;
	}

	return wxApp::Initialize(argc, argv);
}

void RunGame(wxChar *param, wxChar *user, wxChar *pwd)
{
	const wxString eq = L"EQClient.exe";
	const wxString farm = L"EQFarm.exe";

	if (param)
	{
		wxString exe;
		if ( param == wxString(L"-eq") )
			exe = eq + L" -FuckingAutoRun -u " + user + L" " + pwd;
		else // L"-farm"
			exe = farm + L" -FuckingAutoRun -u " + user + L" " + pwd;

		wxExecute(exe);
	}
	else
		wxExecute(eq + L" -FuckingAutoRun");
}

bool EQUpdaterApp::OnInit()
{
	static const char *checkExes[] =
	{
		"EQClient.exe",
		"EQClient_dr.exe",
		"EQFarm.exe",
		"EQFarm_dr.exe"
	};

	//有游戏运行则不升级，直接打开游戏。
	if(XFuns::CheckHasExeRun(checkExes, sizeof(checkExes)/sizeof(const char *)))
	{
		//wxMessageBox(wxT("客户端正在运行中，请先关闭！"), wxT("信息"), wxICON_INFORMATION|wxOK|wxCENTER);
		RunGame(m_para, m_user, m_pwd);
		return false;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//正在升级，不能在启动升级程序了。
	//win7 下提升权限后第一次运行判断错误
	/*m_checker = new wxSingleInstanceChecker(wxT("EQUpdaterI"));
	if(m_checker->IsAnotherRunning())
	{
		wxMessageBox(wxT("游戏正在更新中，请完成更新后在启动！"), wxT("信息"), wxICON_INFORMATION|wxOK|wxCENTER);
		return false;
	}*/
	HANDLE hMutex = CreateMutexA(NULL,TRUE,"D8F27E98-907F-4057-94E8-EBCB0F35230D");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		wxMessageBox(wxT("游戏正在更新中，请完成更新后在启动！"), wxT("信息"), wxICON_INFORMATION|wxOK|wxCENTER);
		return false;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG
#define LOCKER_FILE "EQUpdaterI.exe.locker"
	bool runCopy = false;
	DWORD flag = 0;
	FILE *pf = fopen(LOCKER_FILE, "r+b");
	if(NULL == pf)
		pf = fopen(LOCKER_FILE, "w+b");
	if(pf)
	{
		fread(&flag, sizeof(DWORD), 1, pf);
		if(0 == flag)
		{
			runCopy = true;
			flag = 0xFFFFFFFF;
		}
		else
		{
			flag = 0;
		}
		fseek(pf, 0, SEEK_SET);
		fwrite(&flag, sizeof(DWORD), 1, pf);
		fclose(pf);
	}
	else
		XLog::Write("Open LOCKER_FILE error!");

	if(runCopy)
	{
		static const char *copyFiles[] =
		{
			"EQUpdaterI.exe",
			"Microsoft.VC90.CRT.manifest",
			"msvcp90.dll",
			"msvcr90.dll"/*,
			"curllib.dll",
			"libeay32.dll",
			"openldap.dll",
			"ssleay32.dll"*/
		};

		for(int i = 0; i < sizeof(copyFiles) / sizeof(const char *); i ++)
		{
			std::string srcf = copyFiles[i];
			std::string dstf = "";
			dstf.append("%TEMP%\\");
			dstf.append(copyFiles[i]);
			XFuns::ExpandEnvVars(dstf);

			if(FALSE == DeleteFileA(dstf.c_str()))
				XLog::Write("Error delete [%s]", dstf.c_str());
			if(FALSE == CopyFileA(srcf.c_str(), dstf.c_str(), FALSE))
				XLog::Write("Error copy [%s]", srcf.c_str());
		}

		std::string runExe = "";
		runExe.append("%TEMP%\\");
		runExe.append(copyFiles[0]);
		XFuns::ExpandEnvVars(runExe);

		wxString vars = wxT("");
		if(m_para)
			vars = wxString::Format(wxT("%s %s %s"), m_para, m_user, m_pwd);

		static char curDir[MAX_PATH + 1];
		GetCurrentDirectoryA(MAX_PATH, curDir);

		if(!XFuns::RunExe(runExe.c_str(), curDir, vars.char_str().data()))
		{
			pf = fopen(LOCKER_FILE, "wb");
			if(pf)
			{
				flag = 0;
				fwrite(&flag, sizeof(DWORD), 1, pf);
				fclose(pf);
			}

			wxMessageBox(wxT("无法启动更新程序！"), wxT("错误"), wxICON_ERROR|wxOK|wxCENTER);
		}

		return false;

	}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	XLog::SetLogFile("EQUpdaterI.log");
	XLog::Write("*******************************************************************************");
	XLog::Write("***************************   EQUpdater log start   ***************************");
	XLog::Write("*******************************************************************************");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if wxUSE_XPM
	wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
	wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
	wxImage::AddHandler(new wxGIFHandler);
#endif
	EQUpdaterMainWin* mainWindow = new EQUpdaterMainWin(NULL);
	mainWindow->m_para = m_para;
	mainWindow->m_user = m_user;
	mainWindow->m_pwd = m_pwd;

	mainWindow->Show();

	return true;
}


/*
 * Cleanup for EQUpdaterApp
 */

int EQUpdaterApp::OnExit()
{
	/*if(m_checker)
		delete m_checker;*/

	return wxApp::OnExit();
}

