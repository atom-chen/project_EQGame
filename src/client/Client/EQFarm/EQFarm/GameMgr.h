/*********************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: ��Ϸ����
*		
**********************************************************************/
#ifndef GameMgr_H
#define GameMgr_H


#include "EQGameMgr.h"

class GameMgr : public EQGameMgr
{
public:	
	GameMgr() {}
	~GameMgr(void) {}

	virtual bool init(HINSTANCE h);
	virtual bool update(bool active);
	virtual void cleanup();
	virtual EQMsgManager* getEQMsgManager();

};

#endif