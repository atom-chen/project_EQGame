/******************************************************************************
* ����: Y3
* ʱ��: 2009-10-09
* ����: ����һЩͨ�ú����İ�װ
******************************************************************************/
#ifndef CommFunc_H
#define CommFunc_H

#include "EQMainPrerequisites.h"


//=============================================================================
//Win32 API ��װ
//=============================================================================

/** ִ��Dos������.
	@remark ����copyָ��. ע���⿪��һ���½���.
*/
extern void ExecDosCommand( const std::string& cmd );

/** �ȴ�ִ��Dos������. �˺�������
	@remarks
		����������Dos�����ִ�н��̽�����ż���. ע������һ�����̵ĵȴ�.
*/
extern void WaitExceDosCommand( const std::string& cmd );

/** ִ��exe
	@remarks
		bBlocking = true ʱ��ȴ�exeִ����ϲŷ���
*/
extern void RunExe( const std::string& cmd, bool bBlocking );

/** �ж�aSet�Ƿ����aItem
*/
extern bool BitContains( uint aSet, uint aItem );

/** λֵ
	@remarks
		����n������ 0 - 31
*/
extern uint BitValue( uint n );

/** λֵת��
	@remarks
		����ֵ ������ 0 - 31
*/
extern uint UIntToBit( uint val );

/** �ַ���λ��ת��uint
	@remarks
		�ִ���ʽ 0,1,2,3,4
		���������� 0 - 31
*/
extern uint StringBitsetToUInt( std::string const & sBitset );

/** uintת���ַ���λ��
	@remarks
		�ִ���ʽ 0,1,2,3,4
		���������� 0 - 31
*/
extern std::string UIntToStringBitset( uint aSet );


/** uintת��λ��
	@remarks
		���������� 0 - 31
	�������� arrayof(uint)
*/
extern std::vector<uint> UIntToBitsetArray( uint aSet );

/** uintת��λ��
	@remarks
		���������� 0 - 31
	����λ���� arrayof(0 - 31)
*/
extern std::vector<uint> UIntToBits( uint aSet );



//=============================================================================
// �ִ���غ���
//=============================================================================
/** ȡ�ÿ��ִ�
*/
extern const std::string & EmptyStr();


/** ���з�
*/
extern const std::string & NewLineStr();

/** ����n��ch�ַ���ɵ��ִ�
	���� StringOfChar( 5, '0' ) ���Ϊ �ִ�"00000"
*/
extern std::string StringOfChar( size_t n, char ch );

/** ����ת�ִ�
*/
extern std::string IntToStr( int n );

/** ����ת�ִ�����ǰ�油0
*/
extern std::string IntToStrFillZero( int n, size_t iWidth );

/** ��ʽ���ִ�
*/
extern std::string FormatStr( const char * str, ... );


/** GBK�ִ�ת��Unicode
*/
extern std::wstring GB2312ToUnicode( const char * str );


extern std::string UnicodeToGB2312( const wchar_t* str );


extern std::wstring UTF8ToUnicode( const char* str );


extern std::string UnicodeToUTF8( const wchar_t* str );


extern std::string GB2312ToUTF8(const char* str);


extern std::string UTF8ToGB2312(const char* str);
extern int UTF8Len( const char * str );



//=============================================================================
// ʱ����غ���		
//=============================================================================

//struct _SYSTEMTIME;
/**  ȡ������ʱ���ִ�(yyyymmddhhnnssxxx) 
*/
extern std::string GetDateTimeStr( const _SYSTEMTIME & st );
 
/**  ȡ�õ�ǰ����ʱ���ִ�(yyyymmddhhnnssxxx)
*/
extern std::string GetCurDateTimeStr();

/**  ��ʽ��ʱ��
	@param sFormat ʱ���ʽ�ִ�
	@param n ��ʾ���� ��Ӧ�ĸ�ʽ��Ϊ 'n'
	@param ms ��ʾ���� ��Ӧ�ĸ�ʽ��Ϊ 'x'
	���� FormatTime( "hhʱnn��ss��xxx����", 5, 6, 7, 8 ) 
	     ���н��Ϊ "05ʱ06��07��008����"
*/
extern std::string FormatTime( const std::string & sFormat, size_t h, size_t n, size_t s, size_t ms );

extern std::string FormatDateTime( const std::string & sFormat, size_t y, size_t m, size_t d, 
						   size_t h, size_t n, size_t s, size_t ms );

extern std::string FormatDateTime( const std::string & sFormat, const SYSTEMTIME & st );

extern std::string FormatDate( const std::string & sFormat, size_t y, size_t m, size_t d );

