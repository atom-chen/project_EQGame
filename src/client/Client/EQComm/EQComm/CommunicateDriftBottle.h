/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		30/5/2011	10:59
* 文件: 	CommunicateDriftBottle.h
* 描述:		交往瓶类
******************
*********************************************************************/
#ifndef COMMUNICATEDRIFTBOTTLE_H
#define COMMUNICATEDRIFTBOTTLE_H

#include "DriftBottle.h"

class CommunicateDriftBottle : public DriftBottle
{
public:
	/**
	*/
	CommunicateDriftBottle(uint64 bottle_id, std::string acc, int type, std::string content, stime time, std::string imageset, std::string img);

	/**
	*/
	~CommunicateDriftBottle();

public:
protected:
private:
};
#endif
