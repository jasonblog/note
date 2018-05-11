//
// Created by DGuco on 17-7-13.
//

#include <dbmessage.pb.h>
#include <client_comm_engine.h>
#include <my_assert.h>
#include "../inc/dbctrl.h"
#include "config.h"
#include "connector.h"
#include "server_comm_engine.h"

template<> CDBCtrl *CSingleton<CDBCtrl>::spSingleton = NULL;

int CDBCtrl::m_iProxyId = 0;

char CDBCtrl::m_acRecvBuff[MAX_PACKAGE_LEN] = {0};

CDBCtrl::CDBCtrl()
{
	m_iRunFlag = 0;
	m_tLastSendKeepAlive = 0;
	m_tLastRecvKeepAlive = 0;
	m_pNetWork = new CNetWork();
}

CDBCtrl::~CDBCtrl()
{
}

void CDBCtrl::SetRunFlag(int iFlag)
{
	m_iRunFlag = iFlag;
	LOG_INFO("default", "Set RunFunc Flag {}, All Flag Is {}", iFlag, m_iRunFlag);
}

void CDBCtrl::ClearRunFlag(int iFlag)
{
	m_iRunFlag = 0;
	LOG_INFO("default", "Clear RunFunc Flag {}", iFlag);
}

bool CDBCtrl::IsRunFlagSet(int iFlag)
{
	return iFlag == m_iRunFlag;
}

int CDBCtrl::SendMessageTo(CProxyMessage *pMsg)
{
	if (pMsg == NULL) {
		LOG_ERROR("default", "in CDBHandle::SendMessageTo, pmsg is null");
		return -1;
	}

	CProxyHead *stProxyHead = pMsg->mutable_msghead();
	BYTE abyCodeBuf[MAX_PACKAGE_LEN] = {0};
	unsigned short nCodeLength = sizeof(abyCodeBuf);
	ServerInfo *dbInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_DBSERVER);
	ServerInfo *proxyInfo = CServerConfig::GetSingletonPtr()->GetServerInfo(enServerType::FE_PROXYSERVER);

	pbmsg_setproxy(stProxyHead,
				   enServerType::FE_PROXYSERVER,
				   proxyInfo->m_iServerId,
				   enServerType::FE_DBSERVER,
				   dbInfo->m_iServerId,
				   GetMSTime(),
				   enMessageCmd::MESS_REGIST);

	int iRet = CServerCommEngine::ConvertMsgToStream(pMsg, abyCodeBuf, nCodeLength);
	if (iRet != 0) {
		LOG_ERROR("default", "CDBCtrl::RegisterToProxyServer ConvertMsgToStream failed, iRet = {}.", iRet);
		return 0;
	}

	//int nSendReturn = m_pProxySvrdConns[ m_current_proxy_index ].GetSocket()->SendOneCode( nCodeLength, abyCodeBuf );
	IBufferEvent *tmpConnector = m_pNetWork->FindConnector(CDBCtrl::m_iProxyId);
	if (tmpConnector == NULL) {
		LOG_ERROR("default", "Connection to proxyserver has gone");
		return -1;
	}
	int nSendReturn = tmpConnector->Send(abyCodeBuf, nCodeLength);
	if (nSendReturn < 0) {
		LOG_ERROR("default", "Send Code(len:{}) To Proxy faild(error={})", nCodeLength, nSendReturn);
		return -1;
	}

	Message *pUnknownMessagePara = (Message *) pMsg->msgpara();
	MY_ASSERT(pUnknownMessagePara != NULL, return 0);
	const ::google::protobuf::Descriptor *pDescriptor = pUnknownMessagePara->GetDescriptor();
	LOG_DEBUG("default", "SendMessageTo: MsgName[{}] ProxyHead[{}] MsgHead[{}] MsgPara[{}]",
			  pDescriptor->name().c_str(), stProxyHead->ShortDebugString().c_str(),
			  pMsg->ShortDebugString().c_str(), ((Message *) pMsg->msgpara())->ShortDebugString().c_str());

	return 0;
}

