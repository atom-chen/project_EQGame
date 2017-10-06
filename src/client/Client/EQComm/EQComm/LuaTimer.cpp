// 作者: lyj

#include "EQCommPCH.h"
#include "timer.h"
#include "LuaManager.h"
#include "luabind/luabind/luabind.hpp"


void luaTimerTiming(Timer *pTimer, unsigned long param1, unsigned long param2)
{
	int flag = getTimerFlag(pTimer);
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "timerEvent", (int)TE_TIMER, flag);
}

void luaTimerComplete(Timer *pTimer, unsigned long param1, unsigned long param2)
{
	int flag = getTimerFlag(pTimer);
	luabind::call_function<void>(CLuaManager::getSingleton().getL(), "timerEvent", (int)TE_COMPLETE, flag);
}

// ---------- 以下函数需要注册给lua调用------------------------------------

int createLuaTimer(int delay, int repeatCount)
{
	Timer *pTimer = createTimer(delay, repeatCount);
	addTimerListener(pTimer, TE_TIMER, luaTimerTiming);
	addTimerListener(pTimer, TE_COMPLETE, luaTimerComplete);
	return getTimerFlag(pTimer);
}

void startLuaTimer(int flag)
{
	Timer *pTimer = findTimer(flag);
	assert(pTimer);
	startTimer(pTimer);
}

void stopLuaTimer(int flag)
{
	Timer *pTimer = findTimer(flag);
	assert(pTimer);
	stopTimer(pTimer);
}

bool isLuaTimerRunning(int flag)
{
	Timer *pTimer = findTimer(flag);
	assert(pTimer);
	return isTimerRunning(pTimer);
}

int getLuaTimerCurrentCount(int flag)
{
	Timer *pTimer = findTimer(flag);
	assert(pTimer);
	return getTimerCurrentCount(pTimer);
}


