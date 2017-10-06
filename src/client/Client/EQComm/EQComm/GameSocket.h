/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	BaseType.h
	created:	D.W 2009-9-23 19:00
	purpose:	封装的网络Socket，负责创建Socket连接，发送网络数据，接受网络数据
==============================================================================*/
#pragma once

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <assert.h>
#include "ByteArray.h"
#include "INetPacket.h"
#include <string>

const int BLOCKSECONDS	= 30;		// INIT函数阻塞时间
const int INBUFSIZE	   = 3*1024*1024;	// 循环队列
const int OUTBUFSIZE	= 16*1024;	// 16K  超过16K立即发送  

class GameSocket 
{
public:
	GameSocket(void);
    
	BOOL	Create(const char* serverIP, int serverPort);// 创建并且连接Socket
	BOOL	Send(INetPacket& packet);				// 发送消息
	BOOL	Check(void);							// 检查是否有收到数据  
    BOOL	GetPacket(ByteArray& bytes);				// 接收消息
	BOOL	Flush(void);							// 立即发送                   
	void	Destroy(void);
    BOOL	RecvFromSock(void);					// 从socket接收数据

private:
	void    SetPacketHead(UINT16 size);			// 设置包头（包体大小,为了erlang，外部不管）
	UINT16  ReadPacketHead(ByteArray& bytes);	// 读取包头（包体大小,为了erlang，外部不管）


protected:
   SOCKET	m_socket;				// Socket类
	BOOL	m_state;				// socket状态
															 
	int		m_inbufLen;				// 接收队列内当前长度
	int		m_inbufStart;			// 接收队列指针			
	char	m_bufInput[INBUFSIZE];	// 循环接收队列

	int		m_outbufLen;			// 发送队列长度
	char	m_bufOutput[OUTBUFSIZE];// 发送队列	
	int		m_port;
	std::string m_ip;
};