/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 D.W All rights reserved.

	filename: 	BaseType.h
	created:	D.W 2009-9-23 19:00
	purpose:	��װ������Socket�����𴴽�Socket���ӣ������������ݣ�������������
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

// ����Ƿ���������������
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
	if (0 <= iFind && iFind < routerInfo.size()) // ���·�ɳ�ʱ
	{
		return false;
	}

	iFind = routerInfo.find("unreachable");
	if (0 <= iFind && iFind < routerInfo.size()) // ������ɵ���
	{
		return false;
	}

	return true;
}

// ������ݰ�·��
static void CheckRouterOnError(const std::string & ip, int port)
{
	LOGERR("��������쳣�����ڼ���������ߡ�����");

	std::string routerInfo;
	if (CanRouterToServer(ip, port, routerInfo))
	{
		std::string errMsg = FormatStr("���ܵ�ԭ��\r\n    1. �����Ϸ�߼�����\r\n    2. ����������ά����\r\n");
		LOGERR(errMsg);
		//�������ʾ��
		//MessageBoxA(NULL, errMsg.c_str(), "�����쳣", 0);
	}
	else
	{
		std::string errMsg = "���ݰ��޷����������������������ߺ�����豸�Ƿ�����\r\n";
		errMsg += routerInfo;
		LOGERR(errMsg);
		MessageBoxA(NULL, errMsg.c_str(), "�����쳣", 0);
	}
}


// ������ݰ�·��
static void CheckRouterOnConnectFail(const std::string & ip, int port)
{
	LOGERR("��������쳣�����ڼ���������ߡ�����");

	std::string routerInfo;
	if (CanRouterToServer(ip, port, routerInfo))
	{
		std::string errMsg = FormatStr("���ܵ�ԭ��\r\n    1. �������ޣ��޷����ʶ˿�%d������ϵ��������\r\n    2. ����������ά����\r\n", port);
		LOGERR(errMsg + routerInfo);
		MessageBoxA(NULL, errMsg.c_str(), "�����쳣", 0);
	}
	else
	{
		std::string errMsg = "���ݰ��޷����������������������ߺ�����豸�Ƿ�����\r\n";
		errMsg += routerInfo;
		LOGERR(errMsg);
		MessageBoxA(NULL, errMsg.c_str(), "�����쳣", 0);
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
		if( m_state )	// ����Ѿ�����socket�����³�ʼ��
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

		m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); // ����Socket

		if( m_socket == INVALID_SOCKET )
		{
			int err = WSAGetLastError();
			WSACleanup();
			return FALSE;
		}

		unsigned long	i = 1;  // ����Ϊ��������ʽ

		if(ioctlsocket(m_socket, FIONBIO, &i))
		{
			int err = WSAGetLastError();
			closesocket(m_socket);
			WSACleanup();
			return FALSE;
		}

		BOOL optAlive = 1;
		// ��������(��Щ���ӿ��ܻ�����ʧ��, ����ʧ��ʱ��Ӧ�ùر�socket)
		if (setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, 
						reinterpret_cast<char *>(&optAlive), sizeof(optAlive)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			LOGERR(FormatStr("��������ʧ��(%d)", err));
		}
		
		tcp_keepalive aliveSetting = {0};
		aliveSetting.onoff = 1;
		aliveSetting.keepalivetime = 30 * 1000; // 30��
		aliveSetting.keepaliveinterval = 10 * 1000;// û��Ӧ�Ļ���10���ط�
		tcp_keepalive outSetting = {0};
		DWORD aliveRet = 0; 
		
		
		if (WSAIoctl(m_socket, SIO_KEEPALIVE_VALS, 
					&aliveSetting, sizeof(aliveSetting), 
					&outSetting, sizeof(outSetting), 
					&aliveRet, 
					NULL,NULL) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			LOGERR(FormatStr("��������ʱ��ʧ��(%d)", err));
		}
		//unsigned long serveraddr = inet_addr(serverIP);

		//if(serveraddr == INADDR_NONE) // ���IP��ַ��ʽ����
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
	                LOGERR(FormatStr("����ʧ��! errcode = %d ", err));
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
	LOGMSG("׼�����͵���Ϣ���ݣ�");

	ByteArray bytes;
	packet.build(bytes); 

	void* pBuf = bytes.m_pContent;
	int nSize = bytes.length();
	if(pBuf == 0 || nSize <= 0)
		return false;

	if( !m_state )
		return false;// socket״̬����
	if(m_outbufLen + nSize + HEAD_SIZE > OUTBUFSIZE)// ���BUF���
	{ 
		Flush();// �������Ͷ����е����ݣ�����շ��Ͷ���
		if(m_outbufLen + nSize + HEAD_SIZE > OUTBUFSIZE)
		{
			Destroy();
			return FALSE;
		}
	}

	SetPacketHead(nSize);                        // д���ͷ
	memcpy(m_bufOutput + m_outbufLen, pBuf, nSize);// ������ӵ�BUFβ
	m_outbufLen += nSize;
	assert(m_outbufLen <= OUTBUFSIZE);

	Flush();// �������ݾ���������
	return true;
}



