//
// Created by DGuco on 17-6-21.
// 模块接口
//

#ifndef SERVER_LOGIC_MODULE_H
#define SERVER_LOGIC_MODULE_H

#include "message.pb.h"
#include "../../datamodule/inc/player.h"

// 模块类型
enum EModuleType
{
	EMODULETYPE_INVALID = -1,    // 无效的模块

	EMODULETYPE_PLAYER = 0,    // 核心模块
//    EMODULETYPE_BUILD			= 2,	// 城建模块
//    EMODULETYPE_TIMER			= 3,	// 定时任务模块
//    EMODULETYPE_RESOURCE		= 4,	// 资源田模块
//    EMODULETYPE_TRAIN		    = 5,	// 士兵训练模块
//    EMODULETYPE_ITEM		    = 6,	// 道具模块
		EMODULETYPE_DB = 1,  // 数据模块
	EMODULETYPE_NUM                        // 模块数量
};

#define EMODULETYPE_START (EMODULETYPE_INVALID + 1)    // 模块起始下标

class CLogicModule
{
public:
	CLogicModule();
	virtual ~CLogicModule();

	bool IsRegist();
	void RegistModule(EModuleType eType);

	virtual int Initialize()
	{ return 0; }

public:
	bool mRegist;        // 是否注册
	EModuleType mModueType;        // 模块类型

public:
	// 启动服务
	virtual int OnLaunchServer()
	{ return 0; }

	// 退出服务
	virtual int OnExitServer()
	{ return 0; }

	// 路由消息
	virtual void OnRouterMessage(CProxyMessage *pMsg)
	{}

	// 客户端消息
	virtual void OnClientMessage(CPlayer *pTeam, CMessage *pMsg)
	{}

	// 创建实体
	virtual int OnCreateEntity(CPlayer *pTeam)
	{ return 0; }

	// 创建实体
	virtual int OnPlayerLogin(CPlayer *pTeam)
	{ return 0; }

	// 销毁实体
	virtual void OnDestroyEntity(CPlayer *pTeam)
	{}
};

#endif //SERVER_LOGIC_MODULE_H