/** 
*	��	�ܣ������ڣ�������ʱ���룩����Ϊ���룩(add by hezhr)
*	��	����y,m,d,h,n,s(�꣬�£��գ�ʱ���֣���)
*	����ֵ����
*/
extern long DateToSeconds(int y, int m, int d, int h, int n, int s);
/** 
*	��	�ܣ���ȡ����ʱ���ʱ�����أ��룩(add by hezhr)
*	��	����s1����һ��ʱ�䣩��s2���ڶ���ʱ�䣩����λ��Ϊ���룩
*	����ֵ��double���ͣ�<0��s1��s2�磩��=0��s1����s2����>0��s1��s2��
*/
extern double TimeDiffSeconds(long s1, long s2);
/** 
*	��	�ܣ���ȡ����ʱ���ʱ�����أ��죩(add by hezhr)
*	��	����s1����һ��ʱ�䣩��s2���ڶ���ʱ�䣩����λ��Ϊ���룩
*	����ֵ��double���ͣ����Ե���㼸�죩��<0��s1��s2�磩��=0��s1����s2����>0��s1��s2��
*/
extern double TimeDiffDayFloat(long s1, long s2);
/** 
*	��	�ܣ���ȡ����ʱ���ʱ�����أ��죩(add by hezhr)
*	��	����s1����һ��ʱ�䣩��s2���ڶ���ʱ�䣩����λ��Ϊ���룩
*	����ֵ��int���ͣ�ֻ�ܵ������죩��<0��s1��s2�磩��=0��s1����s2����>0��s1��s2��
*/
extern int TimeDiffDayInt(long s1, long s2);

extern unsigned long GetTickCountByLua(void);



//=============================================================================
// �ļ�Ŀ¼��غ���
//=============================================================================

/**  �ж�Ŀ¼�Ƿ����
*/
extern bool ExistsDir( const std::string & strPath );

/** ����Ŀ¼
*/
extern void ForceDir( const std::string & sDir );

/** ȡ�õ�ǰĿ¼
	@remarks ����Ŀ¼�����Զ����� \ ��
*/
extern std::string GetCurrentDir();

/** ȡ����Ϸ�û�Ŀ¼
@remarks ��������ڻ��Զ�����������Ŀ¼�����Զ����� / ��
*/
extern std::string GetGameUserDir();

/** ȡ��Ŀ¼��
*/
extern std::string GetDir( const std::string & strFile );

/** ��Ŀ¼
*/
extern void OpenDir(const std::string& sDir);

/** ��ȡ�ļ���
	@remarks �ļ���������չ����(myfile.bmp)
*/
extern std::string GetFileName( const std::string & strFile );

/** ��ȡ�ļ�������չ��
	@param out strFileName �ļ�����������չ��
	@param out strFileExt ��������� ".exe"
*/
extern void GetFileNameAndExt( const std::string & strFile, std::string & strFileName, std::string & strFileExt );


/** ��ȡĿ¼�����ļ���
	@remarks �ļ���������չ����(myfile.bmp), ����Ŀ¼�����Զ����� \ ��
*/
extern void GetDirAndFileName( const std::string & strFile, std::string & strDir, std::string & strFileName );


/** ��ȡ����·��
	@remarks ��Ҫ���滻�� "..\", ".\"֮��ķ���
*/
extern std::string GetFullPath( const std::string & strFile );


/** ȡ�ó�������
	�������赱ǰӦ�ó�������ΪD:\\myPath\\myApp.exe, �ǽ��Ϊ "myApp"
*/
extern std::string GetModuleName();





//=============================================================================
// ��Դ��غ���
//=============================================================================

/** ����Դ
@remarks ��Ogre::ResourceGroupManager::openResource�İ�װ. ���д���Դ������
	ʹ���������. ��Ϊֱ�ӵķ�����Դ, �����պ���Դ�����, �����޷�ʹ��.
@par ���Զ��ر���Դ.
*/
extern Ogre::DataStreamPtr OpenResource( const std::string& resourceName );

/** �����Դ�Ƿ����
@remarks ��Ogre::ResourceGroupManager::resourceExists�İ�װ. ������Դ��������
	��������������
*/
extern bool ExistResource( const std::string& resourceName );

/** �ֶ�����Ogre�Ľű������������, �Ա����ޱ�Ҫ��η���
*/
extern void ParseOgreScript( const std::string& scriptName ); 





//=============================================================================
// xml��غ���
//=============================================================================

/** ��ȡxmlԪ�ص�����. ����Ҳ�����Ӧ������, ����Ĭ��ֵ����
@param XMLNode �ڵ�
@param attrib ������
@param defaultValue Ĭ��ֵ
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
// ��������
//=============================================================================
extern std::string Vector3ToString( const Ogre::Vector3 & v );
extern Ogre::Vector3 StringToVector3( const std::string & s );






//=============================================================================
// ��������
//=============================================================================


/** ȡ�þ�̬����Ԫ�ظ���
*/
template <typename T, typename TArray>
uint GetArraySize( const TArray & arr )
{
	return sizeof( arr ) / sizeof( T );
}




// �򵥼���
extern std::string simple_xor(const std::string & data);
	


#endif
