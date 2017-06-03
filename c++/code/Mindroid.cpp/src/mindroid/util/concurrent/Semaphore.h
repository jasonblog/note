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

#ifndef MINDROID_SEMAPHORE_H_
#define MINDROID_SEMAPHORE_H_

#include "mindroid/lang/Object.h"
#include <semaphore.h>

namespace mindroid {

class Semaphore :
        public Object {
public:
    Semaphore(uint32_t value = 0);
    ~Semaphore();
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    void acquire();
    void release();
    bool tryAcquire(uint64_t timeoutMillis);

private:
    sem_t mSemaphore;
};

} /* namespace mindroid */

#endif /* MINDROID_SEMAPHORE_H_ */
