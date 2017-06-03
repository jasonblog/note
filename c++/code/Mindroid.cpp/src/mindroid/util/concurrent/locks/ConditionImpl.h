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

#ifndef MINDROID_CONDITIONIMPL_H_
#define MINDROID_CONDITIONIMPL_H_

#include "mindroid/util/concurrent/locks/Condition.h"
#include <pthread.h>

namespace mindroid {

class Lock;

class ConditionImpl :
        public Condition {
public:
    virtual ~ConditionImpl();
    ConditionImpl(const ConditionImpl&) = delete;
    ConditionImpl& operator=(const ConditionImpl&) = delete;
    virtual void await();
    virtual bool await(uint64_t timeoutMillis);
    virtual void signal();
    virtual void signalAll();

private:
    ConditionImpl(const sp<Lock>& lock);

    pthread_cond_t mCondition;
    pthread_condattr_t mAttributes;
    sp<Lock> mLock;
    pthread_mutex_t* mMutex;

    friend class ReentrantLock;
};

} /* namespace mindroid */

#endif /* MINDROID_CONDITIONIMPL_H_ */
