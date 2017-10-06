/******************************************************************************
* ����: HJX
* ʱ��: 2009-10-26
* ����: EQ����̨
******************************************************************************/

#ifndef EQConsole_H
#define EQConsole_H
#include "EQMainPrerequisites.h"


/// EQConsole ʵ��Ϊ����
class EQConsole 
{
private:
	~EQConsole();
private:
	EQConsole( const std::string & consoleName = "EQ����̨" );
public:
	static EQConsole & getSingleton();
public:
	void write( const std::string & msg, WORD color = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE );
public:
	void writeLine( const std::string & msg, WORD color = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE );
public:
	bool open();
public:
	void close();
public:
	void show();
public:
	void hide();
private:
	static BOOL WINAPI MyCtrlHandler( DWORD dwCtrlType );
private:
	HANDLE mHandle;
	std::string mName;
};




#endif

