//
// Created by dguco on 17-9-12.
//

#include "server_comm_engine.h"
#include "player.pb.h"
#include "dbmessage.pb.h"
#include "client_comm_engine.h"
#include "my_assert.h"
#include "../inc/db_module.h"
#include "../../inc/game_server.h"
#include "../../datamodule/inc/sceneobjmanager.h"

template<> CDbModule *CSingleton<CDbModule>::spSingleton = NULL;

CDbModule::CDbModule()
{

}

CDbModule::~CDbModule()
{

}

int CDbModule::Initialize()
{
	return 0;
}

int CDbModule::OnLaunchServer()
{
	return 0;
}

int CDbModule::OnExitServer()
{
	return 0;
}

void CDbModule::OnRouterMessage(CProxyMessage *pMsg)
{
	MY_ASSERT_LOG("db", pMsg != NULL,
				  return);
	switch (pMsg->msghead().messageid()) {
	case CMsgExecuteSqlResponse::MsgID: {
		OnMsgExecuteSqlResponse(pMsg);
		break;
	}
	default: {
		LOG_ERROR("db", "[{} : {} : {}] invalid message id(%8x).",
				  __MY_FILE__, __LINE__, __FUNCTION__, pMsg->msghead().messageid());
		break;
	}
	}
}

void CDbModule::OnClientMessage(CPlayer *pTeam, CMessage *pMsg)
{

}

int CDbModule::OnCreateEntity(CPlayer *pTeam)
{
	return 0;
}

void CDbModule::OnDestroyEntity(CPlayer *pTeam)
{

}

void CDbModule::OnMsgExecuteSqlResponse(CProxyMessage *pMsg)
{
	MY_ASSERT_LOG("db", pMsg != NULL,
				  return);
	CMsgExecuteSqlResponse *pTmpSqlRsp = (CMsgExecuteSqlResponse *) pMsg->msgpara();
	MY_ASSERT_LOG("db", pTmpSqlRsp != NULL,
				  return);

	CSession *pTmpSession = NULL;
	if (pTmpSqlRsp->sessionid() != 0) {
		// 如果有session
		pTmpSession =
			(CSession *) CGameServer::GetSingletonPtr()->GetTimerManager()->GetObject(pTmpSqlRsp->sessionid());

		if (NULL == pTmpSession) {
			LOG_ERROR("db", "[{} : {} : {}] Session doesn't exsit (logictype:{}:sessionid:%u ), has been deleted.",
					  __MY_FILE__, __LINE__, __FUNCTION__,
					  pTmpSqlRsp->logictype(), pTmpSqlRsp->sessionid());
			return;
		}
		if (pTmpSession->GetTimeout() != (time_t) pTmpSqlRsp->timestamp()) {
			LOG_ERROR("db", "[{} : {} : {}] Session doesn't exsit (logictype:{}:sessionid:%u ), has been reused.",
					  __MY_FILE__, __LINE__, __FUNCTION__,
					  pTmpSqlRsp->logictype(), pTmpSqlRsp->sessionid());
			return;
		}
	}

	switch (pTmpSqlRsp->logictype()) {
		//*********************************************************************************
		// 读取战队数据
		//*********************************************************************************
	case emDBLogicType::emDBTypeFindOrCreateUser : {
		FindOrCreateUserResponse(pTmpSession, pTmpSqlRsp, pMsg->mutable_msghead());
		break;
	}
	}
}

int CDbModule::ExecuteSql(emDBLogicType nLogicType,
						  unsigned long ulTeamID,
						  int iSessionID,
						  long nTimeStamp,
						  SQLTYPE nSqlType,
						  int nProduOutNumber,
						  CALLBACK nIsCallBack,
						  MesHead *mesHead,
						  const char *pSql, ...)
{
	MY_ASSERT_LOG("db", pSql != NULL, return -1);

	// 参数整理
	va_list tmpArgs;
	char acTmpSqlCommond[MAX_PACKAGE_LEN] = {0};
	va_start(tmpArgs, pSql);
	int iTmpLen = ::vsnprintf(acTmpSqlCommond, sizeof(acTmpSqlCommond), pSql, tmpArgs);
	MY_ASSERT_LOG("db", iTmpLen > -1,
				  return -2);
	va_end(tmpArgs);

	// 发往dbserver消息整理
	static CProxyMessage tmpMsg;
	static CMsgExecuteSqlRequest tmpMsgSqlRqt;
	tmpMsg.Clear();
	tmpMsgSqlRqt.Clear();

	CProxyHead *pTmpHead = tmpMsg.mutable_msghead();
	pbmsg_setmessagehead(pTmpHead, CMsgExecuteSqlRequest::MsgID);
	if (mesHead != NULL) {
		CClientCommEngine::CopyMesHead(mesHead, pTmpHead->mutable_msghead());
	}

	tmpMsgSqlRqt.set_logictype(nLogicType);
	tmpMsgSqlRqt.set_sessionid(iSessionID);
	tmpMsgSqlRqt.set_timestamp(nTimeStamp);
	tmpMsgSqlRqt.set_teamid(ulTeamID);
	tmpMsgSqlRqt.set_sqltype(nSqlType);
	tmpMsgSqlRqt.set_outnumber(nProduOutNumber);
	tmpMsgSqlRqt.set_callback(nIsCallBack);
	tmpMsgSqlRqt.set_sql(acTmpSqlCommond);
	tmpMsgSqlRqt.set_hasblob(NONEBLOB);
	tmpMsg.set_msgpara((unsigned long) &tmpMsgSqlRqt);

	LOG_DEBUG("db", "[{}]", ((Message *) tmpMsg.msgpara())->ShortDebugString().c_str());
	MY_ASSERT_LOG("db", CGameServer::GetSingletonPtr()->SendMessageToDB(&tmpMsg), return -3);
	return 0;
}

