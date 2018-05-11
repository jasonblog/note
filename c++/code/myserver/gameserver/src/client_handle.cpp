//
// Created by DGuco on 17-6-21.
//

#include <common_def.h>
#include "shm.h"
#include "code_queue.h"
#include "share_mem.h"
#include "client_comm_engine.h"
#include "my_assert.h"
#include "../datamodule/inc/sceneobjmanager.h"
#include "../inc/message_dispatcher.h"
#include "../inc/message_factory.h"
#include "../inc/client_handle.h"
#include "../inc/game_server.h"

CClientHandle::CClientHandle()
	: CMyThread("CClientHandle", 1000),//阻塞超时1ms
	  m_pRecvBuff(new CByteBuff)
{
}

CClientHandle::~CClientHandle()
{
	SAFE_DELETE(mC2SPipe);
	SAFE_DELETE(mS2CPipe);
}

int CClientHandle::DealClientMessage(std::shared_ptr<CMessage> pMsg)
{
	MesHead *pCSHead = pMsg->mutable_msghead();
	if (pCSHead == NULL) {
		return CLIENTHANDLE_MSGINVALID;
	}
	//gate上行数据必须带client的socket信息
	if (pCSHead->socketinfos().size() != 1) {
		return ClienthandleErrCode::CLIENTHANDLE_MSGINVALID;
	}
	CSocketInfo tmpSocketInfo = pCSHead->socketinfos(0);
	int iTmpSocket = tmpSocketInfo.socketid();
	int tTmpCreateTime = tmpSocketInfo.createtime();
	if (tmpSocketInfo.state() < 0) {
		// 客户端主动关闭连接，也有可能是连接错误被关闭
		LOG_INFO("default",
				 "client({} : {}) commhandle closed by err = {}. ",
				 iTmpSocket,
				 tTmpCreateTime,
				 tmpSocketInfo.state());
		// 从连接容器中取出玩家实体
		CPlayer *pTmpTeam = CSceneObjManager::GetSingletonPtr()->GetPlayerBySocket(iTmpSocket);
		if (NULL == pTmpTeam) {
			// 找不到玩家，连接已经关闭了
			LOG_ERROR("default", "[{} : {} : {}] socket({} : {}) EntityID = {} has closed.",
					  __MY_FILE__, __LINE__, __FUNCTION__, iTmpSocket, tTmpCreateTime);
			return CLIENTHANDLE_HASCLOSED;
		}
		else {
			STConnectInfo *connectInfo = pTmpTeam->GetPlayerBase()->GetSocketInfoPtr();
			if (!connectInfo
				&& (connectInfo->m_iSocket != iTmpSocket || connectInfo->m_tCreateTime != tTmpCreateTime)) {
				// 当前玩家与该连接信息不匹配,说明该玩家的连接已经失效
				LOG_WARN("default",
						 "[{} : {} : {}] socket({} : {}) not match, now({} : {}).",
						 __MY_FILE__,
						 __LINE__,
						 __FUNCTION__,
						 iTmpSocket,
						 tTmpCreateTime,
						 connectInfo->m_iSocket,
						 connectInfo->m_tCreateTime);
				// 重置玩家连接信息
//                CCoreModule::GetSingletonPtr()->LeaveGame(pTmpTeam, false);
//				CCoreModule::GetSingletonPtr()->EraseSockInfoList(iTmpSocket);
//				pTmpTeam->GetSocketInfoPtr()->Clear();
//				pTmpTeam->SetTeamState(CTeam::ETS_INGAMEDISCONNECT);

				return CLIENTHANDLE_NOTSAMETEAM;
			}
			else {
//                // 模拟玩家下线
//                pMsg->mutable_msghead()->set_cmd(CMsgLeaveGameRequest::MsgID);
//
//                CMsgLeaveGameRequest* pTmpMsgPara = new (macMessageBuff) CMsgLeaveGameRequest;
//
//                pTmpMsgPara->set_leavetype(mNetHead.m_cState);
//                pMsg->set_msgpara((unsigned long) pTmpMsgPara);
//
//                pCSHead->set_teamid(pTmpTeam->GetTeamID());
//                pCSHead->set_entityid(pTmpTeam->GetEntityID());
//                pCSHead->set_timestamp(mNetHead.m_tStamp);
				return CLIENTHANDLE_SUCCESS;
			}
		}
	}

	//服务器数据拉完了才能让玩家正常游戏
//	if (CGameServer::GetSingletonPtr()->CanProcessingClientMsg() == false) {
//		long lTmpMsgGuid = tmpSocketInfo.createtime();
//		// 通知客户端服务器未开启并断开连接
//		CGameServer::GetSingletonPtr()
//			->SendMsgSystemErrorResponse(emSystem_noservice, lTmpMsgGuid, iTmpSocket, tTmpCreateTime, true);
//		return CLIENTHANDLE_ISNOTNORMAL;
//	}

//	CPlayer *pTmpTeam = NULL;
//	// 如果是登陆消息
//	if (pMsg && pCSHead->cmd() == PlayerCommandId::USER_ACCOUNT_LOGIN) {
//		// 5500踢掉断线玩家 检测是否需要踢掉断连玩家
//		if (CCoreModule::GetSingletonPtr()->CheckOnlineIsFull() < 0) {
//			LOG_INFO("default",
//					 "[{} : {} : {}]  login failed, team list is full.",
//					 __MY_FILE__,
//					 __LINE__,
//					 __FUNCTION__);
//
//			// 通知客户端服务器已满并断开连接
//			CGameServer::GetSingletonPtr()
//				->SendMsgSystemErrorResponse(emSystem_isfull, lTmpMsgGuid, iTmpSocket, tTmpCreateTime,
//											 mNetHead.m_iSrcIP, mNetHead.m_nSrcPort, true);
//			return CLIENTHANDLE_ONLINEFULL;
//		}
//		pTmpTeam = CCoreModule::GetSingletonPtr()->GetTeamBySocket(iTmpSocket);
//		if (pTmpTeam) {
//			// 同一个fd
//			if (tTmpCreateTime == pTmpTeam->GetSocketInfoPtr()->tCreateTime) {
//				// 连续多次发TryLogin消息则直接把消息抛掉
//				LOG_DEBUG("default", "[{} : {} : {}]  Socket({}) is useed, don't TryLogin again.",
//						  __MY_FILE__, __LINE__, __FUNCTION__, iTmpSocket);
//				// 通知客户端解析消息失败并断开连接
//				CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_msgerr,
//																		   lTmpMsgGuid,
//																		   iTmpSocket,
//																		   tTmpCreateTime,
//																		   mNetHead.m_iSrcIP,
//																		   mNetHead.m_nSrcPort,
//																		   true);
//				return CLIENTHANDLE_MSGINVALID;
//			}
//			else {
//				// 假如是同一个fd,时间戳不一样, 可能socket断掉未检测到
//				LOG_INFO("default",
//						 "[{} : {} : {}]  Socket({}) is used, old team(TeamID=%lu, EntityID={}), reset.",
//						 __MY_FILE__,
//						 __LINE__,
//						 __FUNCTION__,
//						 iTmpSocket,
//						 pTmpTeam->GetTeamID(),
//						 pTmpTeam->GetEntityID());
//
//				// 先把老玩家断连
//				CCoreModule::GetSingletonPtr()->LeaveGame(pTmpTeam, false);
//			}
//		}
//
//		// 解析登陆消息
//		CMsgLoginGameRequest *pTmpMsg = (CMsgLoginGameRequest *) pMsg->msgpara();
//		if (pTmpMsg == NULL) {
//			LOG_ERROR("default", "[{} : {} : {}]  team(Account={}, ServerID={}) login failed, msg is NULL.",
//					  __MY_FILE__, __LINE__, __FUNCTION__);
//
//			// 通知客户端解析消息失败并断开连接
//			CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_msginvalid,
//																	   lTmpMsgGuid,
//																	   iTmpSocket,
//																	   tTmpCreateTime,
//																	   mNetHead.m_iSrcIP,
//																	   mNetHead.m_nSrcPort,
//																	   true);
//			return CLIENTHANDLE_MSGINVALID;
//		}
//		if (strcmp(pTmpMsg->account().c_str(), "") == 0 || strcmp(pTmpMsg->password().c_str(), "") == 0) {
//			// 通知客户端解析消息失败并断开连接
//			CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_msginvalid,
//																	   lTmpMsgGuid,
//																	   iTmpSocket,
//																	   tTmpCreateTime,
//																	   mNetHead.m_iSrcIP,
//																	   mNetHead.m_nSrcPort,
//																	   true);
//			return CLIENTHANDLE_MSGINVALID;
//		}
//
//		// 登陆时候判断session是否已验证
//		string sSession = CCoreModule::GetSingletonPtr()
//			->GetTeamIDBySession(pTmpMsg->account().c_str(), pTmpMsg->serverid(), pTmpMsg->pfrom());
//		LOG_INFO("default",
//				 "[{} : {} : {}]  uid: {}, new session: {}, old session: {}",
//				 __YQ_FILE__,
//				 __LINE__,
//				 __FUNCTION__,
//				 pTmpMsg->account().c_str(),
//				 pTmpMsg->password().c_str(),
//				 sSession.c_str());
//		// 是否验证sdk登陆
//		bool bSdkCheck = CConfigMgr::GetSingletonPtr()->GetConfig()->gameconfig().sdkcheck();
//		if (bSdkCheck && strcmp(pTmpMsg->password().c_str(), sSession.c_str()) != 0) {
//			CWebModule::GetSingletonPtr()->XiaoMiLoginRequest(pMsg,
//															  iTmpSocket,
//															  tTmpCreateTime,
//															  lTmpMsgGuid,
//															  mNetHead.m_iSrcIP,
//															  mNetHead.m_nSrcPort,
//															  mNetHead.m_tStamp);
//			return CLIENTHANDLE_LOGINCHECK;
//		}
//		else {
//			CCoreModule::GetSingletonPtr()->SDKLoginSucces(pTmpMsg->account().c_str(),
//														   pTmpMsg->password().c_str(),
//														   pTmpMsg->serverid(),
//														   pTmpMsg->pfrom(),
//														   iTmpSocket,
//														   tTmpCreateTime,
//														   lTmpMsgGuid,
//														   mNetHead.m_iSrcIP,
//														   mNetHead.m_nSrcPort,
//														   mNetHead.m_tStamp);
//			return CLIENTHANDLE_LOGINCHECK;
//		}
//	}
//	else {
//		// 消息链接玩家
//		pTmpTeam = CCoreModule::GetSingletonPtr()->GetTeamBySocket(iTmpSocket);
//		// 判断 玩家跟消息合法性
//		if (pTmpTeam == NULL || pMsg == NULL || tTmpCreateTime != pTmpTeam->GetSocketInfoPtr()->tCreateTime
//			|| pCSHead->teamid() != pTmpTeam->GetTeamID()
//			|| pTmpTeam->GetTeamDebug() != 0) {
//			LOG_DEBUG("default",
//					  "teamid[%lu]msgid[%u] return -1008",
//					  pCSHead->teamid(),
//					  pMsg->mutable_msghead()->messageid());
//			// 通知客户端解析消息失败并断开连接
//			CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_msgerr,
//																	   lTmpMsgGuid,
//																	   iTmpSocket,
//																	   tTmpCreateTime,
//																	   mNetHead.m_iSrcIP,
//																	   mNetHead.m_nSrcPort,
//																	   true);
//
//			if (pTmpTeam) {
//				LOG_DEBUG("default", "team leavegame 1008");
//				// 断开链接
//				CCoreModule::GetSingletonPtr()->LeaveGame(pTmpTeam, false);
//				// 测试
//				pTmpTeam->SetTeamDebug(0);
//			}
//
//			return CLIENTHANDLE_MSGINVALID;
//		}
//
//		if (pTmpTeam->IsLoginLimitTime()) {
//			// 通知客户端解析消息失败并断开连接
//			CGameServer::GetSingletonPtr()->SendMsgSystemErrorResponse(emSystem_loginlimit,
//																	   lTmpMsgGuid,
//																	   iTmpSocket,
//																	   tTmpCreateTime,
//																	   mNetHead.m_iSrcIP,
//																	   mNetHead.m_nSrcPort,
//																	   true);
//		}
//
//		pTmpTeam->SetTeamState(CTeam::ETS_INGAMECONNECT);
//	}
//
//
//	pTmpTeam->GetSocketInfoPtr()->tLastActiveTime = mNetHead.m_tStamp;
//	pCSHead->set_entityid(pTmpTeam->GetEntityID());
//	if (pTmpTeam->GetSocketInfoPtr()->lMsgGuid == lTmpMsgGuid) {
//		// 如果消息的GUID相等,说明这是客户端重发的消息,服务器已经处理过了,直接抛弃
//		LOG_INFO("default", "[{} : {} : {}] message guid(%ld) is same, ignore it.",
//				 __MY_FILE__, __LINE__, __FUNCTION__, lTmpMsgGuid);
//		return CLIENTHANDLE_SUCCESS;
//	}
//	pTmpTeam->GetSocketInfoPtr()->lMsgGuid = lTmpMsgGuid;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	CMessageDispatcher::ProcessClientMessage(pMsg);
	return CLIENTHANDLE_SUCCESS;
}

