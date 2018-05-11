//
// Created by DGuco on 17-3-1.
//
#include "my_assert.h"
#include "config.h"
#include "server_comm_engine.h"
#include "../inc/game_server.h"
#include "../inc/message_factory.h"

template<> CGameServer *CSingleton<CGameServer>::spSingleton = NULL;

CGameServer::CGameServer()
{
	Initialize();
}

CGameServer::~CGameServer()
{
	m_pClientHandle->StopThread();
	SAFE_DELETE(m_pClientHandle);
	SAFE_DELETE(m_pServerHandle);
	SAFE_DELETE(m_pModuleManager);
	SAFE_DELETE(m_pMessageDispatcher);
	SAFE_DELETE(m_pMessageFactory);
	SAFE_DELETE(m_pTimerManager);
	SAFE_DELETE(m_pLogicThread);
	SAFE_DELETE(m_pIoThread);
}

int CGameServer::Initialize()
{
	miServerState = 0;
	m_pModuleManager = new CModuleManager;
	m_pClientHandle = new CClientHandle;
	m_pMessageDispatcher = new CMessageDispatcher;
	m_pMessageFactory = new CMessageFactory;
	m_pTimerManager = new CTimerManager;
	m_pServerHandle = new CServerHandle();
	m_pLogicThread = new CThreadPool(1);
	m_pIoThread = new CThreadPool(1);
	return 0;
}

// 读取配置
int CGameServer::ReadCfg()
{
	// 读取配置
	CServerConfig *pTmpConfig = new CServerConfig;
	const string filepath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingleton().LoadFromFile(filepath)) {
		LOG_ERROR("default", "Get TcpserverConfig failed");
		delete pTmpConfig;
		pTmpConfig = NULL;
		exit(0);
	}
	return 0;
}

// 运行准备
int CGameServer::PrepareToRun()
{
#ifdef _DEBUG_
	// 初始化日志
	INIT_ROATING_LOG("default", "../log/gameserver.log", level_enum::trace);
#else
	// 初始化日志
	INIT_ROATING_LOG("default", "../log/gameserver.log", level_enum::info);
#endif
	// 读取服务器配置信息
	if (ReadCfg() != 0) {
		return -1;
	}
	if (StartAllTimers() != 0) {
		return -4;
	}

	if (m_pClientHandle->PrepareToRun()) {
		return -1;
	}

	if (m_pServerHandle->PrepareToRun()) {
		return -1;
	}

	// 通知各模块启动
	if (m_pModuleManager->OnLaunchServer() != 0) {
		return -5;
	}

	return 0;
}

// 开启所有定时器
int CGameServer::StartAllTimers()
{
	return 0;
}

// 服务器间心跳检测
void CGameServer::OnTimeCheckStateInServer(CTimerBase *pTimer)
{
//	MY_ASSERT_LOG("default", pTimer != NULL, return);
//
//	time_t tTmpNow = GetMSTime();
//	// 检测服务器与proxy的连接状态，如果状态异常则重新连接，状态正常则发送心跳消息
//	CGameServer *pTmpGameServer =
//		CGameServer::GetSingletonPtr();//		LOG_DEBUG("default", "in loop {}, size={}.", i, pTmpGameServer->mConfig.gameconfig().proxyinfo_size());
//	if ((pTmpGameServer->m_ProxyClient.IsConnected() == false)
//		|| pTmpGameServer->m_ProxyClient.IsTimeout(tTmpNow)) {
//		// 清除proxy已连接状态
//		CGameServer::GetSingletonPtr()->EraseServerState(CGameServer::ESS_CONNECTPROXY);
//		LOG_WARN("default", "[{} : {} : {}] mProxyClient maybe closed, current status({}) .",
//				 __MY_FILE__, __LINE__, __FUNCTION__, pTmpGameServer->m_ProxyClient.GetStatus());
//		if (pTmpGameServer->Connect2Proxy() == true) {
//			if (pTmpGameServer->Register2Proxy() == true) {
//				pTmpGameServer->m_ProxyClient.ResetTimeout(tTmpNow);
//			}
//		}
//		else {
//			pTmpGameServer->SendKeepAlive2Proxy();
//		}
//	}
}

// perf日志打印
void CGameServer::OnTimePerfLog(CTimerBase *pTimer)
{

}

// 运行
void CGameServer::Run()
{
	LOG_INFO("default", "CGameServer start to run now.");
	m_pClientHandle->Run();
	m_pServerHandle->Run();
}

