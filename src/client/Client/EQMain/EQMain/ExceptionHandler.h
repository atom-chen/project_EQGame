/******************************************************************************
* 作者: HJX
* 时间: 2009-10-21
* 描述: 设置异常回调函数，生成异常报告(小内存转储)
* 使用方法: 1. 第一种方法: 
				在main开头 或 应用程序初始化时调用下面语句即可
           		ExceptionHandler::startHandleException(); 
			2. 第二种方法:
				调用__try     __except
******************************************************************************/

#ifndef ExceptionHandler_H
#define ExceptionHandler_H
#include "EQMainPrerequisites.h"





class ExceptionHandler
{
public:
	typedef void ( *TProc )();
public:
	virtual ~ExceptionHandler();
	
	/** 构造函数 
	@param exceptionHandler: 回调函数, 类型为PTOP_LEVEL_EXCEPTION_FILTER
	*/
	explicit ExceptionHandler( void * exceptionHandler = &ExceptionHandler::defaultExceptionFilter );
public:
	/** 设置处理异常后的行为
	@param val 可以为下面值
		EXCEPTION_EXECUTE_HANDLER       1 (默认值)
		EXCEPTION_CONTINUE_SEARCH       0 
		EXCEPTION_CONTINUE_EXECUTION    -1
	*/	
	int setHandleExceptionBehavior( int val );
	int getHandleExceptionBehavior() const;

	/** 处理异常前会调用的事件 
	*/
	TProc setBeforeHandleException( TProc fnBeforeDump );

	/** 处理异常后会调用的事件 
	*/
	TProc setAfterHandleException( TProc fnAfterDump );
	 
	/** 设置Dump文件名
	*/
	void setDumpFile( const std::string & strFile );

	/** 取得Dump文件名
	*/
	const std::string & getDumpFile() const;
public:
	static void startHandleException();
	static ExceptionHandler & defaultHandler();
	static std::string getDefalutDumpFile();
public:
	static LONG WINAPI defaultExceptionFilter( _EXCEPTION_POINTERS *ExceptionInfo );
public:
	static LONG WINAPI showStackAndDump(_EXCEPTION_POINTERS *ExceptionInfo);
private:
	static void defaultDoBeforeHandleException();
	static void defaultDoAfterHandleException();

	void createMiniDump( LPEXCEPTION_POINTERS lpExceptionInfo );
	void init();
private:
	ExceptionHandler( const ExceptionHandler & );
	void operator = ( const ExceptionHandler & );
private:
	// 旧回调函数
	void * mOldHandler;
	std::string mDumpFileName;

	int mHandleExceptionBehavior;
	TProc mfnDoAfterDump;
	TProc mfnDoBeforeDump;
};


//=============================================================================
// 额外的一些错误信息
//=============================================================================
class EQErrorInfoEx
{
public:
	static EQErrorInfoEx & instance()
	{
		static EQErrorInfoEx s_val;
		return s_val;
	}
public:
	std::string errorType; // 错误类型
	std::string account;   // 玩家账号
};

//=============================================================================
// 异常处理函数
//=============================================================================
template <typename TRet, typename T, typename TProc>
TRet EQTryThisCall(T * pThis, TProc proc)
{
	// 设置进程异常捕获函数, 程序运行时该设置可能被其它程序修改
	//	考虑到有可能多线程, 这个方法仍保留
	ExceptionHandler::startHandleException();

	TRet ret = 0;
	__try 
	{ 
		ret = (pThis->*proc)();
	}
	__except(ExceptionHandler::showStackAndDump(GetExceptionInformation()))
	{
	}
	return ret;
}


#endif


