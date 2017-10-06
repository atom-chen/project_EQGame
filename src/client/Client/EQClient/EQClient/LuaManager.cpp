#include "EQClientPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "CEGUI.h"
#include "CEGUILua.h"

#include "LuaManager.h"
#include "GameEventMgr.h"

#include "EQLog.h"
#include "EQOgreSys.h"
#include "ConfigOption.h"

using namespace luabind;
CLuaManager* CLuaManager::m_pInstance = NULL;
//-------------------------------------------------------------------------------
CLuaManager::CLuaManager()
:m_pLuaState(NULL)
,m_pLuaScriptModule(NULL)
{

}

CLuaManager::~CLuaManager()
{
	//lua_close(myLuaState);
	if ( m_pLuaScriptModule )
		delete m_pLuaScriptModule;

}

CLuaManager* CLuaManager::getSingletonPtr(void)
{
	if (!m_pInstance)
		m_pInstance = new CLuaManager();

	return m_pInstance;
}

CLuaManager& CLuaManager::getSingleton(void)
{  
	if (!m_pInstance)
		m_pInstance = new CLuaManager();

	return (*m_pInstance);
}
//----------------------------------------------------------------------------
void fireError(const char* name,lua_State* L)
{
	static string m_Error="";
	m_Error = name;
	m_Error += "\n";
	m_Error += lua_tostring(L , -1);
	printf(m_Error.c_str());
	MessageBoxA( NULL , m_Error.c_str() , "lua error" , MB_OK );
}
//----------------------------------------------------------------------------
// luabind�ص�������������ʾ�ű�����
int callbackError(lua_State* L)
{
	std::string m_Error = lua_tostring(L , -1);
	printf(m_Error.c_str());
	MessageBoxA( NULL , m_Error.c_str() , "luabind error" , MB_OK );
	return 1;
}
//------------------------------------------------------------------------------
bool CLuaManager::Initialize()
{
	// �����ű�ģ��
	m_pLuaScriptModule = new CEGUI::LuaScriptModule();

	// ����CEGUI�ű�ģ��
	CEGUI::System::getSingleton().setScriptingModule(m_pLuaScriptModule);

	// ����״ָ̬��
	m_pLuaState = m_pLuaScriptModule->getLuaState();

	RegisterAll();

	luabind::set_pcall_callback(&callbackError);

	CEGUI::System::getSingleton().executeScriptFile("EQClientMgr.lua"); 

	const char* ip = EQOgreSys::getSingleton().getConfigOption()->getIP();
	CEGUI::WindowManager::getSingleton().getWindow("Login/IP")->setText((CEGUI::utf8*)ip);

	return 1;
}
//----------------------------------------------------------------------------------
void CLuaManager::LoadState(const char* name)
{
	if (luaL_loadfile(m_pLuaState , name) )
	{
		fireError(name,m_pLuaState);
		return;
	}

	int n = lua_pcall(m_pLuaState , 0 , 0 , 0);
	if (n)
	{
		fireError(name,m_pLuaState);
		return;
	}

	printf("Loaded Script %s \n" , name);
}
//----------------------------------------------------------------------------------
void CLuaManager::Terminate()
{
	if (m_pInstance)
	{
		delete m_pInstance;

		m_pInstance = NULL;
	}
}
//---------
void CLuaManager::CallFunction(std::string szFun, string& str)
{
	luabind::call_function<void>(m_pLuaState, szFun.c_str(), str);
}
//----------------------------------------------------------------------------------
void CLuaManager::CallFunction(std::string szFun, int args)
{
	luabind::call_function<int>(m_pLuaState, szFun.c_str(), args);
}
//----------------------------------------------------------------------------------
//void CLuaManager::CallFunction(std::string szFun, GameEventArgs* args)
//{
//	//luabind::call_function<void>(m_pLuaState, szFun.c_str(),args);
//}
//----------------------------------------------------------------------------------
extern std::string getUser();
extern std::string getPwd();
//----------------------------------------------------------------------------------
bool CLuaManager::RegisterAll()
{
	luabind::open(m_pLuaState);

		
	luabind::module(EQ_GLOBA_FUNC)
		[
			luabind::def("getUser",				&getUser),
			luabind::def("getPwd",				&getPwd)
		]
	;

	if (!RegisterGlobalFun())
		return false;

	if (!RegisterGlobalClass())
		return false;

	if (!RegisterUseItemFun())
		return false;

	return true;
}
//-----------------------------------------------------------------------------------
bool CLuaManager::RegisterGlobalClass()
{
/**
ʹ��LuaBind������
*/
	module( EQ_GLOBA_CLASS )[
		/**
		singleton
		*/
		luabind::def("getLuaManager", &CLuaManager::getSingleton),
		luabind::def("getGameEventMgr", &GameEventMgr::getSingleton),

		//luamgr
		class_<CLuaManager>("EQLuaMgr")
			.def("loadState", &CLuaManager::LoadState)
			,
		//entmgr
		class_<GameEventMgr>("GameEventMgr")
			.def("regEvent",&GameEventMgr::regEvent)
			.def("subscribeEvent",(void(GameEventMgr::*)(string,string))&GameEventMgr::subscribeEvent)
			.def("fireEvent",&GameEventMgr::fireEvent)
					
		,
		//luamgr
		class_<EQLog>("EQLog")
			.enum_("LogType")
			[
				value("LOG_TYPE_ALL", EQLog::LOG_TYPE_ALL),
				value("LOG_TYPE_DEBUG", EQLog::LOG_TYPE_DEBUG),
				value("LOG_TYPE_MESSAGE", EQLog::LOG_TYPE_MESSAGE),
				value("LOG_TYPE_WARNING", EQLog::LOG_TYPE_WARNING),
				value("LOG_TYPE_ERROR", EQLog::LOG_TYPE_ERROR),
				value("LOG_TYPE_TRACE", EQLog::LOG_TYPE_TRACE)

			]
			.enum_("LogMode")
			[
				value("LOG_MODE_NONE", EQLog::LOG_MODE_NONE),
				value("LOG_MODE_FILE", EQLog::LOG_MODE_FILE),
				value("LOG_MODE_CONSOLE", EQLog::LOG_MODE_CONSOLE),
				value("LOG_MODE_BOTH", EQLog::LOG_MODE_BOTH)

			]
			.def("setLogMode", &EQLog::setLogMode)
			.def("setLogLevel", &EQLog::setLogLevel)
			.def("setLogFile", &EQLog::setLogFile)
			.def("logDebug", &EQLog::logDebug)
			.def("logMessage", &EQLog::logMessage)
			.def("logWarning", &EQLog::logWarning)
			.def("logError", &EQLog::logError)
			.def("logTrace", &EQLog::logTrace)
		,
		//luamgr
		class_<EQLogManager>("EQLogManager")
		,	
		luabind::def("getLogger",  &EQLogManager::getLogger)
	];
	/**������Ϣ��
	*/
	RegisterNetMsgClass();
	/**
	*/
	RegisterGameItemClass();
	/**
	*/
	RegisterGameObjClass();
	/**
	*/
	RegisterPlayerHouseClass();
	/*
	*/
	RegisterChatClass();
	/*
	*/
	RegisterNPCClass();
	/*
	*/
	RegisterSmallModulesClass();
	/*
	*/
//	RegisterFarmClass();
	/*
	*/
	RegisterMailManagerClass();
	/*CEGUI��չ�ؼ�
	*/
	RegisterCeguiExtentControlClass();

	//
	RegisterCursorMgrClass();
	// ����
	RegisterGiftSystemClass();

	RegisterConfigOptionClass();

	RegisterVoiceMgrClass();

	RegisterDriftBottleSysClass();

	return true;
}