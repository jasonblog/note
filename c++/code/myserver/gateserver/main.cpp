//
//  main.cpp
//  gateserver
//  Created by DGuco on 16/12/8.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#include <memory>
#include "my_macro.h"
#include "inc/gate_ctrl.h"

CGateCtrl *g_pGateServer;

int main(int argc, char **argv)
{
	int iTmpRet;
	g_pGateServer = new CGateCtrl;
	iTmpRet = g_pGateServer->PrepareToRun();
	if (iTmpRet != 0) {
		delete g_pGateServer;
		LOG_ERROR("default", "Tcpserver PrepareToRun failed,iRet = {}", iTmpRet);
		exit(0);
	}
	g_pGateServer->Run();
	SAFE_DELETE(CServerConfig::GetSingletonPtr());
	if (g_pGateServer != NULL) {
		delete g_pGateServer;
		g_pGateServer = NULL;
	}
	// 关闭日志
	LOG_SHUTDOWN_ALL;
}
