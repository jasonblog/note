//
// Created by DGuco on 17-9-6.
// 玩家基础信息
//

#ifndef SERVER_PLAYERBASE_H
#define SERVER_PLAYERBASE_H

#include <memory>
#include <ctime>
#include <cstring>
#include <iostream>
#include "playerdata.h"
#include "player.h"
#include "base.h"
#include "object.h"
#include "my_macro.h"

// 玩家链接信息
struct STConnectInfo
{
    int						m_iSocket;									// socketid即FD
    time_t					m_tCreateTime;								// 创建时间
    time_t					m_tLastActiveTime;							// 最后活跃时间
    time_t					m_tLoginTime;									// 登陆时间

    STConnectInfo()
    {
        Initialize();
    }

    int Initialize()
    {
        m_iSocket = 0;
        m_tCreateTime = 0;
        m_tLastActiveTime = 0;
        m_tLoginTime = 0;
        return 0;
    }

    void SetConnectInfo(int iSocketPara, time_t tCreateTimePara, time_t tLastActiveTimePara, time_t tLoginTimePara)
    {
        m_iSocket = iSocketPara;
        m_tCreateTime = tCreateTimePara;
        m_tLastActiveTime = tLastActiveTimePara;
        m_tLoginTime = tLoginTimePara;
    }

    void Clear()
    {
        m_iSocket = 0;
        m_tCreateTime = 0;
        m_tLastActiveTime = 0;
        m_tLoginTime = 0;
    }
};

class CPlayerBase : public CPlayerData
{
public:
    enum ETeamState
    {
        ETS_INVALID							= 0,	// 无效的状态,也是初始状态
        ETS_STARTLOGIN						= 1,	// 刚创建,初始化了链接信息,其它信息为空
        ETS_LOADACCOUNT				        = 2,	// 刚登陆,拉取帐号信息
        ETS_LOADDATA						= 3,	// 拉取战队数据
        ETS_INGAMECONNECT			        = 4,	// 在游戏中,并且有连接
        ETS_INGAMEDISCONNECT		        = 5,	// 在游戏中,无连接
        ETS_EXITSAVE						= 6,	// 离开存储数据
    };

    enum ESLF_FLAGS
    {
        ELSF_LOADALL	= 0x0,  // todo fix it
        ELSF_SAVEALL	= 0x1,
    };

    enum emFirstLoginFlag
    {
        emLoginFlag_First				= 0,		// 第一次登录
        emLoginFlag_notFirst		= 1,		// 不是地一次登录
        emLoginFlag_haveobj			= 2,		// 玩家存在实体
    };

    // 玩家离线类型
    // < 0 tcp主动断连或检测到连接断开
    // = 0 正常断连
    // > 0 服务器主动断连
    enum ELeaveType
    {
        LEAVE_CLIENTCLOSE		= -1,		// TCP检测到客户端断连,不需要回复
        LEAVE_TIMEOUT				= -2,		// TCP检测到客户端超时,不需要回复
        LEAVE_ERRPACKAGE			= -3,		// TCP检测到包信息非法,不需要回复
        LEAVE_BUFFOVER				= -4,		// TCP缓冲区已满,不需要回复
        LEAVE_PIPEERR					= -5,		// TCP数据包放入管道失败,不需要回复
        LEAVE_SYSTEM					= -6,		// TCP系统错误，暂时未用,不需要回复
        LEAVE_PUSHAGAINST		= -7,		// 被挤下去,不需要回复
        LEAVE_NOCONTRAL			= -8,		// 玩家长时间未操作,不需要回复
        LEAVE_NORMAL 				= 0,		// 玩家正常下线,需要回复
        LEAVE_LOGINFAILED		= 1,		// 玩家登陆失败,需要回复
    };

public:
    CPlayerBase(CPlayer* pPlayer);
    virtual ~CPlayerBase();
    virtual int Initialize();
    // 实体ID
    OBJ_ID GetEntityID() {return GetPlayerId();}

public:
    // 上次登录时间
    void SetLastLoginTime(int v) {m_iLastLoginTime = v;}
    int GetLastLoginTime() {return m_iLastLoginTime;}
    // 离线时间
    void SetLeaveTime(int v) {m_iLeaveTime = v;}
    int GetLeaveTime() {return m_iLeaveTime;}
    // 创建时间
    void SetCreateTime(time_t v) {m_tCreateTime = v;}
    time_t GetCreateTime() {return m_tCreateTime;}

    int GetSocket() {return m_SocketInfo.m_iSocket;}
    int GetSocketCreateTime() { return  m_SocketInfo.m_tCreateTime;}
    int GetLastActiveTime() {return m_SocketInfo.m_tLastActiveTime;}
    int GetLoginTime() {return m_SocketInfo.m_tLoginTime;}
    // 帐号
    void SetAccount(const char* p)
    {
        if (p == NULL) {return;}
        int iTmpLen = strlen(p);
        strncpy(m_acAccount, p, iTmpLen >= ARRAY_CNT(m_acAccount) ? (ARRAY_CNT(m_acAccount) - 1) : iTmpLen);
    }
    char* GetAccount() {return &m_acAccount[0];}
    // 获取连接信息
    STConnectInfo* GetSocketInfoPtr() {return &m_SocketInfo;}

private:
    // 上次登录时间
    int m_iLastLoginTime;
    // 离线时间
    int m_iLeaveTime;
    // 创建时间
    time_t m_tCreateTime;
    // 帐号状态时长( 禁止登陆 禁止说话 禁止...)
    time_t m_tLoginLimitTime;
    // 帐号
    char			m_acAccount[UID_LENGTH];
    //连接信息
    STConnectInfo	m_SocketInfo;
};


#endif //SERVER_PLAYERBASE_H
