/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	BaseType.h
	created:	D.W 2009-9-23 19:00
	purpose:	封装的网络Socket，负责创建Socket连接，发送网络数据，接受网络数据
==============================================================================*/
#include "EQCommPCH.h"
#include "GameSocket.h"
#include "CommFunc.h"
#include <MSTCPiP.h>




const int HEAD_SIZE = 4;

inline int swab32(int x) 
{
	return ((x&0x000000ff) << 24 | (x&0x0000ff00) << 8 | (x&0x00ff0000) >> 8 | (x&0xff000000) >> 24);
}

inline int swab32_array(char *pBuf) 
{
	unsigned char *p = (unsigned char*)pBuf;
	int ret = (p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3]);
	assert(ret >= 0);
	return ret;
}

// 检查是否可正常连到服务端
static bool CanRouterToServer(const std::string & ip, int port, std::string & routerInfo)
{
	const std::string tmpFile = GetGameUserDir() + "CheckRouter.txt";
	std::string cmd = "cmd /c tracert " + ip + " -h 5 -w 200 > ";

	cmd = cmd + "\"" + tmpFile + "\"";
	RunExe(cmd, true);
	
	ifstream f(tmpFile.c_str());
	
	const int fileBufSize = 10240;
	char fileBuf[fileBufSize] = {0};
	f.read(fileBuf, fileBufSize - 1);
	fileBuf[fileBufSize - 1] = '\0';

	routerInfo = fileBuf;
	size_t iFind = routerInfo.find("Request timed out");
	if (0 <= iFind && iFind < routerInfo.size()) // 如果路由超时
	{
		return false;
	}

	iFind = routerInfo.find("unreachable");
	if (0 <= iFind && iFind < routerInfo.size()) // 如果不可到达
	{
		return false;
	}

	return true;
}

// 检查数据包路由
static void CheckRouterOnError(const std::string & ip, int port)
{
	LOGERR("网络出现异常，正在检测网络连线。。。");

	std::string routerInfo;
	if (CanRouterToServer(ip, port, routerInfo))
	{
		std::string errMsg = FormatStr("可能的原因：\r\n    1. 玩家游戏逻辑出错\r\n    2. 服务器正在维护中\r\n");
		LOGERR(errMsg);
		//这个不提示了
		//MessageBoxA(NULL, errMsg.c_str(), "网络异常", 0);
	}
	else
	{
		std::string errMsg = "数据包无法发送至服务器，请检查网线和相关设备是否正常\r\n";
		errMsg += routerInfo;
		LOGERR(errMsg);
		MessageBoxA(NULL, errMsg.c_str(), "网络异常", 0);
	}
}


// 检查数据包路由
static void CheckRouterOnConnectFail(const std::string & ip, int port)
{
	LOGERR("网络出现异常，正在检测网络连线。。。");

	std::string routerInfo;
	if (CanRouterToServer(ip, port, routerInfo))
	{
		std::string errMsg = FormatStr("可能的原因：\r\n    1. 网络受限，无法访问端口%d，请联系当地网管\r\n    2. 服务器正在维护中\r\n", port);
		LOGERR(errMsg + routerInfo);
		MessageBoxA(NULL, errMsg.c_str(), "网络异常", 0);
	}
	else
	{
		std::string errMsg = "数据包无法发送至服务器，请检查网线和相关设备是否正常\r\n";
		errMsg += routerInfo;
		LOGERR(errMsg);
		MessageBoxA(NULL, errMsg.c_str(), "网络异常", 0);
	}
}


GameSocket::GameSocket()
{ 
	m_state	= false;
	m_port = 0;
}

