/*********************************************************************
* ����: ��_Mike
* ʱ��: 2009-11-23
* ����: �ű��ӿ�
**********************************************************************/
#ifndef SCRIPTFUNCTION_H
#define SCRIPTFUNCTION_H
#include "NetPacket.h"
#include "LuaManager.h"
#include "EQLog.h"
#include "CEGUI.h"


/**����ũ������
*/
bool loadFarmScence();

/**
*/
bool logoutFarm();

/**����ũ�����
*/
bool setFarmCurState(int sta);

/*
*/
bool setFarmSeedingCurSta(int sta, std::string imageset, std::string icon);

#endif