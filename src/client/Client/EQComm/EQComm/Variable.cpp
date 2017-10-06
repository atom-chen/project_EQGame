#include "EQCommPCH.h"

#include "Variable.h"

//------------------------------------------------------------------
INT ConvertSectionInVector(LPCTSTR szSectionBuf, 
						   INT nLen, 
						   std::vector< std::string >& vSectionBuf);
//-------------------------------------------------------------------
template<> VariableSystem* 
			Ogre::Singleton<VariableSystem>::ms_Singleton = 0;
//----------------------------------------------------------------
BOOL VariableSystem::LoadVariable(LPCTSTR szFileName, 
								   VARIABLE_MAP& mapBuf)
{
	mapBuf.clear();

	//取得配置文件的大小
	HANDLE hFile = CreateFile(szFileName,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(INVALID_HANDLE_VALUE == hFile) 
		return FALSE;

	DWORD dwHigh;
	DWORD dwFileSize = GetFileSize(hFile, &dwHigh);
	CloseHandle(hFile); hFile = NULL;
	if(0==dwFileSize) 
		return FALSE;

	//分配足够的内存
	CHAR* pTempBuf = new CHAR[dwFileSize+32];
	if(!pTempBuf) 
		return FALSE;

	BOOL rtn = FALSE;

	//从配置文件中读取"Variable"节
	if(GetPrivateProfileSection("Variable", pTempBuf, dwFileSize, szFileName))
	{
		//分解
		std::vector< std::string > vRet;
		if(ConvertSectionInVector(pTempBuf, dwFileSize, vRet))
		{

			//加入变量定义
			for(INT i=0; i<(INT)vRet.size(); i++)
			{
				std::string& strLine = vRet[i];

				if(strLine.find_first_of("#") != std::string.npos)
					continue;

				std::string::size_type tEqu = strLine.find_first_of("= \t");
				if(tEqu == std::string::npos)
					continue;

				std::string strName = strLine.substr(0, tEqu);

				CHAR szTemp[1025];
				::GetPrivateProfileString("Variable", strName.c_str(), "", szTemp, 1024, szFileName);

				SetVariable(strName.c_str(), szTemp, FALSE, TRUE, &mapBuf);
			}

			rtn = TRUE;
		}
	}

	delete[] pTempBuf;

	return rtn;
}

//------------------------------------------------------------------
VOID VariableSystem::SaveVariable(LPCTSTR szFileName, 
								   VARIABLE_MAP& mapBuf)
{
	const CHAR* szSystemCfg = szFileName;
	UINT dwFileAttr = GetFileAttributes(szSystemCfg);

	if(FILE_ATTRIBUTE_READONLY & dwFileAttr)
	{
		SetFileAttributes(szSystemCfg, 
			dwFileAttr&(~FILE_ATTRIBUTE_READONLY));
	}
	VARIABLE_MAP::iterator it;
	for(it=mapBuf.begin(); it!=mapBuf.end(); it++)
	{
		if(it->second.bTemp == FALSE)
		{
			::WritePrivateProfileString("Variable", 
										it->first.c_str(), 
										it->second.vValue.c_str(), 
										szSystemCfg);
		}
	}
	if(FILE_ATTRIBUTE_READONLY & dwFileAttr)
	{
		SetFileAttributes(szSystemCfg, dwFileAttr);
	}

}

//----------------------------------------------------------------------
BOOL VariableSystem::GetVariable(LPCTSTR szName, std::string& strValue)
{
	VARIABLE_MAP::iterator itFind = m_mapCurrent.find(szName);
	if(itFind == m_mapCurrent.end()) return FALSE;

	strValue = itFind->second.vValue;
	return true;
}

//---------------------------------------------------------------------
VOID VariableSystem::SetVariable(LPCTSTR szName, 
								  LPCTSTR szValue, 
								  BOOL bTemp, 
								  BOOL bFireEvent,
								  VARIABLE_MAP* map)
{
	if(map == NULL)
		map = &m_mapCurrent;

	VARIABLE varOld = (*map)[szName];
	VARIABLE new_Variable;

	new_Variable.bTemp = bTemp;
	new_Variable.vValue = szValue;

	(*map)[szName] = new_Variable;

	if(varOld.vValue != szValue && bFireEvent)
	{
		//产生事件
		std::vector< std::string > vParam;
		vParam.push_back(szName);
		vParam.push_back(szValue);
		//CEventSystem::GetMe()->PushEvent(GE_VARIABLE_CHANGED, vParam);
	}
}
//----------------------------------------------------------------------
VOID VariableSystem::SetAs_Float(LPCTSTR szName, 
								  FLOAT fValue, 
								  BOOL bTemp)
{
	CHAR szTemp[32];
	_snprintf(szTemp, 32, "%f", fValue);
	SetVariable(szName, szTemp, bTemp);
}
//---------------------------------------------------------------------
VOID VariableSystem::SetAs_Int(LPCTSTR szName, 
								INT nValue, 
								BOOL bTemp)
{
	CHAR szTemp[32];
	_snprintf(szTemp, 32, "%d", nValue);
	SetVariable(szName, szTemp, bTemp);
}
//---------------------------------------------------------------------
VOID VariableSystem::SetAs_Vector2(LPCTSTR szName, 
									FLOAT fX, 
									FLOAT fY, 
									BOOL bTemp)
{
	CHAR szTemp[64];
	_snprintf(szTemp, 32, "%f,%f", fX, fY);
	SetVariable(szName, szTemp, bTemp);
}
//-------------------------------------------------------------------
VOID VariableSystem::SetVariableDelay(LPCTSTR szName, 
									   LPCTSTR szValue)
{
	m_mapNeedSave[szName].bTemp = FALSE;
	m_mapNeedSave[szName].vValue = szValue;
}
//--------------------------------------------------------------------
VOID VariableSystem::Initial(std::string cfgName)
{
	mCfgName = cfgName;
	LoadVariable(cfgName.c_str(), m_mapCurrent);
	m_mapNeedSave.clear();
}
//--------------------------------------------------------------------
VOID VariableSystem::Release()
{
	//should save m_mapNeedSave ...
	SaveVariable(mCfgName.c_str(), m_mapCurrent);
	SaveVariable(mCfgName.c_str(), m_mapNeedSave);

}

//---------------------------------------------------------------------
const std::string& VariableSystem::GetAs_String(LPCTSTR szName, 
												 BOOL* bHave)
{
	VARIABLE_MAP::iterator itFind = m_mapCurrent.find(szName);
	if(itFind == m_mapCurrent.end())
	{
		static std::string s_strBlank = "";
		if(bHave) *bHave = FALSE;
		return s_strBlank;
	}

	if(bHave) *bHave = true;
	return itFind->second.vValue;
}

//----------------------------------------------------------------
INT	VariableSystem::GetAs_Int(LPCTSTR szName, BOOL* bHave)
{
	VARIABLE_MAP::iterator itFind = m_mapCurrent.find(szName);
	if(itFind == m_mapCurrent.end())
	{
		if(bHave) *bHave = FALSE;
		return 0;
	}

	if(bHave) *bHave = true;
	return atoi(itFind->second.vValue.c_str());
}

//-----------------------------------------------------------------
FLOAT VariableSystem::GetAs_Float(LPCTSTR szName, BOOL* bHave)
{
	VARIABLE_MAP::iterator itFind = m_mapCurrent.find(szName);
	if(itFind == m_mapCurrent.end())
	{
		if(bHave) *bHave = FALSE;
		return 0.0f;
	}

	if(bHave) *bHave = true;
	return (FLOAT)atof(itFind->second.vValue.c_str());
}
//----------------------------------------------------------------
Ogre::Vector2 VariableSystem::GetAs_Vector2(LPCTSTR szName, 
											 BOOL* bHave)
{
	static Ogre::Vector2 fvNULL(0.0f, 0.0f);

	VARIABLE_MAP::iterator itFind = m_mapCurrent.find(szName);
	if(itFind == m_mapCurrent.end())
	{
		if(bHave) *bHave = FALSE;
		return fvNULL;
	}

	if(bHave) *bHave = true;

	CHAR szTemp[64];
	strncpy(szTemp, itFind->second.vValue.c_str(), 64);

	char* pDot = strchr(szTemp, ',');
	if(!pDot) return fvNULL;
	*pDot = '\0';

	return Ogre::Vector2((FLOAT)atof(szTemp), (FLOAT)atof(pDot+1));
}
//---------------------------------------------------------------------
void VariableSystem::SetVariableDefault(LPCTSTR szName, LPCTSTR szValue)
{
	std::string strOld;
	BOOL bExist = GetVariable(szName, strOld);
	if(bExist) return;

	SetVariable(szName, szValue);
}

//----------------------------------------------------------------------
INT ConvertSectionInVector(LPCTSTR szSectionBuf, 
						   INT nLen, 
						   std::vector< std::string >& vSectionBuf)
{
	if ( nLen == 0 || szSectionBuf[0] == '\0' )
		return 0;

	register const char *pDot = szSectionBuf;

	vSectionBuf.clear();

	while(TRUE)
	{
		if( (*pDot == '\0' && *(pDot-1) == '\0') 
			|| (pDot-szSectionBuf >= nLen) )
		{
			break;
		}

		if(*pDot == '\0')
		{
			pDot++;
			continue;
		}

		std::string strTheSingle = pDot;
		vSectionBuf.push_back(strTheSingle);

		pDot += strlen(pDot)+1;
	}

	return (INT)vSectionBuf.size();
}
