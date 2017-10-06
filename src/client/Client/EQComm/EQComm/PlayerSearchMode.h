/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		2/4/2011	15:30
* 文件: 	PlayerSearchMode.h
* 描述:		玩家搜索方式：可以通过昵称搜索，通过邮箱地址搜索，或者其他方式搜索，
			各种方式都只要继承搜索方式基类得以扩展
******************
*********************************************************************/
#ifndef PlayerSearchMode_H
#define PlayerSearchMode_H

#include "EQMainPrerequisites.h"

/**搜索方式基类
*/
class PlayerSearchModeBase
{
public:
	PlayerSearchModeBase(){}
	virtual ~PlayerSearchModeBase(){}

public:
	virtual void searchMode(std::string key);

protected:
private:
};

/**通过昵称搜索
*/
class PlayerSearchByNick : public PlayerSearchModeBase
{
public:
	PlayerSearchByNick(){}
	~PlayerSearchByNick(){}

	static PlayerSearchByNick& getSingleton()
	{
		static PlayerSearchByNick ps_byNick;
		return ps_byNick;
	}

public:
	void searchMode(std::string key);

protected:
private:
};

/**通过邮件帐号搜索
*/
class PlayerSearchByMail : public PlayerSearchModeBase
{
public:
	PlayerSearchByMail(){}
	~PlayerSearchByMail(){}

	static PlayerSearchByMail& getSingleton()
	{
		static PlayerSearchByMail ps_bymail;
		return ps_bymail;
	}

public:
	void searchMode(std::string key);

protected:
private:
};
#endif