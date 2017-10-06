/******************************************************************************
* ����: ��չȻ
* ʱ��: 2010-11-4
* ����: ����ϵͳ
******************************************************************************/
#ifndef BroadCastSystem_H
#define BroadCastSystem_H


#include "EQMainPrerequisites.h"
#include "Scroll.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"
#include <Windows.h>


/*************************************************************************
*	����ϵͳ
*************************************************************************/
class BroadCastSystem : public Ogre::Singleton<BroadCastSystem>
{
public:
	BroadCastSystem();
	~BroadCastSystem();
	// ���㹫��
	void saveMsgAT(std::string id, std::string msg, int times);	// ���湫������
	void deleteMsgAT(std::string id);	// ɾ��ָ����Ϣ
	bool isPlayingMsgAT(std::string id);	// �ж��Ƿ����ڲ��ŵ���Ϣ
	void setCalledFuncNameAT(std::string beginName, std::string name, std::string endName);
	void setPlayParamsAT(std::string pMode, int dTime=0, int rTime=0, float sStep=1, int tStep=1);
	void playAT(std::string pare, std::string child);	// ����
	void stopAT(void);	// ֹͣ����
	void saveOldMsgAT(void);
	int playStateAT(void);	// ����״̬
	// GM����
	void saveMsgGM(std::string id, std::string msg, int times);
	void deleteMsgGM(std::string id);
	bool isPlayingMsgGM(std::string id);
	void setCalledFuncNameGM(std::string beginName, std::string name, std::string endName);
	void setPlayParamsGM(std::string pMode, int dTime=0, int rTime=0, float sStep=1, int tStep=1);
	void playGM(std::string pare, std::string child);
	void stopGM(void);
	// �����Ϊ����
	void saveMsgBE(std::string id, std::string msg, int times);
	void setCalledFuncNameBE(std::string beginName, std::string name, std::string endName);
	void setPlayParamsBE(std::string pMode, int dTime=0, int rTime=0, float sStep=1, int tStep=1);
	void playBE(std::string pare, std::string child);
	void stopBE(void);
	// ����
	void update(void);
	// ����̶��Ĺ���
	void reqBroadCast(int type);

protected:
	// ���㹫����Ӧ����
	static int _playBeginAT(void*)
	{
		BroadCastSystem::getSingleton().m_playBeginFlagAT = 1;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_ATGM_PlayState", true);
		return 0;
	}
	static int _playEndAT(void*)
	{
		BroadCastSystem::getSingleton().m_playEndFlagAT = 1;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_ATGM_PlayState", false);
		return 0;
	}
	// GM������Ӧ����
	static int _playBeginGM(void*)
	{
		BroadCastSystem::getSingleton().m_playBeginFlagGM = 1;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_ATGM_PlayState", true);
		return 0;
	}
	static int _playEndGM(void*)
	{
		BroadCastSystem::getSingleton().m_playEndFlagGM = 1;
		luabind::call_function<void>(sLuaMgr.getL(), "BroadCast_ATGM_PlayState", false);
		return 0;
	}
	// �����Ϊ������Ӧ����
	static int _playBeginBE(void*)
	{
		BroadCastSystem::getSingleton().m_playBeginFlagBE = 1;
		return 0;
	}
	static int _playEndBE(void*)
	{
		BroadCastSystem::getSingleton().m_playEndFlagBE = 1;
		return 0 ;
	}

protected:
	// ���㹫��
	Scroll m_broadCastAT;	// ���㹫�����Ч��������
	bool m_playFlagAT;		// ���ű�ʶ��true���ڲ����У�
	int m_playBeginFlagAT;	// ֵΪ��1��0��
	int m_playEndFlagAT;	// ֵΪ��1��0��
	bool m_stopATByGM;		// true����GM��������ֹͣ����false�������ȼ��ߵı����͹���ֹͣ��
	// GM����
	Scroll m_broadCastGM;	// GM�������Ч��������
	bool m_playFlagGM;		// ���ű�ʶ��true���ڲ����У�
	int m_playBeginFlagGM;
	int m_playEndFlagGM;
	// �����Ϊ����
	Scroll m_broadCastBE;	// �����Ϊ������˸Ч��������
	bool m_playFlagBE;		// ���ű�ʶ��true���ڲ����У�
	int m_playBeginFlagBE;
	int m_playEndFlagBE;
	DWORD m_startTimeBE;	// ��ʼ���ŵ�ʱ��

};	//	end of class BroadCastSystem


#endif	//	end of guard BroadCastSystem_H
