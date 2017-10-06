/********************************************************************
******************

* ����:		huangdj	  
* ʱ��:		2/4/2011	15:30
* �ļ�: 	PlayerSearchMode.h
* ����:		���������ʽ������ͨ���ǳ�������ͨ�������ַ����������������ʽ������
			���ַ�ʽ��ֻҪ�̳�������ʽ���������չ
******************
*********************************************************************/
#ifndef PlayerSearchMode_H
#define PlayerSearchMode_H

#include "EQMainPrerequisites.h"

/**������ʽ����
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

/**ͨ���ǳ�����
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

/**ͨ���ʼ��ʺ�����
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