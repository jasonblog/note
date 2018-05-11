//
// Created by dguco on 18-2-5.
// 服务器之间连接管理
//

#ifndef SERVER_SERVER_HANDLE_H
#define SERVER_SERVER_HANDLE_H
#include <net_work.h>

class CServerHandle
{
public:
	//构造函数
	CServerHandle();
	//系够函数
	virtual ~CServerHandle();
	// 连接到Proxy
	bool Connect2Proxy();
	// 给DB Server发消息
	bool SendMessageToDB(CProxyMessage *pMsg);    //获取收到心跳的时间
	time_t GetLastSendKeepAlive() const;
	//获取上次发送心跳的时间
	time_t GetLastRecvKeepAlive() const;
	//设置上次发送心跳的时间
	void SetLastSendKeepAlive(time_t tLastSendKeepAlive);
	//设置上次收到心跳的时间
	void SetLastRecvKeepAlive(time_t tLastRecvKeepAlive);
public:
	//运行准备
	int PrepareToRun();
	void Run();
private:
	void SendMessageToProxy(char *data, unsigned short len);
	//向Proxy注册
	bool Register2Proxy();
	//想proxy 发送心跳
	bool SendKeepAlive2Proxy();
private:
	//客户端上行数据回调（无用）
	static void lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent);
	static void lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent);
	static void lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent);
	//连接失败（无用）
	static void lcb_OnConnectFailed(CConnector *pConnector);
	static void lcb_OnConnected(CConnector *pConnector);
	static void lcb_OnPingServer(int fd, short what, CConnector *pConnector);
	static void DealServerData(IBufferEvent *pConnector);
	static void SetProxyId(int id);
	static int GetProxyId();
private:
	CNetWork *m_pNetWork;                // 服务器间通信的连接
	time_t m_tLastSendKeepAlive;        // 最后发送proxy心跳消息时间
	time_t m_tLastRecvKeepAlive;        // 最后接收proxy心跳消息时间
private:
	static int m_iProxyId;
	static char m_acRecvBuff[MAX_PACKAGE_LEN];
};


#endif //SERVER_SERVER_HANDLE_H
