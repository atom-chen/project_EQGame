#pragma once

#ifndef __MsgHandleRegist_H
#define __MsgHandleRegist_H

#include "MsgHandle.h"

//��Ϣ��Handle��ע��������ӿڣ��κ�Ҫ�õ�����Ϣ�ַ�ϵͳ�Ĺ�������������������
class IMsgHandleRegist
{
public:
	//ע����Ϣ��Handle��ӳ��
	virtual bool Regist(MsgType msgType, PMsgHandle handle) = 0;
	//ȡ��ĳһ��Ϣ��Handle��ӳ��
	virtual void UnRegist(MsgType msgType) = 0;
	//ȡ��������Ϣӳ��
	virtual void UnRegistAll() = 0;
	//�ַ���Ϣ
	virtual bool Dispatch(MsgType msgType, MsgPack &pack) = 0;
};

//��Ϣ��Handle��ע���������ʵ�֣��κ�Ҫʵ�ָ���Ϣ�ַ��Ĺ�������������������ֱ�������ӿ�
//�ṩ��Ϣ��Handle��ע�ᡢȡ��ע��ͷַ�
//��֧��һ����Ϣ���Handle��ӳ��
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

//ע�����Ա������Ϣӳ�������
//�ṩ�Զ�ȡ��ע�Ṧ��
//���ۣ������ڴ����ġ�����������ʵ�ָ��Ӷ�
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
