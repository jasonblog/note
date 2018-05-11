#include "log.h"
#include <my_assert.h>
#include <event2/bufferevent.h>
#include <my_macro.h>
#include "acceptor.h"

CAcceptor::CAcceptor(SOCKET socket,
					 IEventReactor *pReactor,
					 CNetAddr *netAddr,
					 FuncBufferEventOnDataSend funcOnDataSend,
					 FuncBufferEventOnDataRecv funcOnDataRecv,
					 FuncBufferEventOnDisconnected funcDisconnected)
	: IBufferEvent(pReactor,
				   socket,
				   funcOnDataSend,
				   funcOnDataRecv,
				   funcDisconnected),
	  m_pNetAddr(netAddr),
	  m_eState(eAS_Disconnected),
	  m_tCreateTime(GetMSTime()),
	  m_tLastKeepAlive(GetMSTime())
{
}

CAcceptor::~CAcceptor()
{
	ShutDown();
}

void CAcceptor::GetRemoteIpAddress(char *szBuf, unsigned int uBufSize)
{
	MY_ASSERT_STR(uBufSize >= 16, return, "uBufSize is too small");
	CNetAddr addr;
	m_oSocket.GetRemoteAddress(addr);
	strncpy(szBuf, addr.GetAddress(), 16);
}

bool CAcceptor::IsConnected()
{
	return m_eState == eAS_Connected;
}

time_t CAcceptor::GetCreateTime()
{
	return m_tCreateTime;
}

time_t CAcceptor::GetLastKeepAlive()
{
	return m_tLastKeepAlive;
}

void CAcceptor::SetLastKeepAlive(time_t tmpLastKeepAlive)
{
	m_tLastKeepAlive = tmpLastKeepAlive;
}

void CAcceptor::OnEvent(int16 nWhat)
{
	if (nWhat & EVBUFFER_EOF) {
		m_pFuncDisconnected(this);
		return;
	}
	if (nWhat & EVBUFFER_ERROR) {
		m_pFuncDisconnected(this);
	}
	return;
}

void CAcceptor::ShutDown()
{
	if (!IsConnected())
		return;
	GetReactor()->UnRegister(this);
	m_oSocket.Close();
	SetState(eAS_Disconnected);
	SAFE_DELETE(m_pNetAddr);
}

CAcceptor::eAcceptorState CAcceptor::GetState()
{
	return m_eState;
}

void CAcceptor::SetState(eAcceptorState eState)
{
	m_eState = eState;
}

void CAcceptor::BuffEventUnavailableCall()
{
	m_pFuncDisconnected(this);
}

void CAcceptor::AfterBuffEventCreated()
{
	bufferevent_enable(m_pStBufEv, EV_READ | EV_WRITE);
	SetState(eAS_Connected);
}