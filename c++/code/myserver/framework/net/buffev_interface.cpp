//
// Created by dguco on 18-1-27.
//

#include <my_assert.h>
#include "buffev_interface.h"
#include "byte_buff.h"

IBufferEvent::IBufferEvent(IEventReactor *pReactor,
						   int socket,
						   FuncBufferEventOnDataSend funcOnDataSend,
						   FuncBufferEventOnDataRecv funcOnDataRecv,
						   FuncBufferEventOnDisconnected funcDisconnected)
	: m_pReactor(pReactor),
	  m_pStBufEv(NULL),
	  m_uMaxOutBufferSize(MAX_PACKAGE_LEN),
	  m_uMaxInBufferSize(MAX_PACKAGE_LEN),
	  m_uRecvPackLen(0),
	  m_pFuncOnDataSend(funcOnDataSend),
	  m_pFuncOnDataRecv(funcOnDataRecv),
	  m_pFuncDisconnected(funcDisconnected)
{
	m_oSocket.SetSocket(socket);
}

IBufferEvent::~IBufferEvent()
{
	if (m_pStBufEv != NULL) {
		bufferevent_free(m_pStBufEv);
	}
}

int IBufferEvent::Send(const void *pData, unsigned int uSize)
{
	if (uSize > m_uMaxOutBufferSize) {
		return ePR_OutPipeBuf;
	}

	MY_ASSERT(IsEventBuffAvailable(), return ePR_BufNull);
	bufferevent_write(m_pStBufEv, pData, uSize);
}

int IBufferEvent::SendBySocket(const void *pData, unsigned int uSize)
{
	int iSendBytes;
	while (true) {
		iSendBytes = write(m_oSocket.GetSocket(), pData, uSize);
		if (iSendBytes == uSize) {
			return iSendBytes;
		}
		else {
			if (0 >= iSendBytes && EINTR == errno) {
				continue;
			}
			return iSendBytes;
		}
	}
}

unsigned int IBufferEvent::RecvData(void *data, unsigned int size)
{
	MY_ASSERT(IsEventBuffAvailable(), return 0);
	return bufferevent_read(m_pStBufEv, data, size);
}

unsigned short IBufferEvent::ReadRecvPackLen()
{
	//包头前两个字节为数据总长度，如果数据长度小于两个字节返回0
	if (GetRecvDataSize() < sizeof(unsigned short)) {
		return 0;
	}
	CByteBuff tmpBuff(sizeof(unsigned short));
	RecvData((void *) (tmpBuff.GetData()), sizeof(unsigned short));
	return tmpBuff.ReadUnShort();
}

unsigned int IBufferEvent::GetRecvDataSize()
{
	MY_ASSERT(IsEventBuffAvailable(), return 0);
	struct evbuffer *in = bufferevent_get_input(m_pStBufEv);
	MY_ASSERT(in != NULL, return 0);
	return evbuffer_get_length(in);
}

unsigned int IBufferEvent::GetSendDataSize()
{
	MY_ASSERT(IsEventBuffAvailable(), return 0);
	struct evbuffer *out = bufferevent_get_output(m_pStBufEv);
	MY_ASSERT(out != NULL, return 0);
	return evbuffer_get_length(out);
}

void IBufferEvent::SetMaxSendBufSize(unsigned int uSize)
{
	MY_ASSERT(IsEventBuffAvailable() && uSize > 0, return;);
	m_uMaxOutBufferSize = uSize;
	bufferevent_setwatermark(m_pStBufEv, EV_WRITE, 0, m_uMaxOutBufferSize);
}

unsigned int IBufferEvent::GetMaxSendBufSize()
{
	return m_uMaxOutBufferSize;
}

void IBufferEvent::SetMaxRecvBufSize(unsigned int uSize)
{
	MY_ASSERT(IsEventBuffAvailable() && uSize > 0, return;);
	m_uMaxInBufferSize = uSize;
	bufferevent_setwatermark(m_pStBufEv, EV_READ, 0, m_uMaxInBufferSize);
}

bool IBufferEvent::IsEventBuffAvailable()
{
	if (m_pStBufEv == NULL) {
		BuffEventUnavailableCall();
		return false;
	}
	return true;
}

unsigned short IBufferEvent::GetRecvPackLen() const
{
	return m_uRecvPackLen;
}

void IBufferEvent::CurrentPackRecved()
{
	m_uRecvPackLen = 0;
}

const CSocket &IBufferEvent::GetSocket() const
{
	return m_oSocket;
}

bool IBufferEvent::IsPackageComplete()
{
	unsigned short tmpPackLen = GetRecvPackLen();
	//如果当前包长度为0，则为新的数据包，重新读取数据包总长度保存
	if (tmpPackLen <= 0) {
		tmpPackLen = ReadRecvPackLen();
	}
	if (tmpPackLen <= 0) {
		//数据包不完整继续等其他数据到来
		return false;
	}

	unsigned short tmpLastLen = tmpPackLen - sizeof(unsigned short);
	unsigned int tmpDataLen = GetRecvDataSize();
	//数据包不完整继续接收
	return tmpDataLen >= tmpLastLen;
}

void IBufferEvent::lcb_OnRead(struct bufferevent *bev, void *arg)
{
	IBufferEvent *pEventBuff = (IBufferEvent *) (arg);
	if (pEventBuff != NULL) {
		pEventBuff->m_pFuncOnDataRecv(pEventBuff);
	}
}

void IBufferEvent::lcb_OnWrite(bufferevent *bev, void *arg)
{
	IBufferEvent *pEventBuff = (IBufferEvent *) (arg);
	if (pEventBuff != NULL) {
		pEventBuff->m_pFuncOnDataSend(pEventBuff);
	}
}

void IBufferEvent::lcb_OnEvent(bufferevent *bev, int16 nWhat, void *arg)
{
	IBufferEvent *pEventBuff = (IBufferEvent *) (arg);
	if (pEventBuff != NULL) {
		pEventBuff->OnEvent(nWhat);
	}
}

unsigned int IBufferEvent::GetMaxRecvBufSize()
{
	return m_uMaxInBufferSize;
}

bool IBufferEvent::RegisterToReactor()
{
#ifdef EVENT_THREAD_SAFE
	m_pStBufEv = bufferevent_socket_new(GetReactor()->GetEventBase(),
										m_oSocket.GetSocket(),
										BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
#else
	m_pStBufEv = bufferevent_socket_new(GetReactor()->GetEventBase(),
										m_oSocket.GetSocket(),
										BEV_OPT_CLOSE_ON_FREE /*| BEV_OPT_THREADSAFE */);
#endif
	MY_ASSERT_STR(NULL != m_pStBufEv, return false, "BufferEvent new failed!,error msg: %s", strerror(errno));
	bufferevent_setcb(m_pStBufEv,
					  &IBufferEvent::lcb_OnRead,
					  NULL,
					  &IBufferEvent::lcb_OnEvent,
					  (void *) this);
	AfterBuffEventCreated();
//	bufferevent_setwatermark(m_pStBufEv, EV_READ, 0, m_uMaxInBufferSize);
//	bufferevent_setwatermark(m_pStBufEv, EV_WRITE, 0, m_uMaxOutBufferSize);
	return true;
}

IEventReactor *IBufferEvent::GetReactor()
{
	return m_pReactor;
}

bool IBufferEvent::UnRegisterFromReactor()
{
	bufferevent_disable(m_pStBufEv, EV_READ | EV_WRITE);
	return true;
}
