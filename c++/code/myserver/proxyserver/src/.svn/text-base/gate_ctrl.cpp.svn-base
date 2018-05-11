#include "gate_ctrl.h"
#include "runflag.h"
#include "log.h"
#include "message.pb.h"
#include "tcpmessage.pb.h"
#include "protoconfig.h"
#include "client_comm_engine.h"

extern CRunFlag g_byRunFlag;

#ifdef _POSIX_MT_
	pthread_mutex_t CGateCtrl::stLinkMutex[EHandleType_NUM];
	pthread_mutex_t CGateCtrl::stMutex[MUTEX_NUM];
#endif

CGateCtrl::CGateCtrl()
{
	m_mapConns.initialize();

	for (int i = 0; i < MAX_CONNS_NUM; i++)
	{
		m_UnuseConns.insert(&m_astConns[i]);
	}

	m_iCurrentUnRegisterNum = 0;
	memset((void *)m_astUnRegisterInfo, 0, sizeof(m_astUnRegisterInfo));
	time(&m_tLastCheckTime);
}

CGateCtrl::~CGateCtrl()
{

}

int CGateCtrl::Initialize()
{
	// 读取配置文件
	if (ReadCfg() < 0)
	{
		LOG_ERROR("default", "Read config failed.");
		return -1;
	}

	return 0;
}


int CGateCtrl::ReadCfg()
{
	if (CConfigMgr::GetSingletonPtr()->LoadConfig("../config/gateconfig.proto") < 0)
	{
		LOG_ERROR("default", "load config failed.");
		return -1;
	}

	return 0;
}


CMyTCPConn* CGateCtrl::GetConnByKey(int iKey)
{
	CMyTCPConn* tpConn = NULL;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&stMutex[MUTEX_HASHMAP]);
#endif

	CONNS_MAP::iterator iter = m_mapConns.find(iKey);
	if (iter != m_mapConns.end())
	{
		tpConn = iter->second;
	}

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&stMutex[MUTEX_HASHMAP]);
#endif

	return tpConn;
}


int CGateCtrl::MakeConnKey(const short nType, const short nID)
{
	int iKey = 0;

	iKey = nType;
	iKey = (iKey << 16) | nID;

	return iKey;
}


// 将连接插入handle并唤醒handle
int CGateCtrl::WakeUp2Work(CMyTCPConn* pConn)
{
	int index = 0;
	int minNum = m_stHandleInfos[0].miConnNum;
	int iRet = 0;

	// 找出合适的handle
	for (int i = 1; i < EHandleType_NUM; i++)
	{
		if (m_stHandleInfos[i].miConnNum < minNum)
		{
			minNum = m_stHandleInfos[i].miConnNum;
			index = i;
		}
	}

	iRet = InsertConnIntoMap(pConn, index);
	if (iRet != 0)
	{
		return iRet;
	}

	// 唤醒处理线程
	LOG_DEBUG("default", "Wake up handle(%d) start to work.", index);
	pConn->SetConnState(ENTITY_ON);
	pConn->SetLastKeepalive(time(NULL));
	m_stHandleInfos[index].mpHandle->WakeUp();

	return iRet;
}


// 获取一个可用连接
CMyTCPConn* CGateCtrl::GetCanUseConn()
{
	CMyTCPConn* pConn = (CMyTCPConn*) m_UnuseConns.GetHead();
	if (m_UnuseConns.GetHead() == NULL)
	{
		// 没有可用连接了
		LOG_WARN("default", "no unuse conns.");
		return NULL;
	}

	// 将结点从链表中移除
	m_UnuseConns.erase(pConn);

	return pConn;
}


// 回收一个连接
CMyTCPConn* CGateCtrl::RecycleUnuseConn(CMyTCPConn* pConn, int iIndex)
{
	CMyTCPConn* tpNext = NULL;

	if (!pConn)
	{
		return tpNext;
	}

	tpNext = (CMyTCPConn*) pConn->GetNext();

	if (iIndex >= 0 && iIndex < EHandleType_NUM)
	{
		// 从map中移除连接, 这里处理m_stHandleInfos[iIndex]
		EraseConnFromMap(pConn, iIndex);
	}

	// 回收该结点到可用连接链表
	m_UnuseConns.insert(pConn);

	return tpNext;
}


