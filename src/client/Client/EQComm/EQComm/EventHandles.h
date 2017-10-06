/*********************************************************************
* 作者: 麦_Mike
* 时间: 20010-5-31
* 描述: 游戏事件处理
**********************************************************************/
#include "GameEventMgr.h"

#define sGEMgr GameEventMgr::getSingleton()

class EventHandles
{
public:
	static void init(){
		subscribeAll();
	}

	static void subscribeAll()
	{
		//sGEMgr.subscribeEvent("UPDATE_NPC_UI", &test);
	}

	static int test(void* args)
	{
		TestArgs* arg = (TestArgs*)(args);

		return 1;
	}
};