BOOL GameSocket::Create(const char* serverIP, int serverPort)
{
	m_ip = serverIP;
	m_port = serverPort;
	try
	{
		if( m_state )	// 如果已经创建socket，重新初始化
			Destroy();

		WSADATA		cData;

		if(WSAStartup(0x0101, &cData) != 0)
		{
			return false;
		}
		if(cData.wVersion != 0x0101)
		{
			WSACleanup();
			return false;
		}

		m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); // 创建Socket

		if( m_socket == INVALID_SOCKET )
		{
			int err = WSAGetLastError();
			WSACleanup();
			return FALSE;
		}

		unsigned long	i = 1;  // 设置为非阻塞方式

		if(ioctlsocket(m_socket, FIONBIO, &i))
		{
			int err = WSAGetLastError();
			closesocket(m_socket);
			WSACleanup();
			return FALSE;
		}

		BOOL optAlive = 1;
		// 设置心跳(有些机子可能会设置失败, 所以失败时不应该关闭socket)
		if (setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, 
						reinterpret_cast<char *>(&optAlive), sizeof(optAlive)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			LOGERR(FormatStr("设置心跳失败(%d)", err));
		}
		
		tcp_keepalive aliveSetting = {0};
		aliveSetting.onoff = 1;
		aliveSetting.keepalivetime = 30 * 1000; // 30秒
		aliveSetting.keepaliveinterval = 10 * 1000;// 没回应的话，10秒重发
		tcp_keepalive outSetting = {0};
		DWORD aliveRet = 0; 
		
		
		if (WSAIoctl(m_socket, SIO_KEEPALIVE_VALS, 
					&aliveSetting, sizeof(aliveSetting), 
					&outSetting, sizeof(outSetting), 
					&aliveRet, 
					NULL,NULL) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			LOGERR(FormatStr("设置心跳时间失败(%d)", err));
		}
		//unsigned long serveraddr = inet_addr(serverIP);

		//if(serveraddr == INADDR_NONE) // 检查IP地址格式错误
		//{
		//	closesocket(m_socket);
		//	WSACleanup();
		//	return false;
		//}

        int status;
        struct addrinfo hints;
        struct addrinfo *servinfo; // will point to the results

        memset(&hints, 0, sizeof(hints)); // make sure the struct is empty
        hints.ai_family = AF_INET; // don't care IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
        hints.ai_flags = AI_PASSIVE; // fill in my IP for me

        char port[10];
        itoa(serverPort, port, 10);
        if ((status = getaddrinfo(serverIP, port, &hints, &servinfo)) != 0) {
            closesocket(m_socket);
            WSACleanup();
            return false;
        }

		//sockaddr_in	addr_in;
		//memset((void *)&addr_in, sizeof(addr_in), 0);
		//addr_in.sin_family = AF_INET;
		//addr_in.sin_addr.s_addr = inet_addr(serverIP);
		//addr_in.sin_port = htons(serverPort);


        //if(connect(m_socket, (sockaddr *)&addr_in, sizeof(addr_in)) == SOCKET_ERROR)
		if(connect(m_socket, servinfo->ai_addr, servinfo->ai_addrlen)  == SOCKET_ERROR)
		{
			int err = WSAGetLastError();

			if(err != WSAEWOULDBLOCK)
			{
				closesocket(m_socket);
				WSACleanup();
				return false;
			}
			else  // WSAWOLDBLOCK
			{
				timeval timeout;
				timeout.tv_sec	= BLOCKSECONDS;
				timeout.tv_usec	= 0;
				fd_set writeset, exceptset;
				FD_ZERO(&writeset);
				FD_ZERO(&exceptset);
				FD_SET(m_socket, &writeset);
				FD_SET(m_socket, &exceptset);

				int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
				if (ret == 0)
				{
					closesocket(m_socket);

					CheckRouterOnConnectFail(m_ip, m_port);

					WSACleanup();
					return false;
				}
				else if(ret < 0)
				{
					int	err = WSAGetLastError();
					closesocket(m_socket);
	                LOGERR(FormatStr("连接失败! errcode = %d ", err));
					CheckRouterOnConnectFail(m_ip, m_port);
					WSACleanup();
					return false;
				}
				else	
				{
					if(FD_ISSET(m_socket, &exceptset))
					{
						closesocket(m_socket);
						CheckRouterOnConnectFail(m_ip, m_port);
						WSACleanup();
						return false;
					}
				}
			}
		}

		m_inbufLen		= 0;
		m_inbufStart	= 0;
		m_outbufLen	= 0;
		m_state	= true;

		return true;
	}
	catch(...) 
	{ 
		exit(1); 
	}
}

