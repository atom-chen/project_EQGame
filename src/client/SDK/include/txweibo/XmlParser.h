/** 
@file  
@brief		xml����ʵ����
@version	2010-11-18		
*/
#pragma once
#include "Parser/TinyXml/tinyxml.h"

#ifndef TXWeibo_API 
#define TXWeibo_API  __declspec(dllimport)
#endif

/// Xml Reader�࣬���ڽ���xml�ļ�
class XmlParser
{
public:
	XmlParser();
	~XmlParser();

public:
	/// �жϵ�ǰparser�Ƿ�Ϊreadonly��Ϊreadonlyʱ�����ܵ���Parse�ӿڣ�����ʧ��
	TXWeibo_API bool	IsReadyOnly();

	/// ����xml�ļ�
	/*
		@param	pXmlContent	in��xml�ַ���������Ϊutf8
	*/
	TXWeibo_API bool	Parse(const char* pXmlContent);

	/// �������ֻ�ȡ�ӽڵ�
	/*
		@param	pName	in���ӽڵ�����
	*/
	TXWeibo_API XmlParser	GetChild(const char* pName); 

	/// ������Ż�ȡ�ӽڵ�
	/*
		@param	nIndex	in���ӽڵ����
	*/
	TXWeibo_API XmlParser	GetChild(const int nIndex);

	/// �������ֻ�ȡ�ӽ��ַ���
	/*
		@param	pName	in���ӽڵ�����
	*/
	/*����ֵ�����ReleaseData�ͷ�*/
	TXWeibo_API char*	GetString(const char* pName);

	/// ������Ż�ȡ�ӽڵ��ַ���
	/*
		@param	nIndex	in���ӽڵ����
	*/
	/*����ֵ�����ReleaseData�ͷ�*/
	TXWeibo_API char*	GetString(const int nIndex);

	/// �������ֻ�ȡ�ӽڵ㲼��ֵ
	/*
		@param	pName	in���ӽڵ�����
	*/
	TXWeibo_API bool	GetBool(const char* pName);

	/// ������Ż�ȡ�ӽڵ㲼��ֵ
	/*
		@param	nIndex	in���ӽڵ����
	*/
	TXWeibo_API bool	GetBool(const int nIndex);

	/// �������ֻ�ȡ�ӽڵ�����ֵ
	/*
	@param	pName	in���ӽڵ�����
	*/
	TXWeibo_API int	GetInt(const char* pName);

	/// ������Ż�ȡ�ӽڵ�����ֵ
	/*
	@param	nIndex	in���ӽڵ����
	*/
	TXWeibo_API int	GetInt(const int nIndex);

	/// �������ֻ�ȡ�ӽڵ�Ƿ�������ֵ
	/*
	@param	pName	in���ӽڵ�����
	*/
	TXWeibo_API unsigned int	GetUint(const char* pName);

	/// ������Ż�ȡ�ӽڵ�Ƿ�������ֵ
	/*
		@param	nIndex	in���ӽڵ����
	*/
	TXWeibo_API unsigned int	GetUint(const int nIndex);

	/// �������ֻ�ȡ�ӽڵ�64λ�Ƿ�������ֵ
	/*
		@param	pName	in���ӽڵ�����
	*/
	TXWeibo_API long long	GetUint64(const char* pName);

	/// ������Ż�ȡ�ӽڵ�64λ�Ƿ�������ֵ
	/*
		@param	nIndex	in���ӽڵ����
	*/
	TXWeibo_API long long	GetUint64(const int nIndex);

private:
	static XmlParser	GetReadOnlyReader(TiXmlElement* pRoot);
	TiXmlElement*		GetChildByName(const char* pName);
	TiXmlElement*		GetChildByIndex(const int nIndex);

private:
	
	TiXmlElement*		m_pRoot;		///< xml�ļ����ڵ�	
	bool				m_bReadOnly;	///< �Ƿ�ֻ��	
	bool				m_bDeleteSelf;	///< �Ƿ���Ҫ�Լ������ڴ��ͷ�
};
