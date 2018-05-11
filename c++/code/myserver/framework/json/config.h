//
//  config.h
//  读取配置文件
//  Created by dguco on 17-2-16.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef SERVER_CONFIG_H_H
#define SERVER_CONFIG_H_H
#include <memory>
#include <map>
#include "server_tool.h"
#include "json.h"
#include "common_def.h"
#include "message.pb.h"

struct ServerInfo {
    int m_iServerId;
    string m_sHost;
    int m_iPort;
};

class CServerConfig : public CSingleton<CServerConfig>,public MyJson::Json
{
public:
    CServerConfig();
    ~CServerConfig();
    int Parse();
    void Clear();

public:
//    const std::map<enServerType ,ServerInfo>& GetServerMap() const {return m_mServerMap;}
    const string GetDbInfo() const {return m_sDblInfo;}
    const string GetDbIp() const { return m_sDbIp;}
    const int GetDbSleep() const {return m_iDbSleepTime;}
    const int GetDbLoop() const { return m_iDbLoop;}
    const int GetDbLogLevel() const { return m_iDbLogLevel;}
    const int GetDbRwTimeout() const { return m_iRwTimeOut;}

    const int GetTcpKeepAlive() const {return m_iTcpKeepAlive;}
    const int GetServetTick() const {return m_iServerTick;}
    const int GetCheckTimeOutGap() const {return m_iChecktimeOutGap;}
    const int GetSocketTimeOut() const {return m_iSokcetTimeout;}

    ServerInfo* GetServerInfo(enServerType type);
private:
    string m_sDbIp;
    string m_sDblInfo;
    int m_iDbSleepTime;
    int m_iDbLoop;
    int m_iDbLogLevel;
    int m_iRwTimeOut;

    //心跳
    int m_iTcpKeepAlive;
    int m_iServerTick;
    int m_iChecktimeOutGap;
    int m_iSokcetTimeout;
    std::map<enServerType ,ServerInfo> m_mServerMap;
};

#endif //SERVER_CONFIG_H_H
