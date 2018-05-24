//
//  module_manager.h
//  模块管理 头文件
//  Created by DGuco on 17-06-21.
//  Copyright © 2017年 DGuco. All rights reserved.
//

#ifndef SERVER_MODULE_MANAGER_H
#define SERVER_MODULE_MANAGER_H

#include "message.pb.h"
#include "server_tool.h"
#include "logic_module.h"
#include "../../datamodule/inc/player.h"

class CLogicModule;

class CModuleManager : public CSingleton<CModuleManager>
{
public:
    CModuleManager();
    ~CModuleManager();

    int Initialize();

    // 注册模块
    int RegisterModule(EModuleType eType, CLogicModule* pModule);

    // 启动服务
    int OnLaunchServer();

    // 退出服务
    int OnExitServer();

    // 路由消息
    void OnRouterMessage(int iModuleType, CProxyMessage* pMsg);

    // 客户端消息
    void OnClientMessage(int iModuleType, CPlayer* pTeam, CMessage* pMsg);

    // 创建实体
    int OnCreateEntity(CPlayer* pTeam);

    // 玩家登陆成功
    int OnPlayerLogin(CPlayer* pTeam);

    // 销毁实体
    void OnDestroyEntity(CPlayer* pTeam);

protected:
    CLogicModule* mpLogicModules[EMODULETYPE_NUM];
};

#endif //SERVER_MODULE_MANAGER_H
