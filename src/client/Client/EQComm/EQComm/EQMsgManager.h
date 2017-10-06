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
	//发送消息
	BOOL Send(INetPacket& packet);
	//
	UINT16 getMsgtype(){return mMsgtype;}
	ByteArray& getRecebuff(){return mRecebuff;}

	EQMsgManager() {}
	virtual ~EQMsgManager(void) {}

protected:
	// 接收网络消息处理
	void ReceiveHandle();
	virtual bool logicProc() { return false; }
	/*-----------------------------------------------------------------------------
	场景
	*/
public:
	// 请求退出游戏
	BOOL LogoutRequest();
	/*-----------------------------------------------------------------------------
	glues模块，登录和创建角色，场景载入
	*/
public:
	// 请求开始移动
	BOOL StartWalkRequest(Ogre::Vector3 vCurrPos, Ogre::Vector3 vDestination,int move_type);
	// 请求停止移动
	BOOL StopWalkRequest(Ogre::Vector3 vCurrPos);
	// 移动同步请求
	BOOL MoveSyncPosition(Ogre::Vector3 vCurrPos);

	//GM请示
	BOOL GMRequest(int type, const char* par);
	BOOL GMRequest(int type, const char* par, const char* par2);
	BOOL GMRequest(int type, const char* par, const char* par2, const char* par3);
	BOOL GMRequest(int type, const char* par, const char* par2, const char* par3, const char * par4);
	//房屋相关
	BOOL recoveryReq(int id);

	struct req_placed_furniture;
	BOOL placedReq(INetPacket& place);

protected:
	//返回角色装备信息
	void PlayerEquipReply(ByteArray& recBuff);
	// 返回开始移动结果
	void StartWalkReply(ByteArray& recBuff);
	// 返回移动停止结果
	void StopWalkReply(ByteArray& recBuff);
	// 角色动作
	void recPlayAction(ByteArray& recBuff);
	void recEndBodyAction(ByteArray& recBuff);
	//
	void receiveHousedata(ByteArray& recBuff);
	//
	void OnRecePlayerPackage(ByteArray& recBuff);
	//
	void OnReceSwapItemResult(ByteArray& recBuff);

	//鲜花鸡蛋
	void recFlowerCount(ByteArray& recBuff);
	void recDecoration(ByteArray& recBuff);
	void recFlowerChange(ByteArray& recBuff);
	void recEggChange(ByteArray& recBuff);
	void recFlowerLog(ByteArray& recBuff);
	void recEggLog(ByteArray& recBuff);
	void recFlowerPlay(ByteArray& recBuff);
	void recEggPlay(ByteArray& recBuff);
	void recTodayFlowerEggCount(ByteArray& recBuff);

	//日常重置
	void recDailyReset(ByteArray& recBuff);

	//家具使用相关消息
	void recUseFurniture(ByteArray& recBuff);
	void recUseMultiFurniture(ByteArray& recBuff);
	void recStopFurniture(ByteArray& recBuff);
	void recUpdateCoin(ByteArray& recBuff);
	//家装商城购买
	void recBuyFurniture(ByteArray& recBuff);
	//玩家仓库相关消息
	void OnRecePlayerStorage(ByteArray& recBuff);
	//玩家EQ信息
	void OnRecePlayerEQInfo(ByteArray& recBuff);
	//玩家EQ签名
	void OnRecePlayerEQSign(ByteArray& recBuff);
	/*-----------------------------------------------------------------------------
	各模块
	*/
	//房屋相关
	bool handleByHouse();
	//聊天相关
	bool handleByChat();
	//包相关
	bool handleByPack();
	// 处理服务器返回的场景切换消息
	bool handleByScene();
	//NPC
	bool handleByNPC();
	//HDJ add
	bool handleByFriendSys();
	//角色属性
	bool handleByProPerty();
	//任务
	bool handleByTask();
	//系统消息
	bool handleByMsgSys();
	//其它模块
	bool handleByGM();
	//处理打开UI类型消息
	bool handleOpenUIMsg();
	//礼物系统
	bool handleGiftsystemMsg();
	//公告系统
	bool handleBroadCast();

protected:
	UINT16 mMsgtype; //消息类型
	ByteArray mRecebuff; //消息包
	GameSocket m_Socket;
	MsgHandleRegist m_msgHandleRegist;
};

#endif