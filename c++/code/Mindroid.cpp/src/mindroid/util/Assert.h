/*
 * Copyright (C) 2014 Daniel Himmelein
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

#ifndef MINDROID_ASSERT_H_
#define MINDROID_ASSERT_H_

#include "mindroid/lang/Object.h"
#include <cassert>

namespace mindroid {

class Assert final {
public:
    Assert() noexcept = delete;
    ~Assert() noexcept = delete;
    Assert(const Assert&) = delete;
    Assert& operator=(const Assert&) = delete;

    static void assertTrue(bool condition) {
        assert(condition);
    }
    static void assertTrue(const char* message, bool condition);

    static void assertFalse(bool condition) {
        assert(!condition);
    }
    static void assertFalse(const char* message, bool condition);

    static void assertNull(void* ptr) {
        assert(ptr == nullptr);
    }
    static void assertNull(const char* message, void* ptr);

    static void assertNull(const sp<Object>& object) {
        assert(object == nullptr);
    }
    static void assertNull(const char* message, const sp<Object>& object);

    static void assertNotNull(void* ptr) {
        assert(ptr != nullptr);
    }
    static void assertNotNull(const char* message, void* ptr);

    static void assertNotNull(const sp<Object>& object) {
        assert(object != nullptr);
    }
    static void assertNotNull(const char* message, const sp<Object>& object);

    static void assertEquals(bool expected, bool actual) {
        assert(expected == actual);
    }
    static void assertEquals(const char* message, bool expected, bool actual);

    static void assertEquals(int8_t expected, int8_t actual) {
        assert(expected == actual);
    }
    static void assertEquals(const char* message, int8_t expected, int8_t actual);

    static void assertEquals(uint8_t expected, uint8_t actual) {
        assert(expected == actual);
    }
    static void assertEquals(const char* message, uint8_t expected, uint8_t actual);

    static void assertEquals(int16_t expected, int16_t actual) {
        assert(expected == actual);
    }
    static void assertEquals(const char* message, int16_t expected, int16_t actual);

    static void assertEquals(uint16_t expected, uint16_t actual) {
        assert(expected == actual);
    }
    static void assertEquals(const char* message, uint16_t expected, uint16_t actual);

    static void assertEquals(int32_t expected, int32_t actual) {
        assert(expected == actual);
    }
    static void assertEquals(const char* message, int32_t expected, int32_t actual);

    static void assertEquals(uint32_t expected, uint32_t actual) {
        assert(expected == actual);
    }
    static void assertEquals(const char* message, uint32_t expected, uint32_t actual);

    static void assertEquals(int64_t expected, int64_t actual) {
        assert(expected == actual);
    }
    static void assertEquals(const char* message, int64_t expected, int64_t actual);

    static void assertEquals(uint64_t expected, uint64_t actual) {
        assert(expected == actual);
    }
    static void assertEquals(const char* message, uint64_t expected, uint64_t actual);

    static void assertEquals(void* expected, void* actual) {
        assert(expected == actual);
    }
    static void assertEquals(const char* message, void* expected, void* actual);

    static void fail() {
        assert(false);
    }
    static void fail(const char* message);

private:
    static const char* const TAG;
};

} /* namespace mindroid */

#endif /* MINDROID_ASSERT_H_ */
