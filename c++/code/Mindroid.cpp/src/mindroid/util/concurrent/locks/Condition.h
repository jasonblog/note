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

#ifndef MINDROID_CONDITION_H_
#define MINDROID_CONDITION_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

class Lock;

class Condition :
        public Object {
public:
    Condition() = default;
    virtual ~Condition() = default;
    Condition(const Condition&) = delete;
    Condition& operator=(const Condition&) = delete;
    virtual void await() = 0;
    virtual bool await(uint64_t timeoutMillis) = 0;
    virtual void signal() = 0;
    virtual void signalAll() = 0;
};

} /* namespace mindroid */

#endif /* MINDROID_CONDITION_H_ */
