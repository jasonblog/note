//
//  net_work.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//


#ifndef _NET_WORK_H_
#define _NET_WORK_H_

#include <map>
#include <queue>
#include <unordered_map>
#include <server_tool.h>
#include "network_interface.h"
#include "event_reactor.h"
#include "mythread.h"
#include "listener.h"

using namespace std;
class CNetWork: public CSingleton<CNetWork>
{
public:
	typedef unordered_map<unsigned int, CConnector *> MAP_CONNECTOR;
	typedef unordered_map<unsigned int, CAcceptor *> MAP_ACCEPTOR;
	typedef std::queue<CSystemSignal *> Queue_TimerOrSignals;

public:
	//构造函数
	CNetWork();
	//析构函数
	virtual ~CNetWork();

	//开始监听
	bool BeginListen(const char *szNetAddr,
					 unsigned int uPort,
					 FuncAcceptorOnNew pOnNew,
					 FuncBufferEventOnDataSend funcAcceptorOnDataSend,
					 FuncBufferEventOnDataSend funcAcceptorOnDataRecv,
					 FuncBufferEventOnDataSend funcAcceptorDisconnected,
					 FuncOnTimeOut funcAcceptorTimeOut,
					 int listenQueue,
					 unsigned int uCheckPingTickTime);
	//结束监听
	void EndListen();
	//连接
	bool Connect(const char *szNetAddr,
				 uint16 uPort,
				 int targetId,
				 FuncBufferEventOnDataSend funcOnSomeDataSend,
				 FuncBufferEventOnDataSend funcOnSomeDataRecv,
				 FuncBufferEventOnDisconnected funcOnDisconnected,
				 FuncConnectorOnConnectFailed funcOnConnectFailed,
				 FuncConnectorOnConnectted funcOnConnectted,
				 FuncConnectorOnPingServer funcOnPingServer,
				 unsigned int uPingTick);
	//关闭acceptor
	bool ShutDownAcceptor(unsigned int uId);
	//设置信号回调
	void SetCallBackSignal(unsigned int uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop = false);
	//发送数据
	int ConnectorSendData(unsigned int uId, const void *pData, unsigned int uSize);
	//关闭connector
	bool ShutDownConnectorEx(unsigned int uId);
	//启动
	void DispatchEvents();
	//获取ping
	unsigned int GetConnectorExPingValue(unsigned int uId);
	//查找connector
	CConnector *FindConnector(unsigned int uId);
	//查找acceptor
	CAcceptor *FindAcceptor(unsigned int uId);
	//添加新的acceptor
	void InsertNewAcceptor(unsigned int uid, CAcceptor *pAcceptor);
	//添加新的connector
	void InsertNewConnector(unsigned int uid, CConnector *pConnector);
	//获取event
	IEventReactor *GetEventReactor();
	//获取连接map
	MAP_ACCEPTOR &GetAcceptorMap();
private:
	//新的连接 accept回调
	static void lcb_OnAccept(IEventReactor *pReactor, SOCKET socket, sockaddr *sa);
private:
	//创建acceptor
	void NewAcceptor(IEventReactor *pReactor, SOCKET socket, sockaddr *sa);
private:
	IEventReactor *m_pEventReactor;
	unsigned int m_uGcTime;

	MAP_CONNECTOR m_mapConnector;
	MAP_ACCEPTOR m_mapAcceptor;
	Queue_TimerOrSignals m_qTimerOrSignals;
	CListener *m_pListener;
	CTimerEvent *m_pCheckTimerOut;
	int m_iPingCheckTime;  //单位毫秒

	FuncAcceptorOnNew m_pOnNew;
	FuncBufferEventOnDataSend m_pFuncAcceptorOnDataSend;
	FuncBufferEventOnDataRecv m_pFuncAcceptorOnDataRecv;
	FuncBufferEventOnDisconnected m_pFuncAcceptorDisconnected;
};

#endif
