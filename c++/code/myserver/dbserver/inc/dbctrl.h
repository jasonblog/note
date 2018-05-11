//
// dbctrl.h
// Created by DGuco on 17-7-13.
// Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _DBCTRL_HPP_
#define _DBCTRL_HPP_

#include "net_work.h"
#include "thread_pool.h"
#include "queryresult.h"
#include "database.h"
#include "messagefactory.h"

//#include "protoconfig.h"

#ifdef _ASYNC_SQL_
#include "asqlexe.h"
#endif

#define MAXPROXYNUMBER                2                    // 最大proxy server 数目

#define MAXHANDLENUMBER                2

#define SECSOFONEHOUR                3600

#define CHECKINTERVAL                10                    // 检查时间间隔，单位：秒
#define PROXYKEEPALIVESECONDS        (3*CHECKINTERVAL)    // 与proxy的心跳超时时间
#define MAXPROXYCODELEN                1024                // 向proxy发送纯命令的最大块长度
#define STATISTICSLEN                1024
#define MAXNUMBERLEN                10
#define SAVEINTERVAL                300


class CSharedMem;

class CDBCtrl: public CSingleton<CDBCtrl>
{
public:
	// 运行标志
	enum ERunFlag
	{
		EFLG_CTRL_NO = 0,
		EFLG_CTRL_QUIT = 1,
		EFLG_CTRL_SHUTDOWN = 2,
		EFLG_CTRL_RELOAD = 3
	};
public:
	//构造函数
	CDBCtrl();
	//析构函数
	~CDBCtrl();
	//准备启动
	int PrepareToRun();
	//启动
	int Run();
	//设置运行标志
	void SetRunFlag(int iFlag);
	//清除运行标志
	void ClearRunFlag(int iFlag);
	//运行标志是否设置
	bool IsRunFlagSet(int iFlag);
	//转发笑傲析
	int SendMessageTo(CProxyMessage *pMsg);
	//处理数据
	int Event(CProxyMessage *pMsg);
	//处理sql
	int ProcessExecuteSqlRequest(CProxyMessage *pMsg);
	//释放结果
	void ReleaseResult(QueryResult *res)
	{
		if (res != NULL) {
			delete res;
			res = NULL;
		}
	}
private:
	//连接proxy
	int ConnectToProxyServer();
	//向proxy注册
	int RegisterToProxyServer(CConnector *pConnector);
	//向proxy发送心跳消息
	int SendkeepAliveToProxy(CConnector *pConnector);
	//分发消息
	int DispatchOneCode(int nCodeLength, BYTE *pbyCode);
	//获取收到心跳的时间
	time_t GetLastSendKeepAlive() const;
	//获取上次发送心跳的时间
	time_t GetLastRecvKeepAlive() const;
	//设置上次发送心跳的时间
	void SetLastSendKeepAlive(time_t tLastSendKeepAlive);
	//设置上次收到心跳的时间
	void SetLastRecvKeepAlive(time_t tLastRecvKeepAlive);
private:
	//连接成功回调
	static void lcb_OnConnected(IBufferEvent *pBufferEvent);
	//断开连接回调
	static void lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent);
	//连接失败回调(无用)
	static void lcb_OnConnectFailed(IBufferEvent *pBufferEvent);
	//发送数据回调(无用)
	static void lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent);
	//发送心跳倒计时回调
	static void lcb_OnPingServer(int fd, short event, CConnector *pConnector);
public:
	static char m_acRecvBuff[MAX_PACKAGE_LEN];
	static int m_iProxyId;
private:
	int m_iRunFlag;    // 运行标志
	time_t m_tLastSendKeepAlive;        // 最后发送proxy心跳消息时间
	time_t m_tLastRecvKeepAlive;        // 最后接收proxy心跳消息时间
	Database *m_pDatabase;
	CMessageFactory *mMsgFactory;
//	CProxyHead m_stCurrentProxyHead;    //当前处理请求的Proxy头部
	CNetWork *m_pNetWork;
};

#endif