int CClientHandle::SendResToPlayer(std::shared_ptr<Message> pMessage, CPlayer *pPlayer)
{
	MY_ASSERT((pMessage != NULL && pPlayer != NULL), return -1);
	std::shared_ptr<MesHead> pTmpHead = std::make_shared<MesHead>();
	CSocketInfo *pTmpSocket = pTmpHead->mutable_socketinfos()->Add();
	STConnectInfo *pTmpConnInfo = pPlayer->GetPlayerBase()->GetSocketInfoPtr();
	if (pTmpConnInfo == NULL) {
		MY_ASSERT_STR(0, return -1, "CClientHandle::Send failed, Get player connection info failed.");
	}
	pTmpSocket->set_createtime(pTmpConnInfo->m_tCreateTime);
	pTmpSocket->set_socketid(pTmpConnInfo->m_iSocket);
	Package tmpPackage = pPlayer->GetPackage();
	pTmpHead->set_cmd(tmpPackage.GetCmd());
	pTmpHead->set_seq(tmpPackage.GetSeq());
	pTmpHead->set_serial(tmpPackage.GetSerial());

	tmpPackage.SetDeal(false);
	SendResponse(pMessage, pTmpHead);
	return 0;
}

int CClientHandle::SendResponse(std::shared_ptr<Message> pMessage, std::shared_ptr<MesHead> mesHead)
{
	CByteBuff tmpByteBuff;
	// 是否需要加密，在这里修改参数
	int iRet = CClientCommEngine::ConvertToGateStream(&tmpByteBuff,
													  mesHead.get(),
													  pMessage.get(),
													  mesHead->cmd(),
													  mesHead->serial(),
													  mesHead->seq());
	if (iRet != 0) {
		MY_ASSERT_STR(0,
					  return -2,
					  "CClientHandle::Send failed, CClientCommEngine::ConvertGameServerMessageToStream failed.");
	}

	iRet = mS2CPipe->AppendOneCode((BYTE *) tmpByteBuff.CanReadData(), tmpByteBuff.ReadableDataLen());
	if (iRet < 0) {
		MY_ASSERT_STR(0, return -3, "CClientHandle::Send failed, AppendOneCode return {}.", iRet);
	}

	if (!IsToBeBlocked() && GetStatus() != rt_running) {
		WakeUp();
	}
	LOG_DEBUG("default", "---- Send To Client Succeed ----");
	return 0;
}

