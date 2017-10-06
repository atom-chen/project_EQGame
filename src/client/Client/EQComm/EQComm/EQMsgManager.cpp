#include "EQCommPCH.h"
#include "EQMsgManager.h"
#include "EnumDef.h"
#include "ByteArray.h"
#include "NetPacket.h"
#include "MsgSystem.h"

//------------------------------------------------------------------------------
bool EQMsgManager::Initialize()
{
	return true;
}
//------------------------------------------------------------------------------
void EQMsgManager::Update()
{
	sSysMsg.update();
	ReceiveHandle();
}
//------------------------------------------------------------------------------
BOOL EQMsgManager::Send(INetPacket&  packet)
{
	return m_Socket.Send(packet);
}
//------------------------------------------------------------------------------
void EQMsgManager::ReceiveHandle()
{
	m_Socket.Check();
	if (!m_Socket.RecvFromSock())
		return;

	while(true)
	{
		mRecebuff.reuse();
		if( !m_Socket.GetPacket(mRecebuff) ) 
			return;

		mMsgtype = mRecebuff.read_uint16();

		if(false == logicProc())
		{
			//消息分发
			if(false == Dispatch(mMsgtype, mRecebuff))
			{
				char msg[64];
				sprintf(msg, "msg [%d] not regist process handle.", mMsgtype);
				LOGERR(msg);
			}
		}
	}
}

bool EQMsgManager::Regist(MsgType msgType, PMsgHandle handle)
{
	return m_msgHandleRegist.Regist(msgType, handle);

}

void EQMsgManager::UnRegist(MsgType msgType)
{
	m_msgHandleRegist.UnRegist(msgType);

}

void EQMsgManager::UnRegistAll()
{
	m_msgHandleRegist.UnRegistAll();

}

bool EQMsgManager::Dispatch(MsgType msgType, MsgPack &pack)
{
	return m_msgHandleRegist.Dispatch(msgType, pack);

}