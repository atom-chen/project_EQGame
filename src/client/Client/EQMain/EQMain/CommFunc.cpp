#include "EQMainPCH.h"

#include "CommFunc.h"
#include <Ogre/OgreScriptCompiler.h>
//#include "shellapi.h"
#include <ShellAPI.h>


void ExecDosCommand( const std::string& cmd )
{
	std::string newcmd = "cmd /c " + cmd;
	WinExec(newcmd.c_str(),SW_HIDE);
}
//--------------------------------------------------------------------------
void WaitExceDosCommand( const std::string& cmd )
{
	std::string newcmd = "cmd.exe /c " + cmd;
	RunExe(newcmd, true);
}

// 运行exe
void RunExe( const std::string & cmd, bool bBlocking )
{
	std::string newcmd = cmd;

	PROCESS_INFORMATION           piProcInfo;
	STARTUPINFO                   siStartInfo;

	ZeroMemory(&piProcInfo,sizeof(piProcInfo));
	ZeroMemory(&siStartInfo,sizeof(siStartInfo));

	siStartInfo.cb				= sizeof(STARTUPINFO);  
	siStartInfo.lpReserved		= NULL;  
	siStartInfo.lpReserved2		= NULL;  
	siStartInfo.cbReserved2		= 0;  
	siStartInfo.lpDesktop		= NULL;  
	siStartInfo.dwFlags			= 0;  

	CreateProcess(  
		NULL,  
		(char*)newcmd.c_str(),  
		NULL,					//   process   security   attributes  
		NULL,					//   primary   thread   security   attributes  
		0,						//   handles   are   inherited  
		0,						//   creation   flags  
		NULL,					//   use   parent's   environment  
		NULL,					//   use   parent's   current   directory  
		&siStartInfo,			//   STARTUPINFO   pointer  
		&piProcInfo);			//   receives   PROCESS_INFORMATION  

	if (bBlocking)
	{
		//   Wait   for   the   processs   to   finish  
		DWORD   rc   =   WaitForSingleObject(  
			piProcInfo.hProcess,   //   process   handle  
			INFINITE);   
	}
	else
		::CloseHandle(piProcInfo.hProcess);
}
//--------------------------------------------------------------------------
bool BitContains( uint aSet, uint aItem )
{
	return 0 != (aSet & aItem);
}
//--------------------------------------------------------------------------
uint BitValue( uint n )
{
	return 1 << n;
}
//--------------------------------------------------------------------------
uint UIntToBit( uint val )
{
	std::vector<uint> bits = UIntToBits(val);
	assert(bits.size() == 1);
	return bits.front();
}
//--------------------------------------------------------------------------
uint StringBitsetToUInt( std::string const & sBitset )
{
	uint val = 0;

	std::vector<std::string> arr = Ogre::StringUtil::split( sBitset, " ," );
	for ( uint i = 0; i < arr.size(); ++i )
	{
		if ( arr[i].empty() )
			continue;
		val |= BitValue( Ogre::StringConverter::parseUnsignedLong( arr[i] ) % (sizeof(uint) * 8) );
	}
	return val;
}
//--------------------------------------------------------------------------
std::vector<uint> UIntToBits( uint aSet )
{
	std::vector<uint> result;
	uint idx = 0;
	for ( ; aSet != 0; aSet = (aSet >> 1) )
	{
		if ( (aSet & 1) == 1 )
		{
			result.push_back(idx);
		}
		++idx;
	}

	return result;
}
//--------------------------------------------------------------------------
std::vector<uint> UIntToBitsetArray( uint aSet )
{
	std::vector<uint> result;
	int idx = 0;
	for ( ; aSet != 0; aSet = (aSet >> 1) )
	{
		if ( (aSet & 1) == 1 )
		{
			result.push_back(BitValue(idx));
		}
		++idx;
	}

	return result;
}
//--------------------------------------------------------------------------
std::string UIntToStringBitset( uint aSet )
{
	std::string result;
	int idx = 0;
	for ( ; aSet != 0; aSet = (aSet >> 1) )
	{
		if ( (aSet & 1) == 1 )
		{
			result += Ogre::StringConverter::toString(idx) + ",";
		}
		++idx;
	}

	if ( !result.empty() )
		result.resize( result.size() - 1 );
	return result;
}
//--------------------------------------------------------------------------

//=============================================================================
// 时间相关函数		
//=============================================================================

