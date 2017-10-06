/******************************************************************************
* 作者: lyj		(add by Y3)
* 时间: 2009-12-17
* 描述: 定时器
******************************************************************************/

#ifndef timer_H
#define timer_H

#include "EQMainPrerequisites.h"


struct Timer;
struct stime;

/* 使用范例
void test2(Timer *pTimer, unsigned long param1, unsigned long param2)
{
printf("time2:%d\n", timeGetTime());
}

void test2_complete(Timer *pTimer, unsigned long param1, unsigned long param2)
{
printf("time2 finish:%d\n", timeGetTime());
}

Timer *pTimer = createTimer(1100, 3);
addTimerListener(pTimer, TE_TIMER, test2);
addTimerListener(pTimer, TE_COMPLETE, test2_complete);
startTimer(pTimer2);
*/

// 定时器事件
typedef enum TIMER_EVENT
{
	TE_TIMER,		// 定时器触发事件
	TE_COMPLETE,    // 定时器结束事件
	TE_MAX_COUNT    // 定时器事件的最大数量
} TIMER_EVENT;

// 定时器回调函数的声明
typedef void (*TIMERCB)(Timer *pTimer, unsigned long Param1, unsigned long Param2);

// 使用指定的 delay 和 repeatCount 状态构造新的 Timer 对象。
// delay — 计时器事件间的延迟（以毫秒为单位）。 
// repeatCount (default = 0) — 指定重复次数。如果为 0，则计时器重复无限次数。
//                                  如果不为 0，则将运行计时器，运行次数为指定的次数，然后停止。
Timer *createTimer(int delay, int repeatCount = 0, unsigned long param1 = 0, unsigned long param2 = 0 );

Timer *createTimer(stime& date, int repeatCount = 0, unsigned long param1 = 0, unsigned long param2 = 0 );

// 如果计时器尚未运行，则启动计时器。
int startTimer(Timer *pTimer);

// 停止计时器
int stopTimer(Timer *pTimer);

// 判断定时器是否正在运行
bool isTimerRunning(Timer *pTimer);

// 返回定时器被触发的次数
int getTimerCurrentCount(Timer *pTimer);

// 注册定时器响应事件
int addTimerListener(Timer *pTimer, TIMER_EVENT event, TIMERCB pFun);

// 移除定时器的响应事件
int removeTimerListener(Timer *pTimer, TIMER_EVENT event);

// -------------------------------------------------------------------------------

// 开始定时管理器的运行
int runningTimerMgr();

// 结束定时管理器
int finishTimerMgr();


// 根据timer的flag查找指定的定时器
Timer *findTimer(int timer_flag);

// 获得定时器的标记, 每个定时器的标记是唯一的
int getTimerFlag(Timer* pTimer);


#endif