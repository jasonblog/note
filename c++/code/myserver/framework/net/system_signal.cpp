#include "system_signal.h"

CSystemSignal::CSystemSignal(IEventReactor *pReactor)
	: m_pFuncOnSignal(NULL),
	  m_pContext(NULL),
	  m_pReactor(pReactor),
	  m_bLoop(false)
{
}

CSystemSignal::~CSystemSignal()
{

}

IEventReactor *CSystemSignal::GetReactor()
{
	return m_pReactor;
}

void CSystemSignal::SetCallBackSignal(uint32 uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop /* = false */)
{
	m_pFuncOnSignal = pFunc;
	m_iSignal = uSignal;
	m_pContext = pContext;
	m_bLoop = bLoop;
	GetReactor()->Register(this);
}

bool CSystemSignal::RegisterToReactor()
{
	event_set(&m_event, m_iSignal, EV_SIGNAL, lcb_OnSignal, this);
	event_base_set(static_cast<event_base *>(GetReactor()->GetEventBase()), &m_event);
	event_add(&m_event, NULL);
	return true;
}

void CSystemSignal::lcb_OnSignal(int fd, short event, void *arg)
{
	CSystemSignal *pSignal = static_cast<CSystemSignal *>(arg);
	pSignal->OnSignalReceive();
}

void CSystemSignal::OnSignalReceive()
{
	if (!m_bLoop) {
		UnRegisterFromReactor();
	}

	if (m_iSignal == EVENT_SIGNAL(&m_event)) {
		m_pFuncOnSignal(m_iSignal, m_pContext);
	}
}

bool CSystemSignal::UnRegisterFromReactor()
{
	event_del(&m_event);
	return true;
}

