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

#ifndef MINDROID_CHARACTER_H_
#define MINDROID_CHARACTER_H_

#include "mindroid/lang/Object.h"
#include <climits>

namespace mindroid {

class Character :
        public Object {
public:
    /**
     * The minimum {@code Character} value.
     */
    static const char MIN_VALUE = CHAR_MIN;

    /**
     * The maximum {@code Character} value.
     */
    static const char MAX_VALUE = CHAR_MAX;

    explicit Character(char value) : mValue(value) {
    }

    static sp<Character> valueOf(const char c);

    char charValue() const {
        return mValue;
    }

    char value() const {
        return charValue();
    }

private:
    char mValue;
};

} /* namespace mindroid */

#endif /* MINDROID_CHARACTER_H_ */
