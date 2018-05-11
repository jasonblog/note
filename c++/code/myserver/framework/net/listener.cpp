#include <sstream>
#include "listener.h"
#include <my_assert.h>

CListener::CListener(IEventReactor *pReactor, int listenQueue)
	: m_pEventReactor(pReactor),
	  m_eState(eLS_UnListen),
	  m_pFuncOnAccept(NULL),
	  m_iListenQueueMax(listenQueue)
{

}

CListener::~CListener(void)
{
	ShutDown();
}

bool CListener::RegisterToReactor()
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(m_ListenAddress.GetPort());
	sin.sin_addr.s_addr = inet_addr(m_ListenAddress.GetAddress());
#ifdef EVENT_THREAD_SAFE
	m_pListener = evconnlistener_new_bind(GetReactor()->GetEventBase(),
										  &CListener::lcb_Listen,
										  (void *) this,
										  LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE,
										  m_iListenQueueMax,
										  (struct sockaddr *) &sin,
										  sizeof(sin));
#else
	m_pListener = evconnlistener_new_bind(GetReactor()->GetEventBase(),
										  &CListener::lcb_Listen,
										  (void *) this,
										  LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE /*| LEV_OPT_THREADSAFE */,
										  m_iListenQueueMax,
										  (struct sockaddr *) &sin,
										  sizeof(sin));
#endif
	MY_ASSERT_STR(m_pListener != NULL, exit(0), "Create evconnlistener failed,msg : %s", strerror(errno))
	evconnlistener_set_error_cb(m_pListener, lcb_AcceptError);
	SetState(eLS_Listened);
	return true;
}

void CListener::SetState(eListenerState eState)
{
	m_eState = eState;
}

bool CListener::IsListened()
{
	return eLS_Listened == m_eState;
}

bool CListener::UnRegisterFromReactor()
{
	event_del(&m_event);
	SetState(eLS_UnListen);
	return true;
}

void CListener::ShutDown()
{
	if (IsListened())
		GetReactor()->UnRegister(this);
	if (m_pListener != NULL) {
		evconnlistener_free(m_pListener);
	}
}

IEventReactor *CListener::GetReactor()
{
	return m_pEventReactor;
}

void CListener::lcb_Listen(struct evconnlistener *listener,
						   evutil_socket_t fd,
						   struct sockaddr *sa,
						   int socklen,
						   void *arg)
{
	CListener *pListener = static_cast<CListener *>(arg);
	pListener->HandleInput(fd, sa);
}

void CListener::lcb_AcceptError(struct evconnlistener *listener, void *ctx)
{
	int err = EVUTIL_SOCKET_ERROR();
	MY_ASSERT_STR(false, DO_NOTHING, "Got an error  (%s) on the listener.\n", evutil_socket_error_to_string(err));
}

void CListener::HandleInput(int socket, struct sockaddr *sa)
{
	m_pFuncOnAccept(m_pEventReactor, socket, sa);
}

bool CListener::Listen(CNetAddr &addr, FuncListenerOnAccept pFunc)
{
	m_ListenAddress.SetAddress(addr.GetAddress());
	m_ListenAddress.SetPort(addr.GetPort());
	m_pFuncOnAccept = pFunc;
	GetReactor()->Register(this);
	return true;
}