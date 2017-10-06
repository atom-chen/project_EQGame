#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "EQMsgManager.h"

class CMessageManager : public EQMsgManager
{
public:
	static CMessageManager& getSingleton(void)
	{
		static CMessageManager mgr;
		return mgr;
	}

	CMessageManager() {}
	virtual ~CMessageManager(void) {}

protected:
	virtual bool logicProc();

	//�ظ�����
	bool handleLoginRepeat();
	//ũ��
	bool handleByFarm();
};

#define sMsgMgr CMessageManager::getSingleton()

#endif