// 执行相应的指令
int CDBCtrl::Event(CProxyMessage *pMsg)
{
	if (pMsg == NULL) {
		LOG_ERROR("default", "In CDBHandle::Event, input msg null");
		return -1;
	}

	Message *pUnknownMessagePara = (Message *) pMsg->msgpara();
	MY_ASSERT(pUnknownMessagePara != NULL, return 0);
	const ::google::protobuf::Descriptor *pDescriptor = pUnknownMessagePara->GetDescriptor();
	LOG_DEBUG("default", "ReceveMsg: MsgName[{}] MsgHead[{}] MsgPara[{}]",
			  pDescriptor->name().c_str(),
			  pMsg->ShortDebugString().c_str(),
			  ((Message *) pMsg->msgpara())->ShortDebugString().c_str());

	switch (pMsg->msghead().messageid()) {
	case CMsgExecuteSqlRequest::MsgID:  // 服务器执行SQL请求
	{
		ProcessExecuteSqlRequest(pMsg);
		break;
	}
	default: {
		break;
	}
	}

	return 0;
}

// 执行相应的指令
int CDBCtrl::ProcessExecuteSqlRequest(CProxyMessage *pMsg)
{
	if (pMsg == NULL) {
		LOG_ERROR("default", "Error: [{}][{}][{}], invalid input.\n", __MY_FILE__, __LINE__, __FUNCTION__);
		return -1;
	}

	CMsgExecuteSqlRequest *pReqMsg = (CMsgExecuteSqlRequest * )(pMsg->msgpara());

	if (pReqMsg == NULL) {
		LOG_ERROR("default", "Error: [{}][{}][{}], msgpara null.\n", __MY_FILE__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (pReqMsg->sql().length() <= 0) {
		LOG_ERROR("default", "Error: [{}][{}][{}], sql len({}) invalid.\n",
				  __MY_FILE__,
				  __LINE__,
				  __FUNCTION__,
				  pReqMsg->sql().length());
		return -1;
	}

	LOG_ERROR("default", "{} \n", pReqMsg->ShortDebugString().c_str());

	string sqlStr = "";
	if (pReqMsg->hasblob() == HASBLOB) {
		// 有blob字段的处理
		sqlStr += pReqMsg->sql();
		if (pReqMsg->bufsize() > 0) {
			if (pReqMsg->bufsize() >= MAX_PACKAGE_LEN) {
				// blob字段超出长度
				LOG_ERROR("default", "Error: [{}][{}][{}], exec sql {} faild. sql_len:{} > MAX_PACKAGE_LEN\n",
						  __MY_FILE__,
						  __LINE__,
						  __FUNCTION__,
						  pReqMsg->sql().c_str(),
						  pReqMsg->bufsize());
				return -1;
			}

			char sqlBuff[MAX_PACKAGE_LEN * 2 + 1] = {0};
			int len = m_pDatabase->escape_string(sqlBuff, pReqMsg->buffer().c_str(), pReqMsg->bufsize());
			if (len <= 0) {
				LOG_ERROR("default",
						  "Error: [{}][{}][{}], escape_string error!!!!\n",
						  __MY_FILE__,
						  __LINE__,
						  __FUNCTION__);
				return -1;
			}
			sqlStr += sqlBuff;
		}
		sqlStr += pReqMsg->sqlwhere();
	}
	else {
		// 没有blob字段的处理
		sqlStr += pReqMsg->sql();
	}

	// 需要回执
	if (MUSTCALLBACK == pReqMsg->callback()) {
		CProxyMessage tMsg;  // 该消息为回执
		CMsgExecuteSqlResponse tSqlResMsg;  // tSqlResMsg 为消息体
		tMsg.Clear();
		tSqlResMsg.Clear();

		tSqlResMsg.set_logictype(pReqMsg->logictype());
		tSqlResMsg.set_sessionid(pReqMsg->sessionid());
		tSqlResMsg.set_timestamp(pReqMsg->timestamp());
		tSqlResMsg.set_teamid(pReqMsg->teamid());

		LOG_ERROR("default", "Execute SQL: {}", sqlStr.c_str());    // 用于在日志中查看SQL查询语句

		CProxyHead *pTmpHead = tMsg.mutable_msghead();
		pTmpHead->set_messageid(CMsgExecuteSqlResponse::MsgID);
		if (pMsg->has_msghead()) {
			if (pMsg->mutable_msghead()->has_msghead()) {
				CClientCommEngine::CopyMesHead(pMsg->mutable_msghead()->mutable_msghead(),
											   pTmpHead->mutable_msghead());
			}
		}

		tMsg.set_msgpara((uint64) &tSqlResMsg);

		if (pReqMsg->sqltype() == SELECT || pReqMsg->sqltype() == CALL) {
			QueryResult *res = NULL;
			if (pReqMsg->sqltype() == SELECT) {
				res = m_pDatabase->Query(sqlStr.c_str(), sqlStr.length());
			}
			else {
				res = m_pDatabase->QueryForprocedure(sqlStr.c_str(), sqlStr.length(), pReqMsg->outnumber());
			}
			if (res == NULL)  // 无返回结果
			{
				LOG_ERROR("default", "sql {} exec, but no resultset returned", sqlStr.c_str());
				tSqlResMsg.set_resultcode(0);
			}
			else  // 有返回结果
			{
				tSqlResMsg.set_resultcode(1);
				tSqlResMsg.set_rowcount(res->GetRowCount());  // 行
				tSqlResMsg.set_colcount(res->GetFieldCount());  // 列

				//TODO: 列名 + 列类型 暂时不赋值
				if (res->GetRowCount() > 0) {
					// 列值 和 值长度
					do {
						Field *pField = res->Fetch();
						if (pField == NULL) {
							LOG_ERROR("default", "ERROR: do sql {} success, row[{}], col[{}], but some row is null\n",
									  sqlStr.c_str(),
									  res->GetRowCount(),
									  res->GetFieldCount());
							//TODO: 出错设为0
							tSqlResMsg.set_rowcount(0);
							break;
						}

						for (int j = 0; j < (int) res->GetFieldCount(); j++) {
							tSqlResMsg.add_fieldvalue(pField[j].GetString(), pField[j].GetValueLen());
							tSqlResMsg.add_fieldvaluelen(pField[j].GetValueLen());
						}

					}
					while (res->NextRow());
				}
			}

			// 安全释放结果集
			ReleaseResult(res);
		}
		else  // 非select操作
		{
			bool bExecResult = m_pDatabase->RealDirectExecute(sqlStr.c_str(), sqlStr.length());  // 执行操作
			tSqlResMsg.set_resultcode(bExecResult ? 1 : 0);
		}

		// 回复客户端
		SendMessageTo(&tMsg);
	}
	else {
		// 同步执行
		if (m_pDatabase->RealDirectExecute(sqlStr.c_str(), sqlStr.length()) != true) {
			LOG_ERROR("default", "Error: [{}][{}][{}], direct exec sql {} faild.\n",
					  __MY_FILE__,
					  __LINE__,
					  __FUNCTION__,
					  sqlStr.c_str());
		}
	}
	return 0;
}

int CDBCtrl::ConnectToProxyServer()
{
	int i = 0;

	//如果为null 让程序崩溃
	ServerInfo *proxyInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType::FE_PROXYSERVER);
	if (!m_pNetWork->Connect(proxyInfo->m_sHost.c_str(),
							 proxyInfo->m_iPort,
							 proxyInfo->m_iServerId,
							 &CDBCtrl::lcb_OnCnsSomeDataSend,
							 &CDBCtrl::lcb_OnCnsSomeDataRecv,
							 &CDBCtrl::lcb_OnCnsDisconnected,
							 &CDBCtrl::lcb_OnConnectFailed,
							 &CDBCtrl::lcb_OnConnected,
							 &CDBCtrl::lcb_OnPingServer,
							 CServerConfig::GetSingletonPtr()->GetTcpKeepAlive() / 1000)
		) {
		LOG_ERROR("default", "[{} : {} : {}] Connect to Proxy({}:{})(id={}) failed.",
				  __MY_FILE__, __LINE__, __FUNCTION__,
				  proxyInfo->m_sHost.c_str(), proxyInfo->m_iPort, proxyInfo->m_iServerId);
		return false;
	}

	time_t tmpNow = GetMSTime();
	SetLastRecvKeepAlive(tmpNow);
	SetLastSendKeepAlive(tmpNow);
	LOG_INFO("default", "Connect to Proxy server {}:{} Succeed.\n", proxyInfo->m_sHost.c_str(), proxyInfo->m_iPort);
	return i;
}

/********************************************************
Function:     RegisterToProxyServer
Description:  向proxy发送注册消息
Input:        nIndex:  连接下标
Output:      
Return:       0 :   成功 ，其他失败
Others:		
 ********************************************************/
int CDBCtrl::RegisterToProxyServer(CConnector *pConnector)
{
	CProxyMessage message;
	char message_buffer[1024] = {0};
	unsigned short tTotalLen = sizeof(message_buffer);

	//如果为null 让程序崩溃
	ServerInfo *dbInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType::FE_DBSERVER);
	ServerInfo *proxyInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType::FE_PROXYSERVER);
	pbmsg_setproxy(message.mutable_msghead(),
				   enServerType::FE_DBSERVER,
				   dbInfo->m_iServerId,
				   enServerType::FE_PROXYSERVER,
				   proxyInfo->m_iServerId,
				   GetMSTime(),
				   enMessageCmd::MESS_REGIST);

	int iRet = CServerCommEngine::ConvertMsgToStream(&message, message_buffer, tTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "CDBCtrl::RegisterToProxyServer ConvertMsgToStream failed, iRet = {}.", iRet);
		return 0;
	}

	iRet = pConnector->Send((BYTE *) message_buffer, tTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "CDBCtrl::RegisterToProxyServer SendOneCode failed, iRet = {}.", iRet);
		return -1;
	}

	LOG_INFO("default", "Regist to Proxy now.");
	m_tLastSendKeepAlive = GetMSTime();    // 记录这一次的注册的时间
	return 0;
}

