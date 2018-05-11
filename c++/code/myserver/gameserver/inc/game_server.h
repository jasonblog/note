//
//  gameserver.h
//  gameserver 头文件
//  Created by DGuco on 17-3-1.
//  Copyright © 2017年 DGuco. All rights reserved.
//

#ifndef SERVER_GAMESERVER_H
#define SERVER_GAMESERVER_H

#include <thread_pool.h>
#include "server_tool.h"
#include "runflag.h"
#include "base.h"
#include "timer.h"
#include "message_interface.h"
#include "message.pb.h"
#include "dbmessage.pb.h"
#include "message_dispatcher.h"
#include "client_handle.h"
#include "net_work.h"
#include "server_handle.h"
#include "../datamodule/inc/player.h"
#include "../logicmodule/inc/module_manager.h"

class CGameServer: public CSingleton<CGameServer>
{
public:
	enum EServerState
	{
		ESS_CONNECTPROXY = 0x0001,    // 链接proxy
		ESS_LOADDATA = 0x0002,    // 加载数据
		ESS_PROCESSINGDATA = 0x0004,    // 处理数据
		ESS_SAVEDATA = 0x0008,    // 停服存储数据

		ESS_NORMAL = (ESS_CONNECTPROXY | ESS_LOADDATA | ESS_PROCESSINGDATA),        // 正常
		ESS_PROCESSINGCLIENTMSG = (ESS_LOADDATA | ESS_PROCESSINGDATA)    // 可以正常处理客户端上行的消息
	};

public:
	CGameServer();
	~CGameServer();

	// 初始化
	int Initialize();
	// 运行准备
	int PrepareToRun();
	// 运行
	void Run();
	// 退出
	void Exit();

public:
	// 设置服务器状态
	void SetServerState(int iState)
	{ miServerState = (miServerState | iState); }
	// 清除服务器状态
	void EraseServerState(int iState)
	{ miServerState = (miServerState & (~iState)); }
	// 判断服务器状态
	bool IsOk(int iState)
	{ return ((miServerState & iState) == iState); }
	// 服务器是否正常
	bool IsNormal()
	{ return miServerState == ESS_NORMAL; }

	// 服务器拉取数据完成
	void LoadDataFinish();
	// 服务器开始处理初始数据
	void StartProcessingInitData();

	// 是否能正常处理客户端上行消息
	bool CanProcessingClientMsg()
	{ return (miServerState & ESS_PROCESSINGCLIENTMSG) == ESS_PROCESSINGCLIENTMSG; }
	// 创建实体
	int CreateEntity(CPlayer *pPlayer);
	// 销毁实体
	void DestroyEntity(CPlayer *pPlayer);

	// 处理客户端上行消息
	void ProcessClientMessage(CMessage *pMsg, CPlayer *pPlayer);
	// 处理服务器内部消息
	void ProcessRouterMessage(CProxyMessage *pMsg);

	// 给DB Server发消息
	bool SendMessageToDB(CProxyMessage *pMsg);
	// 给World Server 发消息

	// 广播消息给玩家，广播时，发起人一定放第一个
	int Push(unsigned int iMsgID, std::shared_ptr<Message> pMsgPara, stPointList *pTeamList);
	// 推送消息给单个玩家
	int Push(unsigned int iMsgID, std::shared_ptr<Message> pMsgPara, CPlayer *pPlayer);
	// 回复客户端上行的请求
	int SendResponse(std::shared_ptr<Message> pMsgPara, CPlayer *pPlayer);
	int SendResponse(std::shared_ptr<Message> pMsgPara, std::shared_ptr<MesHead> mesHead);

	// 通过消息ID获取模块类型
	int GetModuleClass(int iMsgID);

	// 主动断开链接
	void DisconnectClient(CPlayer *pPlayer);

	// 设置服务器运行状态
	void SetRunFlag(ERunFlag eRunFlag);
	// 检查服务器状态
	int CheckRunFlags();
	// 刷新服务器状态
	void FreshGame();

	// 读取配置
	int ReadCfg();

	// 根据配置初始化信息
	int InitializeByConfig();

	// 开启所有定时器
	int StartAllTimers();
	// 服务器间心跳检测
	static void OnTimeCheckStateInServer(CTimerBase *pTimer);
	// perf日志打印
	static void OnTimePerfLog(CTimerBase *pTimer);

	// 存储所有玩家数据(一般故障拉起或程序退出时使用)
	void SaveAllTeamdata();
	// 检测是否所有玩家存储结束
	static void CheckSaveAllTeamsFinish(CTimerBase *pTimer);

	// 拉取服务器数据
	void StartLoadAllData();
	// 检测是否开启拉取服务器数据
	static void CheckStartLoadAllData(CTimerBase *pTimer);
	// 存储服务器数据
	void StartSaveAllData();
	// 检测是否停服存储数据
	static void CheckStartSaveAllData(CTimerBase *pTimer);
	// 获取服务器ID
	int InitStaticLog();
	// 限制玩家登陆
	int LimitTeamLogin(unsigned int iTeamID, time_t iTimes); // itimes 暂定为小时
	// 获取消息工厂
	CFactory *GetMessageFactory();
	// 获取逻辑线程
	CThreadPool *GetLogicThread();
	// 获取io线程
	CThreadPool *GetIoThread();
	//获取timer管理器
	CTimerManager *GetTimerManager();
	//获取serverhandle
	CServerHandle *GetServerHandle();
public:
	// 为找不到CTeam的连接发送消息
	void SendMsgSystemErrorResponse(int iResult,
									long lMsgGuid,
									int iServerID,
									time_t tCreateTime,
									bool bKickOff = false);
private:
	CClientHandle *m_pClientHandle;             // 与客户端通信的连接线程
	CServerHandle *m_pServerHandle;             // 与服务器的连接管理(proxyserver)线程
	CModuleManager *m_pModuleManager;           // 模块管理器
	CMessageDispatcher *m_pMessageDispatcher;   // 消息派发器
	CFactory *m_pMessageFactory;                // 消息工厂
	CTimerManager *m_pTimerManager;             // 定时器管理器
	CRunFlag m_RunFlag;                         // 服务器运行状态
	CThreadPool *m_pLogicThread;                // 逻辑线程
	CThreadPool *m_pIoThread;                   // io线程(收发消息)
	CRunFlag mRunFlag;                          // 服务器运行状态
	int miServerState;    // 服务器状态
};
#endif //SERVER_GAMESERVER_H
