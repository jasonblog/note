//
//  socket.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//


#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <base.h>
#include "timer_event.h"
#include "net_inc.h"

class CNetAddr;

class CSocket
{
public:
	//构造函数
	CSocket(SOCKET socket);
	CSocket(int32 nType = SOCK_STREAM, int32 nProtocolFamily = AF_INET, int32 nProtocol = 0);
	//析构函数
	~CSocket();
	//打开
	bool Open();
	//关闭
	void Close();
	//shutdown
	void Shutdown();
	//shutdown read
	void ShutdownRead();
	//shutdown write
	void ShutdownWrite();
	//设置为非阻塞
	void SetNonblocking();
	//绑定端口
	uint32 Bind(const CNetAddr &addr);
	//获取本地ip
	bool GetLocalAddress(CNetAddr &addr) const;
	//获取远程ip
	bool GetRemoteAddress(CNetAddr &add) const;
	//获取socket fd
	SOCKET GetSocket() const
	{ return m_Socket; }
	//设置socket fd
	void SetSocket(SOCKET socket)
	{ m_Socket = socket; }
	//获取socket errpr
	int GetSocketError() const;
	//创建socket
	static SOCKET CreateSocket(int32 Type = SOCK_STREAM, int32 ProtocolFamily = AF_INET, int32 Protocol = 0);
	//创建socket并绑定地址
	static SOCKET CreateBindedSocket(const CNetAddr &address);
	//创建非阻塞socket
	static void MakeSocketNonblocking(SOCKET Socket);
	static void Address2SockAddrIn(sockaddr_in &saiAddress, const CNetAddr &address);
protected:
	SOCKET m_Socket;

private:
	int32 m_nType;
	int32 m_nProtocolFamily;
	int32 m_nProtocol;
};

#endif
