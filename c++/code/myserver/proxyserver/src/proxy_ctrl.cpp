//
//  gate_ctrl.cpp
//  gate_ctrl 管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//
#include "config.h"
#include "my_assert.h"
#include "acceptor.h"
#include "../inc/proxy_ctrl.h"
#include "client_comm_engine.h"
#include "server_comm_engine.h"

using namespace std;

template<> CProxyCtrl *CSingleton<CProxyCtrl>::spSingleton = NULL;

std::map<int/*key*/, int/*socket id*/> CProxyCtrl::m_mapRegister;

std::map<int/*key*/, int/*socket id*/> CProxyCtrl::m_mapSocket2Key;

char CProxyCtrl::m_acRecvBuff[MAX_PACKAGE_LEN] = {0};

CProxyCtrl::CProxyCtrl()
{
	m_pNetWork = new CNetWork();
}

CProxyCtrl::~CProxyCtrl()
{

}

int CProxyCtrl::PrepareToRun()
{
#ifdef _DEBUG_
	// 初始化日志
	INIT_ROATING_LOG("default", "../log/proxyserver.log", level_enum::trace);
#else
	// 初始化日志
	INIT_ROATING_LOG("default", "../log/proxyserver.log", level_enum::info);
#endif

	// 读取配置
	CServerConfig *pTmpConfig = new CServerConfig;
	const string filepath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingleton().LoadFromFile(filepath)) {
		LOG_ERROR("default", "Get TcpserverConfig failed");
		delete pTmpConfig;
		pTmpConfig = NULL;
		exit(0);
	}

	ServerInfo *proxyInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	m_pNetWork->BeginListen(proxyInfo->m_sHost.c_str(),
							proxyInfo->m_iPort,
							&CProxyCtrl::lcb_OnAcceptCns,
							&CProxyCtrl::lcb_OnCnsSomeDataSend,
							&CProxyCtrl::lcb_OnCnsSomeDataRecv,
							&CProxyCtrl::lcb_OnCnsDisconnected,
							&CProxyCtrl::lcb_OnAcceptorTimeOut,
							-1,
							CServerConfig::GetSingletonPtr()->GetTcpKeepAlive());
	return 0;
}

int CProxyCtrl::Run()
{
	LOG_INFO("default", "Libevent run with net module {}",
			 event_base_get_method(reinterpret_cast<const event_base *>(CNetWork::GetSingletonPtr()
				 ->GetEventReactor()->GetEventBase())));
	ServerInfo *proxyInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	LOG_INFO("default", "ProxyServer is going to run at {} : {}", proxyInfo->m_sHost.c_str(), proxyInfo->m_iPort);
	LOG_INFO("default", "ProxyServer startup successes");
	m_pNetWork->DispatchEvents();
	return 0;
}

IBufferEvent *CProxyCtrl::GetConnByKey(int iKey)
{
	auto it = m_mapRegister.find(iKey);
	if (it != m_mapRegister.end()) {
		return m_pNetWork->FindAcceptor(it->second);
	}
	return NULL;
}

int CProxyCtrl::MakeConnKey(const short nType, const short nID)
{
	int iKey = 0;

	iKey = nType;
	iKey = (iKey << 16) | nID;

	return iKey;
}

void CProxyCtrl::lcb_OnAcceptCns(uint32 uId, IBufferEvent *pBufferEvent)
{
	MY_ASSERT(pBufferEvent != NULL && typeid(*pBufferEvent) == typeid(CAcceptor), return);
	CNetWork::GetSingletonPtr()->InsertNewAcceptor(uId, (CAcceptor *) pBufferEvent);
	LOG_DEBUG("default", "New Connector,socket id {}", pBufferEvent->GetSocket().GetSocket());
}

void CProxyCtrl::lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent)
{
	MY_ASSERT(pBufferEvent != NULL, return);
	SOCKET tmpSocket = pBufferEvent->GetSocket().GetSocket();
	auto it = m_mapSocket2Key.find(tmpSocket);
	if (it != m_mapSocket2Key.end()) {
		m_mapRegister.erase(it->second);
		m_mapSocket2Key.erase(tmpSocket);
	}
	LOG_WARN("default", "Connection disconnected,socket id {}", pBufferEvent->GetSocket().GetSocket());
	CNetWork::GetSingletonPtr()->ShutDownAcceptor(tmpSocket);
}