int CClientHandle::Push(int cmd, std::shared_ptr<Message> pMessage, stPointList *pTeamList)
{
	MY_ASSERT((pMessage != NULL && pTeamList != NULL), return -1);

	// 判断是否发送消息后断开连接(这个主动断开只针对与第一个玩家)
	MesHead pTmpHead;
	for (int i = 0; i < pTeamList->GetBroadcastNum(); i++) {
		// 将列表中的实体信息加入nethead头中
		CPlayer *pPlayer = (CPlayer *) pTeamList->GetPointByIdx(i);
		if (pPlayer) {
			if (pPlayer->GetPlayerBase()->GetSocketInfoPtr()->m_iSocket != 0) {
				CSocketInfo *pTmpSocket = pTmpHead.add_socketinfos();
				STConnectInfo *pTmpConnInfo = pPlayer->GetPlayerBase()->GetSocketInfoPtr();
				if (pTmpConnInfo == NULL) {
					MY_ASSERT_STR(0, return -1, "CClientHandle::Send failed, Get player connection info failed.");
				}
				pTmpSocket->set_createtime(pTmpConnInfo->m_tCreateTime);
				pTmpSocket->set_socketid(pTmpConnInfo->m_iSocket);
			}
			else {
				LOG_DEBUG("default",
						  "Client({} | %lu | {}) has disconnected.",
						  pPlayer->GetPlayerId(),
						  pPlayer->GetPlayerId(),
						  pPlayer->GetPlayerBase()->GetAccount());
			}
		}
	}

	CByteBuff tmpByteBuff;
	// 是否需要加密，在这里修改参数
	int iRet = CClientCommEngine::ConvertToGateStream(&tmpByteBuff,
													  &pTmpHead,
													  pMessage.get(),
													  cmd,
													  0,
													  0);
	if (iRet != 0) {
		MY_ASSERT_STR(0,
					  return -2,
					  "CClientHandle::Send failed, CClientCommEngine::ConvertGameServerMessageToStream failed.");
	}

	iRet = mS2CPipe->AppendOneCode((const BYTE *) tmpByteBuff.CanReadData(), tmpByteBuff.ReadableDataLen());
	if (iRet < 0) {
		MY_ASSERT_STR(0, return -3, "CClientHandle::Send failed, AppendOneCode return {}.", iRet);
	}
	return iRet;
}

