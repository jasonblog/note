/*
 * Copyright (C) 2011 Daniel Himmelein
 * Copyright (C) 2016 E.S.R.Labs
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

#include "mindroid/util/concurrent/locks/ConditionImpl.h"
#include "mindroid/util/concurrent/locks/Lock.h"
#include "mindroid/util/Assert.h"
#include "mindroid/os/SystemClock.h"
#include <cstdlib>
#include <ctime>
#include <errno.h>

namespace mindroid {

ConditionImpl::ConditionImpl(const sp<Lock>& lock) :
        mLock(lock),
        mMutex(lock->getMutex()) {
    pthread_condattr_init(&mAttributes);
#ifndef PTHREAD_USE_TIMEDWAIT_NP
    pthread_condattr_setclock(&mAttributes, CLOCK_MONOTONIC);
#endif
    pthread_cond_init(&mCondition, &mAttributes);
}

ConditionImpl::~ConditionImpl() {
    pthread_cond_destroy(&mCondition);
    pthread_condattr_destroy(&mAttributes);
}

void ConditionImpl::await() {
    int32_t errorCode = pthread_cond_wait(&mCondition, mMutex);
    Assert::assertFalse("IllegalMonitorStateException: EPERM", errorCode == EPERM);
    Assert::assertTrue("IllegalMonitorStateException: EINVAL", errorCode == 0);
}

bool ConditionImpl::await(uint64_t timeoutMillis) {
    timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    time.tv_sec += timeoutMillis / 1000;
    time.tv_nsec += (timeoutMillis % 1000) * 1000000;
    if (time.tv_nsec >= 1000000000) {
        time.tv_sec++;
        time.tv_nsec -= 1000000000;
    }
    int32_t errorCode;
#ifdef PTHREAD_USE_TIMEDWAIT_NP
    errorCode = pthread_cond_timedwait_monotonic(&mCondition, mMutex, &time);
#else
    errorCode = pthread_cond_timedwait(&mCondition, mMutex, &time);
#endif
    if (errorCode == ETIMEDOUT) {
        return false;
    } else {
        Assert::assertFalse("IllegalMonitorStateException: EPERM", errorCode == EPERM);
        Assert::assertTrue("IllegalMonitorStateException: EINVAL", errorCode == 0);
        return true;
    }
}

void ConditionImpl::signal() {
    pthread_cond_signal(&mCondition);
}

void ConditionImpl::signalAll() {
    pthread_cond_broadcast(&mCondition);
}

} /* namespace mindroid */
