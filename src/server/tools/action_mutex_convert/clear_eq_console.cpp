#include <iostream>
#include <windows.h>
#include <string>
#include <set>
using namespace std;

BOOL CALLBACK myFun(HWND hWin, LPARAM param)
{
	const char * winName = reinterpret_cast<const char *>(param);
	const int bufCount = 10240;
	char buf[bufCount] = {0};
	::GetWindowTextA(hWin, buf, bufCount);
	std::string s = buf;
	if ( s == std::string(winName) )
	{
		::ShowWindow(hWin, SW_HIDE);
	}
	return 1;
}


void KillWindow(const char * winName)
{
	::EnumWindows(myFun, reinterpret_cast<LPARAM>(winName));
}


void main()
{
	KillWindow("EQ¿ØÖÆÌ¨");
}