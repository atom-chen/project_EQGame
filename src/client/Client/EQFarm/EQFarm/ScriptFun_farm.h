/*********************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: 脚本接口
**********************************************************************/
#ifndef SCRIPTFUNCTION_H
#define SCRIPTFUNCTION_H
#include "NetPacket.h"
#include "LuaManager.h"
#include "EQLog.h"
#include "CEGUI.h"


/**加载农场场景
*/
bool loadFarmScence();

/**
*/
bool logoutFarm();

/**设置农场鼠标
*/
bool setFarmCurState(int sta);

/*
*/
bool setFarmSeedingCurSta(int sta, std::string imageset, std::string icon);

#endif