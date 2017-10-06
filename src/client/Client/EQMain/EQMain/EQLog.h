/******************************************************************************
* 作者: HJX
* 时间: 2009-10-26
* 描述: EQ项目的Log
******************************************************************************/

#ifndef EQLog_H
#define EQLog_H
#include "EQMainPrerequisites.h"
#include "EQConsole.h"



class EQLog
{
public:	
	/// log类型
	enum LogType 
	{ 
		LOG_TYPE_ALL,		// 所有
		LOG_TYPE_DEBUG,		// 调试
		LOG_TYPE_MESSAGE,	// 信息
		LOG_TYPE_WARNING,	// 警告
		LOG_TYPE_ERROR,		// 错误
		LOG_TYPE_TRACE,		// 跟踪
	};

	/// log模式
	enum LogMode
	{
		LOG_MODE_NONE    = 0,	// 不记录到任何地方
		LOG_MODE_FILE    = 1,	// 写到Log文件
		LOG_MODE_CONSOLE = 2,	// 写到控制台
		LOG_MODE_BOTH    = 3,	// 两者都写
	};
public:
	virtual ~EQLog();
public:
	EQLog();
public:
	/** writeLog
		@param logType 参考 enum LogType
	*/
	virtual void writeLog( const std::string & msg, int logType );
public:
	void logDebug( const std::string & msg );
public:
	void logMessage( const std::string & msg );
public:
	void logWarning( const std::string & msg );
public:
	void logError( const std::string & msg );
public:
	/** logTrace 只用在调试跟踪一些数据, 用完,请将logTrace代码删掉		
	*/
	void logTrace( const std::string & msg );
public:
	/** 设置Log模式
		注： setLogMode( LOG_MODE_NONE ) 将不再记录任何Log, 控制台也会被关闭
	*/
	void setLogMode( int mode );
public:
	void setLogLevel( int lvl );
public:
	/** 取得Log模式
		@remark 返回值参考 enum LogMode
	*/
	int getLogMode() const;
public:
	/** 取得Log等级
		@remark 返回值参考 enum LogType
	*/
	int getLogLevel() const;
public:	
	void setLogFile( const std::string & strFile );
public:
	EQConsole & getConsole();
public:
	void flush();
protected:
	/// 取得log类型的对应颜色
	int getColor( int logType );
protected:
	const char * getPrefix( int logType );
protected:
	std::string getDefaultLogFile();
protected:
	int mLogMode;			// 指定log写到控制台或者文本文件
	int mLogLevel;			// 高于mLogLevel的信息才可以显示到控制台
	int mLastDay;			// 最后一次写Log日期，用来判断是否该换log文件了
	std::ofstream mFile;
	std::string mFileName;	
	boost::recursive_mutex mLogLock;
};


class EQLogManager
{
public:
	static EQLog& getLogger();
};


#define LOG				EQLogManager::getLogger().logMessage
#define LOGDBG			EQLogManager::getLogger().logDebug
#define LOGMSG			EQLogManager::getLogger().logMessage
#define LOGWAR			EQLogManager::getLogger().logWarning
#define LOGERR			EQLogManager::getLogger().logError
//没有LOGTRA

#endif

