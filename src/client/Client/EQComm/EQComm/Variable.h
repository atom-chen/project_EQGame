/**********************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: ��Ϸ��������
*		���Է�һЩ��Ϸ����
**********************************************************************/
#ifndef Variable_H
#define Variable_H

#include "EQMainPrerequisites.h"


class VariableSystem : public Ogre::Singleton<VariableSystem>
{
public:
	struct VARIABLE
	{
		std::string	vValue;	//����ֵ
		BOOL	bTemp;	//��ʱ����
	};
#ifdef __SGI_STL_PORT
	typedef std::hash_map< STRING, VARIABLE >	VARIABLE_MAP;
#else
	typedef stdext::hash_map< std::string, VARIABLE >	VARIABLE_MAP;
#endif


	//�õ�ĳ��������ֵ����������ڣ��򷵻�FALSE
	virtual BOOL	GetVariable(LPCTSTR szName, 
								std::string& strValue)		;	
	//����ĳ��������ֵ����������ڣ������ȴ���
	virtual VOID	SetVariable(LPCTSTR szName, 
								LPCTSTR szValue, 
								BOOL bTemp=TRUE, 
								BOOL bFireEvent=TRUE,
								VARIABLE_MAP* map=NULL)	;
	//����ĳ��������ֵ��������������Ч, ��һ������ʱ�Ż���Ч
	virtual VOID	SetVariableDelay(LPCTSTR szName, 
									 LPCTSTR szValue)	;

	//-------------------------------------------------------------------
	//��������ͨ��

	//����
	virtual VOID	SetAs_Int(LPCTSTR szName, 
							  INT nValue, 
							  BOOL bTemp=TRUE)				;
	//������
	virtual VOID	SetAs_Float(LPCTSTR szName, 
								FLOAT fValue, 
								BOOL bTemp=TRUE)			;
	//Vector2
	virtual VOID	SetAs_Vector2(LPCTSTR szName, 
								  FLOAT fX, 
								  FLOAT fY, 
								  BOOL bTemp=TRUE)	;

	//-------------------------------------------------------------------
	//���ٻ�ȡͨ��

	//�ַ���, DEF=""
	virtual const std::string&	GetAs_String(LPCTSTR szName, 
											 BOOL* bHave = NULL)	;
	//����, DEF=0
	virtual INT				GetAs_Int(LPCTSTR szName, 
									  BOOL* bHave = NULL)		;
	//������, DEF=0.0f
	virtual FLOAT			GetAs_Float(LPCTSTR szName, 
										BOOL* bHave = NULL)		;
	//Vector2, DEF=(0.0f, 0.0f)
	virtual Ogre::Vector2		GetAs_Vector2(LPCTSTR szName, 
											  BOOL* bHave = NULL)	;

	//-------------------------------------------------------------------
	//������δ���õı���,����ñ����Ѿ����壬�˺�����Ч
	virtual void			SetVariableDefault(LPCTSTR szName, 
											   LPCTSTR szValue);

	//���ļ��ж�ȡ����
	BOOL	LoadVariable(LPCTSTR szFileName, VARIABLE_MAP& mapBuf);
	//���������浽�ļ���
	VOID	SaveVariable(LPCTSTR szFileName, VARIABLE_MAP& mapBuf);

	virtual VOID	Initial(std::string cfgName="..\\config\\EQSystem.cfg");
	virtual VOID	Release();

protected:
	//��ǰ����ı���
	VARIABLE_MAP		m_mapCurrent;
	//��Ϸ�˳�ʱ���Ĳ�����ı���,�Ա���һ����Ϸ������Ч
	VARIABLE_MAP		m_mapNeedSave;

public:
	std::string		mCfgName;

	VariableSystem(){}
	virtual ~VariableSystem(){}
};

#define sVariableSystem VariableSystem::getSingleton()
#endif