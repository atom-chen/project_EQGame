
#include "stdafx.h"
#include "TestConnect.h"
#include "ErrorReport.h"
#include "ErrorReportDlg.h"
#include "ComputerInfo.h"

#include <string>
#include <vector>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����exe
static void RunExe( const std::string & cmd, bool bBlocking )
{
	std::string newcmd = cmd;

	PROCESS_INFORMATION           piProcInfo;
	STARTUPINFO                   siStartInfo;

	ZeroMemory(&piProcInfo,sizeof(piProcInfo));
	ZeroMemory(&siStartInfo,sizeof(siStartInfo));

	siStartInfo.cb				= sizeof(STARTUPINFO);  
	siStartInfo.lpReserved		= NULL;  
	siStartInfo.lpReserved2		= NULL;  
	siStartInfo.cbReserved2		= 0;  
	siStartInfo.lpDesktop		= NULL;  
	siStartInfo.dwFlags			= 0;  
	siStartInfo.wShowWindow		= SW_HIDE;

	CreateProcess(  
		NULL,  
		(char*)newcmd.c_str(),  
		NULL,					//   process   security   attributes  
		NULL,					//   primary   thread   security   attributes  
		0,						//   handles   are   inherited  
		CREATE_NO_WINDOW,		//   creation   flags  
		NULL,					//   use   parent's   environment  
		NULL,					//   use   parent's   current   directory  
		&siStartInfo,			//   STARTUPINFO   pointer  
		&piProcInfo);			//   receives   PROCESS_INFORMATION  

	if (bBlocking)
	{
		//   Wait   for   the   processs   to   finish  
		DWORD   rc   =   WaitForSingleObject(  
			piProcInfo.hProcess,   //   process   handle  
			INFINITE);   
	}
	else
		::CloseHandle(piProcInfo.hProcess);
}


// ȡOgreLog
static std::string GetOgreLogFile(const std::string & sDmpFile)
{	
	size_t pos = sDmpFile.find("\\Error\\");
	if (0 <= pos && pos < sDmpFile.size())
	{
		std::string ogreLog(sDmpFile.begin(), sDmpFile.begin() + pos);
		ogreLog += "\\Log\\EQClient.Ogre.log";
		return ogreLog;
	}

	return std::string();
}

// ȡ�����ִ�(ע���ʽ���ܸı䣬�����Ӱ���������������)
static std::string GetDateTimeStr()
{
	::SYSTEMTIME st;
	::GetLocalTime(&st);

	return FormatStr("%d-%d-%d %d:%d:%d",
						st.wYear, 
						st.wMonth, 
						st.wDay, 
						st.wHour, 
						st.wMinute, 
						st.wSecond);
}

static std::string GetMonthDayStr()
{
	::SYSTEMTIME st;
	::GetLocalTime(&st);

	return FormatStr("%02d%02d",
						st.wMonth, 
						st.wDay); 
}

static std::string trim(const std::string s)
{
	const std::string sSkip = "\r\n\t ";
	std::string::const_iterator itBegin = s.begin(); 
	std::string::const_iterator itEnd = s.end();

	for (; itBegin < itEnd; ++itBegin)
	{		
		size_t pos = sSkip.find(*itBegin);
		if (!(0 <= pos && pos < sSkip.size()))
			break;
	}

	for (; itBegin < itEnd; --itEnd)
	{		
		size_t pos = sSkip.find(*(itEnd - 1));
		if (!(0 <= pos && pos < sSkip.size()))
			break;
	}

	return std::string(itBegin, itEnd);
}

static std::vector<std::string> SplitArgs(const std::string & cmdLine, const std::string & sDiv)
{
	std::vector<std::string> result;
	if (cmdLine.empty())
		return result;

	std::string arg = cmdLine;
	for (; ;)
	{
		size_t iFind = arg.find(sDiv);
		if (0 <= iFind && iFind < arg.size())
		{
			result.push_back(std::string(arg.begin(), arg.begin() + iFind));
			arg = std::string(arg.begin() + iFind + sDiv.size(), arg.end());
		}
		else
			break;
	}

	result.push_back(arg);

	return result;
}