// 将连接插入map
int CGateCtrl::InsertConnIntoMap(CMyTCPConn* pConn, int iIndex)
{
	if (!pConn || iIndex < 0 || iIndex >= EHandleType_NUM)
	{
		return -1;
	}

	int iKey = MakeConnKey(pConn->GetEntityType(), pConn->GetEntityID());
	pair<CONNS_MAP::iterator, bool> tRet4ConnsMap;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&stMutex[MUTEX_HASHMAP]);
#endif

	// 将连接插入map中
	tRet4ConnsMap = m_mapConns.insert(CONNS_MAP::value_type(iKey, pConn));

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&stMutex[MUTEX_HASHMAP]);
#endif

	if (tRet4ConnsMap.second == false)
	{
		LOG_ERROR("default", "insert conn(key=%d) into m_mapConns failed.", iKey);
		return -2;
	}
	else
	{
		m_stHandleInfos[iIndex].miConnNum++;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&stLinkMutex[iIndex]);
#endif

		// 将结点插入链表
		m_stHandleInfos[iIndex].mLinkerInfo.insert(pConn);

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&stLinkMutex[iIndex]);
#endif

		LOG_INFO("default", "insert conn(key=%d) into m_mapConns succeed.", iKey);
	}

	return 0;
}


// 将连接从map中移除
int CGateCtrl::EraseConnFromMap(CMyTCPConn* pConn, int iIndex)
{
	if (!pConn || iIndex < 0 || iIndex >= EHandleType_NUM)
	{
		return -1;
	}

	int iKey = MakeConnKey(pConn->GetEntityType(), pConn->GetEntityID());

#ifdef _POSIX_MT_
	pthread_mutex_lock(&stMutex[MUTEX_HASHMAP]);
#endif

	m_mapConns.erase(iKey);

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&stMutex[MUTEX_HASHMAP]);
#endif

	LOG_INFO("default", "erase conn(key=%d) from m_mapConns.", iKey);
	// 修改连接计数
	m_stHandleInfos[iIndex].miConnNum--;

	// 将结点从链表中移除
	m_stHandleInfos[iIndex].mLinkerInfo.erase(pConn);

	return 0;
}


int CGateCtrl::CheckRunFlags()
{
	// 暂时不需要支持Reload
	if (g_byRunFlag.CheckRunFlag(ERF_RELOAD))
	{
		g_byRunFlag.SetRunFlag(ERF_RUNTIME);
	}

	return 0;
}