BOOL GameSocket::Send(INetPacket& packet)
{
	LOGMSG("准备发送的消息内容：");

	ByteArray bytes;
	packet.build(bytes); 

	void* pBuf = bytes.m_pContent;
	int nSize = bytes.length();
	if(pBuf == 0 || nSize <= 0)
		return false;

	if( !m_state )
		return false;// socket状态出错
	if(m_outbufLen + nSize + HEAD_SIZE > OUTBUFSIZE)// 检测BUF溢出
	{ 
		Flush();// 立即发送队列中的数据，以清空发送队列
		if(m_outbufLen + nSize + HEAD_SIZE > OUTBUFSIZE)
		{
			Destroy();
			return FALSE;
		}
	}

	SetPacketHead(nSize);                        // 写入包头
	memcpy(m_bufOutput + m_outbufLen, pBuf, nSize);// 数据添加到BUF尾
	m_outbufLen += nSize;
	assert(m_outbufLen <= OUTBUFSIZE);

	Flush();// 先有数据就立即发送
	return true;
}



BOOL GameSocket::Check(void)
{
    if(m_state)// 检查socket状态
    {
        char	buf[1];

        // MSG_PEEK: 数据将被复制到缓冲区中,但并不从输入队列中删除
        int	ret = recv(m_socket, buf, 1, MSG_PEEK);

        //If the connection has been gracefully closed,the return value is 0
        if(ret == 0)			{
            Destroy();
            LOGWAR("the connection has been gracefully closed,the return value is 0");
            if( IDYES == MessageBoxA(NULL,
                "网络异常, 连接被关闭，是否退出游戏？","EQ家园 - 警报",
                MB_YESNO | MB_ICONWARNING | MB_TASKMODAL) )
			{
                exit(1);
			}
            return false;
        }
        else if(ret < 0)//SOCKET_ERROR   
        {
            int err = WSAGetLastError();
            if(err != WSAEWOULDBLOCK)
            {
				CheckRouterOnError(m_ip, m_port);
				char errbuf[256];
                Destroy();
                LOGERR(FormatStr("socket_error, WSAGetLastError() != wsaewouldblock, errcode = %d ", err));
                sprintf(errbuf, "网络异常, 连接被关闭，是否退出游戏？(err:%d)", err);

				if( IDYES == MessageBoxA(NULL,
                    errbuf,"EQ家园 - 警报",
                    MB_YESNO | MB_ICONWARNING | MB_TASKMODAL) )
				{
                    exit(1);
				}
                return false;
            }
            else//阻塞?
            {
                return false;
            }
        }
        //If no error occurs,recv returns the number of bytes received
        //有数据
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

BOOL GameSocket::GetPacket(ByteArray& bytes)
{
    if(m_inbufLen < HEAD_SIZE)  // 检查是否原先有数据
        return false;

    int packsize = swab32_array(&m_bufInput[m_inbufStart]) + HEAD_SIZE;  // 获取包大小
    if(packsize > m_inbufLen)  // 检查逻辑包是否完整了
            return false;

    void* pBuf = bytes.m_pContent;
    // 数据在队列中分段存放（循环队列尾部和头部）
    if(m_inbufStart + packsize > INBUFSIZE)	 
    {
        int copylen = INBUFSIZE - m_inbufStart;  // 尾部需要拷贝出的数据长度
        assert(copylen < packsize);
        assert(m_inbufStart + copylen == INBUFSIZE);

        memcpy(pBuf, m_bufInput + m_inbufStart, copylen); // 从尾部拷贝出数据
        // 从头部拷贝出数据
        memcpy((unsigned char *)pBuf + copylen, m_bufInput, packsize - copylen); 
    }
    else // 数据地址连续，直接从接收队列中取出 
    {
        memcpy(pBuf, m_bufInput + m_inbufStart, packsize);
    }

    m_inbufStart = (m_inbufStart + packsize) % INBUFSIZE;
    m_inbufLen -= packsize;
    assert(m_inbufLen >= 0);

    ReadPacketHead(bytes);
    return	true;
}

BOOL GameSocket::RecvFromSock(void) // 一次从网络中读取尽可能多的数据
{
	if(m_state && m_inbufLen < INBUFSIZE)
	{
		int	savelen, savepos; // 数据可以保存的长度，数据保存的位置

		if(m_inbufStart + m_inbufLen < INBUFSIZE) // 如果接收队列后部剩余的长度够存放，全部保存到队列后部
			savelen = INBUFSIZE - (m_inbufStart + m_inbufLen); // 后部空间长度
		else
			savelen = INBUFSIZE - m_inbufLen; // 总共还能放的长度

		savepos = (m_inbufStart + m_inbufLen) % INBUFSIZE; // 实际开始存储的位置
		
		// 存储数据到接收队列尾部
		int inlen = recv(m_socket, m_bufInput + savepos, savelen, 0); 

		if(inlen > 0) // 接收成功
		{
			m_inbufLen += inlen;
			
			// 如果队列后部空间不足保存所有数据，循环到队列头部开始保存
			if(inlen == savelen && m_inbufLen < INBUFSIZE) 
			{
				int savelen = INBUFSIZE - m_inbufLen;
				int savepos = (m_inbufStart + m_inbufLen) % INBUFSIZE;
				int inlen = recv(m_socket, m_bufInput + savepos, savelen, 0);

				if(inlen > 0)
				{
					m_inbufLen += inlen;
				}
				else if(inlen == 0)
				{
					Destroy();
					return false;
				}
				else
				{
					int err = WSAGetLastError();  // 连接已断开或者错误（包括阻塞）
					if(err != WSAEWOULDBLOCK)
					{
						Destroy();
						return false;
					}
				}
			}
		}
		else if(inlen == 0)
		{
			Destroy();
			return false;
		}
		else	// len < 0
		{
			int err = WSAGetLastError(); // 连接已断开或者错误（包括阻塞）
			if(err != WSAEWOULDBLOCK)
			{
				Destroy();
				return false;
			}
		}
	}

	return true;
}

BOOL GameSocket::Flush(void)
{
	try{
		if(m_outbufLen > 0) // 发送一段数据
		{
			int	outsize;
			outsize = send(m_socket, m_bufOutput, m_outbufLen, 0);
			if(outsize > 0)
			{
				if(m_outbufLen - outsize > 0) // 删除已发送的部分
					memcpy(m_bufOutput, m_bufOutput + outsize, m_outbufLen - outsize);

				m_outbufLen -= outsize;
				assert(m_outbufLen >= 0);
			}
			else // 错误
			{
                char buf[128];
				int err = WSAGetLastError();
                sprintf_s(buf, sizeof(buf), "flush socket data error, error code(%d)", err);
                MessageBox(NULL, buf, "", MB_OK);
				if(err != WSAEWOULDBLOCK)
				{
					Destroy();
					return false;
				}
			}
		}

		return true;

	}
	catch(...)
	{
		exit(1);
	}
}

void GameSocket::Destroy(void)
{
	try
	{
		if(m_state)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			WSACleanup();
			m_outbufLen = 0;
			m_state	= false;
		}

	}
	catch(...)
	{
		exit(1);
	}
}

void GameSocket::SetPacketHead(UINT16 size)
{
	int s = swab32(size);

	memcpy(m_bufOutput + m_outbufLen, &s, HEAD_SIZE);
	m_outbufLen += HEAD_SIZE;
}

UINT16 GameSocket::ReadPacketHead(ByteArray& bytes)
{
	char* pBuf = bytes.m_pContent;

	UINT32 packsize = swab32_array(pBuf);//u_char)low + (u_char)high * 256;  // 获取包头

	memcpy(pBuf , pBuf+HEAD_SIZE, packsize);
	bytes.m_nTotalSize = packsize;

	return packsize;
}


