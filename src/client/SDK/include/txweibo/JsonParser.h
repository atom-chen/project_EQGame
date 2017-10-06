/** 
@file  
@brief		json����ʵ����
@version	2010-11-18		
*/

#pragma once
#include "Parser/json/json.h"
#ifndef TXWeibo_API 
#define TXWeibo_API  __declspec(dllimport)
#endif

class JsonParser
{
public:
	TXWeibo_API JsonParser();
	TXWeibo_API ~JsonParser();

	/// �жϵ�ǰparser�Ƿ�Ϊreadonly��Ϊreadonlyʱ�����ܵ���Parse�ӿڣ�����ʧ��
	TXWeibo_API bool	IsReadyOnly();

	/// ����json�ļ�
	/*
	@param	pJsonContent	in��json�ַ���������Ϊutf8
	*/
	TXWeibo_API bool	Parse(const char* pJsonContent);

	/// �������ֻ�ȡ�ӽڵ�
	/*
	@param	pName	in���ӽڵ�����
	*/
	TXWeibo_API JsonParser	GetChild(const char* pName); 

	/// ������Ż�ȡ�ӽڵ�
	/*
	@param	nIndex	in���ӽڵ����
	*/
	TXWeibo_API JsonParser	GetChild(const int nIndex);

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
	TXWeibo_API int		GetInt(const char* pName);

	/// ������Ż�ȡ�ӽڵ�����ֵ
	/*
	@param	nIndex	in���ӽڵ����
	*/
	TXWeibo_API int		GetInt(const int nIndex);

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
	static JsonParser	GetReadOnlyReader(Json::Value* pRoot);
	Json::Value*		GetChildByName(std::string& strName);
	Json::Value*		GetChildByIndex(const int nIndex);


private:	
	Json::Value*		m_pRoot;		///<  json�ļ����ڵ�
	bool				m_bReadOnly;	///<  �Ƿ�ֻ��
	bool				m_bDeleteSelf;	///<  �Ƿ���Ҫ�Լ������ڴ��ͷ�
};
