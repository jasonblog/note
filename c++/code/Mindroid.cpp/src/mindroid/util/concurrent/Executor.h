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

#ifndef MINDROID_EXECUTOR_H_
#define MINDROID_EXECUTOR_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

class Runnable;

class Executor :
        public Object {
public:
    Executor() = default;
    virtual ~Executor() = default;
    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;

    virtual void execute(const sp<Runnable>& runnable) = 0;
    virtual bool cancel(const sp<Runnable>& runnable) = 0;
};

} /* namespace mindroid */

#endif /* MINDROID_EXECUTOR_H_ */