void CProxyCtrl::lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent)
{
	MY_ASSERT(pBufferEvent != NULL, return);
	//消息不完整
	if (!pBufferEvent->IsPackageComplete()) {
		return;
	}
	unsigned short unTmpLen = pBufferEvent->GetRecvPackLen();
	unsigned short unDataLen = unTmpLen - sizeof(unsigned short);
	pBufferEvent->RecvData(m_acRecvBuff + sizeof(unsigned short), unDataLen);
	pBufferEvent->CurrentPackRecved();
	auto it = m_mapSocket2Key.find(pBufferEvent->GetSocket().GetSocket());
	if (it != m_mapSocket2Key.end()) {
		//转发消息填充数据总长度
		*(unsigned short *) m_acRecvBuff = unTmpLen;
		CProxyCtrl::GetSingletonPtr()->TransferOneCode(pBufferEvent, unDataLen);
	}
	else {    //未注册
		CProxyCtrl::GetSingletonPtr()->DealRegisterMes(pBufferEvent, unDataLen);
	}
}

void CProxyCtrl::lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent)
{

}

void CProxyCtrl::lcb_OnAcceptorTimeOut(CAcceptor *pAcceptor)
{
	SOCKET tmpSocket = pAcceptor->GetSocket().GetSocket();
	auto it = m_mapSocket2Key.find(tmpSocket);
	if (it != m_mapSocket2Key.end()) {
		m_mapRegister.erase(it->second);
		m_mapSocket2Key.erase(tmpSocket);
	}
	LOG_INFO("default", "Connection time out,socket id {} close", pAcceptor->GetSocket().GetSocket());
}

int CProxyCtrl::DealRegisterMes(IBufferEvent *pBufferEvent, unsigned short iTmpLen)
{
	MY_ASSERT(pBufferEvent != NULL, return -1);

	CProxyHead stTmpProxyHead;
	int iRet = CServerCommEngine::ConvertStreamToProxy(m_acRecvBuff + sizeof(unsigned short), iTmpLen, &stTmpProxyHead);
	if (iRet < 0) {
		LOG_ERROR("default", "In DealRegisterMes, ConvertStreamToProxy return {}.", iRet);
		return -1;
	}

	SOCKET iSocket = pBufferEvent->GetSocket().GetSocket();
	ServerInfo *proxyInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);
	// 目标服务器是proxy，id不匹配或者不是注册消息，则直接关闭连接
	if ((stTmpProxyHead.dstfe() != FE_PROXYSERVER)
		|| (stTmpProxyHead.dstid() != proxyInfo->m_iServerId)
		|| (stTmpProxyHead.opflag() != enMessageCmd::MESS_REGIST)) {
		LOG_ERROR("default",
				  "Error CProxyHead is invalid, fd = {}, Src(FE = {} : ID = {}), Dst(FE = {} : ID = {}), OpFlag = {}, TimeStamp = %ld.",
				  pBufferEvent->GetSocket().GetSocket(),
				  stTmpProxyHead.srcfe(),
				  stTmpProxyHead.srcid(),
				  stTmpProxyHead.srcfe(),
				  stTmpProxyHead.dstid(),
				  stTmpProxyHead.opflag(),
				  stTmpProxyHead.timestamp());
		CloseConnection(iSocket);
		return -1;
	}
	LOG_DEBUG("default", "---- Recv Msg ----");
	LOG_DEBUG("default", "[{}]", stTmpProxyHead.ShortDebugString().c_str());
	// 检查该链接是否已占用，通过类型和ID判断
	int iKey = MakeConnKey(stTmpProxyHead.srcfe(), stTmpProxyHead.srcid());
	auto it = m_mapRegister.find(iKey);
	if (it != m_mapRegister.end()) {
		// 该连接已经存在
		LOG_ERROR("default", "conn(fe={} : id={} : key={}) exist, can't regist again.",
				  stTmpProxyHead.srcfe(), stTmpProxyHead.srcid());
		CloseConnection(iSocket);
		return -1;
	}
	//注册成功
	m_mapRegister.insert(std::make_pair(iKey, iSocket));
	m_mapSocket2Key.insert(std::make_pair(iSocket, iKey));
	return 0;
}

