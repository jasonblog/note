//
// Created by dguco on 18-2-5.
//

#include <config.h>
#include <server_comm_engine.h>
#include "connector.h"
#include "my_assert.h"
#include "../inc/server_handle.h"
#include "../inc/game_server.h"

int CServerHandle::m_iProxyId = 0;

char CServerHandle::m_acRecvBuff[MAX_PACKAGE_LEN] = {0};

CServerHandle::CServerHandle()
	: m_pNetWork(new CNetWork()),
	  m_tLastSendKeepAlive(0),
	  m_tLastRecvKeepAlive(0)
{

}

CServerHandle::~CServerHandle()
{

}

int CServerHandle::PrepareToRun()
{
	if (!Connect2Proxy()) {
		return -1;
	}
	return 0;
}

void CServerHandle::Run()
{
	m_pNetWork->DispatchEvents();
}

// 连接到Proxy
bool CServerHandle::Connect2Proxy()
{
	ServerInfo *rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	if (!m_pNetWork->Connect(rTmpProxy->m_sHost.c_str(),
							 rTmpProxy->m_iPort,
							 rTmpProxy->m_iServerId,
							 &CServerHandle::lcb_OnCnsSomeDataSend,
							 &CServerHandle::lcb_OnCnsSomeDataRecv,
							 &CServerHandle::lcb_OnCnsDisconnected,
							 &CServerHandle::lcb_OnConnectFailed,
							 &CServerHandle::lcb_OnConnected,
							 &CServerHandle::lcb_OnPingServer,
							 CServerConfig::GetSingletonPtr()->GetTcpKeepAlive() / 1000)
		) {
		LOG_ERROR("default", "[{} : {} : {}] Connect to Proxy({}:{})(id={}) failed.",
				  __MY_FILE__, __LINE__, __FUNCTION__,
				  rTmpProxy->m_sHost.c_str(), rTmpProxy->m_iPort, rTmpProxy->m_iServerId);
		return false;
	}
	LOG_INFO("default", "Connect to Proxy({}:{})(id={}) succeed.",
			 rTmpProxy->m_sHost.c_str(), rTmpProxy->m_iPort, rTmpProxy->m_iPort);
	m_tLastRecvKeepAlive = GetMSTime();
	m_tLastSendKeepAlive = GetMSTime();
	return true;
}

// 向Proxy注册
bool CServerHandle::Register2Proxy()
{
	CProxyMessage tmpMessage;
	char acTmpMessageBuffer[1024];
	unsigned short unTmpTotalLen = sizeof(acTmpMessageBuffer);

	ServerInfo *rTmpProxy =
		CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	ServerInfo
		*rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
	pbmsg_setproxy(tmpMessage.mutable_msghead(),
				   enServerType::FE_GAMESERVER,
				   rTmpGame->m_iServerId,
				   enServerType::FE_PROXYSERVER,
				   rTmpProxy->m_iServerId,
				   GetMSTime(),
				   enMessageCmd::MESS_REGIST);

	int iRet =
		CServerCommEngine::ConvertMsgToStream(&tmpMessage, acTmpMessageBuffer, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "ConvertMsgToStream failed, iRet = {}.", iRet);
		return false;
	}

	SendMessageToProxy(acTmpMessageBuffer, unTmpTotalLen);
	return true;
}

// 发送心跳到Proxy
bool CServerHandle::SendKeepAlive2Proxy()
{
	CProxyMessage tmpMessage;
	char acTmpMessageBuffer[1024];
	unsigned short unTmpTotalLen = sizeof(acTmpMessageBuffer);
	ServerInfo *rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	ServerInfo *rTmpGame = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_GAMESERVER);
	pbmsg_setproxy(tmpMessage.mutable_msghead(),
				   enServerType::FE_GAMESERVER,
				   rTmpGame->m_iServerId,
				   enServerType::FE_PROXYSERVER,
				   rTmpProxy->m_iServerId,
				   GetMSTime(),
				   enMessageCmd::MESS_KEEPALIVE);

	int iRet = CServerCommEngine::ConvertMsgToStream(&tmpMessage, acTmpMessageBuffer, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[{} : {} : {}] ConvertMsgToStream failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return false;
	}
	SendMessageToProxy(acTmpMessageBuffer, unTmpTotalLen);
	return true;
}

