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

#include "mindroid/util/concurrent/atomic/AtomicInteger.h"
#include <atomic>

namespace mindroid {

int32_t AtomicInteger::getAndIncrement(volatile int32_t* address) {
    volatile std::atomic<int_least32_t>* a = (volatile std::atomic<int_least32_t>*) address;
    return a->fetch_add(1, std::memory_order_release);
}

int32_t AtomicInteger::getAndDecrement(volatile int32_t* address) {
    volatile std::atomic<int_least32_t>* a = (volatile std::atomic<int_least32_t>*) address;
    return a->fetch_sub(1, std::memory_order_release);
}

int32_t AtomicInteger::getAndAdd(int32_t value, volatile int32_t* address) {
    volatile std::atomic<int_least32_t>* a = (volatile std::atomic<int_least32_t>*) address;
    return a->fetch_add(value, std::memory_order_release);
}

int32_t AtomicInteger::getAndAnd(int32_t value, volatile int32_t* address) {
    volatile std::atomic<int_least32_t>* a = (volatile std::atomic<int_least32_t>*) address;
    return a->fetch_and(value, std::memory_order_release);
}

int32_t AtomicInteger::getAndOr(int32_t value, volatile int32_t* address) {
    volatile std::atomic<int_least32_t>* a = (volatile std::atomic<int_least32_t>*) address;
    return a->fetch_or(value, std::memory_order_release);
}

int32_t AtomicInteger::compareAndSwap(int32_t oldValue, int32_t newValue, volatile int32_t* address) {
    volatile std::atomic<int_least32_t>* a = (volatile std::atomic<int_least32_t>*) address;
    return (int32_t) (!a->compare_exchange_strong(oldValue, newValue, std::memory_order_release, std::memory_order_relaxed));
}

} /* namespace mindroid */
