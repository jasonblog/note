//
//  tcpctrl.cpp
//  客户端tcp管理类源文件
//  Created by DGuco on 16/12/6.
//  Copyright © 2016年 DGuco. All rights reserved.
//


#include <my_macro.h>
#include <share_mem.h>
#include "my_assert.h"
#include "../inc/gate_ctrl.h"

template<> CGateCtrl *CSingleton<CGateCtrl>::spSingleton = NULL;

CGateCtrl::CGateCtrl()
	: m_pC2sHandle(new CC2sHandle()),
	  m_pS2cHandle(new CS2cHandle),
	  m_pSingleThead(new CThreadPool(1))
{
}

CGateCtrl::~CGateCtrl()
{
	SAFE_DELETE(m_pC2sHandle);
	SAFE_DELETE(m_pS2cHandle);
	SAFE_DELETE(m_pSingleThead);
}

int CGateCtrl::PrepareToRun()
{
#ifdef _DEBUG_
	//初始化日志
	INIT_ROATING_LOG("default", "../log/gatesvrd.log", level_enum::trace);
#else
	//初始化日志
	INIT_ROATING_LOG("default", "../log/gatesvrd.log", level_enum::info);
#endif
	//创建共享内存管道
	CreatePipe();
	//读取配置文件
	ReadConfig();
	m_pC2sHandle->PrepareToRun();
	m_pS2cHandle->PrepareToRun();
	return 0;
}

int CGateCtrl::Run()
{
	m_pS2cHandle->Run();
	m_pC2sHandle->Run();
}

CThreadPool *CGateCtrl::GetSingleThreadPool()
{
	return m_pSingleThead;
}

CC2sHandle *CGateCtrl::GetCC2sHandle()
{
	return m_pC2sHandle;
}

CS2cHandle *CGateCtrl::GetCS2cHandle()
{
	return m_pS2cHandle;
}

void CGateCtrl::CreatePipe()
{
	int iTempSize = sizeof(CSharedMem) + CCodeQueue::CountQueueSize(PIPE_SIZE);
	system("touch ./cspipefile");
	char *pcTmpCSPipeID = getenv("CS_PIPE_ID");
	int iTmpCSPipeID = 0;
	if (pcTmpCSPipeID) {
		iTmpCSPipeID = atoi(pcTmpCSPipeID);
	}
	key_t iTmpKeyC2S = MakeKey("./cspipefile", iTmpCSPipeID);
	BYTE *pbyTmpC2SPipe = CreateShareMem(iTmpKeyC2S, iTempSize);
	MY_ASSERT(pbyTmpC2SPipe != NULL, exit(0));
	CSharedMem::pbCurrentShm = pbyTmpC2SPipe;
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyC2S, iTempSize, EIMode::SHM_INIT);
	CC2sHandle::m_pC2SPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_C2S);

	system("touch ./scpipefile");
	char *pcTmpSCPipeID = getenv("SC_PIPE_ID");
	int iTmpSCPipeID = 0;
	if (pcTmpSCPipeID) {
		iTmpSCPipeID = atoi(pcTmpSCPipeID);
	}
	key_t iTmpKeyS2C = MakeKey("./scpipefile", iTmpSCPipeID);
	BYTE *pbyTmpS2CPipe = CreateShareMem(iTmpKeyS2C, iTempSize);
	MY_ASSERT(pbyTmpS2CPipe != NULL, exit(0));
	CSharedMem::pbCurrentShm = pbyTmpS2CPipe;
	CCodeQueue::pCurrentShm = CSharedMem::CreateInstance(iTmpKeyS2C, iTempSize, EIMode::SHM_INIT);
	CS2cHandle::m_pS2CPipe = CCodeQueue::CreateInstance(PIPE_SIZE, IDX_PIPELOCK_S2C);
}

void CGateCtrl::ReadConfig()
{
	new CServerConfig;
	const string filepath = "../config/serverinfo.json";
	if (-1 == CServerConfig::GetSingletonPtr()->LoadFromFile(filepath)) {
		LOG_ERROR("default", "Get TcpserverConfig failed");
		exit(0);
	}
}
