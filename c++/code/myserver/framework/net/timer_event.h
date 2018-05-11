//
// Created by dguco on 18-2-8.
// 基于libevet的timer事件
//

#ifndef SERVER_SYS_EVENT_H
#define SERVER_SYS_EVENT_H

#include <event.h>
#include "network_interface.h"

class CTimerEvent: public IReactorHandler
{
public:
	//构造函数
	CTimerEvent(IEventReactor *pReactor,
				FuncOnTimeOut m_pFuncOnTimerOut,
				void *param,
				int sec,
				int usec,
				int loopTimes/*-1 永久 > 0 次数*/);
	//析构函数
	~CTimerEvent();
	//超时处理
	void OnTimerOut(int fd, short event);
	void LaterCall(int sec, int usec);
	void ReCall(int sec, int usec);
	//停止
	void Cancel();
private:
	//注册
	bool RegisterToReactor();
	//卸载
	bool UnRegisterFromReactor();
	//获取event_base
	IEventReactor *GetReactor();
private:
	//超时回调
	static void lcb_TimeOut(int fd, short event, void *arg);
private:
	IEventReactor *m_pReactor;
	FuncOnTimeOut m_pFuncOnTimerOut;
	void *m_pParam; //超时回调参数l
	int m_iSec;  //秒
	int m_iUsec; //微妙
	int m_iLoopTimes; //循环次数
	event m_event;
};

#endif //SERVER_TIMER_EVENT_H
