#include "EQMainPCH.h"
#include "EQConsole.h"
#include "CommFunc.h"

//--------------------------------------------------------------------------
EQConsole::~EQConsole()
{
	close();
}
//--------------------------------------------------------------------------
EQConsole::EQConsole( const std::string & consoleName )
	: mHandle( NULL )
	, mName( consoleName )
{
	//open();
}
//--------------------------------------------------------------------------
BOOL WINAPI EQConsole::MyCtrlHandler( DWORD dwCtrlType )
{
	// 这里不处理传过来的消息
	return TRUE;
}
//--------------------------------------------------------------------------
EQConsole & EQConsole::getSingleton()
{
	static EQConsole s_val;
	return s_val;
}
//--------------------------------------------------------------------------
void EQConsole::write( const std::string & msg, WORD color )
{
	if ( !mHandle )
		return;
    SetConsoleTextAttribute( mHandle, color );
	DWORD lenWrite = 0;
	WriteConsoleA( mHandle, msg.c_str(), msg.size(), &lenWrite, 0 );
	assert( msg.size() == lenWrite );
	SetConsoleTextAttribute( mHandle, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE );
}
//--------------------------------------------------------------------------
void EQConsole::writeLine( const std::string & msg, WORD color )
{
	write( msg, color );
	write( NewLineStr(), color );
}
//--------------------------------------------------------------------------
bool EQConsole::open()
{
	if ( mHandle )
		return true;

	AllocConsole( );

	SetConsoleTitleA( mName.c_str() );

	mHandle = GetStdHandle( STD_OUTPUT_HANDLE );

	if ( !mHandle )
		return false;

	COORD cr;
	cr.X = 80;
	cr.Y = 2000;
	::SetConsoleScreenBufferSize( mHandle, cr );

	BOOL bRet = ::SetConsoleCtrlHandler( &MyCtrlHandler, TRUE );	

	return ( FALSE != bRet );
}
//--------------------------------------------------------------------------
void EQConsole::close()
{
	if ( mHandle )
	{
		FreeConsole();
		mHandle = NULL;
	}
}
//--------------------------------------------------------------------------
void EQConsole::show()
{
	::HWND hWin = ::GetConsoleWindow();
	if ( hWin )
		::ShowWindow( hWin, SW_SHOW );
}
//--------------------------------------------------------------------------
void EQConsole::hide()
{
	::HWND hWin = ::GetConsoleWindow();
	if ( hWin )
		::ShowWindow( hWin, SW_HIDE );
}
//--------------------------------------------------------------------------
