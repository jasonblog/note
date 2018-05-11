//
// Created by DGuco on 17-6-21.
// 客户端连接管理
//

#ifndef SERVER_CLIENT_HANDLE_H
#define SERVER_CLIENT_HANDLE_H

#include <mythread.h>
#include "base.h"
#include "message_interface.h"
#include "message.pb.h"
#include "byte_buff.h"
#include "../datamodule/inc/player.h"


// 提取消息错误码
enum ClienthandleErrCode
{
	CLIENTHANDLE_SUCCESS = 0,            // 序列化消息成功
	CLIENTHANDLE_QUEUE_CRASH = 1,            // 从管道提取消息出错
	CLIENTHANDLE_QUEUE_EMPTY = 2,            // 从管道取出长度为空
	CLIENTHANDLE_SMALL_LENGTH = 3,            // 小于最小长度
	CLIENTHANDLE_TOTAL_LENGTH = 4,            // tcp转发过来的总长度不匹配
	CLIENTHANDLE_PARSE_FAILED = 5,            // 序列化ClientMessage失败
	CLIENTHANDLE_HASCLOSED = 6,            // 连接池中找不到玩家实体，已经关闭连接
	CLIENTHANDLE_NOTSAMETEAM = 7,            // 不是同一个玩家
	CLIENTHANDLE_NOTEAM = 8,            // 找不到玩家实体
	CLIENTHANDLE_CREATETEAMFAILED = 9,            // 创建玩家实体失败
	CLIENTHANDLE_STATUSINVALID = 10,            // 找到玩家但状态不对
	CLIENTHANDLE_ONLINEFULL = 11,            // 服务器在线已满
	CLIENTHANDLE_MSGINVALID = 12,            // 消息异常
	CLIENTHANDLE_ISNOTNORMAL = 13,            // 服务器状态异常
	CLIENTHANDLE_LOGINLIMITTIME = 14,            // 登陆受限制
	CLIENTHANDLE_LOGINCHECK = 15,            // 登陆去验证
	// CLIENTHANDLE_BUSY						        = 16,		    // 当前玩家有消息在处理中
};

class CCSHead;
class CCodeQueue;
class CSharedMem;

class CClientHandle: public CMyThread
{
public:
	CClientHandle();
	virtual ~CClientHandle();

public:
	int PrepareToRun() override;
	void RunFunc() override;
	bool IsToBeBlocked() override;
public:
	// 断开玩家连接
	void DisconnectClient(int iSocket, time_t tCreateTime);
	// 打印管道状态
	void Dump(char *pBuffer, unsigned int &uiLen);
	//检查是有数据可读
	int CheckData();
	int DealClientMessage(std::shared_ptr<CMessage> pMsg);
	int SendResToPlayer(std::shared_ptr<Message> pMessage, CPlayer *pPlayer);
	int SendResponse(std::shared_ptr<Message> pMessage, std::shared_ptr<MesHead> mesHead);
	int Push(int cmd, std::shared_ptr<Message> pMessage, stPointList *pPlayerList);
	int RecvClientData();
protected:
	// tcp --> game 共享内存管道起始地址
	CCodeQueue *mC2SPipe;
	// game --> tcp 共享内存管道起始地址
	CCodeQueue *mS2CPipe;
	CByteBuff *m_pRecvBuff;
};

#endif //SERVER_CLIENT_HANDLE_H