/********************************************************
Function:     SendkeepAliveToProxy
Description:  向proxy发送心跳消息
Input:        nIndex    连接指针
Output:      
Return:       0 :   成功 ，其他失败
Others:		
 ********************************************************/
int CDBCtrl::SendkeepAliveToProxy(CConnector *pConnector)
{
	CProxyMessage message;
	char message_buffer[1024] = {0};
	unsigned short tTotalLen = sizeof(message_buffer);

	ServerInfo *dbInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType::FE_DBSERVER);
	ServerInfo *proxyInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType::FE_DBSERVER);
	pbmsg_setproxy(message.mutable_msghead(),
				   enServerType::FE_DBSERVER,
				   dbInfo->m_iServerId,
				   enServerType::FE_PROXYSERVER,
				   proxyInfo->m_iServerId,
				   GetMSTime(),
				   enMessageCmd::MESS_KEEPALIVE);

	int iRet = CServerCommEngine::ConvertMsgToStream(&message, message_buffer, tTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "CDBCtrsl::SendkeepAliveToProxy ConvertMsgToStream failed, iRet = {}.", iRet);
		return 0;
	}

	iRet = pConnector->Send((BYTE *) message_buffer, tTotalLen);
	if (iRet != 0) {
		LOG_ERROR("default", "CDBCtrl::SendkeepAliveToProxy  proxy SendOneCode failed, iRet = {}.", iRet);
		return -1;
	}

	m_tLastSendKeepAlive = GetMSTime(); // 保存这一次的发送的时间
	LOG_INFO("default", "SendkeepAlive to Proxy now.");
	return 0;
}

