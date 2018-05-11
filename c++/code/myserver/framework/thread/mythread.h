//
// mythread.h
// 线程基类
// Created by DGuco on 17-10-12.
// Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _MY_THREAD_HPP_
#define _MY_THREAD_HPP_

#include <pthread.h>
#include <condition_variable>
#include <mutex>
#include <thread>

enum eRunStatus
{
	rt_init = 0,
	rt_blocked = 1,
	rt_running = 2,
	rt_stopped = 3
};

class CMyThread
{
public:
	CMyThread(const std::string &threadName,
			  long timeOut = 0);
	virtual ~CMyThread();

	virtual int PrepareToRun() = 0;
	virtual void RunFunc() = 0;
	virtual bool IsToBeBlocked() = 0;

	int Run();
	int WakeUp();
	int StopThread();
	void StopForce();
	void Join();
	int GetStatus();
protected:
	void ThreadFunc();
private:
	int m_iRunStatus;
	std::mutex m_condMut;
	std::condition_variable data_cond;
	std::thread *m_pThread;
	std::string m_sThreadName;
	long m_lTimeOut; //阻塞超时时间(微妙)，默认没有超时（如果不wake up永远阻塞）
};


#endif
