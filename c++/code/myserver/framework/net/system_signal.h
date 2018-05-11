//
//  timer_signal.h
//  Created by DGuco on 18-1-7.
//  信号和timer管理
//

#ifndef _PPE_SIGNAL_H_
#define _PPE_SIGNAL_H_

#include <event.h>
#include "network_interface.h"

class CSystemSignal: public IReactorHandler
{
public:
	//构造函数
	explicit CSystemSignal(IEventReactor *pReactor);
	//析构函数
	~CSystemSignal() override;
	//设置信号回调
	void SetCallBackSignal(uint32 uSignal, FuncOnSignal pFunc, void *pContext, bool bLoop = false);
private:
	//注册
	bool RegisterToReactor() override;
	//卸载
	bool UnRegisterFromReactor() override;
	//获取event_base
	IEventReactor *GetReactor() override;
	//收到信号
	void OnSignalReceive();
	//信号回调
	static void lcb_OnSignal(int fd, short event, void *arg);
private:
	FuncOnSignal m_pFuncOnSignal;
	void *m_pContext;
	IEventReactor *m_pReactor;
	bool m_bLoop;
	event m_event;
	int m_iSignal;
};

#endif
