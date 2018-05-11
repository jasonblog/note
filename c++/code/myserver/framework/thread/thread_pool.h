//
// mythread.h
// 基于c++14简单的线程池
// Created by DGuco on 17-10-12.
// Copyright © 2016年 DGuco. All rights reserved.
//


#ifndef SERVER_THREADPOOL_H
#define SERVER_THREADPOOL_H

#include <deque>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <bits/unordered_map.h>

using namespace std;

/**
 * 成员函数生命为ininle，避免multiple definition error
 */
class CThreadPool
{
public:
	CThreadPool();
	CThreadPool(size_t);
	~CThreadPool();
	CThreadPool(const CThreadPool &) = delete;
	const CThreadPool &operator=(CThreadPool &) = delete;
	//向后添加任务
	template<class F, class... Args>
	auto PushTaskBack(F &&f, Args &&... args);
	//向前添加任务
	template<class F, class... Args>
	auto PushTaskFront(F &&f, Args &&... args);
	bool IsInThisThread();
	bool IsThisThreadIn(thread *thrd);

private:
	void ThreadFunc();
private:
	std::unordered_map<std::thread::id, std::thread *> m_mWorkers;
	std::deque<std::function<void()> > m_qTasks;

	std::mutex m_mutex;
	std::condition_variable m_condition;
	bool m_stop;
};

/*
 * inline成员函数定义和生命在同一个源文件中
 */
inline CThreadPool::CThreadPool()
	: CThreadPool(thread::hardware_concurrency())
{

}

/*
 * inline成员函数定义和生命在同一个源文件中
 */
inline CThreadPool::CThreadPool(size_t threads)
	: m_stop(false)
{
	for (size_t i = 0; i < threads; ++i) {
		thread *th = new thread(std::mem_fn(&CThreadPool::ThreadFunc), this);
		if (th) {
			m_mWorkers[th->get_id()] = th;
		}
	}
}

/*
 * inline成员函数定义和生命在同一个源文件中
 */
inline CThreadPool::~CThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_stop = true;
		m_condition.notify_all();
	}
	for (auto it = m_mWorkers.begin(); it != m_mWorkers.end();) {
		it->second->join();
		delete it->second;
		it = m_mWorkers.erase(it);
	}
}

/*
 * inline成员函数定义和生命在同一个源文件中
 */
inline bool CThreadPool::IsInThisThread()
{
	auto it = m_mWorkers.find(this_thread::get_id());
	return it != m_mWorkers.end();
}

/*
 * inline成员函数定义和生命在同一个源文件中
 */
inline bool CThreadPool::IsThisThreadIn(thread *thrd)
{
	if (thrd) {
		auto it = m_mWorkers.find(thrd->get_id());
		return it != m_mWorkers.end();
	}
	else {
		return false;
	}
}

/*
 * inline成员函数定义和生命在同一个源文件中
 */
inline void CThreadPool::ThreadFunc()
{
	while (true) {
		std::function<void()> task;
		std::unique_lock<std::mutex> lock(this->m_mutex);
		this->m_condition.wait(lock,
							   [this]
							   { return (this->m_stop || !this->m_qTasks.empty()); });
		//如果停止线程
		if (this->m_stop) {
			return;
		}
		//如果唤醒时队列任务为空，则认为是意外唤醒继续阻塞
		if (this->m_qTasks.empty()) {
			continue;
		}
		task = std::move(this->m_qTasks.front());
		this->m_qTasks.pop_front();
		lock.unlock();
		task();
	}
}

/*
 * 和函数声明放在统一个源文件中，避免 use of ‘auto...’ before deduction of ‘auto’ .. error
 */
template<class F, class... Args>
auto CThreadPool::PushTaskBack(F &&f, Args &&... args)
{
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared<std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
	);

	if (task == NULL) {
		throw std::runtime_error("Create task error ");
	}

	std::future<return_type> res = task->get_future();

	//如果在当前线程内，则直接执行
	if (IsInThisThread()) {
		(*task)();
	}
	else {
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");
		m_qTasks.emplace_back([task]()
							  { (*task)(); });
		m_condition.notify_one();
	}
	return res;
}

/*
 * 和函数声明放在统一个源文件中，避免 use of ‘auto...’ before deduction of ‘auto’ .. error
 */
template<class F, class... Args>
auto CThreadPool::PushTaskFront(F &&f, Args &&... args)
{
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared<std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
	);

	if (task == NULL) {
		throw std::runtime_error("Create task error ");
	}

	std::future<return_type> res = task->get_future();
	//如果在当前线程内，则直接执行
	if (IsInThisThread()) {
		(*task)();
	}
	else {
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_stop)
			throw std::runtime_error("ThreadPool has stopped");
		else {
			m_qTasks.emplace_front([task]()
								   { (*task)(); });
			m_condition.notify_one();
		}
	}
	return res;
}

#endif //SERVER_THREADPOOL_H
