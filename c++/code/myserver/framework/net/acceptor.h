//
//  acceptor.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "base.h"
#include "buffev_interface.h"
#include "net_inc.h"
#include "socket.h"
#include "event_reactor.h"
#include "timer_event.h"
#include "net_addr.h"


class CAcceptor: public IBufferEvent
{
	enum eAcceptorState
	{
		eAS_Disconnected = 0,
		eAS_Connected,
	};

public:
	//构造函数
	CAcceptor(SOCKET socket,
			  IEventReactor *pReactor,
			  CNetAddr *netAddr,
			  FuncBufferEventOnDataSend funcOnDataSend,
			  FuncBufferEventOnDataRecv funcOnDataRecv,
			  FuncBufferEventOnDisconnected funcDisconnected);
	//析构函数
	virtual ~CAcceptor();
	//获取该连接的ip
	void GetRemoteIpAddress(char *szBuf, unsigned int uBufSize);
	//关闭连接
	void ShutDown();
	//当前是否连接
	bool IsConnected();
	//获取创建时间
	time_t GetCreateTime();
	//获取上次活跃时间
	time_t GetLastKeepAlive();
	//更新上次活跃时间
	void SetLastKeepAlive(time_t tmpLastKeepAlive);
private:
	//bufferEvent 无效处理
	void BuffEventUnavailableCall() override;
	//event buffer 创建成功后处理
	void AfterBuffEventCreated() override;
private:
	//获取连接状态
	eAcceptorState GetState();
	//设置连接状态
	void SetState(eAcceptorState eState);
	//事件回调
	void OnEvent(int16 nWhat) override;
private:
	CNetAddr *m_pNetAddr;
	eAcceptorState m_eState;
	time_t m_tCreateTime;
	time_t m_tLastKeepAlive;
};

#endif