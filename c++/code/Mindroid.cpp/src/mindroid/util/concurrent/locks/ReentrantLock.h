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

#ifndef MINDROID_REENTRANTLOCK_H_
#define MINDROID_REENTRANTLOCK_H_

#include "mindroid/util/concurrent/locks/Lock.h"
#include "mindroid/util/concurrent/locks/Condition.h"
#include <pthread.h>

namespace mindroid {

class ReentrantLock :
        public Lock {
public:
    ReentrantLock();
    virtual ~ReentrantLock();
    ReentrantLock(const ReentrantLock&) = delete;
    ReentrantLock& operator=(const ReentrantLock&) = delete;
    virtual void lock();
    virtual sp<Condition> newCondition();
    virtual bool tryLock(uint64_t timeoutMillis);
    virtual void unlock();

protected:
    virtual pthread_mutex_t* getMutex() {
        return &mMutex;
    }

private:
    pthread_mutex_t mMutex;
    pthread_mutexattr_t mAttributes;
};

} /* namespace mindroid */

#endif /* MINDROID_REENTRANTLOCK_H_ */
