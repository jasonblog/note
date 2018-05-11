//
//  main.cpp
//  gateserver
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//
#include <signal.h>
#include <config.h>
#include <my_macro.h>
#include "runflag.h"
#include "inc/proxy_ctrl.h"

CRunFlag g_byRunFlag;

void sigusr1_handle(int iSigVal)
{
	g_byRunFlag.SetRunFlag(ERF_RELOAD);
	signal(SIGUSR1, sigusr1_handle);
}

void ignore_pipe()
{
	struct sigaction sig;

	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGPIPE, &sig, NULL);
}

int main(int argc, char **argv)
{
	CProxyCtrl *pProxyCtrl = new CProxyCtrl();
	if (pProxyCtrl->PrepareToRun()) {
		LOG_ERROR("default", "ProxyServer prepare to fun failed.");
		if (pProxyCtrl) {
			delete pProxyCtrl;
			pProxyCtrl = NULL;
		}
		exit(3);
	}

	// 安装信号处理函数
	signal(SIGUSR1, sigusr1_handle);
	pProxyCtrl->Run();
	SAFE_DELETE(CServerConfig::GetSingletonPtr());
	if (pProxyCtrl != NULL) {
		delete pProxyCtrl;
		pProxyCtrl = NULL;
	}
	// 关闭日志
	LOG_SHUTDOWN_ALL;
	return 0;
}
