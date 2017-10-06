/******************************************************************************
* 作者: Y3
* 时间: 2009-10-09
* 描述: 用于一些通用函数的包装
******************************************************************************/
#ifndef CommFunc_H
#define CommFunc_H

#include "EQMainPrerequisites.h"


//=============================================================================
//Win32 API 包装
//=============================================================================

/** 执行Dos的命令.
	@remark 比如copy指令. 注意这开了一个新进程.
*/
extern void ExecDosCommand( const std::string& cmd );

/** 等待执行Dos的命令. 此函数慎用
	@remarks
		这个函数会等Dos命令的执行进程结束后才继续. 注意这是一个进程的等待.
*/
extern void WaitExceDosCommand( const std::string& cmd );

/** 执行exe
	@remarks
		bBlocking = true 时会等待exe执行完毕才返回
*/
extern void RunExe( const std::string& cmd, bool bBlocking );

/** 判断aSet是否包含aItem
*/
extern bool BitContains( uint aSet, uint aItem );

/** 位值
	@remarks
		数字n限制在 0 - 31
*/
extern uint BitValue( uint n );

/** 位值转化
	@remarks
		返回值 限制在 0 - 31
*/
extern uint UIntToBit( uint val );

/** 字符串位集转成uint
	@remarks
		字串格式 0,1,2,3,4
		数字限制在 0 - 31
*/
extern uint StringBitsetToUInt( std::string const & sBitset );

/** uint转成字符串位集
	@remarks
		字串格式 0,1,2,3,4
		数字限制在 0 - 31
*/
extern std::string UIntToStringBitset( uint aSet );


/** uint转成位集
	@remarks
		数字限制在 0 - 31
	返回数组 arrayof(uint)
*/
extern std::vector<uint> UIntToBitsetArray( uint aSet );

/** uint转成位集
	@remarks
		数字限制在 0 - 31
	返回位数组 arrayof(0 - 31)
*/
extern std::vector<uint> UIntToBits( uint aSet );



//=============================================================================
// 字串相关函数
//=============================================================================
/** 取得空字串
*/
extern const std::string & EmptyStr();


/** 换行符
*/
extern const std::string & NewLineStr();

/** 返回n个ch字符组成的字串
	例如 StringOfChar( 5, '0' ) 结果为 字串"00000"
*/
extern std::string StringOfChar( size_t n, char ch );

/** 整数转字串
*/
extern std::string IntToStr( int n );

/** 整数转字串并在前面补0
*/
extern std::string IntToStrFillZero( int n, size_t iWidth );

/** 格式化字串
*/
extern std::string FormatStr( const char * str, ... );


/** GBK字串转成Unicode
*/
extern std::wstring GB2312ToUnicode( const char * str );


extern std::string UnicodeToGB2312( const wchar_t* str );


extern std::wstring UTF8ToUnicode( const char* str );


extern std::string UnicodeToUTF8( const wchar_t* str );


extern std::string GB2312ToUTF8(const char* str);


extern std::string UTF8ToGB2312(const char* str);
extern int UTF8Len( const char * str );



//=============================================================================
// 时间相关函数		
//=============================================================================

//struct _SYSTEMTIME;
/**  取得日期时间字串(yyyymmddhhnnssxxx) 
*/
extern std::string GetDateTimeStr( const _SYSTEMTIME & st );
 
/**  取得当前日期时间字串(yyyymmddhhnnssxxx)
*/
extern std::string GetCurDateTimeStr();

/**  格式化时间
	@param sFormat 时间格式字串
	@param n 表示分钟 对应的格式符为 'n'
	@param ms 表示毫秒 对应的格式符为 'x'
	例如 FormatTime( "hh时nn分ss秒xxx毫秒", 5, 6, 7, 8 ) 
	     运行结果为 "05时06分07秒008毫秒"
*/
extern std::string FormatTime( const std::string & sFormat, size_t h, size_t n, size_t s, size_t ms );

extern std::string FormatDateTime( const std::string & sFormat, size_t y, size_t m, size_t d, 
						   size_t h, size_t n, size_t s, size_t ms );

extern std::string FormatDateTime( const std::string & sFormat, const SYSTEMTIME & st );

extern std::string FormatDate( const std::string & sFormat, size_t y, size_t m, size_t d );

/** 
*	功	能：把日期（年月日时分秒）换算为（秒）(add by hezhr)
*	参	数：y,m,d,h,n,s(年，月，日，时，分，秒)
*	返回值：秒
*/
extern long DateToSeconds(int y, int m, int d, int h, int n, int s);
/** 
*	功	能：获取两个时间的时间差，返回（秒）(add by hezhr)
*	参	数：s1（第一个时间），s2（第二个时间），单位都为（秒）
*	返回值：double类型，<0（s1比s2早），=0（s1等于s2），>0（s1比s2晚）
*/
extern double TimeDiffSeconds(long s1, long s2);
/** 
*	功	能：获取两个时间的时间差，返回（天）(add by hezhr)
*	参	数：s1（第一个时间），s2（第二个时间），单位都为（秒）
*	返回值：double类型（可以到零点几天），<0（s1比s2早），=0（s1等于s2），>0（s1比s2晚）
*/
extern double TimeDiffDayFloat(long s1, long s2);
/** 
*	功	能：获取两个时间的时间差，返回（天）(add by hezhr)
*	参	数：s1（第一个时间），s2（第二个时间），单位都为（秒）
*	返回值：int类型（只能到整数天），<0（s1比s2早），=0（s1等于s2），>0（s1比s2晚）
*/
extern int TimeDiffDayInt(long s1, long s2);

