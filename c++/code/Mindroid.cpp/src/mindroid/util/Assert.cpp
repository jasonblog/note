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

#include "mindroid/util/Assert.h"
#include "mindroid/util/Log.h"
#include "mindroid/lang/System.h"

namespace mindroid {

const char* const Assert::TAG = "Assert";

void Assert::assertTrue(const char* message, bool condition) {
    if (!condition) {
        Log::wtf(TAG, "%s (%d)", message, condition);
    }
    assert(condition);
}

void Assert::assertFalse(const char* message, bool condition) {
    if (condition) {
        Log::wtf(TAG, "%s (%d)", message, condition);
    }
    assert(!condition);
}

void Assert::assertNull(const char* message, void* ptr) {
    if (ptr != nullptr) {
        Log::wtf(TAG, "%s (0x%x)", message, ptr);
    }
    assert(ptr == nullptr);
}

void Assert::assertNull(const char* message, const sp<Object>& object) {
    if (object != nullptr) {
        Log::wtf(TAG, "%s (0x%x)", message, object.getPointer());
    }
    assert(object == nullptr);
}

void Assert::assertNotNull(const char* message, void* ptr) {
    if (ptr == nullptr) {
        Log::wtf(TAG, "%s (0x%x)", message, ptr);
    }
    assert(ptr != nullptr);
}

void Assert::assertNotNull(const char* message, const sp<Object>& object) {
    if (object == nullptr) {
        Log::wtf(TAG, "%s (0x%x)", message, object.getPointer());
    }
    assert(object != nullptr);
}

void Assert::assertEquals(const char* message, bool expected, bool actual) {
    if (expected != actual) {
        Log::wtf(TAG, "%s (%d, %d)", message, expected, actual);
    }
    assert(expected == actual);
}

void Assert::assertEquals(const char* message, int8_t expected, int8_t actual) {
    if (expected != actual) {
        Log::wtf(TAG, "%s (%d, %d)", message, expected, actual);
    }
    assert(expected == actual);
}

void Assert::assertEquals(const char* message, uint8_t expected, uint8_t actual) {
    if (expected != actual) {
        Log::wtf(TAG, "%s (%d, %d)", message, expected, actual);
    }
    assert(expected == actual);
}

void Assert::assertEquals(const char* message, int16_t expected, int16_t actual) {
    if (expected != actual) {
        Log::wtf(TAG, "%s (%d, %d)", message, expected, actual);
    }
    assert(expected == actual);
}

void Assert::assertEquals(const char* message, uint16_t expected, uint16_t actual) {
    if (expected != actual) {
        Log::wtf(TAG, "%s (%d, %d)", message, expected, actual);
    }
    assert(expected == actual);
}

void Assert::assertEquals(const char* message, int32_t expected, int32_t actual) {
    if (expected != actual) {
        Log::wtf(TAG, "%s (%d, %d)", message, expected, actual);
    }
    assert(expected == actual);
}

void Assert::assertEquals(const char* message, uint32_t expected, uint32_t actual) {
    if (expected != actual) {
        Log::wtf(TAG, "%s (%d, %d)", message, expected, actual);
    }
    assert(expected == actual);
}

void Assert::assertEquals(const char* message, int64_t expected, int64_t actual) {
    if (expected != actual) {
        Log::wtf(TAG, "%s (%lld, %lld)", message, expected, actual);
    }
    assert(expected == actual);
}

void Assert::assertEquals(const char* message, uint64_t expected, uint64_t actual) {
    if (expected != actual) {
        Log::wtf(TAG, "%s (%lld, %lld)", message, expected, actual);
    }
    assert(expected == actual);
}

void Assert::assertEquals(const char* message, void* expected, void* actual) {
    if (expected != actual) {
        Log::wtf(TAG, "%s (0x%x, 0x%x)", message, expected, actual);
    }
    assert(expected == actual);
}

void Assert::fail(const char* message) {
    Log::wtf(TAG, "%s", message);
    assert(false);
    System::exit(-1);
}

} /* namespace mindroid */
