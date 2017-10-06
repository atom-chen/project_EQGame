#include "EQMainPCH.h"
#include "ExceptionHandler.h"
#include "CommFunc.h"
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

namespace 
{
	boost::recursive_mutex g_getStackLock;

	class symbol_context : boost::noncopyable
    {
        public:
            symbol_context()
            {
                if (!SymInitialize(GetCurrentProcess(), 0, TRUE))
                    throw std::runtime_error("Failed to initialize symbol context");
            }
            ~symbol_context() { SymCleanup(GetCurrentProcess()); }
    };



	std::vector<std::string> getStack( ::CONTEXT context )
	{
		::boost::recursive_mutex::scoped_lock autoLocker( g_getStackLock );

		std::vector<std::string> frames;

        symbol_context sc;

		STACKFRAME frame = {0};

        frame.AddrPC.Offset = context.Eip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Esp;
        frame.AddrStack.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Ebp;
        frame.AddrFrame.Mode = AddrModeFlat;

        HANDLE process = GetCurrentProcess();
        HANDLE thread = GetCurrentThread();

        char symbol_buffer[sizeof(IMAGEHLP_SYMBOL) + 255];
        char module_name_raw[MAX_PATH];

        while(StackWalk(IMAGE_FILE_MACHINE_I386, process, thread, &frame, &context, 0, SymFunctionTableAccess, SymGetModuleBase, 0))
        {          
            IMAGEHLP_SYMBOL *symbol = reinterpret_cast<IMAGEHLP_SYMBOL *>(symbol_buffer);
            symbol->SizeOfStruct = (sizeof *symbol) + 255;
            symbol->MaxNameLength = 254;

            DWORD module_base = SymGetModuleBase(process, frame.AddrPC.Offset);
            std::string module_name = "[unknown module]";
            if (module_base && GetModuleFileNameA(reinterpret_cast<HINSTANCE>(module_base), module_name_raw, MAX_PATH))
                module_name = module_name_raw;

            // 取函数
            DWORD dummy = 0;
            BOOL got_symbol = SymGetSymFromAddr(process, frame.AddrPC.Offset, &dummy, symbol);

            std::string func = got_symbol ? symbol->Name : "[unknown function]";
			module_name = module_name.c_str() + module_name.find_last_of("/\\");

			IMAGEHLP_LINE imgLine = {0};
			imgLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);			

			// 取行号
			dummy = 0;
			BOOL got_line = SymGetLineFromAddr(process, frame.AddrPC.Offset, &dummy, &imgLine);
			
			std::string sLine;
			for (DWORD n = imgLine.LineNumber; n > 0; n /= 10)
			{
				sLine.insert(sLine.begin(), (n % 10 + '0'));
			}

			sLine = got_line ? (" line " + sLine) : ""; 

			std::string str = module_name + " " + func + sLine;
            frames.push_back(str);
        }
		return frames;
	}

	void showStack( const ::CONTEXT & con )
	{
		std::vector<std::string> stack = getStack(con);

		LOGERR("============Error Stack==============");
		for (std::vector<std::string>::const_iterator it = stack.begin();
			it != stack.end(); ++it)
		{			
			LOGERR(*it);
		}
		LOGERR("============End Stack==============");
	}

	// 非法参数
	void InvalidParameterHandler(const wchar_t* expression,
										const wchar_t* function, 
										const wchar_t* file, 
										unsigned int line, 
										uintptr_t pReserved)
	{
		// 随便丢个异常，不然没法得到错误stack
		throw line;
	}
}


