#include "EQCommPCH.h"
#include "MsgHandleRegist.h"

MsgHandleRegist::MsgHandleRegist()
{

}

MsgHandleRegist::~MsgHandleRegist()
{
	UnRegistAll();

}

bool MsgHandleRegist::Regist(MsgType msgType, PMsgHandle handle)
{
	if(m_msgHandlers.find(msgType) != m_msgHandlers.end())
		return false;//or throw exception

	m_msgHandlers.insert(std::make_pair(msgType, handle));

	return true;

}

void MsgHandleRegist::UnRegist(MsgType msgType)
{
	MSGHANDLEMAP::iterator itr = m_msgHandlers.find(msgType);

	if(itr == m_msgHandlers.end())
		return;

	itr->second->Release();
	m_msgHandlers.erase(itr);

}

void MsgHandleRegist::UnRegistAll()
{
	m_msgHandlers.clear();

}

bool MsgHandleRegist::Dispatch(MsgType msgType, MsgPack &pack)
{
	MSGHANDLEMAP::iterator itr = m_msgHandlers.find(msgType);

	if(itr != m_msgHandlers.end())
	{
		(*(itr->second))(pack);
		return true;
	}

	return false;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MemberMsgHandleRegistHelper::MemberMsgHandleRegistHelper()
: m_pMsgRegist(0)
{

}

MemberMsgHandleRegistHelper::MemberMsgHandleRegistHelper(IMsgHandleRegist *msgRegist)
: m_pMsgRegist(msgRegist)
{

}

MemberMsgHandleRegistHelper::~MemberMsgHandleRegistHelper(void)
{
	UnRegistAll();
}

void MemberMsgHandleRegistHelper::SetMsgRegist(IMsgHandleRegist *msgRegist)
{
	m_pMsgRegist = msgRegist;
}

bool MemberMsgHandleRegistHelper::Regist(MsgType msgType, PMsgHandle handle)
{
	if(0 == m_pMsgRegist)
		return false;

	if(m_pMsgRegist->Regist(msgType, handle))
	{
		m_keys.push_back(msgType);
		return true;
	}

	return false;
}

void MemberMsgHandleRegistHelper::UnRegist(MsgType msgType)
{
	if(0 == m_pMsgRegist)
		return;

	m_pMsgRegist->UnRegist(msgType);

	MSGHANDLEKEYLIST::iterator itr = std::find(m_keys.begin(), m_keys.end(), msgType);
	if(itr != m_keys.end())
		m_keys.erase(itr);
}

void MemberMsgHandleRegistHelper::UnRegistAll()
{
	if(0 == m_pMsgRegist)
		return;

	for(MSGHANDLEKEYLIST::iterator itr = m_keys.begin(); itr != m_keys.end(); ++itr)
		m_pMsgRegist->UnRegist(*itr);

	m_keys.clear();
	//m_keys.~vector();
}
