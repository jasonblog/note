//
//  CListener.h
//  Created by DGuco on 18/01/20.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _LISTENER_H
#define _LISTENER_H

#include "event_reactor.h"
#include "net_addr.h"
#include "socket.h"
#include "network_interface.h"
#include <timer_event.h>
#include <event2/listener.h>

enum eListenerState
{
	eLS_UnListen = 0,
	eLS_Listened,
};

class CListener: public IReactorHandler
{
public:
	//构造函数
	CListener(IEventReactor *pReactor, int listenQueue);
	//析构函数
	virtual ~CListener(void);
	//监听
	bool Listen(CNetAddr &addr, FuncListenerOnAccept pFunc);
	//获取event_base
	IEventReactor *GetReactor();
	//关闭
	void ShutDown();
	//是否监听中
	bool IsListened();

private:
	//设置状态
	void SetState(eListenerState eState);
	//注册
	bool RegisterToReactor();
	//卸载
	bool UnRegisterFromReactor();
	//监听回调
	static void lcb_Listen(struct evconnlistener *listener,
						   evutil_socket_t fd,
						   struct sockaddr *sa,
						   int socklen, void *arg);
	//监听出错
	static void lcb_AcceptError(struct evconnlistener *listener, void *ctx);
	//处理监听
	void HandleInput(int Socket, struct sockaddr *sa);
private:
	CNetAddr m_ListenAddress;
	event m_event;

	IEventReactor *m_pEventReactor;
	eListenerState m_eState;
	FuncListenerOnAccept m_pFuncOnAccept;
	int m_iListenQueueMax;
	struct evconnlistener *m_pListener;
};
#endif
