// EQUpdaterProxy.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "EQUpdaterProxy.h"
#include "curl.h"
#include "tinyxml.h"
#include "xpath_static.h"
#include "XIniFile.h"
#include "XFuns.h"
#include <process.h>
#include <string>
#include <hash_map>

#define CLOSE_BOOTWIN_MSG (WM_USER + 1)

#define ELEMENT_LIST "List"
#define ELEMENT_GROUP "Group"
#define ELEMENT_ITEM "Item"
#define ATTRIBUTE_DIRECTORY "Directory"
#define ATTRIBUTE_TYPE "Type"
#define ATTRIBUTE_FILE "File"
#define ATTRIBUTE_CODE "Code"
#define ATTRIBUTE_PATH "Path"

#define CONFIG_FILE "UpdateCfg.ini"
#define CONFIG_FILE2 "../Config/UpdateCfg.ini"
#define CONFIG_VARS "VARS"

HINSTANCE hInst;
HBITMAP hBmpBg;
BITMAP bmpInfo;
HFONT hFont;
HWND hWndBoot;
HANDLE hThread;
unsigned int nThreadId;
unsigned int listFileSize = 0, loadedSize = 0, loadCurPos = 0;
FILE *listFileObj;
bool loadListFileResult = true;
DWORD updateTimeStart;
bool isToUpdate = false;
RECT rcLoadState = {410, 246, 410 + 30, 246 + 14};
char *cmdParam = NULL, *cmdUser = "", *cmdPwd = "";

std::string outText = "���ڳ�ʼ��...";
std::string listFilePath = "%HOMEDRIVE%%HOMEPATH%/";
std::string listFileName = "List.xml";
std::string serverAddr = "http://218.5.81.155/client_update/";

const char *checkExes[] =
{
	"EQClient.exe",
	"EQClient_dr.exe",
	"EQFarm.exe",
	"EQFarm_dr.exe"
};

const char *checkUpdateExe[] =
{
	"EQUpdaterI.exe"
};


INT_PTR CALLBACK BootWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL RunExe(const char *file, const char *dir, const char *vars);
unsigned int __stdcall LoadListPro(void *pv);
bool CheckIsToUpdate();
void LoadConfig();
void RunEQGame(const char *param, const char *user, const char *pwd);
void SplitString(const char *str, std::vector<std::string> &strArry, char spc);


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//��ȡ��������
	std::vector<std::string> args;
	XFuns::SplitString(lpCmdLine, args, ' ');
	if(args.size() >= 3)
	{
		cmdParam = (char *)args[0].c_str();
		cmdUser = (char *)args[1].c_str();
		cmdPwd = (char *)args[2].c_str();
	}

	//����Ϸ������������ֱ�Ӵ���Ϸ��
	if(XFuns::CheckHasExeRun(checkExes, sizeof(checkExes)/sizeof(const char *)))
	{
		RunEQGame(cmdParam, cmdUser, cmdPwd);
		return 0;
	}
	//���ڸ��³���
	HANDLE hMutex = CreateMutexA(NULL,TRUE,"5FC49517-F671-4053-87C2-EF8B67864746");
	if(GetLastError() == ERROR_ALREADY_EXISTS || XFuns::CheckHasExeRun(checkUpdateExe, sizeof(checkUpdateExe)/sizeof(const char *)))
	{
		MessageBox(NULL, "��Ϸ���ڸ����У�����ɸ��º���������", "��Ϣ", MB_OK|MB_ICONINFORMATION);
		return 0;
	}

	//��ʹ������
	hInst = hInstance;
	hBmpBg = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
	GetObject(hBmpBg,sizeof(BITMAP) , &bmpInfo);
	hFont = CreateFont(12, 0, 0, 0, 400, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "����");

	//�������ò���
	LoadConfig();

	//��������������Ƿ���Ҫ���¼��
	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, BootWndProc);

	DeleteObject(hBmpBg);
	DeleteObject(hFont);
	CloseHandle(hThread);

	if(false == loadListFileResult)
	{
		MessageBox(NULL, "��ʹ��ʧ�ܣ��������磡", "����", MB_OK|MB_ICONERROR);
		return 0;
	}

	//û��Ҫ���µ�
	if(false == isToUpdate)
	{
		RunEQGame(cmdParam, cmdUser, cmdPwd);
		return 0;
	}

	//���и��½���
	XFuns::RunExe("EQUpdaterI.exe", NULL, lpCmdLine);

	return 0;
}

