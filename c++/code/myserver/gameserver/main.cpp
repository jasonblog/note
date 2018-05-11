//
//  main.cpp
//  gameserver
//  Created by DGuco on 17-3-1.
//  Copyright © 2017年 DGuco. All rights reserved.
//

#include <signal.h>
#include "config.h"
#include "inc/game_server.h"

void sigusr1_handle(int iSigVal)
{
	CGameServer::GetSingletonPtr()->SetRunFlag(ERF_RELOAD);
	signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle(int iSigVal)
{
	CGameServer::GetSingletonPtr()->SetRunFlag(ERF_QUIT);
	signal(SIGUSR2, sigusr2_handle);
}

int main(int argc, char *argv[])
{
	// 准备启动服务器
	CGameServer *pTmpGameServer = new CGameServer;
	int iRet = pTmpGameServer->PrepareToRun();
	if (iRet != 0) {
		LOG_ERROR("default", "CGameServer prepare to run failed, iRet = {}.", iRet);
		exit(0);
	}

	// 信号
	signal(SIGUSR1, sigusr1_handle);
	signal(SIGUSR2, sigusr2_handle);

	// 启动服务器
	pTmpGameServer->Run();
	LOG_INFO("default", "Game server is going to stop...");
	if (pTmpGameServer) {
		delete pTmpGameServer;
		pTmpGameServer = NULL;
	}
	SAFE_DELETE(CServerConfig::GetSingletonPtr());
	// 关闭日志
	LOG_SHUTDOWN_ALL;
	return 0;
}

