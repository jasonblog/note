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

#ifndef MINDROID_FUTURE_H_
#define MINDROID_FUTURE_H_

#include "mindroid/util/concurrent/Awaitable.h"

namespace mindroid {

template<typename T>
class Future :
        public Awaitable {
public:
    virtual T get() const = 0;

    virtual T get(uint64_t timeout) const = 0;

    virtual bool isDone() const = 0;

    template<typename Functor>
    sp<Future> done(Functor func) {
        try {
            await();
            if (isDone()) {
                func();
            }
        } catch (const CancellationException& e) {
        } catch (const ExecutionException& e) {
        }
        return this;
    }

    template<typename Functor>
    sp<Future> fail(Functor func) {
        try {
            await();
            if (isCancelled()) {
                func();
            }
        } catch (...) {
            func();
        }
        return this;
    }
};

} /* namespace mindroid */

#define FUTURE(T) ::mindroid::sp<Future<T>>

#endif /* MINDROID_FUTURE_H_ */
