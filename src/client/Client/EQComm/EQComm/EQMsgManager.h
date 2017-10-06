#ifndef EQMESSAGEMANAGER_H
#define EQMESSAGEMANAGER_H

#include "CEGUI.h"
#include "GameSocket.h"
#include "MsgHandleRegist.h"
#include <Ogre.h>

class EQMsgManager : public IMsgHandleRegist
{
public:
	virtual bool Initialize();
	virtual void Update();
	virtual bool Regist(MsgType msgType, PMsgHandle handle);
	virtual void UnRegist(MsgType msgType);
	virtual void UnRegistAll();
	virtual bool Dispatch(MsgType msgType, MsgPack &pack);

	bool createNet(string ip, int port)
	{
		return m_Socket.Create(ip.c_str(),port) == TRUE;
	}
	//������Ϣ
	BOOL Send(INetPacket& packet);
	//
	UINT16 getMsgtype(){return mMsgtype;}
	ByteArray& getRecebuff(){return mRecebuff;}

	EQMsgManager() {}
	virtual ~EQMsgManager(void) {}

protected:
	// ����������Ϣ����
	void ReceiveHandle();
	virtual bool logicProc() { return false; }
	/*-----------------------------------------------------------------------------
	����
	*/
public:
	// �����˳���Ϸ
	BOOL LogoutRequest();
	/*-----------------------------------------------------------------------------
	gluesģ�飬��¼�ʹ�����ɫ����������
	*/
public:
	// ����ʼ�ƶ�
	BOOL StartWalkRequest(Ogre::Vector3 vCurrPos, Ogre::Vector3 vDestination,int move_type);
	// ����ֹͣ�ƶ�
	BOOL StopWalkRequest(Ogre::Vector3 vCurrPos);
	// �ƶ�ͬ������
	BOOL MoveSyncPosition(Ogre::Vector3 vCurrPos);

	//GM��ʾ
	BOOL GMRequest(int type, const char* par);
	BOOL GMRequest(int type, const char* par, const char* par2);
	BOOL GMRequest(int type, const char* par, const char* par2, const char* par3);
	BOOL GMRequest(int type, const char* par, const char* par2, const char* par3, const char * par4);
	//�������
	BOOL recoveryReq(int id);

	struct req_placed_furniture;
	BOOL placedReq(INetPacket& place);

protected:
	//���ؽ�ɫװ����Ϣ
	void PlayerEquipReply(ByteArray& recBuff);
	// ���ؿ�ʼ�ƶ����
	void StartWalkReply(ByteArray& recBuff);
	// �����ƶ�ֹͣ���
	void StopWalkReply(ByteArray& recBuff);
	// ��ɫ����
	void recPlayAction(ByteArray& recBuff);
	void recEndBodyAction(ByteArray& recBuff);
	//
	void receiveHousedata(ByteArray& recBuff);
	//
	void OnRecePlayerPackage(ByteArray& recBuff);
	//
	void OnReceSwapItemResult(ByteArray& recBuff);

	//�ʻ�����
	void recFlowerCount(ByteArray& recBuff);
	void recDecoration(ByteArray& recBuff);
	void recFlowerChange(ByteArray& recBuff);
	void recEggChange(ByteArray& recBuff);
	void recFlowerLog(ByteArray& recBuff);
	void recEggLog(ByteArray& recBuff);
	void recFlowerPlay(ByteArray& recBuff);
	void recEggPlay(ByteArray& recBuff);
	void recTodayFlowerEggCount(ByteArray& recBuff);

	//�ճ�����
	void recDailyReset(ByteArray& recBuff);

	//�Ҿ�ʹ�������Ϣ
	void recUseFurniture(ByteArray& recBuff);
	void recUseMultiFurniture(ByteArray& recBuff);
	void recStopFurniture(ByteArray& recBuff);
	void recUpdateCoin(ByteArray& recBuff);
	//��װ�̳ǹ���
	void recBuyFurniture(ByteArray& recBuff);
	//��Ҳֿ������Ϣ
	void OnRecePlayerStorage(ByteArray& recBuff);
	//���EQ��Ϣ
	void OnRecePlayerEQInfo(ByteArray& recBuff);
	//���EQǩ��
	void OnRecePlayerEQSign(ByteArray& recBuff);
	/*-----------------------------------------------------------------------------
	��ģ��
	*/
	//�������
	bool handleByHouse();
	//�������
	bool handleByChat();
	//�����
	bool handleByPack();
	// ������������صĳ����л���Ϣ
	bool handleByScene();
	//NPC
	bool handleByNPC();
	//HDJ add
	bool handleByFriendSys();
	//��ɫ����
	bool handleByProPerty();
	//����
	bool handleByTask();
	//ϵͳ��Ϣ
	bool handleByMsgSys();
	//����ģ��
	bool handleByGM();
	//�����UI������Ϣ
	bool handleOpenUIMsg();
	//����ϵͳ
	bool handleGiftsystemMsg();
	//����ϵͳ
	bool handleBroadCast();

protected:
	UINT16 mMsgtype; //��Ϣ����
	ByteArray mRecebuff; //��Ϣ��
	GameSocket m_Socket;
	MsgHandleRegist m_msgHandleRegist;
};

#endif