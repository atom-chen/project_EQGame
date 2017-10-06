/********************************************************************
******************

* ����:		huangdj	  
* ʱ��:		30/5/2011	10:42
* �ļ�: 	DriftBottle.h
* ����:		Ư��ƿ����
******************
*********************************************************************/
#ifndef DRIFTBOTTLE_H
#define DRIFTBOTTLE_H

#include "NetPacket.h"

enum BottleType
{
	BT_NORMAL = 1,
	BT_COMMUNICATE,
};

class DriftBottle
{
public:
	/**
	*/
	DriftBottle(uint64 bottle_id, std::string acc, int type, std::string content, stime time, std::string imageset, std::string img);

	/**
	*/
	virtual ~DriftBottle();

public:
	/**
	*/
	typedef struct  DriftBottleInfo
	{
		DriftBottleInfo(uint64		bottle_id,
						std::string acc,
						int			type,
						std::string content,
						stime		time
						)
			: mBottleID(bottle_id)
			,mAccount(acc)
			, mType(type)
			, mContent(content)
			, mTime(time)
		{

		}
		uint64		mBottleID;
		std::string mAccount;
		int			mType;
		std::string mContent;
		stime		mTime;
	} DriftBottleInfoType;
	
	/**
	*/
	typedef struct  DriftBottleTexture
	{
		DriftBottleTexture(std::string imgset, std::string img)
			: imageset(imgset)
			, image(img)
		{

		}
		std::string imageset;
		std::string image;
	} DriftBottleTextureType;

public:
	/**
	*/
	DriftBottleInfoType* getBottleInfo(){return &mDriftBottle_info;}

protected:
	/**ƿ����Ϣ
	*/
	DriftBottleInfoType mDriftBottle_info;

	DriftBottleTextureType mBottleTexture;

private:
};
#endif
