/******************************************************************************
* ����: lyj		(add by Y3)
* ʱ��: 2009-12-17
* ����: ��ʱ��
******************************************************************************/

#ifndef timer_H
#define timer_H

#include "EQMainPrerequisites.h"


struct Timer;
struct stime;

/* ʹ�÷���
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

// ��ʱ���¼�
typedef enum TIMER_EVENT
{
	TE_TIMER,		// ��ʱ�������¼�
	TE_COMPLETE,    // ��ʱ�������¼�
	TE_MAX_COUNT    // ��ʱ���¼����������
} TIMER_EVENT;

// ��ʱ���ص�����������
typedef void (*TIMERCB)(Timer *pTimer, unsigned long Param1, unsigned long Param2);

// ʹ��ָ���� delay �� repeatCount ״̬�����µ� Timer ����
// delay �� ��ʱ���¼�����ӳ٣��Ժ���Ϊ��λ���� 
// repeatCount (default = 0) �� ָ���ظ����������Ϊ 0�����ʱ���ظ����޴�����
//                                  �����Ϊ 0�������м�ʱ�������д���Ϊָ���Ĵ�����Ȼ��ֹͣ��
Timer *createTimer(int delay, int repeatCount = 0, unsigned long param1 = 0, unsigned long param2 = 0 );

Timer *createTimer(stime& date, int repeatCount = 0, unsigned long param1 = 0, unsigned long param2 = 0 );

// �����ʱ����δ���У���������ʱ����
int startTimer(Timer *pTimer);

// ֹͣ��ʱ��
int stopTimer(Timer *pTimer);

// �ж϶�ʱ���Ƿ���������
bool isTimerRunning(Timer *pTimer);

// ���ض�ʱ���������Ĵ���
int getTimerCurrentCount(Timer *pTimer);

// ע�ᶨʱ����Ӧ�¼�
int addTimerListener(Timer *pTimer, TIMER_EVENT event, TIMERCB pFun);

// �Ƴ���ʱ������Ӧ�¼�
int removeTimerListener(Timer *pTimer, TIMER_EVENT event);

// -------------------------------------------------------------------------------

// ��ʼ��ʱ������������
int runningTimerMgr();

// ������ʱ������
int finishTimerMgr();


// ����timer��flag����ָ���Ķ�ʱ��
Timer *findTimer(int timer_flag);

// ��ö�ʱ���ı��, ÿ����ʱ���ı����Ψһ��
int getTimerFlag(Timer* pTimer);


#endif