extern unsigned long GetTickCountByLua(void);



//=============================================================================
// 文件目录相关函数
//=============================================================================

/**  判断目录是否存在
*/
extern bool ExistsDir( const std::string & strPath );

/** 建立目录
*/
extern void ForceDir( const std::string & sDir );

/** 取得当前目录
	@remarks 返回目录名会自动加上 \ 号
*/
extern std::string GetCurrentDir();

/** 取得游戏用户目录
@remarks 如果不存在会自动创建，返回目录名会自动加上 / 号
*/
extern std::string GetGameUserDir();

/** 取得目录名
*/
extern std::string GetDir( const std::string & strFile );

/** 打开目录
*/
extern void OpenDir(const std::string& sDir);

/** 提取文件名
	@remarks 文件名包含扩展名如(myfile.bmp)
*/
extern std::string GetFileName( const std::string & strFile );

/** 提取文件名及扩展名
	@param out strFileName 文件名不包含扩展名
	@param out strFileExt 包含点号如 ".exe"
*/
extern void GetFileNameAndExt( const std::string & strFile, std::string & strFileName, std::string & strFileExt );


/** 提取目录名，文件名
	@remarks 文件名包含扩展名如(myfile.bmp), 返回目录名会自动加上 \ 号
*/
extern void GetDirAndFileName( const std::string & strFile, std::string & strDir, std::string & strFileName );


/** 提取完整路径
	@remarks 主要是替换掉 "..\", ".\"之类的符号
*/
extern std::string GetFullPath( const std::string & strFile );


/** 取得程序名称
	例：假设当前应用程序名称为D:\\myPath\\myApp.exe, 那结果为 "myApp"
*/
extern std::string GetModuleName();





//=============================================================================
// 资源相关函数
//=============================================================================

/** 打开资源
@remarks 对Ogre::ResourceGroupManager::openResource的包装. 所有打开资源都必须
	使用这个函数. 因为直接的访问资源, 对于日后资源打包后, 会变的无法使用.
@par 会自动关闭资源.
*/
extern Ogre::DataStreamPtr OpenResource( const std::string& resourceName );

/** 检查资源是否存在
@remarks 对Ogre::ResourceGroupManager::resourceExists的包装. 对于资源的搜索都
	必须调用这个函数
*/
extern bool ExistResource( const std::string& resourceName );

/** 手动分析Ogre的脚本请用这个函数, 以避免无必要多次分析
*/
extern void ParseOgreScript( const std::string& scriptName ); 





//=============================================================================
// xml相关函数
//=============================================================================

/** 获取xml元素的属性. 如果找不到对应的属性, 则用默认值代替
@param XMLNode 节点
@param attrib 属性名
@param defaultValue 默认值
*/
extern std::string xmlGetAttrib(TiXmlElement* XMLNode, const std::string& attrib, const std::string& defaultValue = "");
extern float xmlGetAttribFloat(TiXmlElement* XMLNode, const std::string& attrib, float defaultValue = 0.0f);
extern int xmlGetAttribInt(TiXmlElement* XMLNode, const std::string& attrib, int defaultValue = 0);
extern uint xmlGetAttribUint(TiXmlElement* XMLNode, const std::string& attrib, uint defaultValue = 0);
extern bool xmlGetAttribBool(TiXmlElement* XMLNode, const std::string& attrib, bool defaultValue = false);
extern Ogre::Vector3 xmlGetAttribVector3(TiXmlElement* XMLNode, const std::string& attrib, const Ogre::Vector3& defaultValue = Ogre::Vector3::ZERO);
extern Ogre::Vector4 xmlGetAttribVector4(TiXmlElement* XMLNode, const std::string& attrib, const Ogre::Vector4& defaultValue = Ogre::Vector4::ZERO);
extern Ogre::Quaternion xmlGetAttribQuaternion(TiXmlElement* XMLNode, const std::string& attrib, const Ogre::Quaternion& defaultValue = Ogre::Quaternion::IDENTITY);









//=============================================================================
// 其它函数
//=============================================================================
extern std::string Vector3ToString( const Ogre::Vector3 & v );
extern Ogre::Vector3 StringToVector3( const std::string & s );






//=============================================================================
// 其它函数
//=============================================================================


/** 取得静态数组元素个数
*/
template <typename T, typename TArray>
uint GetArraySize( const TArray & arr )
{
	return sizeof( arr ) / sizeof( T );
}




// 简单加密
extern std::string simple_xor(const std::string & data);
	


#endif