int CGateCtrl::CheckConnRequest()
{
	fd_set fds_read;
	timeval tvListen;
	int i = 0, iTmp;
	int iListenSocketFD = -1;
	int iNewSocketFD = -1;
	struct sockaddr_in stConnAddr;
	TName szConnAddr;
	int iMaxSocketFD = -1;
	
	memset(&stConnAddr, 0, sizeof(stConnAddr));
	socklen_t iAddrLength = sizeof(stConnAddr);

	// 每隔100毫秒超时
	tvListen.tv_sec = 0;
	tvListen.tv_usec = 100000;
	FD_ZERO( &fds_read );  // 清除端口集
	iListenSocketFD = m_stListenSocket.GetSocketFD();

	FD_SET( iListenSocketFD, &fds_read );  // 将listen端口加入端口集
	iMaxSocketFD = iListenSocketFD;

	for (i = 0; i < m_iCurrentUnRegisterNum; i++)
	{
		if (m_astUnRegisterInfo[i].m_iRegisted == 0)
		{
			FD_SET(m_astUnRegisterInfo[i].m_iSocketFD, &fds_read);
			if (m_astUnRegisterInfo[i].m_iSocketFD > iMaxSocketFD)
			{
				iMaxSocketFD = m_astUnRegisterInfo[i].m_iSocketFD;
			}	
		}
	}
	
	// 等待读取
	iTmp = select(iMaxSocketFD + 1, &fds_read, NULL, NULL, &tvListen);

	// 如果没有可读或者出错则返回
	if (iTmp <= 0)
	{
		if (iTmp < 0)
		{
			LOG_ERROR("default", "Select error, %s.", strerror(errno));
		}
		return iTmp;
	}
	
	// 如果iListenSocketFD在fds_read中
	if (FD_ISSET(iListenSocketFD, &fds_read))
	{
		// 接受这个链接
		iNewSocketFD = accept(iListenSocketFD, (struct sockaddr *)&stConnAddr, &iAddrLength);
//		// 性能优化分析
//		// 正式部署的时候可以将此注释打开, 目前测试都是在同一台主机上
//		if(stConnAddr.sin_addr.s_addr  == inet_addr("127.0.0.1"))
//		{
//			// 外网经常收到127.0.0.1的连接，所以直接丢弃这些
//			close(iNewSocketFD);
//		}
//		else
//		{
			SockAddrToString(&stConnAddr, (char *)szConnAddr);
			LOG_INFO("default", "Get a conn request from %s socket fd %d.", szConnAddr, iNewSocketFD);

			if (m_iCurrentUnRegisterNum >= MAX_UNREGISTER_NUM)
			{
				LOG_ERROR("default", "Error there is no empty space(cur num: %d) to record.", m_iCurrentUnRegisterNum);
				close(iNewSocketFD);
			}
			else
			{
				m_astUnRegisterInfo[m_iCurrentUnRegisterNum].m_iSocketFD = iNewSocketFD;
				m_astUnRegisterInfo[m_iCurrentUnRegisterNum].m_ulIPAddr = stConnAddr.sin_addr.s_addr;
				m_astUnRegisterInfo[m_iCurrentUnRegisterNum].m_tAcceptTime = time(NULL);
				m_iCurrentUnRegisterNum++;

				int iOptLen = sizeof(socklen_t);
				int iOptVal = SENDBUFSIZE;
//				if (setsockopt(iNewSocketFD, SOL_SOCKET, SO_SNDBUF, (const void *)&iOptVal, iOptLen))  // 设置发送缓冲区的大小
//				{
//					LOG_ERROR( "default", "Socket(%d) Set send buffer size to %d failed!", iNewSocketFD, iOptVal);
//				}
//				else
				if ( getsockopt( iNewSocketFD, SOL_SOCKET, SO_SNDBUF, (void *)&iOptVal, (socklen_t *)&iOptLen ) == 0 )  // 查看是否设置成功
				{
					LOG_INFO( "default", "Socket(%d) Set Send buf of socket is %d.", iNewSocketFD, iOptVal);
				}
			}
		//}
	}
	
	// 注意这里是从后向前扫描，因为在此过程中
	// 有可能会删除当前元素并把最后一个填充到此处
	for (i = m_iCurrentUnRegisterNum - 1; i >= 0; i--) {
		if (FD_ISSET(m_astUnRegisterInfo[i].m_iSocketFD, &fds_read)) {
			ReceiveAndProcessRegister(i);
		}
	}

//	sleep(0);

	return 1;
}

