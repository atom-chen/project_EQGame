/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		6/4/2011	10:44
* 文件: 	PlayerSearchOutput.h
* 描述:		玩家搜索结果的输出
******************
*********************************************************************/
#ifndef PlayerSearchOutput_H
#define PlayerSearchOutput_H

#include "EQMainPrerequisites.h"
#include "PlayerSearch.h"

/**搜索结果输出基类
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

/**搜索结果输出到列表控件
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