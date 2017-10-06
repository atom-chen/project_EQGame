/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	BaseType.h
	created:	D.W 2009-9-23 19:00
	purpose:	��װ������Socket�����𴴽�Socket���ӣ������������ݣ�������������
==============================================================================*/
#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <assert.h>
#include "ByteArray.h"
#include "INetPacket.h"
#include <string>

const int BLOCKSECONDS	= 30;		// INIT��������ʱ��
const int INBUFSIZE	   = 3*1024*1024;	// ѭ������
const int OUTBUFSIZE	= 16*1024;	// 16K  ����16K��������  

class GameSocket 
{
public:
	GameSocket(void);
    
	BOOL	Create(const char* serverIP, int serverPort);// ������������Socket
	BOOL	Send(INetPacket& packet);				// ������Ϣ
	BOOL	Check(void);							// ����Ƿ����յ�����  
    BOOL	GetPacket(ByteArray& bytes);				// ������Ϣ
	BOOL	Flush(void);							// ��������                   
	void	Destroy(void);
    BOOL	RecvFromSock(void);					// ��socket��������

private:
	void    SetPacketHead(UINT16 size);			// ���ð�ͷ�������С,Ϊ��erlang���ⲿ���ܣ�
	UINT16  ReadPacketHead(ByteArray& bytes);	// ��ȡ��ͷ�������С,Ϊ��erlang���ⲿ���ܣ�


protected:
   SOCKET	m_socket;				// Socket��
	BOOL	m_state;				// socket״̬
															 
	int		m_inbufLen;				// ���ն����ڵ�ǰ����
	int		m_inbufStart;			// ���ն���ָ��			
	char	m_bufInput[INBUFSIZE];	// ѭ�����ն���

	int		m_outbufLen;			// ���Ͷ��г���
	char	m_bufOutput[OUTBUFSIZE];// ���Ͷ���	
	int		m_port;
	std::string m_ip;
};