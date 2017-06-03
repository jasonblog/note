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

#include "mindroid/os/SystemClock.h"
#include "mindroid/util/Assert.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"
#include "mindroid/util/concurrent/locks/ConditionImpl.h"
#include <cstdlib>

namespace mindroid {

ReentrantLock::ReentrantLock() {
    pthread_mutexattr_init(&mAttributes);
    pthread_mutexattr_settype(&mAttributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mMutex, &mAttributes);
}

ReentrantLock::~ReentrantLock() {
    pthread_mutex_destroy(&mMutex);
    pthread_mutexattr_destroy(&mAttributes);
}

void ReentrantLock::lock() {
    pthread_mutex_lock(&mMutex);
}

sp<Condition> ReentrantLock::newCondition() {
    return new ConditionImpl(this);
}

bool ReentrantLock::tryLock(uint64_t timeoutMillis) {
#ifndef ANDROID
    timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    time.tv_sec += timeoutMillis / 1000;
    time.tv_nsec += (timeoutMillis % 1000) * 1000000;
    if (time.tv_nsec >= 1000000000) {
        time.tv_sec++;
        time.tv_nsec -= 1000000000;
    }
    return (pthread_mutex_timedlock(&mMutex, &time) == 0);
#else
    // Old Bionic versions lack pthread_mutex_timedlock.
    Assert::assertTrue(timeoutMillis <= UINT_MAX);
    return pthread_mutex_lock_timeout_np(&mMutex, (unsigned) timeoutMillis);
#endif
}

void ReentrantLock::unlock() {
    pthread_mutex_unlock(&mMutex);
}

} /* namespace mindroid */
