//
// Created by DGuco on 17-7-27.
//

#ifndef SERVER_SCENEOBJMANAGER_H
#define SERVER_SCENEOBJMANAGER_H

#include <cstddef>
#include <unordered_map>
#include "player.h"
#include "object_manager.h"
#include "server_tool.h"
#include "base.h"

#ifdef _DEBUG_
#define SERVER_CAP_TEAM				(500)		// 服务器最大战队实体数量
#define SERVER_TEAM_LIST			(5000)		// 服务器最大注册列表
#define SERVER_CAP_ONLINE			(SERVER_CAP_TEAM) // 服务器最大链接数量
#define SERVER_LOGIN_ONLINE			(400)		// 玩家链接最大限制
#define SERVER_LOGIN_MAX			(450)		// 登陆玩家数量删除标准
#define SERVER_CAP_MAIL				(500)		// 服务器最大邮件数量
#else
#define SERVER_CAP_TEAM				(6000)			// 服务器最大战队实体数量
#define SERVER_TEAM_LIST			(500000)		// 服务器最大注册列表
#define SERVER_CAP_ONLINE			(SERVER_CAP_TEAM) // 服务器最大链接数量
#define SERVER_LOGIN_ONLINE			(5000)			// 玩家链接最大限制
#define SERVER_LOGIN_MAX			(5500)			// 登陆玩家数量删除标准
#define SERVER_CAP_MAIL 			(100000)		// 服务器最大邮件数量
#endif

// 最大服务器数量
#define MAX_SERVER_NUM		(9999)

// 服务器timer数量
#define SERVER_CAP_TIMER		(20000)

// 服务器session数量
#define SERVER_CAP_SESSION		(20000)

class CObj;

class CSceneObjManager : public CSingleton<CSceneObjManager>
{
public:
    CSceneObjManager();
    ~CSceneObjManager();

    int Initialize();

public:
    // 获取玩家
    CPlayer* GetPlayer(OBJ_ID ulPlayerid);
    // 获取玩家
    CPlayer* GetPlayerBySocket(int socket);
    // 删除玩家
    int DestroyPlayer(OBJ_ID iObjID);
    // 添加玩家
    void AddNewPlayer(CPlayer *pPlayer);
    // 获取玩家管理器
    CObjectManager* GetPlayerManager();
private:
    CObjectManager* m_pPlayerManager;
    //key:玩家id value:玩家
    std::unordered_map<OBJ_ID, CPlayer*> m_mPlayerMap;
    //key:在线玩家socket value:玩家id
    std::unordered_map<int,OBJ_ID> m_mSocketMap;
};


#endif //SERVER_SCENEOBJMANAGER_H
