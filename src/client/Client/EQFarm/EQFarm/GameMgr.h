/*********************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: 游戏管理
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