// ***************************************************************
//  Function: 	DisPatchOneCode   
//  Description:分派一个消息
//
// ***************************************************************
int CDBCtrl::DispatchOneCode(int nCodeLength, BYTE *pbyCode)
{
	int iTempRet = 0;

	CProxyMessage stTempMsg;
	// 将解析出的消息头和消息体分别存放在 m_stCurrentProxyHead stTempMsg
	int tRet = CServerCommEngine::ConvertStreamToMsg((const void *) (pbyCode),
													 nCodeLength,
													 &stTempMsg,
													 mMsgFactory);
	if (tRet != 0) {
		LOG_ERROR("default", "In CDBCtrl::DispatchOneCodecode, ConvertStreamToMsg failed. tRet = {}", tRet);
		return -1;
	}

	CProxyHead tmpProxyHead = stTempMsg.msghead();

	// 加入 proxy 命令处理处理心跳消息
	if (enServerType::FE_PROXYSERVER == tmpProxyHead.srcfe()
		&& enMessageCmd::MESS_KEEPALIVE == tmpProxyHead.opflag()) {
		m_tLastRecvKeepAlive = GetMSTime(); // 保存这一次的注册的时间
		LOG_DEBUG("default", "Recv proxyServer keepalive");
		return 0;
	}

	iTempRet = Event(&stTempMsg);  // 服务器执行相应的 Msg ，其实就是执行 SQL

	if (iTempRet) {
		LOG_ERROR("default", "Handle event returns {}.\n", iTempRet);
	}
	// 消息回收
	Message *pMessagePara = (Message *) (stTempMsg.msgpara());
	if (pMessagePara) {
		pMessagePara->~Message();
		stTempMsg.set_msgpara((unsigned long) NULL);
	}

	return iTempRet;
}

