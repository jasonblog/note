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

#include "mindroid/lang/Thread.h"
#include <cstdlib>
#include <cstring>
#include <unistd.h>

namespace mindroid {

Thread::Thread(const sp<Runnable>& runnable, const sp<String>& name) :
        mName(name),
        mRunnable(runnable),
        mStarted(false),
        mInterrupted(false) {
}

Thread::Thread(pthread_t thread) :
        mThread(thread),
        mStarted(true),
        mInterrupted(false) {
}

bool Thread::start() {
    if (!mStarted) {
        mSelf = this;
        if (pthread_create(&mThread, nullptr, &Thread::exec, this) != 0) {
            mSelf.clear();
        }
        if (mName != nullptr) {
            pthread_setname_np(mThread, mName->c_str());
        }
        mStarted = (mSelf != nullptr);
        return mStarted;
    }
    return false;
}

void Thread::sleep(uint32_t milliseconds) {
    ::usleep((milliseconds % 1000) * 1000);
    ::sleep(milliseconds / 1000);
}

void Thread::join() const {
    pthread_join(mThread, nullptr);
}

void* Thread::exec(void* args) {
    Thread* const self = (Thread*) args;
    sp<Runnable> runnable = (self->mRunnable != nullptr) ? self->mRunnable : self;
    runnable->run();
    self->mSelf.clear();
    return nullptr;
}

void Thread::interrupt() {
    mInterrupted = true;
}

bool Thread::isInterrupted() const {
    return mInterrupted;
}

sp<Thread> Thread::currentThread() {
    return new Thread(pthread_self());
}

bool Thread::isAlive() const {
    return mSelf != nullptr;
}

pthread_t Thread::getId() const {
    return pthread_self();
}

void Thread::setSchedulingParams(int32_t policy, int32_t priority) {
    sched_param schedulingParameters;
    memset(&schedulingParameters, 0, sizeof(schedulingParameters));
    schedulingParameters.sched_priority = priority;
    pthread_setschedparam(mThread, policy, &schedulingParameters);
}

} /* namespace mindroid */
