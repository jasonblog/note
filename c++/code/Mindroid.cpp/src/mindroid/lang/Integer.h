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

#ifndef MINDROID_INTEGER_H_
#define MINDROID_INTEGER_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

class String;

class Integer :
        public Object {
public:
    /**
     * Constant for the maximum {@code int} value, 2<sup>31</sup>-1.
     */
    static const int32_t MAX_VALUE = 0x7FFFFFFF;

    /**
     * Constant for the minimum {@code int} value, -2<sup>31</sup>.
     */
    static const int32_t MIN_VALUE = 0x80000000;

    /**
     * Constant for the number of bits needed to represent an {@code int} in
     * two's complement form.
     */
    static const int32_t SIZE = 32;

    explicit Integer(int32_t value) : mValue(value) {
    }

    static sp<Integer> valueOf(const char* s);
    static sp<Integer> valueOf(const sp<String>& s);
    static sp<Integer> valueOf(const char* s, int32_t radix);
    static sp<Integer> valueOf(const sp<String>& s, int32_t radix);

    int32_t intValue() const {
        return mValue;
    }

    int32_t value() const {
        return intValue();
    }

private:
    int32_t mValue;
};

} /* namespace mindroid */

#endif /* MINDROID_INTEGER_H_ */
