#include <my_assert.h>
#include "event_reactor.h"
#ifdef EVENT_THREAD_SAFE
#include <event2/thread.h>
#endif


CEventReactor::CEventReactor()
	: m_uReactorHandlerCounter(0),
	  m_pEventBase(NULL)
{
	Init();
}

CEventReactor::~CEventReactor()
{
	Release();
}

void CEventReactor::Init()
{
	m_pConfig = event_config_new();
	event_config_require_features(m_pConfig, EV_FEATURE_O1);
#ifdef EVENT_THREAD_SAFE
	evthread_use_pthreads();
#else
	event_config_set_flag(m_pConfig, EVENT_BASE_FLAG_NOLOCK);
#endif
	m_pEventBase = event_base_new_with_config(m_pConfig);
	MY_ASSERT_STR(NULL != m_pEventBase, exit(0), "Create Event Base error Init error");
}

event_base *CEventReactor::GetEventBase()
{
	return m_pEventBase;
}

void CEventReactor::Release()
{
	event_base_loopexit(m_pEventBase, NULL);
	event_base_free(m_pEventBase);
	event_config_free(m_pConfig);
}

void CEventReactor::DispatchEvents()
{
	event_base_dispatch(m_pEventBase);
	MY_ASSERT_STR(false, return, "Event loop exited..");
}

bool CEventReactor::Register(IReactorHandler *pHandler)
{
	m_uReactorHandlerCounter++;
	pHandler->RegisterToReactor();
	return true;
}

bool CEventReactor::UnRegister(IReactorHandler *pHandler)
{
	m_uReactorHandlerCounter--;
	pHandler->UnRegisterFromReactor();
	return true;
}