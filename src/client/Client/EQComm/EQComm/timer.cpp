#include "EQCommPCH.h"

#include "timer.h"
#include "NetPacket.h"
#include "Timekeeper.h"
#include "CommonHelper.h"



enum TIMER_STATUS
{
	TS_RUNNING,		// ��ʱ����������
	TS_STOP,		// ��ʱ��û������
	TS_DELETE		// ɾ����ʱ��
};

struct Timer
{
	int flag;		  // ��ʱ����Ψһ���
	int currentCount; // ��ʱ���� 0 ��ʼ�󴥷����ܴ�����
	int delay;        // ��ʱ���¼�����ӳ٣��Ժ���Ϊ��λ����
	int repeatCount;  // ���õļ�ʱ�������ܴ�����
	int status;       // ��ʱ���ĵ�ǰ״̬��
	int startTime;    // ��ʱ����ʼ��ʱ��
	TIMERCB pFun[TE_MAX_COUNT];	
	unsigned long param1; // ����1
	unsigned long param2; // ����2
};

int addTimer(Timer *pTimer);

Timer * createTimer(int delay, int repeatCount /*=0*/, unsigned long param1/*=0*/, unsigned long param2 /*=0*/)
{
	assert(delay >= 0);
	assert(repeatCount >= 0);

	Timer *pTimer = new Timer;
	pTimer->currentCount = 0;
	pTimer->delay = delay;
	pTimer->repeatCount = repeatCount;
	pTimer->status = TS_STOP;
	pTimer->param1 = param1;
	pTimer->param2 = param2;
	memset(pTimer->pFun, 0, sizeof(TIMERCB) * TE_MAX_COUNT);

	pTimer->flag = addTimer(pTimer);
	
	return pTimer;
}

Timer * createTimer( stime& when, int repeatCount /*= 0*/, unsigned long param1 /*= 0*/, unsigned long param2 /*= 0 */ )
{
	stime now = Timekeeper::getSingleton().getTimeNow();
	int dt = CommonHelper::diffTime(&now, &when);
	assert(dt >= 0);
	return createTimer(dt, repeatCount, param1, param2);
}

int startTimer( Timer *pTimer )
{
	assert(pTimer);
	pTimer->status = TS_RUNNING;
	pTimer->startTime = clock();
	return 0;
}

int stopTimer( Timer *pTimer )
{
	assert(pTimer);
	if (pTimer)
	{
		if (pTimer->pFun[TE_COMPLETE])
			pTimer->pFun[TE_COMPLETE](pTimer, pTimer->param1, pTimer->param2);
		pTimer->status = TS_DELETE;
	}

	return 0;
}

int deleteTimer( Timer *pTimer )
{
	assert(pTimer);
	if (pTimer)
		delete pTimer;

	return 0;
}

bool isTimerRunning( Timer *pTimer )
{
	assert(pTimer);
	return pTimer->status == TS_RUNNING;
}

bool isTimerDelete( Timer *pTimer )
{
	assert(pTimer);
	return pTimer->status == TS_DELETE;
}

int getTimerCurrentCount( Timer *pTimer )
{
	assert(pTimer);
	return pTimer->currentCount;
}

void triggerTimer(Timer *pTimer, int dt)
{
	assert(pTimer);
	if (pTimer->pFun[TE_TIMER])
		pTimer->pFun[TE_TIMER](pTimer, pTimer->param1, pTimer->param2);
}


int addTimerListener(Timer *pTimer, TIMER_EVENT event, TIMERCB pFun)
{
	assert(pTimer);
	pTimer->pFun[event] = pFun;
	return 0;
}

int removeTimerListener( Timer *pTimer, TIMER_EVENT event )
{
	assert(pTimer);
	pTimer->pFun[event] = 0;
	return 0;
}


//-------------------------------------------------------------------
int lastTime;
std::map<int, Timer*> timers;

int runningTimerMgr()
{
	std::map<int, Timer*>::iterator it = timers.begin();
	int time = clock();
	if (lastTime == time)
		return 0;

	while (it != timers.end())
	{
		Timer *pTime = it->second;
		if (isTimerRunning(pTime))
		{
			int repeatCount = pTime->repeatCount;
			if (repeatCount == 0 || pTime->currentCount < repeatCount)
			{
				int dt = time - (pTime->startTime + pTime->delay * (pTime->currentCount+1));
				if (dt >= 0)
				{
					pTime->currentCount++;
					triggerTimer(pTime, dt);
				}
			}
			else
				stopTimer(pTime);
		}

		if (isTimerDelete(pTime))
		{
			deleteTimer(pTime);
			it = timers.erase(it);
		}
		else
			++it;
	}

	lastTime = time;
	return 0;
}

int finishTimerMgr()
{
	std::map<int, Timer*>::iterator it = timers.begin();
	for (; it != timers.end(); ++it)
	{
		stopTimer(it->second);
		deleteTimer(it->second);
	}
	timers.clear();
	return 0;
}

static int g_timer_flag = 0;
int addTimer(Timer *pTimer)
{
	timers.insert(std::pair<int, Timer*>(++g_timer_flag, pTimer));
	return g_timer_flag;
}

Timer *findTimer(int timer_flag)
{
	std::map<int, Timer*>::iterator it = timers.find(timer_flag);
	if (it == timers.end())
		return NULL;
	return it->second;
}

int getTimerFlag(Timer* pTimer)
{
	return pTimer->flag;
}
