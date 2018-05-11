//
// Created by dguco on 17-9-12.
//

#ifndef SERVER_DBMODULE_H
#define SERVER_DBMODULE_H

#include "message_interface.h"
#include "logic_module.h"
#include "timer.h"
#include "dbmessage.pb.h"
#include "dblogin_type.h"
#include "message.pb.h"
#include "../../datamodule/inc/player.h"


class CDbModule : public CLogicModule, public CSingleton<CDbModule> {
public:
    CDbModule();

    virtual  ~CDbModule();

    int Initialize();

    // 启动服务
    int OnLaunchServer();

    // 退出服务
    int OnExitServer();

    // 路由消息
    void OnRouterMessage(CProxyMessage *pMsg);

    // 客户端消息
    void OnClientMessage(CPlayer *pTeam,CMessage *pMsg);

    // 创建实体
    int OnCreateEntity(CPlayer *pTeam);

    // 销毁实体
    void OnDestroyEntity(CPlayer *pTeam);

private:
    int ExecuteSql(emDBLogicType nLogicType,
                   unsigned long ulTeamID,
                   int iSessionID,
                   long nTimeStamp,
                   SQLTYPE nSqlType,
                   int nProduOutNumber,
                   CALLBACK nIsCallBack,
                   MesHead* mesHead,
                   const char* pSql, ... );

    int ExecuteSqlForBlob(emDBLogicType nLogicType,
                          unsigned long ulTeamID,
                          int iSessionID, long nTimeStamp,
                          SQLTYPE nSqlType,
                          int nProduOutNumber,
                          CALLBACK nIsCallBack,
                          const char* pSql,
                          const int iBlobSize,
                          const char* pBlob,
                          const char* pSQLWhere,
                          MesHead* mesHead, ... );

    void OnMsgExecuteSqlResponse(CProxyMessage* pMsg);
public:
    //查找玩家帐号信息没有则创建
    void FindOrCreateUserRequest(const std::string& platform,const std::string& puid,MesHead* mesHead);
    void FindOrCreateUserResponse(CSession* pSession, CMsgExecuteSqlResponse* pMsgSql,CProxyHead* mesHead);
};

#endif //SERVER_DBMODULE_H
