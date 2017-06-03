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

#ifndef MINDROID_SHORT_H_
#define MINDROID_SHORT_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

class String;

class Short :
        public Object {
public:
    /**
     * Constant for the maximum {@code short} value, 2<sup>15</sup>-1.
     */
    static const int16_t MAX_VALUE = 0x7FFF;

    /**
     * Constant for the minimum {@code short} value, -2<sup>15</sup>.
     */
    static const int16_t MIN_VALUE = 0x8000;

    /**
     * Constant for the number of bits needed to represent a {@code short} in
     * two's complement form.
     */
    static const int32_t SIZE = 16;

    explicit Short(int16_t value) : mValue(value) {
    }

    static sp<Short> valueOf(const char* s);
    static sp<Short> valueOf(const sp<String>& s);
    static sp<Short> valueOf(const char* s, int32_t radix);
    static sp<Short> valueOf(const sp<String>& s, int32_t radix);

    int16_t shortValue() const {
        return mValue;
    }

    int16_t value() const {
        return shortValue();
    }

private:
    int16_t mValue;
};

} /* namespace mindroid */

#endif /* MINDROID_SHORT_H_ */
