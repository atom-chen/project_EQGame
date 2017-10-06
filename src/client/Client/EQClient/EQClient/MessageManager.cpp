#include "EQClientPCH.h"
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
    //���������Ϣ����
    if(handleByHouse())
        return true;
    //�������
    if(handleByChat())
        return true;
    //
    if(handleByPack())
        return true;
    //XiaoMing ADD
    if(handleByScene())
        return true;
    //NPC
    if(handleByNPC())
        return true;
    //��ɫ����
    if(handleByProPerty())
        return true;
    //HDJ Add
    if (handleByFriendSys())
        return true;
    //����
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
    if(handleByOthers())
		return true;
    if(handleByGM())
		return true;
    //����ϵͳ
    if(handleGiftsystemMsg())
        return true;
    //����ϵͳ
    if (handleBroadCast())
        return true;

	return false;
}
