#include "EQFarmPCH.h"
#include "MessageManager.h"
#include "EnumDef.h"
#include "ByteArray.h"
#include "NetPacket.h"
#include "SmallModules.h"
#include "GameEventMgr.h"
#include "GameState.h"
#include "Login.h"
#include "MsgSystem.h"

bool CMessageManager::logicProc()
{
    //�ظ�����
    if(handleLoginRepeat())
        return true;
    //�򿪽������
    if(handleOpenUIMsg())
        return true;
    //�������
    if(handleByChat())
        return true;
    if(handleByNPC())
        return true;
    //��ɫ����
    if(handleByProPerty())
        return true;
    //HDJ Add
    if (handleByFriendSys())
        return true;
    //ũ��
    if(handleByFarm())
        return true;
    //ũ��
    if(handleByTask())
        return true;
    //��¼��������ɫ
    if(!sStateMgr.gaming())
    {
		if(Login::getSingleton().handleMsg())
			return true;
    }
    //��Ϣϵͳ
    if(handleByMsgSys())
		return true;
    //����Сģ��
    if(handleByGM())
		return true;
    //����ϵͳ
    if(handleBroadCast())
        return true;

	return false;
}
