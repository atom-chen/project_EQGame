#include "EQMainPCH.h"
#include "EQLog.h"
#include "EQConsole.h"
#include "CommFunc.h"


//--------------------------------------------------------------------------
EQLog::~EQLog()
{
}
//--------------------------------------------------------------------------
EQLog::EQLog()
	: mLogMode( LOG_MODE_BOTH )
	, mLogLevel( LOG_TYPE_ALL )
	, mLastDay( -1 )
{
	setLogMode( mLogMode );
}
//--------------------------------------------------------------------------
void EQLog::writeLog( const std::string & msg, int logType )
{
	::boost::recursive_mutex::scoped_lock autoLocker( mLogLock );

	SYSTEMTIME st;
	::GetLocalTime( &st );

	const std::string sInfo = FormatStr( "%02d:%02d:%02d.%03d%s: %s", 
								st.wHour, 
								st.wMinute, 
								st.wSecond, 
								st.wMilliseconds,
								getPrefix( logType ),
								msg.c_str() );

	// 写到控制台
	if ( BitContains( mLogMode, LOG_MODE_CONSOLE ) )
	{
		// 高于mLogLevel的信息才可以显示到控制台
		if ( logType >= mLogLevel )
			EQConsole::getSingleton().writeLine( sInfo, getColor( logType ) );
	}

	// 写到文件中
	if ( BitContains( mLogMode, LOG_MODE_FILE ) )
	{
		if ( !mFile.is_open() /*mLastDay != st.wDay*/ )
		{			
			setLogFile( getDefaultLogFile() );
		}
		mFile.write( sInfo.c_str(), sInfo.size() );
		mFile.write( NewLineStr().c_str(), NewLineStr().size() );
		//mFile.flush();
	}
}
//--------------------------------------------------------------------------
void EQLog::logDebug( const std::string & msg )
{
	writeLog( msg, LOG_TYPE_DEBUG );
}
//--------------------------------------------------------------------------
void EQLog::logMessage( const std::string & msg )
{
	writeLog( msg, LOG_TYPE_MESSAGE );
}
//--------------------------------------------------------------------------
void EQLog::logWarning( const std::string & msg )
{
	writeLog( msg, LOG_TYPE_WARNING );
}
//--------------------------------------------------------------------------
void EQLog::logError( const std::string & msg )
{
	writeLog( msg, LOG_TYPE_ERROR );
}
//--------------------------------------------------------------------------
void EQLog::logTrace( const std::string & msg )
{
	writeLog( msg, LOG_TYPE_TRACE );
}
//--------------------------------------------------------------------------
void EQLog::flush()
{
	if (!mFile)
		return;

	mFile.flush();
}
//--------------------------------------------------------------------------
int EQLog::getColor( int logType )
{
	int color = 0;

	switch( logType )
	{
	case LOG_TYPE_DEBUG:
		color = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
		break;
	case LOG_TYPE_MESSAGE:
		color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case LOG_TYPE_WARNING:
		color = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case LOG_TYPE_ERROR:
		color = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case LOG_TYPE_TRACE:
		color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_GREEN;
		break;
	default: 
		color = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
		break;
	}

	return color;
}
//--------------------------------------------------------------------------
const char * EQLog::getPrefix( int logType )
{
	switch( logType )
	{
	case LOG_TYPE_DEBUG:
		return "DBG";
	case LOG_TYPE_MESSAGE:
		return "MSG";
	case LOG_TYPE_WARNING:
		return "WAR";
	case LOG_TYPE_ERROR:
		return "ERR";
	case LOG_TYPE_TRACE:
		return "TRA";
	default: 
		return "";
	}	
}
//--------------------------------------------------------------------------
void EQLog::setLogMode( int mode )
{
	::boost::recursive_mutex::scoped_lock autoLocker( mLogLock );
	mLogMode = mode;
	if ( !BitContains( mLogMode, LOG_MODE_CONSOLE ) )
	{
		getConsole().close();
	}
}
//--------------------------------------------------------------------------
void EQLog::setLogLevel( int lvl )
{
	::boost::recursive_mutex::scoped_lock autoLocker( mLogLock );
	mLogLevel = lvl;
}
//--------------------------------------------------------------------------
int EQLog::getLogMode() const
{
	return mLogMode;
}
//--------------------------------------------------------------------------
int EQLog::getLogLevel() const
{
	return mLogLevel;
}
//--------------------------------------------------------------------------
void EQLog::setLogFile( const std::string & strFile )
{
	::boost::recursive_mutex::scoped_lock autoLocker( mLogLock );

	//if ( strFile == mFileName )
	//	return;

	ForceDir( GetDir( strFile ) );

	std::locale loc = std::locale::global(std::locale("")); //要打开的文件路径含中文，设置全局locale为本地环境
	mFile.open( strFile.c_str(), std::ios::app );
	std::locale::global(loc);//恢复全局locale

	mFileName = strFile;
}
//--------------------------------------------------------------------------
EQConsole & EQLog::getConsole()
{
	return EQConsole::getSingleton();
}
//--------------------------------------------------------------------------
std::string EQLog::getDefaultLogFile()
{
	const std::string strDir = GetGameUserDir() + "Log/";

	// 组织日期格式
	SYSTEMTIME st;
	::GetLocalTime( &st );
	//st.wYear = st.wYear % 100;
	std::string strDateTime = FormatStr( "%02d%02d", st.wMonth, st.wDay );
	
	return strDir + GetModuleName() + strDateTime + ".Log";
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
EQLog& EQLogManager::getLogger()
{
	static EQLog s_val;
	return s_val;
}
//--------------------------------------------------------------------------