bool CServerHandle::SendMessageToDB(CProxyMessage *pMsg)   //获取收到心跳的时间
{
	CProxyHead *pHead = pMsg->mutable_msghead();
	char acTmpMessageBuffer[MAX_PACKAGE_LEN];
	unsigned short unTmpTotalLen = sizeof(acTmpMessageBuffer);
	ServerInfo *gameInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType::FE_GAMESERVER);
	ServerInfo *dbInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType::FE_DBSERVER);
	int iTmpServerID = gameInfo->m_iServerId;
	int iTmpDBServerID = dbInfo->m_iServerId;

	pbmsg_setproxy(pHead,
				   FE_GAMESERVER,
				   iTmpServerID,
				   FE_DBSERVER,
				   iTmpDBServerID,
				   GetMSTime(),
				   enMessageCmd::MESS_NULL);

	int iRet = CServerCommEngine::ConvertMsgToStream(pMsg, acTmpMessageBuffer, unTmpTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "[{} : {} : {}] ConvertMsgToStream failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return false;
	}
//	iRet = m_ProxyClient.SendOneCode(unTmpTotalLen, (BYTE *) acTmpMessageBuffer);
	if (iRet != 0) {
		LOG_ERROR("default", "[{} : {} : {}] proxy(index={}) SendOneCode failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, 0, iRet);
		return false;
	}

	Message *pTmpUnknownMessagePara = (Message *) pMsg->msgpara();
// 如果是打印出错依然返回成功
	MY_ASSERT(pTmpUnknownMessagePara != NULL, return true);
	const ::google::protobuf::Descriptor *pDescriptor = pTmpUnknownMessagePara->GetDescriptor();
	LOG_DEBUG("default",
			  "---- Send DB({}) Msg[ {} ][id: {} / {}] ----",
			  pHead->dstid(),
			  pDescriptor->name().c_str(),
			  pMsg->msghead().messageid(),
			  pMsg->msghead().messageid());
	LOG_DEBUG("default", "[{}]", ((Message *) pMsg->msgpara())->ShortDebugString().c_str());
	return true;
}

time_t CServerHandle::GetLastSendKeepAlive() const
{
	return m_tLastSendKeepAlive;
}
time_t CServerHandle::GetLastRecvKeepAlive() const
{
	return m_tLastRecvKeepAlive;
}

void CServerHandle::SetLastSendKeepAlive(time_t tLastSendKeepAlive)
{
	m_tLastSendKeepAlive = tLastSendKeepAlive;
}

void CServerHandle::SetLastRecvKeepAlive(time_t tLastRecvKeepAlive)
{
	m_tLastRecvKeepAlive = tLastRecvKeepAlive;
}

void CServerHandle::SendMessageToProxy(char *data, unsigned short len)
{
	IBufferEvent *pConn = m_pNetWork->FindConnector(CServerHandle::GetProxyId());
	if (pConn == NULL) {
		LOG_ERROR("default", "ProxyServer connection has gone");
		return;
	}
	//直接通过socket发送，不同过buffer_event保证buffer_event线程安全
	int iRet = pConn->SendBySocket(data, len);
	if (iRet <= 0) {
		LOG_ERROR("default", "[{} : {} : {}] proxy SendOneCode failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return;
	}
}

void CServerHandle::lcb_OnConnected(CConnector *pConnector)
{
	MY_ASSERT(pConnector != NULL, return);
	CGameServer::GetSingletonPtr()->GetIoThread()
		->PushTaskBack([pConnector]
					   {
						   MY_ASSERT(pConnector != NULL, return);
						   SetProxyId(pConnector->GetTargetId());
						   CGameServer::GetSingletonPtr()->GetServerHandle()->Register2Proxy();
					   });
}

void CServerHandle::lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent)
{
	CGameServer::GetSingletonPtr()->GetIoThread()
		->PushTaskBack([pBufferEvent]
					   {
						   MY_ASSERT(pBufferEvent != NULL && typeid(*pBufferEvent) == typeid(CConnector), return);
						   // 断开连接重新连接到proxy服务器
						   if (((CConnector *) pBufferEvent)->ReConnect() < 0) {
							   LOG_ERROR("default", "Reconnect to proxyServer failed!");
							   return;
						   }
					   });
}

