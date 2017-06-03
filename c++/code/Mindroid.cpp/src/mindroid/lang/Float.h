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

#ifndef MINDROID_FLOAT_H_
#define MINDROID_FLOAT_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

class String;

class Float :
        public Object {
public:
    /**
     * Constant for the maximum {@code float} value, (2 - 2<sup>-23</sup>) * 2<sup>127</sup>.
     */
    static const float MAX_VALUE;

    /**
     * Constant for the minimum {@code float} value, 2<sup>-149</sup>.
     */
    static const float MIN_VALUE;

    /**
     * Constant for the number of bits needed to represent a {@code float} in
     * two's complement form.
     */
    static const int32_t SIZE = 32;

    explicit Float(float value) : mValue(value) {
    }

    static sp<Float> valueOf(const char* s);
    static sp<Float> valueOf(const sp<String>& s);

    float floatValue() const {
        return mValue;
    }

    float value() const {
        return floatValue();
    }

private:
    float mValue;
};

} /* namespace mindroid */

#endif /* MINDROID_FLOAT_H_ */