int CDbModule::ExecuteSqlForBlob(emDBLogicType nLogicType,
								 unsigned long ulTeamID,
								 int iSessionID,
								 long nTimeStamp,
								 SQLTYPE nSqlType,
								 int nProduOutNumber,
								 CALLBACK nIsCallBack,
								 const char *pSql,
								 const int iBlobSize,
								 const char *pBlob,
								 const char *pSQLWhere,
								 MesHead *mesHead, ...)
{
	MY_ASSERT_LOG("db", pSql != NULL && pBlob != NULL && pSQLWhere != NULL, return -1);

	// 发往dbserver消息整理
	static CProxyMessage tmpMsg;
	static CMsgExecuteSqlRequest tmpMsgSqlRqt;
	tmpMsg.Clear();
	tmpMsgSqlRqt.Clear();

	CProxyHead *pTmpHead = tmpMsg.mutable_msghead();
	pbmsg_setmessagehead(pTmpHead, CMsgExecuteSqlRequest::MsgID);
	if (mesHead != NULL) {
		CClientCommEngine::CopyMesHead(mesHead, pTmpHead->mutable_msghead());
	}

	tmpMsgSqlRqt.set_logictype(nLogicType);
	tmpMsgSqlRqt.set_sessionid(iSessionID);
	tmpMsgSqlRqt.set_timestamp(nTimeStamp);
	tmpMsgSqlRqt.set_teamid(ulTeamID);
	tmpMsgSqlRqt.set_sqltype(nSqlType);
	tmpMsgSqlRqt.set_outnumber(nProduOutNumber);
	tmpMsgSqlRqt.set_callback(nIsCallBack);
	tmpMsgSqlRqt.set_sql(pSql);

	tmpMsgSqlRqt.set_bufsize(iBlobSize);
	tmpMsgSqlRqt.set_buffer((void *) pBlob, iBlobSize);
	tmpMsgSqlRqt.set_sqlwhere(pSQLWhere);
	tmpMsgSqlRqt.set_hasblob(HASBLOB);

	tmpMsg.set_msgpara((unsigned long) &tmpMsgSqlRqt);
	LOG_DEBUG("db", "[{}]", ((Message *) tmpMsg.msgpara())->ShortDebugString().c_str());
	MY_ASSERT_LOG("db", CGameServer::GetSingletonPtr()->SendMessageToDB(&tmpMsg),
				  return -2);
	return 0;
}

void CDbModule::FindOrCreateUserRequest(const std::string &platform, const std::string &puid, MesHead *mesHead)
{
	char *pcTmpSql = (char *) "SELECT `player_id` FROM user WHERE `platform` = {} AND `puid` = {}";
	int iRet = CDbModule::GetSingletonPtr()->ExecuteSql(
		emDBLogicType::emDBTypeFindOrCreateUser,
		0,
		0,
		0,
		SELECT,
		0,
		MUSTCALLBACK,
		mesHead,
		pcTmpSql,
		platform.c_str(),
		puid.c_str()
	);
	if (iRet != 0) {
		LOG_ERROR("db", "[{} : {} : {}] ExecuteSql failed, iRet={}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, iRet);
//        exit(0);
	}
}

void CDbModule::FindOrCreateUserResponse(CSession *pSession, CMsgExecuteSqlResponse *pMsgSql, CProxyHead *proxyHead)
{
	MY_ASSERT_LOG("db", pMsgSql != NULL && pSession != NULL && proxyHead != NULL, return);

	int iTmpRowCount = pMsgSql->rowcount();        // 行数
	int iTmpColCount = pMsgSql->colcount();        // 列数

	if (1 == pMsgSql->resultcode()) {
		OBJ_ID player_id = -1;    // 帐号
		//没有找到玩家
		if (0 == iTmpRowCount || 0 == iTmpColCount) {
			player_id = (OBJ_ID) CSceneObjManager::GetSingletonPtr()->GetPlayerManager()->GetValidId();
		}
		else {
			// 表示有返回结果
			player_id = atol((char *) pMsgSql->fieldvalue(0).c_str());
		}
		if (player_id == -1) {
			player_id = (OBJ_ID) CSceneObjManager::GetSingletonPtr()->GetPlayerManager()->GetValidId();
		}
		UserAccountLoginResponse *pRes = new UserAccountLoginResponse;
		pRes->set_playerid(player_id);
		MesHead *pHead = new MesHead;
		CClientCommEngine::CopyMesHead(proxyHead->mutable_msghead(), pHead);
		CGameServer::GetSingletonPtr()->SendResponse(std::shared_ptr<Message>(pRes), std::shared_ptr<MesHead>(pHead));
	}
	else {
		// 拉取数据失败,则退出
		LOG_ERROR("db", "[{} : {} : {}] LoadAllAccountResponse failed, resultcode={}, row={}, col={}.",
				  __MY_FILE__, __LINE__, __FUNCTION__, pMsgSql->resultcode(), iTmpRowCount, iTmpColCount);
		return;
	}
}