INT_PTR CALLBACK BootWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
			hWndBoot = hDlg;

			int x = GetSystemMetrics(SM_CXSCREEN);
			int y = GetSystemMetrics(SM_CYSCREEN);

			MoveWindow(hDlg, (x - bmpInfo.bmWidth)/2, (y - bmpInfo.bmHeight)/2, bmpInfo.bmWidth, bmpInfo.bmHeight, TRUE);

			hThread = (HANDLE)_beginthreadex(NULL, 0, LoadListPro, NULL, 0, &nThreadId);
		}
		return (INT_PTR)TRUE;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hDlg, &ps);

			HDC hdcBmp = CreateCompatibleDC(hdc);
			SelectObject(hdcBmp, hBmpBg);
			BitBlt(hdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, hdcBmp, 0, 0, SRCCOPY);
			DeleteDC(hdcBmp);

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(33,132,202));
			SelectObject(hdc, hFont);
			TextOut(hdc, 6, 246, outText.c_str(), outText.length());


			char txt[32];
			sprintf(txt, "%d%%", listFileSize ? ((loadedSize + loadCurPos) * 100) / listFileSize : 0);
			DrawText(hdc, txt, strlen(txt), &rcLoadState, DT_CENTER|DT_VCENTER);

			EndPaint(hDlg, &ps);
		}
		break;

	case CLOSE_BOOTWIN_MSG:
		EndDialog(hDlg, wParam);
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}

size_t CURLDownLoadCallBack( void *buffer, size_t size, size_t nmemb, void *userp )
{

	return fwrite(buffer, size, nmemb, (FILE *)userp);
}

int CURLProgressCallBack(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	unsigned int total = (unsigned int)dltotal, dcur = (unsigned int)dlnow;

	if(0 == total && 0 == dcur)
		return 0;

	if(0 == listFileSize)
		listFileSize = total;

	loadCurPos = dcur;

	if(GetTickCount() - updateTimeStart > 100)
	{
		updateTimeStart = GetTickCount();
		InvalidateRect(hWndBoot, &rcLoadState, 1);
	}

	return 0;
}

