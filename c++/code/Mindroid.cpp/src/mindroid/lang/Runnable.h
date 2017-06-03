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

#ifndef MINDROID_RUNNABLE_H_
#define MINDROID_RUNNABLE_H_

#include "mindroid/lang/Object.h"
#include <functional>

namespace mindroid {

class Runnable :
        public Object {
public:
    Runnable(const std::function<void (void)>& func) : mFunc(func) { }
    Runnable(std::function<void (void)>&& func) : mFunc(std::move(func)) { }

    virtual ~Runnable() = default;
    virtual void run() {
        if (mFunc) {
            mFunc();
        }
    }

protected:
    Runnable() : mFunc(nullptr) { }

private:
    const std::function<void (void)> mFunc;
};

} /* namespace mindroid */

#endif /* MINDROID_RUNNABLE_H_ */