static bool _ExistsDir( const std::string & strPath )
{
	::WIN32_FIND_DATAA wfd;
	HANDLE hFind = FindFirstFileA( strPath.c_str(), &wfd );
	FindClose(hFind);

	return ( (hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) );
}

bool ExistsDir( const std::string & strPath )
{
	if ( strPath.empty() )
		return false;

	char ch = strPath[strPath.size() - 1];
	if ( '/' == ch || '\\' == ch ) 
		return _ExistsDir( strPath.substr( 0, strPath.size() - 1 ) );

	return _ExistsDir( strPath );
}

static void _ForceDir( const std::string & sDir )
{
	::CreateDirectoryA( sDir.c_str(), NULL );	
}

void ForceDir( const std::string & sDir )
{
	if ( sDir.empty() )
		return;

	std::string myDir;

	size_t startPos = 0;

	for ( ; startPos < sDir.size(); )
	{
		size_t pos = sDir.find_first_of( "/\\", startPos );
		if ( pos < sDir.size() )
		{
			myDir = sDir.substr( 0, pos );
			if ( !ExistsDir( myDir ) )
				_ForceDir( myDir );
			startPos = pos + 1;
		}
		else
			break;
	}

	if( !ExistsDir( sDir ) )
		_ForceDir( sDir );
}

std::string GetGameUserDir()
{
	std::string strDir;
	char tpath[MAX_PATH+1];

	const char *userDir = "%HOMEDRIVE%%HOMEPATH%/EQGame/";

	tpath[0] = 0;
	ExpandEnvironmentStringsA(userDir, tpath, MAX_PATH);

	strDir = tpath;

	ForceDir(strDir);

	return strDir;

}

CErrorReportDlg::CErrorReportDlg(const std::string & cmdLine)
	: CDialog(CErrorReportDlg::IDD, NULL)
{
	// ��ǰʱ��
	mDateTimeStr = GetDateTimeStr();
	// �����û���
	mComputer = ComputerInfo::GetMyComputerName() + "." + ComputerInfo::GetMyIP();

	// ��������
	//mReportServer = "http://localhost:2277/Upload/Default.aspx";
	//mReportServer = "http://10.35.60.41/Default.aspx";
	//mReportServer = "http://218.5.81.155/upload/Default.aspx";

	// ģ���ļ�
	mTemplateFile = "ErrorReportTemplate.txt";
	std::vector<std::string> arr = SplitArgs(cmdLine, "ErrorReportArg:");
	if (arr.size() >= 4)
	{
		// ȡ��������
		mErrorType = arr[1];
		// ȡdmp�ļ�
		mDmpFile = arr[2];
		// ȡ�˺�
		mAccount = arr[3];		
	}	
}

void CErrorReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, mEdit1);
}

BEGIN_MESSAGE_MAP(CErrorReportDlg, CDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CErrorReportDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CErrorReportDlg::OnBnClickedButton2)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


BOOL CErrorReportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();	
	
	return TRUE;  
}

void CErrorReportDlg::OnBnClickedButton1()
{
	OnCancel();
}

void CErrorReportDlg::LoadTemplateFile()
{
	std::string sText;
	int iLine = 0;
	// ��ȡ�ļ�����
	const int bufSize = 1024;
	char buf[bufSize];
	for (std::ifstream f(mTemplateFile.c_str()); !f.fail(); )
	{
		f.getline(buf, bufSize);
		++iLine;
		// ǰ������Ƿ�������ַ, ������֮������ʾģ��
		if (iLine == 2)
		{
			mReportServer = buf;
			mReportServer = trim(mReportServer);
			//::MessageBox(NULL, mReportServer.c_str(), "", 0);
		}
		else if (iLine > 3)
		{
			sText += buf;
			sText += "\r\n";
		}
	}

	sText += "  ����ԭ��: " + mErrorType;

	mEdit1.SetWindowText(sText.c_str());
}

