#ifndef _DBWINDOW_
#define _DBWINDOW_

#include <windows.h>

#define WDS_T_RED FOREGROUND_RED
#define WDS_T_GREEN FOREGROUND_GREEN
#define WDS_T_BLUE FOREGROUND_BLUE

#define WDS_BG_RED BACKGROUND_RED
#define WDS_BG_GREEN BACKGROUND_GREEN
#define WDS_BG_BLUE BACKGROUND_BLUE

class ConsoleWindow  
{
public:
	ConsoleWindow();
	virtual ~ConsoleWindow();

	BOOL Init();
	void Dispose();
	BOOL SetTile(char * lpTile);
	BOOL WriteString(char * lpString, ...);
	BOOL WriteString(WORD Attrs,char * lpString, ...);
private:
	HANDLE m_hConsole;
	BOOL m_bCreate;
	BOOL m_bAttrs;
	WORD m_OldColorAttrs;  
};

#endif