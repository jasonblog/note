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

#include "mindroid/util/concurrent/ThreadPoolExecutor.h"

namespace mindroid {

ThreadPoolExecutor::ThreadPoolExecutor(uint32_t threadPoolSize) :
        THREAD_POOL_SIZE(threadPoolSize),
        mWorkerThreads(nullptr),
        mQueue(new LinkedBlockingQueue<sp<Runnable>>()) {
    start();
}

ThreadPoolExecutor::~ThreadPoolExecutor() {
    shutdown();
}

void ThreadPoolExecutor::start() {
    if (mWorkerThreads == nullptr) {
        mWorkerThreads = new sp<WorkerThread>[THREAD_POOL_SIZE];
        for (uint32_t i = 0; i < THREAD_POOL_SIZE; i++) {
            mWorkerThreads[i] = new WorkerThread(String::valueOf("ThreadPoolExecutor[Worker i]"));
            mWorkerThreads[i]->setQueue(mQueue);
            mWorkerThreads[i]->start();
        }
    }
}

void ThreadPoolExecutor::shutdown() {
    if (mWorkerThreads != nullptr) {
        for (uint32_t i = 0; i < THREAD_POOL_SIZE; i++) {
            mWorkerThreads[i]->interrupt();
            mQueue->put(nullptr);
        }
        for (uint32_t i = 0; i < THREAD_POOL_SIZE; i++) {
            mWorkerThreads[i]->join();
            mWorkerThreads[i] = nullptr;
        }
        delete[] mWorkerThreads;
        mWorkerThreads = nullptr;
    }
}

void ThreadPoolExecutor::WorkerThread::run() {
    while (!isInterrupted()) {
        sp<Runnable> runnable = mQueue->take();
        if (runnable != nullptr) {
            runnable->run();
        } else {
            break;
        }
    }
}

void ThreadPoolExecutor::WorkerThread::setQueue(const sp<LinkedBlockingQueue<sp<Runnable>>>& queue) {
    mQueue = queue;
}

void ThreadPoolExecutor::execute(const sp<Runnable>& runnable) {
    mQueue->put(runnable);
}

bool ThreadPoolExecutor::cancel(const sp<Runnable>& runnable) {
    return mQueue->remove(runnable);
}

} /* namespace mindroid */
