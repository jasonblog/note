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

#ifndef MINDROID_LOCK_H_
#define MINDROID_LOCK_H_

#include "mindroid/lang/Object.h"
#include "pthread.h"

namespace mindroid {

class Condition;

class Lock :
        public Object {
public:
    Lock() = default;
    virtual ~Lock() = default;
    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;
    virtual void lock() = 0;
    virtual sp<Condition> newCondition() = 0;
    virtual bool tryLock(uint64_t timeoutMillis) = 0;
    virtual void unlock() = 0;

protected:
    virtual pthread_mutex_t* getMutex() = 0;

    friend class ConditionImpl;
};

class AutoLock {
public:
    AutoLock(const sp<Lock>& lock) :
            mLock(lock) {
        mLock->lock();
    }

    ~AutoLock() {
        mLock->unlock();
    }

    AutoLock(const AutoLock&) = delete;
    AutoLock& operator=(const AutoLock&) = delete;

private:
    sp<Lock> mLock;

    void* operator new(size_t);
    void* operator new[](size_t);
    void operator delete(void*);
    void operator delete[](void*);
};

} /* namespace mindroid */

#endif /* MINDROID_LOCK_H_ */
