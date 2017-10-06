/**********************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: 游戏变量控制
*		可以放一些游戏设置
**********************************************************************/
#ifndef Variable_H
#define Variable_H

#include "EQMainPrerequisites.h"


class VariableSystem : public Ogre::Singleton<VariableSystem>
{
public:
	struct VARIABLE
	{
		std::string	vValue;	//变量值
		BOOL	bTemp;	//临时变量
	};
#ifdef __SGI_STL_PORT
	typedef std::hash_map< STRING, VARIABLE >	VARIABLE_MAP;
#else
	typedef stdext::hash_map< std::string, VARIABLE >	VARIABLE_MAP;
#endif


	//得到某个变量的值，如果不存在，则返回FALSE
	virtual BOOL	GetVariable(LPCTSTR szName, 
								std::string& strValue)		;	
	//设置某个变量的值，如果不存在，则首先创建
	virtual VOID	SetVariable(LPCTSTR szName, 
								LPCTSTR szValue, 
								BOOL bTemp=TRUE, 
								BOOL bFireEvent=TRUE,
								VARIABLE_MAP* map=NULL)	;
	//设置某个变量的值，但并不马上生效, 下一次启动时才会生效
	virtual VOID	SetVariableDelay(LPCTSTR szName, 
									 LPCTSTR szValue)	;

	//-------------------------------------------------------------------
	//快速设置通道

	//整数
	virtual VOID	SetAs_Int(LPCTSTR szName, 
							  INT nValue, 
							  BOOL bTemp=TRUE)				;
	//浮点数
	virtual VOID	SetAs_Float(LPCTSTR szName, 
								FLOAT fValue, 
								BOOL bTemp=TRUE)			;
	//Vector2
	virtual VOID	SetAs_Vector2(LPCTSTR szName, 
								  FLOAT fX, 
								  FLOAT fY, 
								  BOOL bTemp=TRUE)	;

	//-------------------------------------------------------------------
	//快速获取通道

	//字符串, DEF=""
	virtual const std::string&	GetAs_String(LPCTSTR szName, 
											 BOOL* bHave = NULL)	;
	//整数, DEF=0
	virtual INT				GetAs_Int(LPCTSTR szName, 
									  BOOL* bHave = NULL)		;
	//浮点数, DEF=0.0f
	virtual FLOAT			GetAs_Float(LPCTSTR szName, 
										BOOL* bHave = NULL)		;
	//Vector2, DEF=(0.0f, 0.0f)
	virtual Ogre::Vector2		GetAs_Vector2(LPCTSTR szName, 
											  BOOL* bHave = NULL)	;

	//-------------------------------------------------------------------
	//设置尚未设置的变量,如果该变量已经定义，此函数无效
	virtual void			SetVariableDefault(LPCTSTR szName, 
											   LPCTSTR szValue);

	//从文件中读取变量
	BOOL	LoadVariable(LPCTSTR szFileName, VARIABLE_MAP& mapBuf);
	//将变量保存到文件中
	VOID	SaveVariable(LPCTSTR szFileName, VARIABLE_MAP& mapBuf);

	virtual VOID	Initial(std::string cfgName="..\\config\\EQSystem.cfg");
	virtual VOID	Release();

protected:
	//当前保存的变量
	VARIABLE_MAP		m_mapCurrent;
	//游戏退出时更改并保存的变量,以便下一次游戏启动生效
	VARIABLE_MAP		m_mapNeedSave;

public:
	std::string		mCfgName;

	VariableSystem(){}
	virtual ~VariableSystem(){}
};

#define sVariableSystem VariableSystem::getSingleton()
#endif