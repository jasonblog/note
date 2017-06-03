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

#ifndef MINDROID_MATH_H_
#define MINDROID_MATH_H_

#include <cstdint>

namespace mindroid {

class Math {
public:
    Math() noexcept = delete;
    ~Math() noexcept = delete;
    Math(const Math&) = delete;
    Math& operator=(const Math&) = delete;

    static int8_t min(int8_t a, int8_t b) {
        return (a < b) ? a : b;
    }
    static uint8_t min(uint8_t a, uint8_t b) {
        return (a < b) ? a : b;
    }

    static int16_t min(int16_t a, int16_t b) {
        return (a < b) ? a : b;
    }
    static uint16_t min(uint16_t a, uint16_t b) {
        return (a < b) ? a : b;
    }

    static int32_t min(int32_t a, int32_t b) {
        return (a < b) ? a : b;
    }
    static uint32_t min(uint32_t a, uint32_t b) {
        return (a < b) ? a : b;
    }

    static int64_t min(int64_t a, int64_t b) {
        return (a < b) ? a : b;
    }
    static uint64_t min(uint64_t a, uint64_t b) {
        return (a < b) ? a : b;
    }

    static int8_t max(int8_t a, int8_t b) {
        return (a > b) ? a : b;
    }
    static uint8_t max(uint8_t a, uint8_t b) {
        return (a > b) ? a : b;
    }

    static int16_t max(int16_t a, int16_t b) {
        return (a > b) ? a : b;
    }
    static uint16_t max(uint16_t a, uint16_t b) {
        return (a > b) ? a : b;
    }

    static int32_t max(int32_t a, int32_t b) {
        return (a > b) ? a : b;
    }
    static uint32_t max(uint32_t a, uint32_t b) {
        return (a > b) ? a : b;
    }

    static int64_t max(int64_t a, int64_t b) {
        return (a > b) ? a : b;
    }
    static uint64_t max(uint64_t a, uint64_t b) {
        return (a > b) ? a : b;
    }
};

} /* namespace mindroid */

#endif /* MINDROID_MATH_H_ */
