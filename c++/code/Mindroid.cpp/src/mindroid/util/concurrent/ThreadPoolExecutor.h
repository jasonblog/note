/*
 * Copyright (C) 2011 Daniel Himmelein
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDROID_THREADPOOLEXECUTOR_H_
#define MINDROID_THREADPOOLEXECUTOR_H_

#include "mindroid/lang/Thread.h"
#include "mindroid/util/concurrent/Executor.h"
#include "mindroid/util/concurrent/LinkedBlockingQueue.h"

namespace mindroid {

class ThreadPoolExecutor :
        public Executor {
public:
    ThreadPoolExecutor(uint32_t threadPoolSize);
    virtual ~ThreadPoolExecutor();
    ThreadPoolExecutor(const ThreadPoolExecutor&) = delete;
    ThreadPoolExecutor& operator=(const ThreadPoolExecutor&) = delete;

    virtual void execute(const sp<Runnable>& runnable);
    virtual bool cancel(const sp<Runnable>& runnable);

private:
    class WorkerThread : public Thread {
    public:
        WorkerThread(const sp<String>& name) :
                Thread(name),
                mQueue(nullptr) {
        }
        virtual ~WorkerThread() = default;
        virtual void run();

    private:
        void setQueue(const sp<LinkedBlockingQueue<sp<Runnable>>>& queue);

        sp<LinkedBlockingQueue<sp<Runnable>>> mQueue;

        friend class ThreadPoolExecutor;
    };

    void start();
    void shutdown();

    const uint32_t THREAD_POOL_SIZE;
    sp<WorkerThread>* mWorkerThreads;
    sp<LinkedBlockingQueue<sp<Runnable>>> mQueue;
};

} /* namespace mindroid */

#endif /* MINDROID_THREADPOOLEXECUTOR_H_ */
