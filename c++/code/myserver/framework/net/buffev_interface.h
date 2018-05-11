//
//  buffer_event.h
//  Created by DGuco on 18/01/27.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef SERVER_CBUFFEREVENT_H
#define SERVER_CBUFFEREVENT_H


#include "network_interface.h"
#include "socket.h"

class IBufferEvent: public IReactorHandler
{
public:
	//构造函数
	IBufferEvent(IEventReactor *pReactor,
				 int socket,
				 FuncBufferEventOnDataSend funcOnDataSend,
				 FuncBufferEventOnDataRecv funcOnDataRecv,
				 FuncBufferEventOnDisconnected funcDisconnected);
	//析构函数
	virtual ~IBufferEvent();
	//发送数据
	int Send(const void *pData, unsigned int uSize);
	//通过socket发送
	int SendBySocket(const void *pData, unsigned int uSize);
	//获取数据(返回读取数据长度)
	unsigned int RecvData(void *data, unsigned int size);
	//读取读缓冲区当前数据包的总长度
	unsigned short ReadRecvPackLen();
	//获取读缓冲区数据长度
	unsigned int GetRecvDataSize();
	//获取写缓冲区数据长度
	unsigned int GetSendDataSize();
	//设置发送缓冲区最大值
	void SetMaxSendBufSize(unsigned int uSize);
	//获取发送缓冲区最大值
	unsigned int GetMaxSendBufSize();
	//设置接收缓冲区最大值
	void SetMaxRecvBufSize(unsigned int uSize);
	//获取接收缓冲区最大值
	unsigned int GetMaxRecvBufSize();
	//检测event_base是否有效
	bool IsEventBuffAvailable();
	//获取当前数据包的总长度
	unsigned short GetRecvPackLen() const;
	//当前数据包已读取
	void CurrentPackRecved();
	//获取socket
	const CSocket &GetSocket() const;
	//数据是否完整
	bool IsPackageComplete();
protected:
	//读回调
	static void lcb_OnRead(struct bufferevent *bev, void *arg);
	//写回调
	static void lcb_OnWrite(bufferevent *bev, void *arg);
	//错误回调
	static void lcb_OnEvent(bufferevent *bev, int16 nWhat, void *arg);
public:    //获取event base
	IEventReactor *GetReactor() override;
	//注册event
	bool RegisterToReactor() override;
	//卸载event
	bool UnRegisterFromReactor() override;
private:
	//事件回调
	virtual void OnEvent(int16 nWhat) = 0;
	//bufferEvent 无效处理
	virtual void BuffEventUnavailableCall() = 0;
	//event buffer 创建成功后处理
	virtual void AfterBuffEventCreated() = 0;
protected:
	IEventReactor *m_pReactor;
	bufferevent *m_pStBufEv;
	CSocket m_oSocket;
	unsigned int m_uMaxOutBufferSize;
	unsigned int m_uMaxInBufferSize;
	unsigned short m_uRecvPackLen;
	FuncBufferEventOnDataSend m_pFuncOnDataSend;
	FuncBufferEventOnDataRecv m_pFuncOnDataRecv;
	FuncBufferEventOnDisconnected m_pFuncDisconnected;
};


#endif //SERVER_CBUFFEREVENT_H