//--------------------------------------------------------------------------
ExceptionHandler::~ExceptionHandler()
{
	::SetUnhandledExceptionFilter( static_cast<PTOP_LEVEL_EXCEPTION_FILTER>( mOldHandler ) );
}
//--------------------------------------------------------------------------
ExceptionHandler::ExceptionHandler( void * exceptionHandler )
	: mOldHandler( NULL )
	, mHandleExceptionBehavior( EXCEPTION_EXECUTE_HANDLER )
	, mfnDoAfterDump( &ExceptionHandler::defaultDoAfterHandleException )
	, mfnDoBeforeDump( &ExceptionHandler::defaultDoBeforeHandleException )
{
	if ( !::IsDebuggerPresent() ) 
	{	
		_set_invalid_parameter_handler(InvalidParameterHandler); // 专门处理非法参数
		mOldHandler = ::SetUnhandledExceptionFilter( static_cast<PTOP_LEVEL_EXCEPTION_FILTER>( exceptionHandler ) );
	}
	else
	{
		// 如果在调试状态, 直接交给调试器处理
	}
}
//--------------------------------------------------------------------------
int ExceptionHandler::setHandleExceptionBehavior( int val )
{
	int oldVal = mHandleExceptionBehavior;
	mHandleExceptionBehavior = val;
	return oldVal;
}
//--------------------------------------------------------------------------
int ExceptionHandler::getHandleExceptionBehavior() const
{
	return mHandleExceptionBehavior;
}
//--------------------------------------------------------------------------
ExceptionHandler::TProc ExceptionHandler::setBeforeHandleException( TProc fnBeforeDump )
{
	TProc fnOld = mfnDoBeforeDump;
	mfnDoBeforeDump = fnBeforeDump;
	return fnOld;
}
//--------------------------------------------------------------------------
ExceptionHandler::TProc ExceptionHandler::setAfterHandleException( TProc fnAfterDump )
{
	TProc fnOld = mfnDoAfterDump;
	mfnDoAfterDump = fnAfterDump;
	return fnOld;
}
//--------------------------------------------------------------------------
void ExceptionHandler::setDumpFile( const std::string & strFile )
{
	mDumpFileName = strFile;
}
//--------------------------------------------------------------------------
const std::string & ExceptionHandler::getDumpFile() const
{
	return mDumpFileName;
}
//--------------------------------------------------------------------------
std::string ExceptionHandler::getDefalutDumpFile()
{
	std::string strDir = GetGameUserDir() + "Error/";
	ForceDir(strDir);

	// 组织日期格式
	SYSTEMTIME st;
	::GetLocalTime( &st );
	st.wYear = st.wYear % 100;
	std::string strDateTime = FormatDateTime( "YYMMDDhhnnss", st );
	
	return strDir + GetModuleName() + strDateTime + ".dmp";
}
//--------------------------------------------------------------------------
ExceptionHandler & ExceptionHandler::defaultHandler()
{
	static ExceptionHandler s_val;
	return s_val;
}
//--------------------------------------------------------------------------
void ExceptionHandler::startHandleException()
{
	defaultHandler().init();
}
//--------------------------------------------------------------------------
void ExceptionHandler::createMiniDump( LPEXCEPTION_POINTERS lpExceptionInfo ) 
{
	// build file name
	std::string strFile = mDumpFileName;

	if ( strFile.empty() )
	{				
		strFile = ExceptionHandler::getDefalutDumpFile();
	}

	ForceDir( GetDir( strFile ) );
	
	// Open a file
	HANDLE hFile = CreateFileA( strFile.c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( NULL == hFile ||  INVALID_HANDLE_VALUE == hFile ) 
	{
		return;
	}

	// Create the minidump 
	MINIDUMP_EXCEPTION_INFORMATION mdei;
	mdei.ThreadId          = GetCurrentThreadId();
	mdei.ExceptionPointers = lpExceptionInfo;
	mdei.ClientPointers    = FALSE;	

	MINIDUMP_TYPE mdt      = MiniDumpNormal;
	BOOL retv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(),
		hFile, mdt, ( lpExceptionInfo != 0 ) ? &mdei : 0, 0, 0 );
    
	// Close the file
	CloseHandle( hFile );
}
/** 默认回调函数
*/
//--------------------------------------------------------------------------
LONG WINAPI ExceptionHandler::defaultExceptionFilter( _EXCEPTION_POINTERS *ExceptionInfo )
{
	// 显示堆栈
	showStack(*ExceptionInfo->ContextRecord);
	// 保证堆栈信息写入文件
	EQLogManager::getLogger().flush();

	if ( defaultHandler().mfnDoBeforeDump )
	{
		(*defaultHandler().mfnDoBeforeDump)();
	}

	defaultHandler().createMiniDump( ExceptionInfo );

	if ( defaultHandler().mfnDoAfterDump )
	{
		(*defaultHandler().mfnDoAfterDump)();
	}

	return defaultHandler().mHandleExceptionBehavior;
}
//--------------------------------------------------------------------------
void ExceptionHandler::defaultDoBeforeHandleException()
{
	defaultHandler().setDumpFile( ExceptionHandler::getDefalutDumpFile() );
}
//--------------------------------------------------------------------------
void ExceptionHandler::defaultDoAfterHandleException()
{
	// 发生异常后显示错误讯息
	//std::string strMsg = "程序发生错误！具体信息请查看文件:\n  " + GetFullPath( defaultHandler().getDumpFile() );
	//std::string strTitle = "程序错误报告";
	//::MessageBoxA( NULL, strMsg.c_str(), strTitle.c_str(), MB_ICONERROR );

	// 取得错误信息
	std::string sAccount = EQErrorInfoEx::instance().account;
	std::string sDmpFile = GetFullPath( defaultHandler().getDumpFile() );
	std::string sErrorType = EQErrorInfoEx::instance().errorType;
	if (sErrorType.empty())
		sErrorType = "未知错误";

	std::string cmd = FormatStr("ErrorReport.exe ErrorReportArg:%sErrorReportArg:%sErrorReportArg:%s", 
								sErrorType.c_str(),
								sDmpFile.c_str(),
								sAccount.c_str()
								);
	RunExe(cmd, true);

	::exit( EXIT_FAILURE );
}
//--------------------------------------------------------------------------
void ExceptionHandler::init()
{
	// do nothing	
}
//--------------------------------------------------------------------------
LONG WINAPI ExceptionHandler::showStackAndDump( _EXCEPTION_POINTERS * ExceptionInfo )
{
	if ( ::IsDebuggerPresent() ) // 如果在调试状态, 直接交给调试器处理
	{
		// 显示堆栈
		showStack(*ExceptionInfo->ContextRecord);
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// 因为不想要看到各种对话框，所以这里去掉其它监听函数 
	::SetUnhandledExceptionFilter( NULL );
	::SetErrorMode( SEM_NOGPFAULTERRORBOX );
	defaultHandler().setHandleExceptionBehavior( EXCEPTION_EXECUTE_HANDLER );

	return defaultHandler().defaultExceptionFilter( ExceptionInfo );
}


