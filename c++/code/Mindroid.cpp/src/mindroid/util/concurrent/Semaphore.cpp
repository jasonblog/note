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

#include "mindroid/os/SystemClock.h"
#include "mindroid/util/concurrent/Semaphore.h"

namespace mindroid {

Semaphore::Semaphore(uint32_t value) {
    sem_init(&mSemaphore, 0, value);
}

Semaphore::~Semaphore() {
    sem_destroy(&mSemaphore);
}

void Semaphore::acquire() {
    sem_wait(&mSemaphore);
}

void Semaphore::release() {
    sem_post(&mSemaphore);
}

bool Semaphore::tryAcquire(uint64_t timeoutMillis) {
    timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    time.tv_sec += timeoutMillis / 1000;
    time.tv_nsec += (timeoutMillis % 1000) * 1000000;
    if (time.tv_nsec >= 1000000000) {
        time.tv_sec++;
        time.tv_nsec -= 1000000000;
    }
    return (sem_timedwait(&mSemaphore, &time) == 0);
}

} /* namespace mindroid */
