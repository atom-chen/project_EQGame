#pragma once
#include "NetPacket.h"
#include "EQMsgManager.h"
#include "EQGameMgr.h"
#include "CommFunc.h"

class Timekeeper
{
public:
	Timekeeper(void):mTimeDiff(0)
	{
	}

	~Timekeeper(void)
	{
	}

public:
	static Timekeeper& getSingleton()
	{
		static Timekeeper time;
		return time;
	}
	
	void update()
	{
		static DWORD lastTime = GetTickCount()
					,elapsed = 0;
		
		elapsed += GetTickCount() - lastTime;

		int seconds = elapsed / 1000;
		elapsed = elapsed - seconds * 1000;

		if(seconds > 0)
			_updateTime(seconds);

		lastTime = GetTickCount();
	}

	void reqServerTime()
	{
		static req_sys_time systime;
		sGameMgr.getEQMsgManager()->Send(systime);
	}

	void recvServerTime()
	{
		notify_sys_time sysTime;
		sysTime.decode(sGameMgr.getEQMsgManager()->getRecebuff());
// 		LOGERR("*****************recvServerTime******************");
// 		LOGERR(Ogre::StringConverter::toString(time.sys_time.year).c_str());
// 		LOGERR(Ogre::StringConverter::toString(time.sys_time.month).c_str());
// 		LOGERR(Ogre::StringConverter::toString(time.sys_time.day).c_str());
// 		LOGERR(Ogre::StringConverter::toString(time.sys_time.hour).c_str());
// 		LOGERR(Ogre::StringConverter::toString(time.sys_time.minute).c_str());
// 		LOGERR(Ogre::StringConverter::toString(time.sys_time.second).c_str());
// 		LOGERR("*****************recvServerTime******************");
		mServer = sysTime.sys_time;
		// 计算客户端与服务器的时差(add by hezhr)
		tm server;
		server.tm_year = sysTime.sys_time.year - 1900;
		server.tm_mon = sysTime.sys_time.month - 1;
		server.tm_mday = sysTime.sys_time.day;
		server.tm_hour = sysTime.sys_time.hour;
		server.tm_min = sysTime.sys_time.minute;
		server.tm_sec = sysTime.sys_time.second;
		time_t serverTime = mktime(&server);
		time_t clientTime = time(NULL); 
		mTimeDiff = (long)difftime(serverTime, clientTime);
	}

	const stime& getTimeNow()
	{
		return mServer;
	}
	std::string getTimeNowTolua()
	{
		return FormatDateTime("yyyy,mm,dd,hh,nn,ss,xxx",	mServer.year,
															mServer.month,
															mServer.day,
															mServer.hour,
															mServer.minute,
															mServer.second,
															0);
	}
	// 获取客户端和服务器的时差(add by hezhr)
	long getTimeDiff(void)
	{
		return mTimeDiff;	//<0（服务器时间<客户端时间），>0（服务器时间>客户端时间）
	}
	// 获取服务端时间(add by hezhr)
	long getServerTime(void)
	{
		return (time(NULL) + getTimeDiff());
	}

	int calculElapsedSecs(const stime& last, const stime& now)
	{
		int ret;

		ret = _totalSeconds(now) - _totalSeconds(last); 


		return (int)ret;
	}
//此算法有问题，具体什么问题没看o(∩_∩)o 
	DWORD _totalSeconds(const stime& time)
	{
		return 
			time.second + 
			time.minute * 60 +
			time.hour * 3600 + 
			time.day * 24 * 3600 + 
			(time.month * 31
			- (int)(time.month / 2)
			- (time.month >=2 ? 2 : 0))
			* 24 * 3600;//todo year
	}

private:
	void _updateTime(int seconds)
	{
		mServer.second += seconds;
		if (mServer.second >= 60)
		{
			int mins = mServer.second / 60;
			mServer.second -= mins * 60;

			mServer.minute += mins;
			if (mServer.minute >= 60)
			{
				int hours = mServer.minute / 60;
				mServer.minute -= hours * 60;

				mServer.hour += hours;
				if (mServer.hour >= 24)
				{
					int days = mServer.hour / 24;
					mServer.hour -= days * 24;

					mServer.day += days;
					//todo...
				}
			}
		}
// 		LOGERR("*****************_updateTime******************");
// 		LOGERR(Ogre::StringConverter::toString(mServer.year).c_str());
// 		LOGERR(Ogre::StringConverter::toString(mServer.month).c_str());
// 		LOGERR(Ogre::StringConverter::toString(mServer.day).c_str());
// 		LOGERR(Ogre::StringConverter::toString(mServer.hour).c_str());
// 		LOGERR(Ogre::StringConverter::toString(mServer.minute).c_str());
// 		LOGERR(Ogre::StringConverter::toString(mServer.second).c_str());
// 		LOGERR("*****************_updateTime******************");


	}

	stime	mServer;
	long	mTimeDiff;	//客户端与服务器的时差(add by hezhr)
};

#define sTimekeeper Timekeeper::getSingleton()