// 退出
void CGameServer::Exit()
{
	LOG_INFO("default", "Save all data finished. now exit.");
	printf("exit gameserver succeed!\n");
	exit(0);
}

void CGameServer::ProcessClientMessage(CMessage *pMsg, CPlayer *pPlayer)
{
	MY_ASSERT(pMsg != NULL && pPlayer != NULL,
			  return);
//    MY_ASSERT(pMsg->has_msghead() == true, return);
	int iTmpType = GetModuleClass(pMsg->msghead().cmd());
	try {
		m_pModuleManager->OnClientMessage(iTmpType, pPlayer, pMsg);
	}
	catch (std::logic_error error) {
		LOG_ERROR("default", "Catch logic exception,msg {}", error.what());
	}
}

void CGameServer::ProcessRouterMessage(CProxyMessage *pMsg)
{
	MY_ASSERT(pMsg != NULL, return);
//    MY_ASSERT(pMsg->has_msghead() == true, return);
	int iTmpType = GetModuleClass(pMsg->msghead().messageid());
	try {
		m_pModuleManager->OnRouterMessage(iTmpType, pMsg);
	}
	catch (std::logic_error error) {
		LOG_ERROR("default", "Catch logic exception,msg {}", error.what());
	}
}

bool CGameServer::SendMessageToDB(CProxyMessage *pMsg)
{

}

// 通过消息ID获取模块类型
int CGameServer::GetModuleClass(int iMsgID)
{
//    return ((iMsgID >> 16) & 0xFF);
	return iMsgID / 100;
}

void CGameServer::DisconnectClient(CPlayer *pPlayer)
{
	if (!pPlayer) {
		return;
	}

	GetIoThread()->PushTaskBack(std::mem_fn(&CClientHandle::DisconnectClient),
								m_pClientHandle,
								pPlayer->GetPlayerBase()->GetSocket(),
								pPlayer->GetPlayerBase()->GetCreateTime());
}
// 设置服务器运行状态
void CGameServer::SetRunFlag(ERunFlag eRunFlag)
{
	m_RunFlag.SetRunFlag(eRunFlag);
}

// 广播消息给玩家，广播时，发起人一定放第一个
int CGameServer::Push(unsigned int iMsgID, std::shared_ptr<Message> pMsgPara, stPointList *pTeamList)
{
	GetIoThread()->PushTaskBack(std::mem_fn(&CClientHandle::Push), m_pClientHandle, iMsgID, pMsgPara, pTeamList);
	return 0;
}

// 发送消息给单个玩家
int CGameServer::Push(unsigned int iMsgID, std::shared_ptr<Message> pMsgPara, CPlayer *pPlayer)
{
	MY_ASSERT(pPlayer != NULL && pMsgPara != NULL, return -1);
	stPointList tmpList;
	tmpList.push_back(pPlayer);
	GetIoThread()->PushTaskBack(std::mem_fn(&CClientHandle::Push), m_pClientHandle, iMsgID, pMsgPara, &tmpList);
	return 0;
}

// 回复客户端上行的请求
int CGameServer::SendResponse(std::shared_ptr<Message> pMsgPara, CPlayer *pPlayer)
{
	MY_ASSERT(pPlayer != NULL && pMsgPara != NULL,
			  return -1);
	GetIoThread()->PushTaskBack(std::mem_fn(&CClientHandle::SendResToPlayer), m_pClientHandle, pMsgPara, pPlayer);
	return 0;
}

// 回复客户端上行的请求
int CGameServer::SendResponse(std::shared_ptr<Message> pMsgPara, std::shared_ptr<MesHead> mesHead)
{
	MY_ASSERT(mesHead != NULL && pMsgPara != NULL, return -1);
	GetIoThread()->PushTaskBack(std::mem_fn(&CClientHandle::SendResponse), m_pClientHandle, pMsgPara, mesHead);
}

// 检查服务器状态
int CGameServer::CheckRunFlags()
{
	if (true == mRunFlag.CheckRunFlag(ERF_RELOAD)) {
		// 重新加载模板数据
//		if (CTemplateMgr::GetSingletonPtr()->ReloadAllTemplate() < 0) {
//			printf("\n 重新加载模板数据失败, 请查看具体的错误!!!!\n");
//		}
//		else {
//			printf("\n 重新加模板数据成功!!!!\n");
//		}
		SetRunFlag(ERF_RUNTIME);
		return 1;
	}
	else if (true == mRunFlag.CheckRunFlag(ERF_QUIT)) {
		// 保存数据,退出游戏
		StartSaveAllData();
		SetRunFlag(ERF_RUNTIME);
		return 1;
	}
	return 0;
}

