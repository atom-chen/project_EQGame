#pragma once

#ifndef __MsgHandleRegist_H
#define __MsgHandleRegist_H

#include "MsgHandle.h"

//消息和Handle的注册管理器接口，任何要用到该消息分发系统的管理器都必需派生于它
class IMsgHandleRegist
{
public:
	//注册消息和Handle的映射
	virtual bool Regist(MsgType msgType, PMsgHandle handle) = 0;
	//取消某一消息和Handle的映射
	virtual void UnRegist(MsgType msgType) = 0;
	//取消所有消息映射
	virtual void UnRegistAll() = 0;
	//分发消息
	virtual bool Dispatch(MsgType msgType, MsgPack &pack) = 0;
};

//消息和Handle的注册管理器基实现，任何要实现该消息分发的管理器都可以派生它或直接派生接口
//提供消息和Handle的注册、取消注册和分发
//不支持一个消息多个Handle的映射
class MsgHandleRegist : public IMsgHandleRegist
{
public:
	MsgHandleRegist();
	~MsgHandleRegist();

	virtual bool Regist(MsgType msgType, PMsgHandle handle);
	virtual void UnRegist(MsgType msgType);
	virtual void UnRegistAll();
	virtual bool Dispatch(MsgType msgType, MsgPack &pack);

protected:
	MSGHANDLEMAP m_msgHandlers;
};

//注册类成员函数消息映射的助理
//提供自动取消注册功能
//代价：增加内存消耗、增加派生类实现复杂度
class MemberMsgHandleRegistHelper
{
public:
	typedef std::vector<MsgType> MSGHANDLEKEYLIST;

public:
	MemberMsgHandleRegistHelper();
	MemberMsgHandleRegistHelper(IMsgHandleRegist *msgRegist);
	virtual ~MemberMsgHandleRegistHelper(void);

	virtual bool Regist(MsgType msgType, PMsgHandle handle);
	virtual void UnRegist(MsgType msgType);
	virtual void UnRegistAll();

	void SetMsgRegist(IMsgHandleRegist *msgRegist);

protected:
	MSGHANDLEKEYLIST m_keys;
	IMsgHandleRegist *m_pMsgRegist;
};

#endif
