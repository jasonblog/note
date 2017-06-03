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

#ifndef MINDROID_BYTE_H_
#define MINDROID_BYTE_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

class String;

class Byte :
        public Object {
public:
    /**
     * The maximum {@code Byte} value, 2<sup>7</sup>-1.
     */
    static const int8_t MAX_VALUE = 0x7F;

    /**
     * The minimum {@code Byte} value, -2<sup>7</sup>.
     */
    static const int8_t MIN_VALUE = 0x80;

    /**
     * The number of bits needed to represent a {@code Byte} value in two's
     * complement form.
     */
    static const int32_t SIZE = 8;

    explicit Byte(int8_t value) : mValue(value) {
    }

    static sp<Byte> valueOf(const char* s);
    static sp<Byte> valueOf(const sp<String>& s);
    static sp<Byte> valueOf(const char* s, int32_t radix);
    static sp<Byte> valueOf(const sp<String>& s, int32_t radix);

    int8_t byteValue() const {
        return mValue;
    }

    int8_t value() const {
        return byteValue();
    }

private:
    int8_t mValue;
};

} /* namespace mindroid */

#endif /* MINDROID_BYTE_H_ */