void CDBCtrl::SetLastSendKeepAlive(time_t tLastSendKeepAlive)
{
	CDBCtrl::m_tLastSendKeepAlive = tLastSendKeepAlive;
}

void CDBCtrl::SetLastRecvKeepAlive(time_t tLastRecvKeepAlive)
{
	CDBCtrl::m_tLastRecvKeepAlive = tLastRecvKeepAlive;
}

time_t CDBCtrl::GetLastSendKeepAlive() const
{
	return m_tLastSendKeepAlive;
}

time_t CDBCtrl::GetLastRecvKeepAlive() const
{
	return m_tLastRecvKeepAlive;
}

int CDBCtrl::PrepareToRun()
{
#ifdef _DEBUG_
	// 初始化日志
	INIT_ROATING_LOG("default", "../log/dbserver.log", level_enum::trace);
#else
	// 初始化日志
	INIT_ROATING_LOG("default", "../log/dbserver.log", level_enum::info);
#endif

	CServerConfig *pTmpConfig = new CServerConfig;
	const string filepath = "../config/serverinfo.json";
	if (-1 == pTmpConfig->LoadFromFile(filepath)) {
		LOG_ERROR("default", "Get TcpserverConfig failed");
		return -1;
	}

	if (ConnectToProxyServer() < 0)  // 连接到proxy服务器
	{
		LOG_ERROR("default", "Error: in CDBCtrl::PrepareToRun connect proxy  server  failed!\n");
		return -1;
	}
	return 0;
}

int CDBCtrl::Run()
{
	m_pNetWork->DispatchEvents();
}

void CDBCtrl::lcb_OnConnected(IBufferEvent *pBufferEvent)
{
	MY_ASSERT(pBufferEvent != NULL && typeid(*pBufferEvent) == typeid(CConnector), return);
	CConnector *pConnector = (CConnector *) pBufferEvent;
	ServerInfo *proxyInfo = CServerConfig::GetSingleton().GetServerInfo(enServerType::FE_PROXYSERVER);
	if (CDBCtrl::GetSingletonPtr()->RegisterToProxyServer(pConnector)) {
		LOG_ERROR("default", "Error: Register to Proxy Server {} failed.\n", proxyInfo->m_iServerId);
		return;
	}

	m_iProxyId = pBufferEvent->GetSocket().GetSocket();
}

void CDBCtrl::lcb_OnCnsDisconnected(IBufferEvent *pBufferEvent)
{
	MY_ASSERT(pBufferEvent != NULL && typeid(*pBufferEvent) == typeid(CConnector), return);
	// 断开连接重新连接到proxy服务器
	if (((CConnector *) pBufferEvent)->ReConnect() < 0) {
		LOG_ERROR("default", "Reconnect to proxyServer failed!");
		return;
	}
	return;
}

void CDBCtrl::lcb_OnCnsSomeDataRecv(IBufferEvent *pBufferEvent)
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
	CDBCtrl::GetSingletonPtr()->DispatchOneCode(iTmpLen, (BYTE *) (m_acRecvBuff));
}

void CDBCtrl::lcb_OnCnsSomeDataSend(IBufferEvent *pBufferEvent)
{
	MY_ASSERT(pBufferEvent != NULL, return);
}

void CDBCtrl::lcb_OnConnectFailed(IBufferEvent *pBufferEvent)
{
	MY_ASSERT(pBufferEvent != NULL, return);
}

void CDBCtrl::lcb_OnPingServer(int fd, short event, CConnector *pConnector)
{
	MY_ASSERT(pConnector != NULL, return);
	time_t tNow = GetMSTime();
	CServerConfig *tmpConfig = CServerConfig::GetSingletonPtr();
	CDBCtrl *tmpDBCtrl = CDBCtrl::GetSingletonPtr();
	CDBCtrl::GetSingletonPtr()->SendkeepAliveToProxy(pConnector);
	if (pConnector->GetState() == CConnector::eCS_Connected &&
		pConnector->GetSocket().GetSocket() > 0 &&
		tNow - tmpDBCtrl->GetLastRecvKeepAlive() < tmpConfig->GetTcpKeepAlive() * 3) {
		if (tNow - tmpDBCtrl->GetLastSendKeepAlive() >= tmpConfig->GetTcpKeepAlive()) {
			tmpDBCtrl->SendkeepAliveToProxy(pConnector);
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
}