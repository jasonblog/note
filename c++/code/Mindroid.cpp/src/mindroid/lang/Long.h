/*
 * Copyright (C) 2016 Daniel Himmelein
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

#ifndef MINDROID_LONG_H_
#define MINDROID_LONG_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

class String;

class Long :
        public Object {
public:
    /**
     * Constant for the maximum {@code long} value, 2<sup>63</sup>-1.
     */
    static const int64_t MAX_VALUE = 0x7FFFFFFFFFFFFFFFL;

    /**
     * Constant for the minimum {@code long} value, -2<sup>63</sup>.
     */
    static const int64_t MIN_VALUE = 0x8000000000000000L;

    /**
     * Constant for the number of bits needed to represent a {@code long} in
     * two's complement form.
     */
    static const int32_t SIZE = 64;

    explicit Long(int64_t value) : mValue(value) {
    }

    static sp<Long> valueOf(const char* s);
    static sp<Long> valueOf(const sp<String>& s);
    static sp<Long> valueOf(const char* s, int32_t radix);
    static sp<Long> valueOf(const sp<String>& s, int32_t radix);

    int64_t longValue() const {
        return mValue;
    }

    int64_t value() const {
        return longValue();
    }

private:
    int64_t mValue;
};

} /* namespace mindroid */

#endif /* MINDROID_LONG_H_ */
