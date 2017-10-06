#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "EQMsgManager.h"

class CMessageManager : public EQMsgManager
{
public:
	static CMessageManager& getSingleton(void)
	{
		static CMessageManager mgr;
		return mgr;
	}
	
	CMessageManager() {}
	virtual ~CMessageManager(void) {}

protected:
	virtual bool logicProc();

	// ��¼����������
	BOOL EnterOtherSceneRequest();
	// �뿪��������
	BOOL LeaveSceneRequest();
	//
	void notifyDayOrNight(ByteArray& recBuff);
	// ���ؽ�ɫ��ϸ��Ϣ
	void PlayerDataReply(ByteArray& recBuff);
	// �������������ϸ��Ϣ
	void OtherPlayerDataReply(ByteArray& recBuff);
	void PlayerPlayAnimation(ByteArray& recBuff);
	void notifyPlayerBodyState(ByteArray& recBuff);
	// �����뿪�������
	void LeaveSceneReply(ByteArray& recBuff);
	//����ģ��
	bool handleByOthers();
	//�ظ�����
	bool handleLoginRepeat();
};

#define sMsgMgr CMessageManager::getSingleton()

#endif