/**
  *函数名          : TransferOneCode
  *功能描述        : 转发一个数据包
**/
int CProxyCtrl::TransferOneCode(IBufferEvent *pBufferEvent, unsigned short nCodeLength)
{
	CProxyMessage stTmpMessage;

	int iTempRet = 0;
	if (nCodeLength <= 0) {
		LOG_INFO("default", "In TransferOneCode, invalid input params.");
		return -1;
	}

	unsigned short unOffset = 0;
	int iRet = CServerCommEngine::ConvertStreamToMsg(m_acRecvBuff + sizeof(unsigned short), nCodeLength, &stTmpMessage);
	if (iRet < 0) {
		LOG_ERROR("default", "In TransferOneCode, ConvertStreamToCSHead return {}.", iRet);
		return -1;
	}

	CProxyHead stTmpHead = stTmpMessage.msghead();
	LOG_INFO("default", "TransMsg({}).", nCodeLength);

#ifdef _DEBUG_
	LOG_INFO("default", "---- Recv Msg ----");
	LOG_INFO("default", "[{}]", stTmpHead.ShortDebugString().c_str());
#endif

	LOG_INFO("default", "Transfer code begin, from(FE = {} : ID = {}) to(FE = {} : ID = {}), timestamp = {}",
			 stTmpHead.srcfe(), stTmpHead.srcid(),
			 stTmpHead.dstfe(), stTmpHead.dstid(), stTmpHead.timestamp());

	// 处理直接发送到 proxy 的消息
	if (stTmpHead.dstfe() == FE_PROXYSERVER) {
		switch (stTmpHead.opflag()) {
		case enMessageCmd::MESS_KEEPALIVE: {
			CProxyMessage stRetMessage;
			CProxyHead *stRetHead = stRetMessage.mutable_msghead();
			ServerInfo *serverInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);

			pbmsg_setproxy(stRetHead,
						   FE_PROXYSERVER,
						   serverInfo->m_iServerId,
						   stTmpHead.srcfe(),
						   stTmpHead.srcid(),
						   GetMSTime(),
						   enMessageCmd::MESS_KEEPALIVE);

			// keepalive的包长度一般都很短
			char message_buffer[1024];
			unsigned short tTotalLen = sizeof(message_buffer);

			int iRet = CServerCommEngine::ConvertMsgToStream(&stRetMessage, message_buffer, tTotalLen);
			if (iRet != 0) {
				LOG_ERROR("default", "CDBCtrsl::SendkeepAliveToProxy ConvertMsgToStream failed, iRet = {}.", iRet);
				return 0;
			}
			else {
				int iKey = MakeConnKey(stRetHead->dstfe(), stRetHead->dstid());
				int iRet = SendOneCodeTo(tTotalLen, (BYTE *) message_buffer, iKey, true);
				if (iRet != 0) {
					LOG_INFO("default", "send keepalive to (FE = {} : ID = {}), SendOneCodeTo failed, iRet = {}.",
							 stRetHead->dstfe(), stRetHead->dstid(), iRet);
				}
				else {
					LOG_INFO("default",
							 "send keepalive to (FE = {} : ID = {}) succeed.",
							 stRetHead->dstfe(),
							 stRetHead->dstid());
				}
			}

			break;
		}
		default: {
			LOG_INFO("default",
					 "unknown command id {}, from(FE = {} : ID = {}) to(FE = {} : ID = {}), timestamp = {].",
					 stTmpHead.opflag(),
					 stTmpHead.srcfe(),
					 stTmpHead.srcid(),
					 stTmpHead.dstfe(),
					 stTmpHead.dstid(),
					 stTmpHead.timestamp());
			break;
		}
		}

		return 0;
	}

	int iKey = MakeConnKey(stTmpHead.dstfe(), stTmpHead.dstid());
	iTempRet = SendOneCodeTo(nCodeLength, (BYTE *) m_acRecvBuff, iKey, true);
	m_stStatLog.iSndCnt++;
	m_stStatLog.iSndSize += nCodeLength;
	if (iTempRet) {
		LOG_INFO("default", "transfer one code from (FE = {} : ID = {}) to (FE = {} : ID = {}) failed of {}.",
				 stTmpHead.srcfe(), stTmpHead.srcid(), stTmpHead.dstfe(), stTmpHead.dstid(), iTempRet);
		m_stStatLog.iSndCnt--;
		m_stStatLog.iSndSize -= nCodeLength;
	}

	LOG_INFO("default", "Transfer code ended.");

	return 0;
}

int CProxyCtrl::SendOneCodeTo(short nCodeLength, BYTE *pbyCode, int iKey, bool bKeepalive/* = false*/)
{
	IBufferEvent *pWriteConn = NULL;
	int iTempRet = 0;

	if (nCodeLength <= 0 || !pbyCode) {
		LOG_ERROR("default", "While send one code to (key={}), null code.", iKey);
		return -1;
	}

	pWriteConn = GetConnByKey(iKey);
	if (!pWriteConn) {
		LOG_ERROR("default", "While send one code to (key={}), invalid key.", iKey);
		return -1;
	}

	iTempRet = pWriteConn->Send(pbyCode, nCodeLength);
	if (iTempRet == 0 && bKeepalive) {
		CAcceptor *tmpAcceptor = (CAcceptor *) pWriteConn;
		// 设置keepalive时间
		tmpAcceptor->SetLastKeepAlive(GetMSTime());
	}
	return iTempRet;
}

void CProxyCtrl::CloseConnection(int socket)
{
	auto it = m_mapSocket2Key.find(socket);
	if (it != m_mapSocket2Key.end()) {
		m_mapRegister.erase(it->second);
		m_mapSocket2Key.erase(it);
	}
	m_pNetWork->ShutDownAcceptor(socket);
}
