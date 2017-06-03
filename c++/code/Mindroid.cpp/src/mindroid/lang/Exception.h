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

#ifndef MINDROID_EXCEPTION_H_
#define MINDROID_EXCEPTION_H_

#include "mindroid/lang/String.h"
#include <exception>

namespace mindroid {

class Exception :
        public std::exception {
public:
    Exception() = default;
    ~Exception() noexcept {}

    Exception(const char* message) : mMessage(String::valueOf(message)) {
    }

    Exception(const sp<String>& message) : mMessage(message) {
    }

    sp<String> getMessage() {
        return mMessage;
    }

private:
    sp<String> mMessage;
};

} /* namespace mindroid */

#endif /* MINDROID_EXCEPTION_H_ */
