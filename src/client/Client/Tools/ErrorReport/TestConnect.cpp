#include "StdAfx.h"
#include "TestConnect.h"
#include <winsock2.h>
#pragma  comment (lib, "ws2_32.lib")

class AutoCleanWSA
{
public:
	~AutoCleanWSA()
	{
	    WSACleanup();
	}
};

class AutoCloseSocket
{
public:
	~AutoCloseSocket()
	{
		::closesocket(mSock);
	}
public:
	explicit AutoCloseSocket(::SOCKET s)
		: mSock(s)
	{
	}
private:
	::SOCKET mSock;
};


bool TestConnect::Run(const char * ip, int port, int waitSeconds)
{
    WSADATA wsaData;
    int ret = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if(ret != 0) 
        return false;	

	AutoCleanWSA autoCleanWSA;

	// �ж��Ƿ�����
	if (!('0' <= ip[0] && ip[0] <= '9'))
	{
		const hostent * host = gethostbyname(ip);
		if(host == NULL) 
			return false;

		ip = inet_ntoa(*reinterpret_cast<in_addr*>(host->h_addr_list[0]));
	}

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	if (sock == INVALID_SOCKET)
		return false;

	AutoCloseSocket autoCloseSocket(sock);

	// ���Ŀ��ip, port
	::sockaddr_in addr;
	::memset(&addr, 0, sizeof(addr));
	
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	// ��Ϊ������
	u_long ul = 1;
	::ioctlsocket(sock, FIONBIO, &ul);
	::connect(sock, (sockaddr *)&addr, sizeof(addr));
	
	// �ȴ������Ƿ�����������	
	::timeval tm;
	tm.tv_usec = 0;
	tm.tv_sec = waitSeconds;

	::fd_set writeSet;
	writeSet.fd_array[0] = sock;
	writeSet.fd_count = 1;
	::select(sock + 1, NULL, &writeSet, NULL, &tm);

	bool isConnected = (writeSet.fd_count == 1);
	return isConnected;
}