unsigned int __stdcall LoadListPro(void *pv)
{
	char buf[64];
	std::string furl, fpath;
	CURLcode rtn;
	CURL *urlObj;

	//
	curl_global_init(CURL_GLOBAL_WIN32);

	urlObj = curl_easy_init();
	if (NULL == urlObj)
	{
		MessageBox(hWndBoot, "curl init error!", "ERROR", MB_OK);
		goto EXIT_LOAD_LIST_FILE_THREAD;
	}

	furl = serverAddr + listFileName;
	fpath = listFilePath + listFileName;

	XFuns::ExpandEnvVars(fpath);
	listFileObj = fopen(fpath.c_str(), "wb");
	if (NULL == listFileObj)
	{
		MessageBox(hWndBoot, "open list file error!", "ERROR", MB_OK);
		goto EXIT_LOAD_LIST_FILE_THREAD;
	}

	curl_easy_setopt( urlObj, CURLOPT_URL, furl.c_str() ); 
	curl_easy_setopt( urlObj, CURLOPT_WRITEDATA, (void *)listFileObj ); 
	curl_easy_setopt( urlObj, CURLOPT_WRITEFUNCTION, CURLDownLoadCallBack );//���ñ����������ݻص�����
	curl_easy_setopt( urlObj, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt( urlObj, CURLOPT_PROGRESSFUNCTION, CURLProgressCallBack );//���ý��Ȼ�ȡ�ص�����
	curl_easy_setopt( urlObj, CURLOPT_CONNECTTIMEOUT, 20L);//����20�����ӳ�ʱ
	curl_easy_setopt( urlObj, CURLOPT_TIMEOUT, 20L);//����20�����س�ʱ
	curl_easy_setopt( urlObj, CURLOPT_FORBID_REUSE, 1);//�ر��������ӻ��ƣ���ֹ����CLOSE_WAIT��TCP����

	rtn = curl_easy_perform( urlObj );//����
	updateTimeStart = GetTickCount();

	//����Ƿ�������أ�������;ʧ�ܣ��������������ʣ�ಿ�֡�
	while(CURLE_OK != rtn)
	{
		bool tryAgain = true;
		std::string errMsg;

		switch(rtn)
		{
		case CURLE_RECV_ERROR:
			errMsg.append("�����ļ�ʱ��������");
			break;
		case CURLE_COULDNT_CONNECT:
			errMsg.append("������ʱ�޷�����");
			break;
		case CURLE_OPERATION_TIMEDOUT:
			errMsg.append("�����ļ����ӳ�ʱ");
			break;
		default:
			tryAgain = false;
			break;
		}

		if(tryAgain)
		{
			errMsg.append("���Ƿ����ԣ�");
			sprintf(buf, "���� %d", rtn);
			if(IDNO == MessageBox(hWndBoot, errMsg.c_str(), buf, MB_YESNO|MB_ICONERROR))
			{
				tryAgain = false;
			}
		}

		if(false == tryAgain)
			break;

		Sleep(3000);

		loadedSize += loadCurPos;
		sprintf(buf, "%d-%d", loadedSize, listFileSize);
		curl_easy_setopt( urlObj, CURLOPT_RANGE, buf);

		rtn = curl_easy_perform( urlObj );
		updateTimeStart = GetTickCount();
	}

	//�жϻ�ȡ��Ӧ��http��ַ�Ƿ����,�������򷵻�200,206Ϊֻ�²���,400������Ϊ������,һ�㲻����Ϊ404����
	int retcode = 0;   
	CURLcode rtng = curl_easy_getinfo(urlObj, CURLINFO_RESPONSE_CODE , &retcode);     
	if(CURLE_OK != rtn || !(CURLE_OK == rtng && (200 == retcode || 206 == retcode)))   
		loadListFileResult = false;

	InvalidateRect(hWndBoot, &rcLoadState, 1);

EXIT_LOAD_LIST_FILE_THREAD:

	if(urlObj)
	{
		curl_easy_cleanup( urlObj );
		urlObj = NULL;
	}

	if(listFileObj)
	{
		fclose(listFileObj);
		listFileObj = NULL;
	}

	//updateTimeStart = GetTickCount();
	isToUpdate = CheckIsToUpdate();//����Ƿ���Ҫ����
	//DWORD itv = GetTickCount() - updateTimeStart;

	PostMessage(hWndBoot, CLOSE_BOOTWIN_MSG, 0, 0);

	return 0U;
}

typedef stdext::hash_map<std::string, int> FINDLIST;
void LoadFindList(FINDLIST &flist, TiXmlDocument &xmlDoc)
{
	std::string fkey;
	fkey.reserve(512);

	TiXmlElement *eleList = xmlDoc.RootElement();
	if(eleList)
	{
		TiXmlElement *eleGroup = eleList->FirstChildElement();
		while(eleGroup)
		{
			TiXmlElement *eleItem = eleGroup->FirstChildElement();
			while(eleItem)
			{
				fkey.clear();
				fkey.append(eleGroup->Attribute(ATTRIBUTE_DIRECTORY));
				fkey.append(" ");
				fkey.append(eleItem->Attribute(ATTRIBUTE_FILE));
				fkey.append(" ");
				fkey.append(eleItem->Attribute(ATTRIBUTE_CODE));

				flist.insert(std::make_pair(fkey, 0));

				eleItem = eleItem->NextSiblingElement();
			}

			eleGroup = eleGroup->NextSiblingElement();
		}
	}
}

//��XPath�ٶȹ���
//#define USE_XPATH

bool CheckIsToUpdate()
{
	std::string curListFile = listFileName;
	std::string downListFIle = listFilePath + listFileName;

	XFuns::ExpandEnvVars(downListFIle);

	//�����ļ�md5��ͬ����������û��Ҫ���µ��ļ����п���ֻ��˳�򲻶Ե���md5��ͬ��
	if(XFuns::Check2FileMd5(downListFIle.c_str(), curListFile.c_str()))
		return false;

	TiXmlDocument xmlDocDown, xmlDocCur;

	if(false == xmlDocDown.LoadFile(downListFIle.c_str()) || false == xmlDocCur.LoadFile(curListFile.c_str()))
		return true;

#ifdef USE_XPATH
	char fstr[MAX_PATH];
#else
	FINDLIST flist;
	LoadFindList(flist, xmlDocCur);

	std::string fkey;
	fkey.reserve(512);
#endif

	TiXmlElement *eleList = xmlDocDown.RootElement();
	if(eleList)
	{
		TiXmlElement *eleGroup = eleList->FirstChildElement();
		while(eleGroup)
		{
			TiXmlElement *eleItem = eleGroup->FirstChildElement();
			while(eleItem)
			{
#ifdef USE_XPATH
				sprintf(fstr, "/%s/%s[@%s='%s']/%s[@%s='%s' and @%s='%s']", ELEMENT_LIST, ELEMENT_GROUP, ATTRIBUTE_DIRECTORY, eleGroup->Attribute(ATTRIBUTE_DIRECTORY), ELEMENT_ITEM, ATTRIBUTE_FILE, eleItem->Attribute(ATTRIBUTE_FILE), ATTRIBUTE_CODE, eleItem->Attribute(ATTRIBUTE_CODE));
				if(false == TinyXPath::o_xpath_bool(xmlDocCur.RootElement(), fstr))
					return true;
#else
				fkey.clear();
				fkey.append(eleGroup->Attribute(ATTRIBUTE_DIRECTORY));
				fkey.append(" ");
				fkey.append(eleItem->Attribute(ATTRIBUTE_FILE));
				fkey.append(" ");
				fkey.append(eleItem->Attribute(ATTRIBUTE_CODE));

				if(flist.end() == flist.find(fkey))
					return true;
#endif

				eleItem = eleItem->NextSiblingElement();
			}

			eleGroup = eleGroup->NextSiblingElement();
		}
	}

	return false;
}

void LoadConfig()
{
	XIniFile iniFObj(CONFIG_FILE);
	if(false == iniFObj.ReadFile())
	{
		iniFObj.SetPath(CONFIG_FILE2);
		if (false == iniFObj.ReadFile())
			return;
	}

	serverAddr = iniFObj.GetValue(CONFIG_VARS, "SAddr");
	listFileName = iniFObj.GetValue(CONFIG_VARS, "FList");

	XFuns::ReplaceChars(listFileName, "\\", "/");

	int p = listFileName.find_last_of('/');
	if(-1 != p)
		listFileName.erase(0, p + 1);

}

void RunEQGame(const char *param, const char *user, const char *pwd)
{
	std::string eq = "EQClient.exe";
	std::string farm = "EQFarm.exe";
	std::string vars = "";
	std::string exe = "";

	if(param)
	{
		vars.append(" -FuckingAutoRun -u ");
		vars.append(user);
		vars.append(" ");
		vars.append(pwd);

		if(0 == strcmp(param, "-eq"))
			exe = eq;
		else//"-farm"
			exe = farm;
	}
	else
	{
		vars = " -FuckingAutoRun";
		exe = eq;
	}

	XFuns::RunExe(exe.c_str(), NULL, vars.c_str(), true);
}
