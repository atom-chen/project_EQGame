/******************************************************************************
* ����: HJX
* ʱ��: 2009-10-26
* ����: EQ��Ŀ��Log
******************************************************************************/

#ifndef EQLog_H
#define EQLog_H
#include "EQMainPrerequisites.h"
#include "EQConsole.h"



class EQLog
{
public:	
	/// log����
	enum LogType 
	{ 
		LOG_TYPE_ALL,		// ����
		LOG_TYPE_DEBUG,		// ����
		LOG_TYPE_MESSAGE,	// ��Ϣ
		LOG_TYPE_WARNING,	// ����
		LOG_TYPE_ERROR,		// ����
		LOG_TYPE_TRACE,		// ����
	};

	/// logģʽ
	enum LogMode
	{
		LOG_MODE_NONE    = 0,	// ����¼���κεط�
		LOG_MODE_FILE    = 1,	// д��Log�ļ�
		LOG_MODE_CONSOLE = 2,	// д������̨
		LOG_MODE_BOTH    = 3,	// ���߶�д
	};
public:
	virtual ~EQLog();
public:
	EQLog();
public:
	/** writeLog
		@param logType �ο� enum LogType
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
	/** logTrace ֻ���ڵ��Ը���һЩ����, ����,�뽫logTrace����ɾ��		
	*/
	void logTrace( const std::string & msg );
public:
	/** ����Logģʽ
		ע�� setLogMode( LOG_MODE_NONE ) �����ټ�¼�κ�Log, ����̨Ҳ�ᱻ�ر�
	*/
	void setLogMode( int mode );
public:
	void setLogLevel( int lvl );
public:
	/** ȡ��Logģʽ
		@remark ����ֵ�ο� enum LogMode
	*/
	int getLogMode() const;
public:
	/** ȡ��Log�ȼ�
		@remark ����ֵ�ο� enum LogType
	*/
	int getLogLevel() const;
public:	
	void setLogFile( const std::string & strFile );
public:
	EQConsole & getConsole();
public:
	void flush();
protected:
	/// ȡ��log���͵Ķ�Ӧ��ɫ
	int getColor( int logType );
protected:
	const char * getPrefix( int logType );
protected:
	std::string getDefaultLogFile();
protected:
	int mLogMode;			// ָ��logд������̨�����ı��ļ�
	int mLogLevel;			// ����mLogLevel����Ϣ�ſ�����ʾ������̨
	int mLastDay;			// ���һ��дLog���ڣ������ж��Ƿ�û�log�ļ���
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
//û��LOGTRA

#endif

