/*
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

#ifndef MINDROID_PROMISE_H_
#define MINDROID_PROMISE_H_

#include "mindroid/util/concurrent/Future.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"
#include "mindroid/util/concurrent/CancellationException.h"
#include "mindroid/util/concurrent/ExecutionException.h"
#include "mindroid/util/concurrent/TimeoutException.h"
#include "mindroid/os/SystemClock.h"

namespace mindroid {

template<typename T>
class Promise :
        public Future<T> {
public:
    Promise() :
            mLock(new ReentrantLock()),
            mCondition(mLock->newCondition()),
            mIsDone(false),
            mIsCancelled(false) {
    }

    virtual ~Promise() = default;

    virtual void await() const {
        AutoLock autoLock(mLock);
        while (!mIsDone) {
            if (mIsCancelled) {
                throw CancellationException("Cancellation exception");
            }
            mCondition->await();
        }
    }

    virtual void await(uint64_t timeout) const {
        AutoLock autoLock(mLock);
        uint64_t start = SystemClock::uptimeMillis();
        int64_t duration = timeout;
        while (!mIsDone && (duration > 0)) {
            if (mIsCancelled) {
                throw CancellationException("Cancellation exception");
            }
            mCondition->await(duration);
            duration = start + timeout - SystemClock::uptimeMillis();
        }
        if (!mIsDone && !mIsCancelled) {
            throw TimeoutException("Future timed out");
        }
    }

    virtual bool cancel() {
        AutoLock autoLock(mLock);
        if (!mIsDone && !mIsCancelled) {
            mIsCancelled = true;
            mCondition->signal();
            return true;
        } else {
            return false;
        }
    }

    virtual T get() const {
        AutoLock autoLock(mLock);
        while (!mIsDone) {
            if (mIsCancelled) {
                throw CancellationException("Cancellation exception");
            }
            mCondition->await();
        }
        return mResult;
    }

    virtual T get(uint64_t timeout) const {
        AutoLock autoLock(mLock);
        uint64_t start = SystemClock::uptimeMillis();
        int64_t duration = timeout;
        while (!mIsDone && (duration > 0)) {
            if (mIsCancelled) {
                throw CancellationException("Cancellation exception");
            }
            mCondition->await(duration);
            duration = start + timeout - SystemClock::uptimeMillis();
        }
        if (!mIsDone && !mIsCancelled) {
            throw TimeoutException("Future timed out");
        }
        return mResult;
    }

    virtual bool isCancelled() const {
        AutoLock autoLock(mLock);
        return mIsCancelled;
    }

    virtual bool isDone() const {
        AutoLock autoLock(mLock);
        return mIsDone;
    }

    bool set(const T& object) {
        AutoLock autoLock(mLock);
        if (!mIsCancelled) {
            mResult = object;
            mIsDone = true;
            mCondition->signal();
            return true;
        } else {
            return false;
        }
    }

private:
    mutable sp<Lock> mLock;
    mutable sp<Condition> mCondition;
    T mResult;
    bool mIsDone;
    bool mIsCancelled;
};

template<typename T>
sp<Promise<T>> asPromise(const T& value) {
    sp<Promise<T>> promise = new Promise<T>();
    promise->set(value);
    return promise;
}

} /* namespace mindroid */

#define PROMISE(T) ::mindroid::sp<PROMISE<T>>

#endif /* MINDROID_PROMISE_H_ */