/********************************************************
  Function:     ReceiveAndProcessRegister
  Description:  接收并处理注册消息
  Input:          iUnRegisterIdx:  未注册结构索引
  Output:      
  Return:       0 :   成功 ，其他失败
  Others:		
********************************************************/
int CGateCtrl::ReceiveAndProcessRegister(int iUnRegisterIdx)
{
	char acTmpBuf[MAX_TMP_BUF_LEN/*1024*/] = {0};
	int iRecvedBytes = 0;
	CMyTCPConn* pAcceptConn = NULL;
	int iAcceptRst = 0;
	
	// 想要索引的id超过了范围
	if (iUnRegisterIdx < 0 || iUnRegisterIdx >= m_iCurrentUnRegisterNum)
	{
		LOG_ERROR("default", "Error in ReceiveAndProcessRegister, Unregister idx(%d) is invalid", iUnRegisterIdx);
		return -1;	
	}

	int iSocketFD = m_astUnRegisterInfo[iUnRegisterIdx].m_iSocketFD;
	u_long ulIPAddr = m_astUnRegisterInfo[iUnRegisterIdx].m_ulIPAddr;

	// 接受数据
	iRecvedBytes = recv(iSocketFD, acTmpBuf, sizeof(acTmpBuf), 0);
	// 如果连接已中断
	if (iRecvedBytes == 0)
	{
		LOG_ERROR("default", "The remote site may closed this conn, fd = %d, errno = %d.", iSocketFD, errno);
		DeleteOneUnRegister(iUnRegisterIdx);
		close(iSocketFD);
		return -1;
	}
	// 如果出错
	if (iRecvedBytes < 0)
	{
		if (errno != EAGAIN)
		{
			LOG_ERROR("default", "Error in read conn, fd = %d, errno = %d.", iSocketFD, errno);
			DeleteOneUnRegister(iUnRegisterIdx);
			close(iSocketFD);
		}
		return -1;
	}

	// 删除相应的索引
	DeleteOneUnRegister(iUnRegisterIdx);

	CTcpHead stTmpHead;
	stTmpHead.Clear();
	unsigned short unOffset = 0;
	int iRet = ClientCommEngine::ConvertStreamToMsg(acTmpBuf, iRecvedBytes, unOffset, &stTmpHead);
	if (iRet < 0)
	{
		LOG_ERROR("default", "[%s : %d : %s] ConvertStreamToMsg return %d.",
				__YQ_FILE__, __LINE__, __FUNCTION__, iRet);
		return -1;
	}

	LOG_DEBUG("default", "Recv(%d) data.", iRecvedBytes);

	// 目标服务器是gate，id不匹配或者不是注册消息，则直接关闭连接
	if ((stTmpHead.dstfe() != FE_GATESERVER)
			|| (stTmpHead.dstid() != CConfigMgr::GetSingletonPtr()->GetGateConfig().id())
			|| (stTmpHead.opflag() != EGC_REGIST))
	{
		LOG_ERROR("default", "Error CCSHead is invalid, fd = %d, Src(FE = %d : ID = %d), Dst(FE = %d : ID = %d), OpFlag = %d, TimeStamp = %ld.",
				iSocketFD, stTmpHead.srcfe(), stTmpHead.srcid(),
				stTmpHead.srcfe(), stTmpHead.dstid(),
				stTmpHead.opflag(), stTmpHead.timestamp());
		close(iSocketFD);
		return -1;
	}


#ifdef _DEBUG_
	LOG_DEBUG("default", "---- Recv Msg ----");
	LOG_DEBUG("default", "[%s]", stTmpHead.ShortDebugString().c_str());
#endif

	// 检查该链接是否已占用，通过类型和ID判断
	int iKey = MakeConnKey(stTmpHead.srcfe(), stTmpHead.srcid());
	pAcceptConn = GetConnByKey(iKey);
	if (pAcceptConn != NULL)
	{
		// 该连接已经存在
		LOG_ERROR("default", "conn(fe=%d : id=%d : key=%d) exist, can't regist again.",
				stTmpHead.srcfe(), stTmpHead.srcid());
		close(iSocketFD);
		return -1;
	}

	// 获取可用连接
	pAcceptConn = GetCanUseConn();
	if (pAcceptConn == NULL)
	{
		// 没有可用连接了
		close(iSocketFD);
		return -1;
	}

	// 初始化连接
	pAcceptConn->Initialize(stTmpHead.srcfe(), stTmpHead.srcid(), ulIPAddr, 0);

	// accept该连接并改为ENTITY_OFF状态
	iAcceptRst = pAcceptConn->EstConn(iSocketFD);
	if (iAcceptRst < 0)
	{
		if (iAcceptRst == -2)
		{
			LOG_ERROR("default", "The server had connected, ignore the conn request (fd: %d).", iSocketFD);
		}
		// 回收连接
		RecycleUnuseConn(pAcceptConn);
		close(iSocketFD);
		return -1;
	}

	LOG_INFO("default", "Conn(FE = %d, ID = %d, KEY = %d) connected OK, the FD is %d.",
		pAcceptConn->GetEntityType(), pAcceptConn->GetEntityID(), iKey, iSocketFD);

	// 分配线程并唤醒
	iAcceptRst = WakeUp2Work(pAcceptConn);
	if (iAcceptRst != 0)
	{
		LOG_ERROR("default", "WakeUp2Work failed, conn(FE = %D, ID = %d, KEY = %d) FD is %d.",
				pAcceptConn->GetEntityType(), pAcceptConn->GetEntityID(), iKey, iSocketFD);
		// 回收连接
		RecycleUnuseConn(pAcceptConn);
		close(iSocketFD);
		return -1;
	}

	return 0;
}