int CClientHandle::RecvClientData()
{
	m_pRecvBuff->Reset();
	int iTmpCodeLength;
	// 从共享内存管道提取消息
	int iRet = mC2SPipe->GetHeadCode((BYTE *) m_pRecvBuff->CanWriteData(), iTmpCodeLength);

	if (iRet < 0) {
		LOG_ERROR("default", "[{} : {} : {}] When GetHeadCode from C2SPipe, error ocurr {}",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
		return ClienthandleErrCode::CLIENTHANDLE_QUEUE_CRASH;
	}

	if (iTmpCodeLength == 0) {
		return ClienthandleErrCode::CLIENTHANDLE_QUEUE_EMPTY;
	}

	std::shared_ptr<CMessage> tmpMes = std::make_shared<CMessage>();
	if (CClientCommEngine::ConvertStreamToMessage(m_pRecvBuff,
												  iTmpCodeLength,
												  tmpMes.get(),
												  CMessageFactory::GetSingletonPtr()) != 0) {
		return ClienthandleErrCode::CLIENTHANDLE_PARSE_FAILED;
	}

	CGameServer::GetSingletonPtr()->GetLogicThread()
		->PushTaskBack(std::mem_fn(&CClientHandle::DealClientMessage), this, tmpMes);
	return CLIENTHANDLE_SUCCESS;
}

// 断开玩家连接
void CClientHandle::DisconnectClient(int iSocket,
									 time_t tCreateTime)
{
//	BYTE abyTmpCodeBuff[MAX_PACKAGE_LEN];
//	unsigned char *pucTmpBuff = (unsigned char *) abyTmpCodeBuff;
//	unsigned short unTmpLen = 0;
//
//	// 这里必须用临时变量，因为有可能是在接收到消息的时候发起断连
//	MesHead tmpNetHead;
//	tmpNetHead.Initialize(time(NULL), -1, uiIP, unPort);
//	tmpNetHead.AddEntity(iSocket, tCreateTime);
//
//	// 总长度
//	*(unsigned short *) pucTmpBuff =
//		((sizeof(unsigned short) * 2) + tmpNetHead.Size());
//	pucTmpBuff += sizeof(unsigned short);
//	unTmpLen += sizeof(unsigned short);
//
//	// CNetHead长度
//	*(unsigned short *) pucTmpBuff = tmpNetHead.Size();
//	pucTmpBuff += sizeof(unsigned short);
//	unTmpLen += sizeof(unsigned short);
//
//	// 序列化CNetHead
//	unsigned short unRet = tmpNetHead.SerializeToArray(pucTmpBuff,
//													   MAX_PACKAGE_LEN - unTmpLen);
//	if (unRet < 0) {
//		LOG_ERROR("default", "[{} : {} : {}] SerializeToArray failed, tRet = {}.",
//				  __MY_FILE__, __LINE__, __FUNCTION__, unRet);
//		return;
//	}
//	if (unRet != tmpNetHead.Size()) {
//		LOG_ERROR("default", "[{} : {} : {}] length is not same ({} : {}).",
//				  __MY_FILE__, __LINE__, __FUNCTION__, unRet, tmpNetHead.Size());
//		return;
//	}
//	pucTmpBuff += unRet;
//	unTmpLen += unRet;
//
//	unRet = m_pS2CPipe->AppendOneCode(abyTmpCodeBuff, unTmpLen);
//	if (unRet < 0) {
//		LOG_ERROR("default", "[{} : {} : {}] AppendOneCode failed, tRet = {}.",
//				  __MY_FILE__, __LINE__, __FUNCTION__, unRet);
//		return;
//	}

	LOG_NOTICE("default", "Disconnect Client [Socket = {} : CreateTime = {}].", iSocket, tCreateTime);
}

// 打印管道状态
void CClientHandle::Dump(char *pBuffer, unsigned int &uiLen)
{
	unsigned int uiTmpMaxLen = uiLen;
	uiLen = 0;

	int iTmpBegin = 0;
	int iTmpEnd = 0;
	int iTmpLeft = 0;

	uiLen += snprintf(pBuffer + uiLen, uiTmpMaxLen - uiLen,
					  "------------------------------CClientHandle------------------------------");
	uiLen += snprintf(pBuffer + uiLen, uiTmpMaxLen - uiLen, "\n%30s\t%10s\t%10s",
					  "name", "free", "total");

	mC2SPipe->GetCriticalData(iTmpBegin, iTmpEnd, iTmpLeft);
	uiLen += snprintf(pBuffer + uiLen, uiTmpMaxLen - uiLen, "\n%30s\t%10d\t%10d",
					  "c2s pipe", iTmpLeft, PIPE_SIZE);

	mS2CPipe->GetCriticalData(iTmpBegin, iTmpEnd, iTmpLeft);
	uiLen += snprintf(pBuffer + uiLen, uiTmpMaxLen - uiLen, "\n%30s\t%10d\t%10d",
					  "s2c pipe", iTmpLeft, PIPE_SIZE);
}

int CClientHandle::CheckData()
{
	if (!IsToBeBlocked() && GetStatus() != eRunStatus::rt_running) {
		//唤醒线程
		WakeUp();
		return 1;
	}
	return 0;
}

int CClientHandle::PrepareToRun()
{
	int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);

	////////////////////////////////mS2CPipe/////////////////////////////////////////
	system("touch ./scpipefile");
	char *pcTmpSCPipeID = getenv("SC_PIPE_ID");
	int iTmpSCPipeID = 0;
	if (pcTmpSCPipeID) {
		iTmpSCPipeID = atoi(pcTmpSCPipeID);
	}
	key_t iTmpKeyS2C = MakeKey("./scpipefile", iTmpSCPipeID);
	BYTE *pbyTmpS2CPipe = CreateShareMem(iTmpKeyS2C, iTempSize);
	MY_ASSERT(pbyTmpS2CPipe != NULL, exit(0));
	CSharedMem::pbCurrentShm = pbyTmpS2CPipe;
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyS2C, iTempSize, SHM_INIT);
	//mS2CPipe地址相对pbyTmpS2CPipe的偏移地址sizeof(CSharedMem) + sizeof(CCodeQueue)
	mS2CPipe = CCodeQueue::CreateInstance(PIPE_SIZE, enLockIdx::IDX_PIPELOCK_S2C);

	////////////////////////////////mC2SPipe/////////////////////////////////////////
	system("touch ./cspipefile");
	char *pcTmpCSPipeID = getenv("CS_PIPE_ID");
	int iTmpCSPipeID = 0;
	if (pcTmpCSPipeID) {
		iTmpCSPipeID = atoi(pcTmpCSPipeID);
	}
	key_t iTmpKeyC2S = MakeKey("./cspipefile", iTmpCSPipeID);
	BYTE *pbyTmpC2SPipe = CreateShareMem(iTmpKeyC2S, iTempSize);
	MY_ASSERT(pbyTmpC2SPipe != NULL, exit(0));
	CSharedMem::pbCurrentShm = pbyTmpC2SPipe;
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyC2S, iTempSize, SHM_INIT);
	//mC2SPipe地址相对pbyTmpS2CPipe的偏移地址sizeof(CSharedMem) + sizeof(CCodeQueue)
	mC2SPipe = CCodeQueue::CreateInstance(PIPE_SIZE, enLockIdx::IDX_PIPELOCK_C2S);
	return 0;
}

void CClientHandle::RunFunc()
{
	//获取客户端上行数据
	RecvClientData();
}

bool CClientHandle::IsToBeBlocked()
{
	return mC2SPipe->IsQueueEmpty();
}