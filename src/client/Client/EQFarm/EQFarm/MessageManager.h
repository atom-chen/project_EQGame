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

	//重复登入
	bool handleLoginRepeat();
	//农场
	bool handleByFarm();
};

#define sMsgMgr CMessageManager::getSingleton()

#endif