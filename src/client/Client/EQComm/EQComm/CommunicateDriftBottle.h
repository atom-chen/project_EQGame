/********************************************************************
******************

* ����:		huangdj	  
* ʱ��:		30/5/2011	10:59
* �ļ�: 	CommunicateDriftBottle.h
* ����:		����ƿ��
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