void CServerHandle::lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent)
{
	CGameServer::GetSingletonPtr()->GetIoThread()
		->PushTaskBack([pBufferEvent]
					   {
						   DealServerData(pBufferEvent);
					   });
}

void CServerHandle::lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent)
{
	MY_ASSERT(pBufferEvent != NULL, return);

}
void CServerHandle::lcb_OnConnectFailed(CConnector *pConnector)
{
	MY_ASSERT(pConnector != NULL, return);
}

void CServerHandle::lcb_OnPingServer(int fd, short what, CConnector *pConnector)
{
	CGameServer::GetSingletonPtr()->GetIoThread()
		->PushTaskBack([pConnector]
					   {
						   MY_ASSERT(pConnector != NULL, return);
						   time_t tNow = GetMSTime();
						   CServerConfig *tmpConfig = CServerConfig::GetSingletonPtr();
						   CServerHandle *tmpServerHandle = CGameServer::GetSingletonPtr()->GetServerHandle();
						   if (pConnector->GetState() == CConnector::eCS_Connected &&
							   pConnector->GetSocket().GetSocket() > 0 &&
							   tNow - tmpServerHandle->GetLastRecvKeepAlive() < tmpConfig->GetTcpKeepAlive() * 3) {
							   if (tNow - tmpServerHandle->GetLastSendKeepAlive() >= tmpConfig->GetTcpKeepAlive()) {
								   tmpServerHandle->SendKeepAlive2Proxy();
								   LOG_DEBUG("default", "SendkeepAliveToProxy succeed..");
							   }
						   }
						   else {
							   //断开连接
							   pConnector->SetState(CConnector::eCS_Disconnected);
							   // 断开连接重新连接到proxy服务器
							   if (pConnector->ReConnect() < 0) {
								   LOG_ERROR("default", "Reconnect to proxyServer failed!\n");
								   return;
							   }
						   }
					   });

}

void CServerHandle::DealServerData(IBufferEvent *pBufferEvent)
{
	MY_ASSERT(pBufferEvent != NULL, return);
	//数据不完整
	if (!pBufferEvent->IsPackageComplete()) {
		return;
	}
	unsigned short iTmpLen = pBufferEvent->GetRecvPackLen() - sizeof(unsigned short);
	//读取数据
	pBufferEvent->RecvData(m_acRecvBuff, iTmpLen);
	pBufferEvent->CurrentPackRecved();
	// 将收到的二进制数据转为protobuf格式
	CProxyMessage tmpMessage;
	int iRet = CServerCommEngine::ConvertStreamToMsg(m_acRecvBuff,
													 iTmpLen,
													 &tmpMessage,
													 CGameServer::GetSingletonPtr()->GetMessageFactory());
	if (iRet < 0) {
		LOG_ERROR("default", "[{} : {} : {}] convert stream to message failed, iRet = {}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return;
	}

	ServerInfo *rTmpProxy = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	CProxyHead tmpProxyHead = tmpMessage.msghead();
	if (tmpProxyHead.has_srcfe() && tmpProxyHead.srcfe() == FE_PROXYSERVER) {
		if (tmpProxyHead.has_srcid() && tmpProxyHead.srcid() == (unsigned short) rTmpProxy->m_iServerId
			&& tmpProxyHead.has_opflag() && tmpProxyHead.opflag() == enMessageCmd::MESS_KEEPALIVE) {
			// 设置proxy为已连接状态
			CGameServer::GetSingletonPtr()->GetServerHandle()->SetLastRecvKeepAlive(GetMSTime());
		}
		return;
	}
	// 处理服务器间消息
	CGameServer::GetSingletonPtr()->GetLogicThread()->PushTaskBack(
		&CMessageDispatcher::ProcessServerMessage, &tmpMessage);
}

void CServerHandle::SetProxyId(int id)
{
	m_iProxyId = id;
}

int CServerHandle::GetProxyId()
{
	return m_iProxyId;
}