/********************************************************
  Function:     DeleteOneUnRegister
  Description:  删除一个未注册的连接
  Input:          iUnRegisterIdx:  未注册结构索引
  Output:      
  Return:       0 :   成功 ，其他失败
  Others:		
********************************************************/
int CGateCtrl::DeleteOneUnRegister(int iUnRegisterIdx)
{
	if (iUnRegisterIdx < 0 || iUnRegisterIdx >= m_iCurrentUnRegisterNum)
	{
		LOG_ERROR("default", "Error in DeleteOneUnRegister, Unregister idx(%d) is invalid", iUnRegisterIdx);
		return -1;		
	}

	m_iCurrentUnRegisterNum--;
	if ((m_iCurrentUnRegisterNum > 0) && (iUnRegisterIdx < m_iCurrentUnRegisterNum))
	{
		m_astUnRegisterInfo[iUnRegisterIdx] = m_astUnRegisterInfo[m_iCurrentUnRegisterNum];
		m_astUnRegisterInfo[m_iCurrentUnRegisterNum].Clear();
	}

	return 0;
}

/********************************************************
  Function:     CheckRoutines
  Description:  定时检查
  Input:         
  Output:      
  Return:       0 :   成功 ，其他失败
  Others:		
********************************************************/
int CGateCtrl::CheckRoutines() {
	time_t tCurrentTime = time(NULL);

	if (tCurrentTime - m_tLastCheckTime >= CHECK_INTERVAL_SECONDS/*10秒*/)
	{
		m_tLastCheckTime = tCurrentTime;
		for (int i = m_iCurrentUnRegisterNum-1; i >= 0; i--)
		{
			// 连接超时，直接关闭这个注册
			if (tCurrentTime - m_astUnRegisterInfo[i].m_tAcceptTime > CHECK_TIMEOUT_SECONDS)
			{
				int iSocketFD = m_astUnRegisterInfo[i].m_iSocketFD;
				LOG_ERROR("default", "Wait register timeout so close it, fd = %d.", iSocketFD);
				DeleteOneUnRegister(i);
				close(iSocketFD);
			}
		}


		// 检查各handle中是否有已关闭的连接，并回收
		for (int i = 0; i < EHandleType_NUM; i++)
		{
#ifdef _POSIX_MT_
		pthread_mutex_lock(&stLinkMutex[i]);
#endif

			CMyTCPConn* tpConn = (CMyTCPConn*) m_stHandleInfos[i].mLinkerInfo.GetHead();
			while (tpConn != NULL)
			{
				if (tpConn->IsStateOn() == false || (tCurrentTime - tpConn->GetLastKeepalive() >= CHECK_TIMEOUT_SECONDS))
				{
					// 连接已改为关闭状态或者keepalive超时
					tpConn = RecycleUnuseConn(tpConn, i);
				}
				else
				{
					tpConn = (CMyTCPConn*) tpConn->GetNext();
				}
			}

#ifdef _POSIX_MT_
		pthread_mutex_unlock(&stLinkMutex[i]);
#endif
		}
	}


		return 0;
}

int CGateCtrl::PrepareToRun()
{
	int i;
	// 创建主线程的监听socket
	if (m_stListenSocket.CreateServer(CConfigMgr::GetSingletonPtr()->GetGateConfig().port()))
	{
		return -1;
	}

	for (i = 0; i < EHandleType_NUM; i++)
	{
		m_stHandleInfos[i].mpHandle = new CGateHandle;
		// new线程失败，直接退出
		if (!m_stHandleInfos[i].mpHandle)
		{
			return -1;
		}
		// 初始化线程
		m_stHandleInfos[i].mpHandle->Initialize((EMHandleType)i, &(m_stHandleInfos[i].mLinkerInfo), &m_mapConns);
		// 真正的创建线程，失败直接退出
		if (m_stHandleInfos[i].mpHandle->CreateThread())
		{
			return -1;
		}
	}

	return 0;
}

int CGateCtrl::Run()
{

	while (True)
	{
		CheckRunFlags();  // 检查是否设置了flags

		CheckConnRequest();  // 检查是否有连接请求，如果有则连接并处理注册信息

		CheckRoutines();  // 检查这些注册请求是否超时，超时则关闭
	}	//end while

	return 0;
}
