//
//  connector.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include "network_interface.h"
#include "socket.h"
#include "event_reactor.h"
#include "net_addr.h"
#include "buffev_interface.h"
#include "timer_event.h"

class CConnector: public IBufferEvent
{
public:
	enum eConnectorState
	{
		eCS_Disconnected = 0,
		eCS_Connecting,
		eCS_Connected,
	};
public:
	//构造函数
	CConnector(IEventReactor *pReactor,
			   FuncBufferEventOnDataSend funcOnDataSend,
			   FuncBufferEventOnDataRecv funcOnDataRecv,
			   FuncBufferEventOnDisconnected funcDisconnected,
			   int iTargetId,
			   int iPingTick);
	//析构函数
	virtual ~CConnector(void);
	//获取连接ip
	void GetRemoteIpAddress(char *szBuf, uint32 uBufSize);
	//连接
	bool Connect(const CNetAddr &addr);
	//重新连接
	bool ReConnect();
	//设置相关回调
	void SetCallbackFunc(FuncConnectorOnConnectFailed pOnConnectFailed,
						 FuncConnectorOnConnectted pOnConnected,
						 FuncConnectorOnPingServer pOnPingServer);
	//关闭连接
	void ShutDown();
	//是否连接成功
	bool IsConnected();
	//是否正在连接
	bool IsConnecting();
	//是否断开连接
	bool IsDisconnected();
	int GetTargetId() const;
	//设置当前状态
	void SetState(eConnectorState eState);
	//当前连接状态
	eConnectorState GetState();
private:
	//bufferEvent 无效处理
	void BuffEventUnavailableCall() override;
	//event buffer 创建成功后处理
	void AfterBuffEventCreated() override;
	//事件回调
	void OnEvent(int16 nWhat) override;
private:
	//连接成功
	void OnConnectted();
private:
	static void lcb_OnPingServer(int fd, short event, void *param);

private:
	CNetAddr m_oAddr;
	eConnectorState m_eState;
	event m_oConnectEvent;
	int m_iTargetId;
	int m_iPingTick; //单位秒
	CTimerEvent *m_pKeepLiveEvent;
	FuncConnectorOnConnectFailed m_pFuncOnConnectFailed;
	FuncConnectorOnConnectted m_pFuncOnConnectted;
	FuncConnectorOnPingServer m_pFuncOnPingServer;
};

#endif
