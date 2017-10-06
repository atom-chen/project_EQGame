/********************************************************************
******************

* 作者:		huangdj	  
* 时间:		30/5/2011	10:48
* 文件: 	NormalDriftBottle.h
* 描述:		普通漂流瓶类
******************
*********************************************************************/
#ifndef NORMALDRIFTBOTTLE_H
#define NORMALDRIFTBOTTLE_H

#include "DriftBottle.h"

class NormalDriftBottle : public DriftBottle
{
public:
	/**
	*/
	NormalDriftBottle(uint64 bottle_id, std::string acc, int type, std::string content, stime time, std::string imageset, std::string img);

	/**
	*/
	~NormalDriftBottle();

public:
protected:
private:
};
#endif
