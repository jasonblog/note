#ifndef THREADPOOL_H_
#define THREADPOOL_H_
#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>

#include "14-2-locker.h"

//线程池类， 将它定义为模板类是为了代码复用。模板参数T是任务类

template< typename T>
class threadpool
{
public:
    //参数thread_number是线程中线程的数量，max_process是请求队列中最多允许的、等待处理的请求的数量
    threadpool(int thread_number = 8, int max_requests = 10000);
    ~threadpool();

    //往请求队列中添加任务
    bool append(T *request);
private:
    //工作线程运行的函数，它不断从工作队列中取出任务并执行之
    static void *worker(void *arg);
    void run();

private:
    int m_thread_number;  //
    int m_max_requests;
    pthread_t *m_threads;  //array of threadpool, size is m_thread_nubmer
    std::list<T*> m_workqueue; //request queue
    locker m_queuelocker;  //
    sem m_queuestat;    //stat of need to process request
    bool m_stop;       //flag of thread to stop
};

template<typename T>
threadpool<T>::threadpool(int thread_number, int max_requests):
    m_thread_number(thread_number), m_max_requests(max_requests),m_stop(false),m_threads(NULL)
{
    if ((thread_number <= 0) || (max_requests <= 0)) 
    {
        throw std::exception();
    }

    m_threads = new pthread_t[ m_thread_number];
    if (!m_threads) 
    {
        throw std::exception();
    }

    //
    for(int i  = 0; i  < thread_number; i ++) 
    {
        fprintf(stdout, "create the %dth thread\n", i);
        if (pthread_create(m_threads + i, NULL, worker, this) != 0) 
        {
            delete[] m_threads;
            throw std::exception();
        }

        if (pthread_detach(m_threads[i])) 
        {
            delete [] m_threads;
            throw std::exception();
        }
    }
}


template<typename T>
threadpool<T>:: ~threadpool()
{
    delete [] m_threads;
    m_stop = true;
}

template<typename T> 
bool threadpool< T> ::append(T* request)
{
    //
    m_queuelocker.lock();
    if (m_workqueue.size() > m_max_requests) 
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template<typename T>
void *threadpool<T>::worker(void *arg)
{
    threadpool *pool = (threadpool *)arg;
    pool->run();
    return pool;
}

template<typename T>
void threadpool<T>::run()
{
    while(!m_stop)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if (m_workqueue.empty()) 
        {
            m_queuelocker.unlock();
            continue;
        }

        T *request = m_workqueue.front();
        m_workqueue.pop_front();

        m_queuelocker.unlock();
        if (!request) 
        {
            continue;
        }
        request->process();
    }
}
#endif
