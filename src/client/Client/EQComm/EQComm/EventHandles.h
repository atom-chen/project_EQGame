/*********************************************************************
* ����: ��_Mike
* ʱ��: 20010-5-31
* ����: ��Ϸ�¼�����
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