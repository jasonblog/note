//
// Created by dguco on 18-1-30.
//

#include <share_mem.h>
#include <acceptor.h>
#include <client_comm_engine.h>
#include <my_macro.h>
#include "net_work.h"
#include "../inc/s2c_handle.h"
#include "../inc/gate_ctrl.h"

CCodeQueue *CS2cHandle::m_pS2CPipe = NULL;

CS2cHandle::CS2cHandle()
	: CMyThread("CS2cHandle", 1000) //超时时间1000微妙1ms
{
}

CS2cHandle::~CS2cHandle()
{

}

int CS2cHandle::PrepareToRun()
{
	return 0;
}

void CS2cHandle::RunFunc()
{
	CheckWaitSendData();
}

bool CS2cHandle::IsToBeBlocked()
{
	return m_pS2CPipe->IsQueueEmpty();
}

void CS2cHandle::CheckWaitSendData()
{
	int iTmpRet = 0;
	int unTmpCodeLength = 0;

	std::shared_ptr<CByteBuff> tmpBuff(new CByteBuff);
	iTmpRet = RecvServerData(tmpBuff->CanWriteData());
	if (iTmpRet == 0) {
		return;
	}

	CMessage tmpMes;
	iTmpRet = CClientCommEngine::ConvertStreamToMessage(tmpBuff.get(),
														unTmpCodeLength,
														&tmpMes,
														NULL);
	//序列化失败继续发送
	if (iTmpRet < 0) {
		LOG_ERROR("default",
				  "CTCPCtrl::CheckWaitSendData Error, ClientCommEngine::ConvertMsgToStream return {}.",
				  iTmpRet);
		return;
	}

	CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()->PushTaskBack(
		std::mem_fn(&CS2cHandle::SendClientData), this, tmpMes, tmpBuff);

}

int CS2cHandle::SendClientData(CMessage &tmpMes, std::shared_ptr<CByteBuff> tmpBuff)
{
	int nTmpSocket;
	auto tmpSendList = tmpMes.msghead().socketinfos();
	int tmpDataLen = tmpBuff->ReadableDataLen();
	const char *data = tmpBuff->CanReadData();
	for (int i = 0; i < tmpSendList.size(); ++i) {
		//向后移动socket索引
		CSocketInfo tmpSocketInfo = tmpSendList.Get(i);
		nTmpSocket = tmpSocketInfo.socketid();
		//socket 非法
		if (nTmpSocket <= 0 || MAX_SOCKET_NUM <= nTmpSocket) {
			LOG_ERROR("default", "Invalid socket index {}", nTmpSocket);
			continue;
		}
		CGateCtrl::GetSingletonPtr()->GetSingleThreadPool()->PushTaskBack(
			std::mem_fn(&CS2cHandle::SendToClient), this, tmpSocketInfo, data, tmpDataLen);
	}
	return 0;
}

int CS2cHandle::RecvServerData(char *data)
{
	int unTmpCodeLength = MAX_PACKAGE_LEN;
	if (m_pS2CPipe->GetHeadCode((BYTE *) data, unTmpCodeLength) < 0) {
		unTmpCodeLength = 0;
	}
	return unTmpCodeLength;
}

void CS2cHandle::SendToClient(const CSocketInfo &socketInfo, const char *data, unsigned int len)
{
	CAcceptor *pAcceptor = CNetWork::GetSingletonPtr()->FindAcceptor(socketInfo.socketid());
	if (pAcceptor == NULL) {
		LOG_ERROR("default", "CAcceptor has gone, socket = {}", socketInfo.socketid());
		return;
	}

	/*
	 * 时间不一样，说明这个socket是个新的连接，原来的连接已经关闭,注(原来的
	 * 的连接断开后，新的客户端用了原来的socket fd ，因此数据不是现在这个连
	 * 接的数据，原来连接的数据,中断发送
	*/
	if (pAcceptor->GetCreateTime() != socketInfo.createtime()) {
		LOG_ERROR("default",
				  "sokcet[{}] already closed(tcp createtime:{}:gate createtime:{}) : gate ==> client failed",
				  socketInfo.socketid(),
				  pAcceptor->GetCreateTime(),
				  socketInfo.createtime());
		return;
	}
	int iTmpCloseFlag = socketInfo.state();
	int iRet = pAcceptor->Send(data, len);
	if (iRet != 0) {
		//发送失败
		CC2sHandle::ClearSocket(pAcceptor, Err_ClientClose);
		LOG_ERROR("default",
				  "send to client {} Failed due to error {}",
				  pAcceptor->GetSocket().GetSocket(),
				  errno);
		return;
	}

	//gameserver 主动关闭
	if (iTmpCloseFlag < 0) {
		CC2sHandle::ClearSocket(pAcceptor, Client_Succeed);
	}
}
