/******************************************************************************
* ����: HJX
* ʱ��: 2009-10-21
* ����: �����쳣�ص������������쳣����(С�ڴ�ת��)
* ʹ�÷���: 1. ��һ�ַ���: 
				��main��ͷ �� Ӧ�ó����ʼ��ʱ����������伴��
           		ExceptionHandler::startHandleException(); 
			2. �ڶ��ַ���:
				����__try     __except
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
	
	/** ���캯�� 
	@param exceptionHandler: �ص�����, ����ΪPTOP_LEVEL_EXCEPTION_FILTER
	*/
	explicit ExceptionHandler( void * exceptionHandler = &ExceptionHandler::defaultExceptionFilter );
public:
	/** ���ô����쳣�����Ϊ
	@param val ����Ϊ����ֵ
		EXCEPTION_EXECUTE_HANDLER       1 (Ĭ��ֵ)
		EXCEPTION_CONTINUE_SEARCH       0 
		EXCEPTION_CONTINUE_EXECUTION    -1
	*/	
	int setHandleExceptionBehavior( int val );
	int getHandleExceptionBehavior() const;

	/** �����쳣ǰ����õ��¼� 
	*/
	TProc setBeforeHandleException( TProc fnBeforeDump );

	/** �����쳣�����õ��¼� 
	*/
	TProc setAfterHandleException( TProc fnAfterDump );
	 
	/** ����Dump�ļ���
	*/
	void setDumpFile( const std::string & strFile );

	/** ȡ��Dump�ļ���
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
	// �ɻص�����
	void * mOldHandler;
	std::string mDumpFileName;

	int mHandleExceptionBehavior;
	TProc mfnDoAfterDump;
	TProc mfnDoBeforeDump;
};


//=============================================================================
// �����һЩ������Ϣ
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
	std::string errorType; // ��������
	std::string account;   // ����˺�
};

//=============================================================================
// �쳣������
//=============================================================================
template <typename TRet, typename T, typename TProc>
TRet EQTryThisCall(T * pThis, TProc proc)
{
	// ���ý����쳣������, ��������ʱ�����ÿ��ܱ����������޸�
	//	���ǵ��п��ܶ��߳�, ��������Ա���
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