void CErrorReportDlg::OnBnClickedButton2()
{
	// ��λ��ĳ���ļ�
	if (mDmpFile.size() > 0)
	{
		std::string cmd = "explorer.exe /n, /select, " + mDmpFile;
		RunExe(cmd, false);	
	}
}

bool CErrorReportDlg::CanUpload() const
{
	const std::string sHeader = "http://";
	
	std::string sServer = mReportServer;
	sServer.erase(0, sHeader.size());
	
	std::string sIp;
	int port = 0;
	size_t pos = sServer.find('/');
	if (pos < sServer.size())
	{
		size_t mid = sServer.find(':');
		if (mid < sServer.size())
		{
			sIp = std::string(sServer.begin(), sServer.begin() + mid);
			std::string sPort = std::string(sServer.begin() + mid + 1, sServer.begin() + pos);
			port = ::atoi(sPort.c_str());	
		}
		else
		{
			sIp = std::string(sServer.begin(), sServer.begin() + pos);
			port = 80; // Ĭ��80�˿�
		}
	}

	// �ȴ��������������ʱ��̫�٣����Ե���һ��
	int waitSeconds = 2;
	return TestConnect::Run(sIp.c_str(), port, waitSeconds);
}

void CErrorReportDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	this->BringWindowToTop();

	// ����ģ���ı�
	LoadTemplateFile();	

	// ����������Ϣ
	std::string sComputerFile = GetGameUserDir() + "ComputerInfo.txt";

	ComputerInfo::SaveToFile(sComputerFile);

	// �жϿɷ��ϴ�����ֹ��ʱ��ȴ�
	if (CanUpload())
	{
		// �ϴ��������Ϣ
		UploadFile(sComputerFile);

		// �ϴ�dmp�ļ�
		UploadFile(mDmpFile);

		// �ϴ�OgreLog
		UploadFile(GetGameUserDir() + "EQClient.Ogre.log");
		UploadFile(GetGameUserDir() + "EQFarm.Ogre.log");

		// �ϴ�CeguiLog
		UploadFile(GetGameUserDir() + "EQClient.CEGUI.log");
		UploadFile(GetGameUserDir() + "EQFarm.CEGUI.log");

		// �ϴ�/Log/EQClientMMDD.log
		UploadFile(GetGameUserDir() + "Log/EQClient" + GetMonthDayStr() + ".log");
		// �ϴ�/Log/EQFarmMMDD.log
		UploadFile(GetGameUserDir() + "Log/EQFarm" + GetMonthDayStr() + ".log");

		// �ϴ�EQUpdaterI.log
		UploadFile("EQUpdaterI.log");	
	}
	else
	{
		CString sText;
		
		mEdit1.GetWindowTextA(sText);
		sText += "\r\n";
		sText += "  �Զ�����ʧ�ܣ�\r\n";
		sText += "    �޷����ӱ��������\r\n";
		sText += "    ";
		sText += mReportServer.c_str();

		mEdit1.SetWindowTextA(sText);
	}
}

// �ϴ��ļ�
void CErrorReportDlg::UploadFile(const std::string & sFile)
{
	if (sFile.empty())
		return;

	size_t pos = sFile.find_last_of(":/\\");
	std::string targetFile = sFile;
	if (0 <= pos && pos < sFile.size())
		targetFile = std::string(sFile.begin() + pos + 1, sFile.end());

	//MessageBoxA(sFile.c_str(), targetFile.c_str());

	std::string sUser = mAccount.empty() ? mComputer : mAccount;
	// Ŀ���ļ���(��������)
	std::string destFile = FormatStr("%s__%s__%s", 
									sUser.c_str(),     // �û�
									mDateTimeStr.c_str(),  // ʱ��	
									targetFile.c_str());

	std::string cmd = FormatStr("curl.exe -F \"action=upload\" "
		"-F \"file=@%s;"
		"filename=%s;type=application/octet-stream\" %s", 
		sFile.c_str(), 
		destFile.c_str(), 
		mReportServer.c_str());

	RunExe(cmd, true);
}
