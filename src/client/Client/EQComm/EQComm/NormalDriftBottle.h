/********************************************************************
******************

* ����:		huangdj	  
* ʱ��:		30/5/2011	10:48
* �ļ�: 	NormalDriftBottle.h
* ����:		��ͨƯ��ƿ��
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
