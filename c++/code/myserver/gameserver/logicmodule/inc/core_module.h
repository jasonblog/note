//
//  coremodule.h
//  coremodule.h类头文件 服务器全局的逻辑，非玩家个人的逻辑
//  Created by DGuco on 17-3-1.
//  Copyright © 2017年 DGuco. All rights reserved.
//
#ifndef SERVER_COREMODULE_H
#define SERVER_COREMODULE_H

#include <unordered_map>
#include "logic_module.h"
#include "server_tool.h"
#include "dblogin_type.h"

class CCoreModule: public CLogicModule, public CSingleton<CCoreModule>
{
public:
	CCoreModule();

	virtual  ~CCoreModule();

	int Initialize();

	// 启动服务
	int OnLaunchServer();

	// 退出服务
	int OnExitServer();

	// 路由消息
	void OnRouterMessage(CProxyMessage *pMsg);

	// 客户端消息
	void OnClientMessage(CPlayer *pTeam, CMessage *pMsg);

	// 创建实体
	int OnCreateEntity(CPlayer *pTeam);

	// 销毁实体
	void OnDestroyEntity(CPlayer *pTeam);

public:
	// 玩家登陆游戏申请
	void OnMsgUserLoginRequest(CMessage *pMsg);
	void OnMsgPlayerLoginRequest(CMessage *pMsg);
	bool CheckOnlineIsFull();
};

#endif //SERVER_COREMODULE_H