/*
// 刷新服务器状态
void CGameServer::FreshGame()
{
	// 这里计时器精度为毫秒
	unsigned long long tNowCount = GetMSTime();
	unsigned int tInterval = (unsigned int) ( tNowCount - mLastTickCount );
	if( tInterval < 100 )
	{
		return;
	}

	if( !CanProcessingClientMsg())
	{
		returnl;
	}
	CCoreModule::GetSingletonPtr()->OnTimerRefreshModule();
	mLastTickCount = tNowCount;
}
*/

// 服务器拉取数据完成
void CGameServer::LoadDataFinish()
{
	// 数据拉取完成,处理服务器状态
	SetServerState(CGameServer::ESS_LOADDATA);
	LOG_INFO("default", "[{} : {} : {}] Launch server load data finished.", __MY_FILE__, __LINE__, __FUNCTION__);
	// 开始处理服务器初始数据
	StartProcessingInitData();
}

// 服务器开始处理初始数据
void CGameServer::StartProcessingInitData()
{
}

// 存储所有玩家数据(一般故障拉起或程序退出时使用)
void CGameServer::SaveAllTeamdata()
{
//	CSceneObjManagerImp::ObjMng_Team* pTmpObjMngTeam = CSceneObjManager::GetSingletonPtr()->GetObjMngTeam();
//	LOG_DEBUG("default", "[{} : {} : {}] begin save all teams(num={}) data.", __YQ_FILE__, __LINE__, __FUNCTION__, pTmpObjMngTeam->size());
//
//	std::vector<CTeam*> vecDeleteList;
//
//	CSceneObjManagerImp::ObjMng_Team::iterator itTeam = pTmpObjMngTeam->begin();
//	for (; itTeam != pTmpObjMngTeam->end(); itTeam++)
//	{
//		CTeam* pTmpTeam = &(*itTeam);
//		if (pTmpTeam)
//		{
//			if (pTmpTeam->GetTeamState() == CTeam::ETS_INGAMECONNECT
//					|| pTmpTeam->GetTeamState() == CTeam::ETS_INGAMEDISCONNECT)
//			{
//				CCoreModule::GetSingletonPtr()->LeaveGame(pTmpTeam, CTeam::LEAVE_NORMAL);
//			}
//			else
//			{
//				vecDeleteList.push_back(pTmpTeam);
//			}
//		}
//	}
//
//	std::vector<CTeam*>::iterator itBegin = vecDeleteList.begin();
//	for (; itBegin != vecDeleteList.end(); itBegin++)
//	{
//		CCoreModule::GetSingletonPtr()->LeaveGame(*itBegin, CTeam::LEAVE_LOGINFAILED);
//	}
//
//	// 开启服务器退出检测
//	CreateTimer
//	(
//			CTimerManager::GetSingletonPtr(),
//			ETM_EXITSAVE,
//			CTimer::ERTM_GAME,
//			1000,
//			CGameServer::GetSingletonPtr()->CheckSaveAllTeamsFinish,
//			CTimer::TIMER_FOREVER
//	);
}

// 检测是否所有玩家存储结束
void CGameServer::CheckSaveAllTeamsFinish(CTimerBase *pTimer)
{
}

// 拉取服务器数据
void CGameServer::StartLoadAllData()
{

}

// 检测是否开启拉取服务器数据
void CGameServer::CheckStartLoadAllData(CTimerBase *pTimer)
{
}

// 存储服务器数据
void CGameServer::StartSaveAllData()
{
}

// 限制玩家登陆
int CGameServer::LimitTeamLogin(unsigned int iTeamID, time_t iTimes)// itimes 暂定为小时
{
	return 0;
}

// 检测是否停服存储数据
void CGameServer::CheckStartSaveAllData(CTimerBase *pTimer)
{
}

void CGameServer::SendMsgSystemErrorResponse(int iResult,
											 long lMsgGuid,
											 int iServerID,
											 time_t tCreateTime,
											 bool bKickOff)
{
}

int CGameServer::InitStaticLog()
{
	return 0;
}

CThreadPool *CGameServer::GetLogicThread()
{
	return m_pLogicThread;
}

CThreadPool *CGameServer::GetIoThread()
{
	return m_pIoThread;
}

CTimerManager *CGameServer::GetTimerManager()
{
	return m_pTimerManager;
}

CServerHandle *CGameServer::GetServerHandle()
{
	return m_pServerHandle;
}

CFactory *CGameServer::GetMessageFactory()
{
	return m_pMessageFactory;
}