BOOL GameSocket::Check(void)
{
    if(m_state)// ���socket״̬
    {
        char	buf[1];

        // MSG_PEEK: ���ݽ������Ƶ���������,�����������������ɾ��
        int	ret = recv(m_socket, buf, 1, MSG_PEEK);

        //If the connection has been gracefully closed,the return value is 0
        if(ret == 0)			{
            Destroy();
            LOGWAR("the connection has been gracefully closed,the return value is 0");
            if( IDYES == MessageBoxA(NULL,
                "�����쳣, ���ӱ��رգ��Ƿ��˳���Ϸ��","EQ��԰ - ����",
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
                sprintf(errbuf, "�����쳣, ���ӱ��رգ��Ƿ��˳���Ϸ��(err:%d)", err);

				if( IDYES == MessageBoxA(NULL,
                    errbuf,"EQ��԰ - ����",
                    MB_YESNO | MB_ICONWARNING | MB_TASKMODAL) )
				{
                    exit(1);
				}
                return false;
            }
            else//����?
            {
                return false;
            }
        }
        //If no error occurs,recv returns the number of bytes received
        //������
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
    if(m_inbufLen < HEAD_SIZE)  // ����Ƿ�ԭ��������
        return false;

    int packsize = swab32_array(&m_bufInput[m_inbufStart]) + HEAD_SIZE;  // ��ȡ����С
    if(packsize > m_inbufLen)  // ����߼����Ƿ�������
            return false;

    void* pBuf = bytes.m_pContent;
    // �����ڶ����зֶδ�ţ�ѭ������β����ͷ����
    if(m_inbufStart + packsize > INBUFSIZE)	 
    {
        int copylen = INBUFSIZE - m_inbufStart;  // β����Ҫ�����������ݳ���
        assert(copylen < packsize);
        assert(m_inbufStart + copylen == INBUFSIZE);

        memcpy(pBuf, m_bufInput + m_inbufStart, copylen); // ��β������������
        // ��ͷ������������
        memcpy((unsigned char *)pBuf + copylen, m_bufInput, packsize - copylen); 
    }
    else // ���ݵ�ַ������ֱ�Ӵӽ��ն�����ȡ�� 
    {
        memcpy(pBuf, m_bufInput + m_inbufStart, packsize);
    }

    m_inbufStart = (m_inbufStart + packsize) % INBUFSIZE;
    m_inbufLen -= packsize;
    assert(m_inbufLen >= 0);

    ReadPacketHead(bytes);
    return	true;
}

BOOL GameSocket::RecvFromSock(void) // һ�δ������ж�ȡ�����ܶ������
{
	if(m_state && m_inbufLen < INBUFSIZE)
	{
		int	savelen, savepos; // ���ݿ��Ա���ĳ��ȣ����ݱ����λ��

		if(m_inbufStart + m_inbufLen < INBUFSIZE) // ������ն��к�ʣ��ĳ��ȹ���ţ�ȫ�����浽���к�
			savelen = INBUFSIZE - (m_inbufStart + m_inbufLen); // �󲿿ռ䳤��
		else
			savelen = INBUFSIZE - m_inbufLen; // �ܹ����ܷŵĳ���

		savepos = (m_inbufStart + m_inbufLen) % INBUFSIZE; // ʵ�ʿ�ʼ�洢��λ��
		
		// �洢���ݵ����ն���β��
		int inlen = recv(m_socket, m_bufInput + savepos, savelen, 0); 

		if(inlen > 0) // ���ճɹ�
		{
			m_inbufLen += inlen;
			
			// ������к󲿿ռ䲻�㱣���������ݣ�ѭ��������ͷ����ʼ����
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
					int err = WSAGetLastError();  // �����ѶϿ����ߴ��󣨰���������
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
			int err = WSAGetLastError(); // �����ѶϿ����ߴ��󣨰���������
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
		if(m_outbufLen > 0) // ����һ������
		{
			int	outsize;
			outsize = send(m_socket, m_bufOutput, m_outbufLen, 0);
			if(outsize > 0)
			{
				if(m_outbufLen - outsize > 0) // ɾ���ѷ��͵Ĳ���
					memcpy(m_bufOutput, m_bufOutput + outsize, m_outbufLen - outsize);

				m_outbufLen -= outsize;
				assert(m_outbufLen >= 0);
			}
			else // ����
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

	UINT32 packsize = swab32_array(pBuf);//u_char)low + (u_char)high * 256;  // ��ȡ��ͷ

	memcpy(pBuf , pBuf+HEAD_SIZE, packsize);
	bytes.m_nTotalSize = packsize;

	return packsize;
}


