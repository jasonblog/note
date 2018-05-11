//
// Created by dguco on 18-1-30.
// 发送客户端消息线程
//

#ifndef SERVER_S2C_THREAD_H
#define SERVER_S2C_THREAD_H

#include <message.pb.h>
#include "code_queue.h"
#include "mythread.h"

class CS2cHandle: public CMyThread
{
public:
	//构造函数
	CS2cHandle();
	//析构函数
	virtual ~CS2cHandle();
public:
	//准备run
	int PrepareToRun() override;
	//run
	void RunFunc() override;
	//线程阻塞条件
	bool IsToBeBlocked() override;
private:
	//检测发送队列
	void CheckWaitSendData();
	//向client下行数据包
	int SendClientData(CMessage &tmpMes, std::shared_ptr<CByteBuff> tmpBuff);
	//接收gameserver 数据
	int RecvServerData(char *data);
	//给特定client发送数据
	void SendToClient(const CSocketInfo &socketInfo, const char *data, unsigned int len);
public:
	// game --> tcp通信共享内存管道
	static CCodeQueue *m_pS2CPipe;
};


#endif //SERVER_S2C_THREAD_H
