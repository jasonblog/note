/*
 * Copyright (C) 2006 The Android Open Source Project
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

#ifndef MINDROID_OPERATIONNOTSUPPORTEDEXCEPTION_H_
#define MINDROID_OPERATIONNOTSUPPORTEDEXCEPTION_H_

#include "mindroid/lang/RuntimeException.h"

namespace mindroid {

/**
 * Thrown to indicate that the requested operation is not supported.
 */
class UnsupportedOperationException : public RuntimeException {
public:
    UnsupportedOperationException() = default;

    UnsupportedOperationException(const char* message) : RuntimeException(message) {
    }

    UnsupportedOperationException(const sp<String>& message) : RuntimeException(message) {
    }
};

} /* namespace mindroid */

#endif /* MINDROID_OPERATIONNOTSUPPORTEDEXCEPTION_H_ */
