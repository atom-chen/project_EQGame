/*==============================================================================
    EQ GAME 
    Copyright (c) 2009 Mr.Q All rights reserved.

	filename: 	LuaManager.h
	created:	Mr.Q 2009-10-27 22:46
	purpose:	��Ϸ�ű�������
 ==============================================================================*/
#pragma once

#include "EQWndProc.h"

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include <string>
#include "ByteArray.h"
#include "INetPacket.h"

#define EQ_GLOBA_FUNC m_pLuaState, "EQGlobaFunc"
#define EQ_GLOBA_CLASS m_pLuaState, "EQGlobaClass"

#include "NetPacket.h"

namespace CEGUI
{
	class LuaScriptModule;
}

class CLuaManager
{
public:
	virtual ~CLuaManager();

	static CLuaManager& getSingleton(void);
	static CLuaManager* getSingletonPtr(void);

public:
	bool Initialize();
	void Terminate();
	lua_State*	getL(){return 	m_pLuaState;}

	void LoadState(const char* name);
	void CallFunction(std::string szFun, int args = -1);
	//
	void CallFunction(std::string szFun, int msgtype, INetPacket* ba);
	void CallFunction(std::string szFun, string& str);
	//error call back faction
	void SetHandler(void (*handler)(const char* buffer))
	{
		m_Handler = handler;
	}

protected:
	CLuaManager();

	/*ע����������*/
	bool RegisterAll();

	/*ע��ȫ�ֺ���*/
	bool RegisterGlobalFun();

	/*ע����*/
	bool RegisterGlobalClass();
	//ע��������Ϣ
	bool RegisterNetMsgClass();
	//ע��GameObj���������
	bool RegisterGameObjClass();
	//��Ʒ
	bool RegisterGameItemClass();
	//����
	bool RegisterPlayerHouseClass();
	//����
	bool RegisterChatClass();
	//NPC
	bool RegisterNPCClass();
	//farm
	bool RegisterFarmClass();
	//Сģ��
	bool RegisterSmallModulesClass();
	//�ʼ�
	bool RegisterMailManagerClass();
	//CEGUI��չ�ؼ�
	bool RegisterCeguiExtentControlClass();
	//��Ʒʹ��
	bool RegisterUseItemFun();

	//
	bool RegisterCursorMgrClass();

	bool RegisterGiftSystemClass();
	
	bool RegisterConfigOptionClass();

	bool RegisterVoiceMgrClass();

	bool RegisterDriftBottleSysClass();

private:
	lua_State*			m_pLuaState;
	CEGUI::LuaScriptModule* m_pLuaScriptModule;
	static CLuaManager* m_pInstance;

	void (*m_Handler)(const char* buffer);
};
#define sLuaMgr CLuaManager::getSingleton()