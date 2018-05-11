//
//  gate_ctrl.h
//  gate_ctrl 管理类头文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _PROXY_CTRL_HPP_
#define _PROXY_CTRL_HPP_

#include <mutex>
#include "server_tool.h"
#include "net_work.h"

struct TStatLog
{
	int iRcvCnt;
	int iRcvSize;
	int iRcvErrCnt;
	int iSndCnt;
	int iSndSize;
	int iClnCnt;
	int iClnSize;

	TStatLog()
	{
		Clear();
	}

	void Clear()
	{
		iRcvCnt = 0;
		iRcvSize = 0;
		iRcvErrCnt = 0;
		iSndCnt = 0;
		iSndSize = 0;
		iClnCnt = 0;
		iClnSize = 0;
	}
};

class CProxyCtrl: public CSingleton<CProxyCtrl>
{
public:
	//构造函数
	CProxyCtrl();
	//析构函数
	~CProxyCtrl();
	// 准备
	int PrepareToRun();
	// 执行线程
	int Run();
	// 通过KEY获取连接信息
	IBufferEvent *GetConnByKey(int iKey);
	// 通过FE和ID创建KEY
	int MakeConnKey(const short nType, const short nID);
	//处理注册消息
	int DealRegisterMes(IBufferEvent *pBufferEvent, unsigned short iLen);
	// 转发数据
	int TransferOneCode(IBufferEvent *pBufferEvent, unsigned short nCodeLength);
	//关闭socket
	void CloseConnection(int socket);
	// 发送数据
	int SendOneCodeTo(short nCodeLength, BYTE *pbyCode, int iKey, bool bKeepalive);
protected:
	//客户端连接还回调
	static void lcb_OnAcceptCns(uint32 uId, IBufferEvent *pBufferEvent);
	//客户端断开连接回调
	static void lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent);
	//客户端上行数据回调
	static void lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent);
	//连接超时
	static void lcb_OnAcceptorTimeOut(CAcceptor *pAcceptor);
private:
	CNetWork *m_pNetWork;
	TStatLog m_stStatLog;
	static std::map<int/*key*/, int/*socket id*/> m_mapRegister;
	static std::map<int/*socket id*/, int/*key*/> m_mapSocket2Key;
	static char m_acRecvBuff[MAX_PACKAGE_LEN];
};

#endif // _PROXY_CTRL_HPP_
