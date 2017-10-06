#include "DBWindow.h"

#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

#define CONSOLE_TILE "Console"
#define MAX_BUF_LEN 4096

ConsoleWindow::ConsoleWindow()
{
	
}

ConsoleWindow::~ConsoleWindow()
{
	
}

BOOL ConsoleWindow::Init()
{
	m_hConsole=NULL;
	m_bCreate=FALSE;
	if(AllocConsole())
	{
		m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitleA(CONSOLE_TILE);
		SetConsoleMode(m_hConsole,ENABLE_PROCESSED_OUTPUT);
		m_bCreate=TRUE;
	}
	else{
		m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
		if(m_hConsole==INVALID_HANDLE_VALUE)
			m_hConsole=NULL;
	}
	if(m_hConsole)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;  
		if(GetConsoleScreenBufferInfo(m_hConsole, &csbiInfo))
		{
			m_bAttrs=TRUE;
			m_OldColorAttrs = csbiInfo.wAttributes;   
		}
		else{
			m_bAttrs=FALSE;
			m_OldColorAttrs = 0;
		}
	}

	return m_bCreate;
}

void ConsoleWindow::Dispose()
{
	if(m_bCreate)
		FreeConsole();
}

BOOL ConsoleWindow::SetTile(char * lpTile)
{
	return SetConsoleTitle(lpTile);
}

BOOL ConsoleWindow::WriteString(char * lpString, ...)
{
	BOOL ret = FALSE;
	if(m_hConsole)
	{
		static char message[MAX_BUF_LEN];
		va_list cur_arg;
		va_start(cur_arg,lpString);
		_vsnprintf(message,MAX_BUF_LEN,lpString,cur_arg);
		va_end(cur_arg);

		ret = WriteConsoleA(m_hConsole,message,strlen(message),NULL,NULL);
	}
	return ret;
}

BOOL ConsoleWindow::WriteString(WORD Attrs,char * lpString, ...)
{
	BOOL ret = FALSE;

	if(m_hConsole)
	{
		if(m_bAttrs)
			SetConsoleTextAttribute(m_hConsole,Attrs);

		static char message[MAX_BUF_LEN];
		va_list cur_arg;
		va_start(cur_arg,lpString);
		_vsnprintf(message,MAX_BUF_LEN,lpString,cur_arg);
		va_end(cur_arg);
		ret = WriteConsoleA(m_hConsole,message,strlen(message),NULL,NULL);

		if(m_bAttrs)
			SetConsoleTextAttribute(m_hConsole,m_OldColorAttrs);
	}
	return ret;  
}
