/** 
@file  
@brief		xml解析实现类
@version	2010-11-18		
*/
#pragma once
#include "Parser/TinyXml/tinyxml.h"

#ifndef TXWeibo_API 
#define TXWeibo_API  __declspec(dllimport)
#endif

/// Xml Reader类，用于解析xml文件
class XmlParser
{
public:
	XmlParser();
	~XmlParser();

public:
	/// 判断当前parser是否为readonly。为readonly时，不能调用Parse接口；否则失败
	TXWeibo_API bool	IsReadyOnly();

	/// 解析xml文件
	/*
		@param	pXmlContent	in：xml字符串，必须为utf8
	*/
	TXWeibo_API bool	Parse(const char* pXmlContent);

	/// 根据名字获取子节点
	/*
		@param	pName	in：子节点名字
	*/
	TXWeibo_API XmlParser	GetChild(const char* pName); 

	/// 根据序号获取子节点
	/*
		@param	nIndex	in：子节点序号
	*/
	TXWeibo_API XmlParser	GetChild(const int nIndex);

	/// 根据名字获取子节字符串
	/*
		@param	pName	in：子节点名字
	*/
	/*返回值需调用ReleaseData释放*/
	TXWeibo_API char*	GetString(const char* pName);

	/// 根据序号获取子节点字符串
	/*
		@param	nIndex	in：子节点序号
	*/
	/*返回值需调用ReleaseData释放*/
	TXWeibo_API char*	GetString(const int nIndex);

	/// 根据名字获取子节点布尔值
	/*
		@param	pName	in：子节点名字
	*/
	TXWeibo_API bool	GetBool(const char* pName);

	/// 根据序号获取子节点布尔值
	/*
		@param	nIndex	in：子节点序号
	*/
	TXWeibo_API bool	GetBool(const int nIndex);

	/// 根据名字获取子节点整数值
	/*
	@param	pName	in，子节点名字
	*/
	TXWeibo_API int	GetInt(const char* pName);

	/// 根据序号获取子节点整数值
	/*
	@param	nIndex	in，子节点序号
	*/
	TXWeibo_API int	GetInt(const int nIndex);

	/// 根据名字获取子节点非符号整数值
	/*
	@param	pName	in，子节点名字
	*/
	TXWeibo_API unsigned int	GetUint(const char* pName);

	/// 根据序号获取子节点非符号整数值
	/*
		@param	nIndex	in：子节点序号
	*/
	TXWeibo_API unsigned int	GetUint(const int nIndex);

	/// 根据名字获取子节点64位非符号整数值
	/*
		@param	pName	in：子节点名字
	*/
	TXWeibo_API long long	GetUint64(const char* pName);

	/// 根据序号获取子节点64位非符号整数值
	/*
		@param	nIndex	in：子节点序号
	*/
	TXWeibo_API long long	GetUint64(const int nIndex);

private:
	static XmlParser	GetReadOnlyReader(TiXmlElement* pRoot);
	TiXmlElement*		GetChildByName(const char* pName);
	TiXmlElement*		GetChildByIndex(const int nIndex);

private:
	
	TiXmlElement*		m_pRoot;		///< xml文件根节点	
	bool				m_bReadOnly;	///< 是否只读	
	bool				m_bDeleteSelf;	///< 是否需要自己管理内存释放
};
