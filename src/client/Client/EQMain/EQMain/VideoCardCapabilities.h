/********************************************************************
* 作者: lf
* 时间: 2011-2-23
* 描述: 游戏配置
**********************************************************************/
#ifndef VideoCardCapabilities_H
#define VideoCardCapabilities_H

#include "EQMainPrerequisites.h"

class VideoCardCapabilities
{
public:
	VideoCardCapabilities():mSM2(true),mSM3(true){};
	~VideoCardCapabilities(){};

	bool isSupportSM2() { return mSM2; }
	bool isSupportSM3() { return mSM3; }

protected:
	bool mSM2;
	bool mSM3;
};

#endif