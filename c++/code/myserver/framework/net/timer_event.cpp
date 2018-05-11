//
// Created by dguco on 18-2-8.
//

#include "timer_event.h"

CTimerEvent::CTimerEvent(IEventReactor *pReactor,
						 FuncOnTimeOut pFuncOnTimerOut,
						 void *param,
						 int sec,
						 int usec,
						 int loopTimes)
	: m_pReactor(pReactor),
	  m_pFuncOnTimerOut(pFuncOnTimerOut),
	  m_pParam(param),
	  m_iSec(sec),
	  m_iUsec(usec),
	  m_iLoopTimes(loopTimes)
{
	GetReactor()->Register(this);
}

CTimerEvent::~CTimerEvent()
{
	UnRegisterFromReactor();
}

void CTimerEvent::LaterCall(int sec, int usec)
{
	struct timeval tv;
	tv.tv_sec = sec;    // 秒
	tv.tv_usec = usec;  // 微秒
	//timer 事件
	evtimer_assign(&m_event, GetReactor()->GetEventBase(), &lcb_TimeOut, (void *) this);
	evtimer_add(&m_event, &tv);
}

void CTimerEvent::ReCall(int sec, int usec)
{
	evtimer_del(&m_event);
	LaterCall(sec, usec);
}

void CTimerEvent::Cancel()
{
	evtimer_del(&m_event);
}

void CTimerEvent::OnTimerOut(int fd, short event)
{
	m_pFuncOnTimerOut(fd, event, m_pParam);
	if (m_iLoopTimes == -1) {
		LaterCall(m_iSec, m_iUsec);
		return;
	}
	m_iLoopTimes--;
	if (m_iLoopTimes > 1) {
		LaterCall(m_iSec, m_iUsec);
		m_iLoopTimes--;
	}
}

void CTimerEvent::lcb_TimeOut(int fd, short event, void *arg)
{
	CTimerEvent *pTimer = static_cast<CTimerEvent *>(arg);
	pTimer->OnTimerOut(fd, event);
}

bool CTimerEvent::RegisterToReactor()
{
	LaterCall(m_iSec, m_iUsec);
	return true;
}

bool CTimerEvent::UnRegisterFromReactor()
{
	Cancel();
	GetReactor()->UnRegister(this);
}

IEventReactor *CTimerEvent::GetReactor()
{
	return m_pReactor;
}




