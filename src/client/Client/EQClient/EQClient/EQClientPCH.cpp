// stdafx.cpp : source file that includes just the standard includes
// EQClient.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "EQClientPCH.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#define MAX_CHAR 256

void DebugMessage(const char *msg,...)
{		
//#ifdef _DEBUG
	va_list va;
	static char str[MAX_CHAR];
	static char buf[MAX_CHAR];
	memset(str, 0, MAX_CHAR);
	memset(buf, 0, MAX_CHAR);

	va_start(va,msg);
	vsprintf_s(str,msg,va);
	va_end(va);

	sprintf_s(buf, "%s\n", str);
	printf(buf, "%s\n", str);
	//	OutputDebugStringA( buf );
	// 	if( g_LogFile != NULL )
	// 		fwrite(buf , 1 , strlen(buf) , g_LogFile);
//#endif
}