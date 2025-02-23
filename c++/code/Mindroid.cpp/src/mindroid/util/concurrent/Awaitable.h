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

#ifndef MINDROID_AWAITABLE_H_
#define MINDROID_AWAITABLE_H_

#include "mindroid/util/concurrent/Cancellable.h"
#include "mindroid/util/concurrent/CancellationException.h"
#include "mindroid/util/concurrent/ExecutionException.h"
#include "mindroid/util/concurrent/TimeoutException.h"

namespace mindroid
{

class Awaitable :
    public Cancellable
{
public:
    virtual ~Awaitable() = default;

    virtual void await() const = 0;

    virtual void await(uint64_t timeout) const = 0;
};

} /* namespace mindroid */

#endif /* MINDROID_AWAITABLE_H_ */