//--------------------------------------------------------------------------
std::string GetDateTimeStr( const ::SYSTEMTIME & st)
{
	const int maxLength = 32;
	char buf[maxLength];
	
	sprintf_s( buf, "%04d%02d%02d%02d%02d%02d%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
	return std::string( buf );	 
}
//--------------------------------------------------------------------------
std::string GetCurDateTimeStr()
{
	::SYSTEMTIME st;
	::GetLocalTime( &st );
	return GetDateTimeStr( st );
}
//--------------------------------------------------------------------------
static void _ReplaceCharsWithNum( std::string & sRef, const std::string & strChars, size_t n )
{
	size_t startPos = sRef.find_first_of( strChars );
	if ( startPos < sRef.size() )
	{
		size_t endPos = sRef.find_first_not_of( strChars, startPos + 1 );
		if ( endPos > sRef.size() )
			endPos = sRef.size();

		size_t count = endPos - startPos;
		const std::string strNum = IntToStrFillZero( n, count );
		// 将符号替换成数字
		if ( strNum.size() > count )
		{
			const int oldSize = sRef.size();
			sRef.resize( sRef.size() + strNum.size() - count );
			std::copy_backward( sRef.begin() + endPos, sRef.begin() + oldSize, sRef.end() );
		}

		std::copy( strNum.begin(), strNum.end(), sRef.begin() + startPos );
	}
}
//--------------------------------------------------------------------------
std::string FormatDateTime( const std::string & sFormat, size_t y, size_t m, size_t d, 
						   size_t h, size_t n, size_t s, size_t ms )
{
	std::string sTmp = sFormat;
	_ReplaceCharsWithNum( sTmp, "Yy", y );
	_ReplaceCharsWithNum( sTmp, "Mm", m );
	_ReplaceCharsWithNum( sTmp, "Dd", d );
	_ReplaceCharsWithNum( sTmp, "Hh", h );
	_ReplaceCharsWithNum( sTmp, "Nn", n );
	_ReplaceCharsWithNum( sTmp, "Ss", s );
	_ReplaceCharsWithNum( sTmp, "Xx", ms );

	return sTmp;
}
//--------------------------------------------------------------------------
std::string FormatDateTime( const std::string & sFormat, const SYSTEMTIME & st )
{
	return FormatDateTime( sFormat, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
}
//--------------------------------------------------------------------------
std::string FormatDate( const std::string & sFormat, size_t y, size_t m, size_t d )
{
	std::string sTmp = sFormat;
	_ReplaceCharsWithNum( sTmp, "Yy", y );
	_ReplaceCharsWithNum( sTmp, "Mm", m );
	_ReplaceCharsWithNum( sTmp, "Dd", d );

	return sTmp;
}
//--------------------------------------------------------------------------
std::string FormatTime( const std::string & sFormat, size_t h, size_t n, size_t s, size_t ms )
{
	std::string sTmp = sFormat;
	_ReplaceCharsWithNum( sTmp, "Hh", h );
	_ReplaceCharsWithNum( sTmp, "Nn", n );
	_ReplaceCharsWithNum( sTmp, "Ss", s );
	_ReplaceCharsWithNum( sTmp, "Xx", ms );

	return sTmp;
}
//--------------------------------------------------------------------------
long DateToSeconds(int y, int m, int d, int h, int n, int s)
{
	if (y<1900 || m>12 || m<1 || d>31 || d<1)
	{
		return 0;
	}
	if (h>23 || h<0 || n>59 || n<0 || s>59 || s<0)
	{
		return 0;
	}
	// 日期换算成秒
	tm date;
	date.tm_year = y - 1900;
	date.tm_mon = m - 1;
	date.tm_mday = d;
	date.tm_hour = h;
	date.tm_min = n;
	date.tm_sec = s;
	return mktime(&date);
}
//--------------------------------------------------------------------------
double TimeDiffSeconds(long s1, long s2)
{
	return difftime(s1, s2);
}
//--------------------------------------------------------------------------
double TimeDiffDayFloat(long s1, long s2)
{
	return difftime(s1, s2)/(24*60*60);
}
//--------------------------------------------------------------------------
int TimeDiffDayInt(long s1, long s2)
{
	time_t seconds = (time_t)difftime(s1, s2);
	int day = seconds/(24*60*60);
	if (0 != seconds%(24*60*60))	// 不是整数天
	{
		if (day >= 0)
			day += 1;
		else
			day -= 1;
	}
	return day;
}
//--------------------------------------------------------------------------
unsigned long GetTickCountByLua(void)
{
	return GetTickCount();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

//=============================================================================
// 文件目录相关函数
//=============================================================================

//--------------------------------------------------------------------------
static bool _ExistsDir( const std::string & strPath )
{
	::WIN32_FIND_DATAA wfd;
    HANDLE hFind = FindFirstFileA( strPath.c_str(), &wfd );
    FindClose(hFind);

	return ( (hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) );
}
//--------------------------------------------------------------------------
bool ExistsDir( const std::string & strPath )
{
	if ( strPath.empty() )
		return false;

	char ch = strPath[strPath.size() - 1];
	if ( '/' == ch || '\\' == ch ) 
		return _ExistsDir( strPath.substr( 0, strPath.size() - 1 ) );

	return _ExistsDir( strPath );
}

//--------------------------------------------------------------------------
static void _ForceDir( const std::string & sDir )
{
	::CreateDirectoryA( sDir.c_str(), NULL );	
}
//--------------------------------------------------------------------------
void ForceDir( const std::string & sDir )
{
	if ( sDir.empty() )
		return;

	std::string myDir;

	size_t startPos = 0;

	for ( ; startPos < sDir.size(); )
	{
		size_t pos = sDir.find_first_of( "/\\", startPos );
		if ( pos < sDir.size() )
		{
			myDir = sDir.substr( 0, pos );
			if ( !ExistsDir( myDir ) )
				_ForceDir( myDir );
			startPos = pos + 1;
		}
		else
			break;
	}

	if( !ExistsDir( sDir ) )
		_ForceDir( sDir );
}
//--------------------------------------------------------------------------
std::string GetCurrentDir()
{
	const int bufLen = 1024;
	char buf[bufLen];
	::GetCurrentDirectoryA( bufLen, buf );
	std::string s = buf;
	s.push_back( '\\' );
	return s;
}

std::string GetGameUserDir()
{
	std::string strDir;
	char tpath[MAX_PATH+1];

	const char *userDir = "%HOMEDRIVE%%HOMEPATH%/EQGame/";

	tpath[0] = 0;
	ExpandEnvironmentStringsA(userDir, tpath, MAX_PATH);

	strDir = tpath;

	ForceDir(strDir);

	return strDir;

}
//--------------------------------------------------------------------------
std::string GetDir( const std::string & strFile )
{
	size_t pos = strFile.find_last_of( "/\\:" );
	if ( pos < strFile.size() )
	{
		return strFile.substr( 0, pos + 1 );
	}	
	
	return "";
}
//--------------------------------------------------------------------------
void OpenDir(const std::string& sDir)
{
	::ShellExecute(NULL, "open", (char*)sDir.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
//--------------------------------------------------------------------------
void GetFileNameAndExt( const std::string & strFile, std::string & strFileName, std::string & strFileExt )
{
	strFileExt = "";
	strFileName = GetFileName( strFile );
	size_t pos = strFileName.find_last_of( '.' );
	if ( pos < strFileName.size() )
	{
		strFileExt = strFileName.c_str() + pos;
		strFileName.resize( pos );
	}
}
//--------------------------------------------------------------------------
std::string GetFileName( const std::string & strFile )
{
	size_t pos = strFile.find_last_of( "/\\:" );
	if ( pos < strFile.size() )
	{
		return strFile.c_str() + ( pos + 1 );
	}
	return strFile;
}
//--------------------------------------------------------------------------
void GetDirAndFileName( const std::string & strFile, std::string & strDir, std::string & strFileName )
{
	size_t pos = strFile.find_last_of( "/\\:" );
	if ( pos < strFile.size() )
	{
		strDir = strFile.substr( 0, pos + 1 );
		strFileName = strFile.c_str() + ( pos + 1 );
	}
	else
	{
		strDir = "";
		strFileName = strFile;
	}	
}
//--------------------------------------------------------------------------
std::string GetFullPath( const std::string & strFile )
{	
	char buf[1024];
	::GetFullPathNameA( strFile.c_str(), 1024, buf, NULL );
	return buf;
}
//--------------------------------------------------------------------------
std::string GetModuleName()
{
	const int MaxFilePath = 1024;
	char szModuleName[MaxFilePath];
	::GetModuleFileNameA( NULL, szModuleName, MaxFilePath );

	std::string strModuleName, strExt;		
	GetFileNameAndExt( szModuleName, strModuleName, strExt );
	return strModuleName;
}
//--------------------------------------------------------------------------


//=============================================================================
// 字串相关函数
//=============================================================================
//--------------------------------------------------------------------------
const std::string & NewLineStr()
{
	static const std::string s_val = "\r\n";
	return s_val;
}
//--------------------------------------------------------------------------
const std::string & EmptyStr()
{
	static const std::string s_val;
	return s_val;
}
//--------------------------------------------------------------------------
std::string StringOfChar( size_t n, char ch )
{
	return std::string( n, ch );
}
//--------------------------------------------------------------------------
std::string IntToStr( int n )
{
	char buf[64];
	::_itoa_s( n, buf, 10 );
	return buf;
}
//--------------------------------------------------------------------------
std::string IntToStrFillZero( int n, size_t iWidth )
{
	std::string s = IntToStr( n );	
	if ( s.size() >= iWidth )
	{
		return s;
	}
	
	std::string strPad = StringOfChar( iWidth - s.size() , '0' );
	if ( n < 0 )
	{
		s.insert( 1, strPad );
		return s;
	}
	
	return strPad + s;
}
//--------------------------------------------------------------------------
std::string FormatStr( const char * str, ... )
{
    char buf[1024];
    va_list args;

    va_start(args, str);
	vsprintf_s( buf, str, args );
    va_end(args);

	return buf;
}
//--------------------------------------------------------------------------
std::wstring GB2312ToUnicode( const char * str )
{
	int textlen = MultiByteToWideChar( 936, 0, str, -1, NULL, 0 ); 	
	std::wstring buf( textlen, 0 );
	
	MultiByteToWideChar( 936, 0, str, -1, const_cast<LPWSTR>( buf.c_str() ) , buf.size() ); 

	return buf.c_str(); 
}
//--------------------------------------------------------------------------
std::string UnicodeToGB2312( const wchar_t * str )
{
	int textlen = WideCharToMultiByte( 936, 0, str, -1, NULL, 0, NULL, NULL );
	std::string buf( textlen, 0 );

	WideCharToMultiByte( 936, 0, str, -1, const_cast<LPSTR>( buf.c_str() ), buf.size(), NULL, NULL );

	return buf.c_str();
}
//--------------------------------------------------------------------------
std::wstring UTF8ToUnicode( const char * str )
{
	int textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 ); 	
	std::wstring buf( textlen, 0);

	MultiByteToWideChar( CP_UTF8, 0,str,-1, const_cast<LPWSTR>( buf.c_str() ), buf.size() ); 

	return buf.c_str(); 
}
//--------------------------------------------------------------------------
std::string UnicodeToUTF8( const wchar_t * str )
{
	int textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
	std::string buf( textlen, 0 );

	WideCharToMultiByte( CP_UTF8, 0, str, -1, const_cast<LPSTR>( buf.c_str() ), buf.size(), NULL, NULL );

	return buf.c_str();
}
//--------------------------------------------------------------------------
std::string GB2312ToUTF8( const char * str )
{
	return UnicodeToUTF8( GB2312ToUnicode(str).c_str() );
}
//--------------------------------------------------------------------------
std::string UTF8ToGB2312( const char * str )
{
	return UnicodeToGB2312( UTF8ToUnicode(str).c_str() );
}
//--------------------------------------------------------------------------
int UTF8Len( const char * str )
{
	return (MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 ) - 1);	//减1是末尾\0结束符的字节 	
}


//=============================================================================
// 资源相关函数
//=============================================================================
//--------------------------------------------------------------------------
Ogre::DataStreamPtr OpenResource( const std::string& resourceName )
{
	return Ogre::ResourceGroupManager::getSingleton().openResource(resourceName);
}
//--------------------------------------------------------------------------
bool ExistResource( const std::string& resourceName )
{
	return Ogre::ResourceGroupManager::getSingleton().resourceExists(
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, resourceName);
}
//--------------------------------------------------------------------------
void ParseOgreScript( const std::string& scriptName )
{
	static std::set<std::string> scriptSet;

	std::string lcResourceName = scriptName;
	Ogre::StringUtil::toLowerCase(lcResourceName);
	std::replace( lcResourceName.begin(), lcResourceName.end(), '\\', '/' );

	if ( scriptSet.find(lcResourceName) != scriptSet.end() )
		return;

	Ogre::ScriptCompilerManager::getSingleton().parseScript(OpenResource(scriptName),
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	scriptSet.insert(lcResourceName);
}



//=============================================================================
// xml相关函数
//=============================================================================
//--------------------------------------------------------------------------
std::string xmlGetAttrib(TiXmlElement* XMLNode, const std::string& attrib, const std::string& defaultValue)
{
	const std::string* str =  XMLNode->Attribute(attrib);
	if(str)
		return *str;
	else
		return defaultValue;
}
//--------------------------------------------------------------------------
float xmlGetAttribFloat(TiXmlElement* XMLNode, const std::string& attrib, float defaultValue)
{
	const std::string* str =  XMLNode->Attribute(attrib);
	if(str)
		return Ogre::StringConverter::parseReal(*str);
	else
		return defaultValue;
}
//--------------------------------------------------------------------------
int xmlGetAttribInt(TiXmlElement* XMLNode, const std::string& attrib, int defaultValue)
{
	const std::string* str =  XMLNode->Attribute(attrib);
	if(str)
		return Ogre::StringConverter::parseInt(*str);
	else
		return defaultValue;
}
//--------------------------------------------------------------------------
uint xmlGetAttribUint(TiXmlElement* XMLNode, const std::string& attrib, uint defaultValue)
{
	const std::string* str =  XMLNode->Attribute(attrib);
	if(str)
		return Ogre::StringConverter::parseUnsignedInt(*str);
	else
		return defaultValue;
}
//--------------------------------------------------------------------------
bool xmlGetAttribBool(TiXmlElement* XMLNode, const std::string& attrib, bool defaultValue)
{
	const std::string* str =  XMLNode->Attribute(attrib);
	if(str)
		return Ogre::StringConverter::parseBool(*str);
	else
		return defaultValue;
}
//--------------------------------------------------------------------------
Ogre::Vector3 xmlGetAttribVector3(TiXmlElement* XMLNode, const std::string& attrib, const Ogre::Vector3& defaultValue)
{
	const std::string* str =  XMLNode->Attribute(attrib);
	if(str)
		return Ogre::StringConverter::parseVector3(*str);
	else
		return defaultValue;
}
//--------------------------------------------------------------------------
Ogre::Vector4 xmlGetAttribVector4(TiXmlElement* XMLNode, const std::string& attrib, const Ogre::Vector4& defaultValue)
{
	const std::string* str =  XMLNode->Attribute(attrib);
	if(str)
		return Ogre::StringConverter::parseVector4(*str);
	else
		return defaultValue;
}
//--------------------------------------------------------------------------
Ogre::Quaternion xmlGetAttribQuaternion(TiXmlElement* XMLNode, const std::string& attrib, const Ogre::Quaternion& defaultValue)
{
	const std::string* str =  XMLNode->Attribute(attrib);
	if(str)
		return Ogre::StringConverter::parseQuaternion(*str);
	else
		return defaultValue;
}
//--------------------------------------------------------------------------
std::string Vector3ToString( const Ogre::Vector3 & v )
{
	return FormatStr( "%.1f, %.1f, %.1f", v.x, v.y, v.z );
}
//--------------------------------------------------------------------------
Ogre::Vector3 StringToVector3( const std::string & s )
{
	std::vector<std::string> strs;
	strs = Ogre::StringUtil::split( s, " ," );

	Ogre::Vector3 v3;
	if ( strs.size() == 3 )
	{
		v3.x = Ogre::StringConverter::parseReal(strs[0]);
		v3.y = Ogre::StringConverter::parseReal(strs[1]);
		v3.z = Ogre::StringConverter::parseReal(strs[2]);
	}

	return v3;
}


//--------------------------------------------------------------------------


// 简单加密
std::string simple_xor(const std::string & data)
{
	const std::string key = "X_+#@!GRa^|E%D"; // 此密钥要和服务器的对应
	std::string adjustKey = key;
	if (data.size() < key.size())
	{
		adjustKey = std::string(key.begin(), key.begin() + data.size());
	}
	else
	{
		adjustKey.append(data.size() - adjustKey.size(), ' ');
	}

	std::string result = data;
	for (int i = 0; i < static_cast<int>(data.size()); ++i)
	{
		result[i] = data[i] ^ adjustKey[i];
	}
	return result;
}
