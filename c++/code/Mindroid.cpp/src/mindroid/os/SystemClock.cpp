/*
 * Copyright (C) 2006 The Android Open Source Project
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

#include "mindroid/os/SystemClock.h"
#include "mindroid/lang/Thread.h"

namespace mindroid {

void SystemClock::sleep(uint64_t ms) {
    Thread::sleep(ms);
}

uint64_t SystemClock::uptimeMillis() {
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (((uint64_t) now.tv_sec * 1000LL) + (now.tv_nsec / 1000000LL));
}

uint64_t SystemClock::elapsedRealtime() {
    timespec now;
#ifdef CLOCK_BOOTTIME
    clock_gettime(CLOCK_BOOTTIME, &now);
#else
    #warning SystemClock does not support CLOCK_BOOTTIME
    clock_gettime(CLOCK_MONOTONIC, &now);
#endif
    return (((uint64_t) now.tv_sec * 1000LL) + (now.tv_nsec / 1000000LL));
}

} /* namespace mindroid */
