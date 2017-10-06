/********************************************************************
******************

* ����:		huangdj	  
* ʱ��:		6/4/2011	10:44
* �ļ�: 	PlayerSearchOutput.h
* ����:		���������������
******************
*********************************************************************/
#ifndef PlayerSearchOutput_H
#define PlayerSearchOutput_H

#include "EQMainPrerequisites.h"
#include "PlayerSearch.h"

/**��������������
*/
class PlayerSearchOutput
{
public:
	PlayerSearchOutput(){}
	virtual ~PlayerSearchOutput(){}
	
	static PlayerSearchOutput& getSingleton()
	{
		static PlayerSearchOutput ps_output;
		return ps_output;
	}

public:
	/**
	*/
	virtual void output(PlayerSearch::PlayerList* out_playerList){}

protected:
private:
};

/**�������������б�ؼ�
*/
class PlayerSearchOutputToListbox : public PlayerSearchOutput
{
public:
	PlayerSearchOutputToListbox(){}
	~PlayerSearchOutputToListbox(){}

	static PlayerSearchOutputToListbox& getSingleton()
	{
		static PlayerSearchOutputToListbox ps_output_tolistbox;
		return ps_output_tolistbox;
	}

public:
	/**
	*/
	void output(PlayerSearch::PlayerList* out_playerList);

protected:
private:
};
#endif