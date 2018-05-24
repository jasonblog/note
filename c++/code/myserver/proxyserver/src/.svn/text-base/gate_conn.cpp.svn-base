#include "gate_conn.h"

int CMyTCPConn::EstConn(int iAcceptFD) {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	iTempRet = GetSocket()->Accept(iAcceptFD);

	m_iConnState = ENTITY_OFF;

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return iTempRet;
}

int CMyTCPConn::IsConnCanRecv() {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif
	if (GetSocket()->GetSocketFD() > 0 && GetSocket()->GetStatus() == tcs_connected) {
		iTempRet = True;
	} else {
		iTempRet = False;
	}
#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return iTempRet;
}

int CMyTCPConn::RegToCheckSet(fd_set *pCheckSet) {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	iTempRet = GetSocket()->AddToCheckSet(pCheckSet);

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return iTempRet;
}

int CMyTCPConn::IsFDSetted(fd_set *pCheckSet) {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif
	// 查看该套接字的FD是否在套接字集合中
	iTempRet = GetSocket()->IsFDSetted(pCheckSet);

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return iTempRet;
}

int CMyTCPConn::RecvAllData() {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	iTempRet = GetSocket()->RecvData();

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return iTempRet;
}

int CMyTCPConn::GetOneCode(short &nCodeLength, BYTE *pCode) {
	return GetSocket()->GetOneCode((unsigned short int &)nCodeLength, pCode);
}

int CMyTCPConn::SendCode(short nCodeLength, BYTE *pCode, int iFlag /* = FLG_CONN_IGNORE */) {
	int iTempRet = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	if (GetEntityType() < 0 || GetEntityID() < 0) {
		// 未配置节点直接返回 
#ifdef _POSIX_MT_
		pthread_mutex_unlock(&m_stMutex);
#endif
		return -101;
	}

	if (iFlag == FLG_CONN_CTRL || iFlag == FLG_CONN_IGNORE) {
		// 外带数据和只读操作优先发送, 不参与排队
		if (GetSocket()->GetSocketFD() > 0 && GetSocket()->GetStatus() == tcs_connected) {
			iTempRet = GetSocket()->SendOneCode(nCodeLength, pCode);
		} else {
			iTempRet = -102;
		}

#ifdef _POSIX_MT_
		pthread_mutex_unlock(&m_stMutex);
#endif
		return iTempRet;
	}

	return iTempRet;
}

int CMyTCPConn::CleanBlockQueue(int iQueueLength) {
	int iRedoCount = 0, i, iTempRet = 0;
	int nCodeLength;
	BYTE  abyCodeBuf[MAX_PACKAGE_LEN];

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	if (m_iConnState == ENTITY_ON && GetSocket()->GetSocketFD() > 0 &&
			GetSocket()->GetStatus() == tcs_connected) {
		// 如果有滞留数据，则重新发送，知道发送完成或出错
		if (GetSocket()->HasReserveData() == True) {
			LOG_DEBUG("default", "CleanReserveData begin.");
			iTempRet = GetSocket()->CleanReserveData();
			if (iTempRet != 0)
			{
#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif
				LOG_ERROR("default", "CleanReserveData failed. iTempRet = %d.", iTempRet);
				return 0;
			}
			else
			{
#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif
				return 1;
			}
		}
	}
	/*
	// 如果连接没有问题但是没有发出数据，则组织队列重试
	if (m_iConnState == ENTITY_ON && GetSocket()->GetSocketFD() > 0 && 
			GetSocket()->GetStatus() == tcs_connected && m_iBlockStatus != BLK_EMPTY && 
			GetSocket()->CleanReserveData() == 0 ) {
		LOG_DEBUG("default", "Now begin to redo transfer, will recover no more than %d codes.", iQueueLength);
		// 如果重试队列尚未建立，则建立它
		if (m_pRedoQueue == NULL) {
			m_pRedoQueue = new CCodeQueue(BLOCKQUEUESIZE);
			LOG_DEBUG("default", "Redo queue created now.");
		}
		if (m_pRedoQueue) {
			for (i = 0; i < iQueueLength; i++) {
				// 没有剩下的数据，则退出循环
				if (GetSocket()->HasReserveData()) {
					LOG_ERROR("default", "Redo process is stopped because of data block.");
					break;
				}

				m_pRedoQueue->GetHeadCode((BYTE *)&abyCodeBuf, &nCodeLength);
				// 重试队列为空
				if (nCodeLength == 0) {
					LOG_DEBUG("default", "The redo queue is empty now.");
					if (m_iCurrentRedoSeq < m_iCurrentDumpSeq) {
						TFName szDumpFile;

						snprintf(szDumpFile, sizeof(szDumpFile)-1, "CODE%02d_%02d_%02d.tmp", GetEntityType(), GetEntityID(), m_iCurrentRedoSeq);
						LOG_DEBUG("default", "Load redo queue from file %s.", szDumpFile);
						m_pRedoQueue->LoadFromFile(szDumpFile);  // 将重试队列保存在文件中，并且重新加载该文件，来执行下一次重试
						m_iCurrentRedoSeq++;
						i--;
						unlink((const char *)szDumpFile);
					} else if (m_pRedoQueue != m_pBlockQueue) {
						delete m_pRedoQueue;
						m_pRedoQueue = m_pBlockQueue;
						i--;
						LOG_DEBUG("default", "Now begin to redo current block queue."); // 执行上一次被Block的队列 
					} else {
						delete m_pRedoQueue;
						m_pRedoQueue = m_pBlockQueue = NULL;
						m_iBlockStatus = BLK_EMPTY;
						m_iCurrentRedoSeq = m_iCurrentDumpSeq = 0;
						LOG_DEBUG("default", "Now all block queue is cleaned.");  // RedoQueue和BlockQueue均为空，重试结束
						break;
					}
				} else { // 如果重试队列不为空，则尝试发送一个Code，如果发送失败则退出
					iTempRet = GetSocket()->SendOneCode(nCodeLength, abyCodeBuf);
					if (iTempRet == 0) {
						iRedoCount++;
						LOG_DEBUG("default", "Redo one code OK.");
					} else {
						LOG_ERROR("default", "Resend one code failed of %d.", iTempRet);
						break;
					}
				}	// end if code length == 0
			}	// end for
		} // end if(pRedoQueue)
	}
	*/
#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	//return iRedoCount;
	return 0;
}

int CMyTCPConn::SetConnState(int iConnState) {
#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	m_iConnState = iConnState;

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return 0;
}

bool CMyTCPConn::IsStateOn()
{
	bool bOn = false;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	bOn = (m_iConnState == ENTITY_ON);

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return bOn;
}

int CMyTCPConn::SetLastKeepalive(time_t tNow)
{
#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	m_tLastKeepalive = tNow;

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return 0;
}

time_t CMyTCPConn::GetLastKeepalive()
{
	time_t tTime = 0;

#ifdef _POSIX_MT_
	pthread_mutex_lock(&m_stMutex);
#endif

	tTime = m_tLastKeepalive;

#ifdef _POSIX_MT_
	pthread_mutex_unlock(&m_stMutex);
#endif

	return tTime;
}

