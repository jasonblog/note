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

#ifndef MINDROID_RUNTIMEEXCEPTION_H_
#define MINDROID_RUNTIMEEXCEPTION_H_

#include "mindroid/lang/Exception.h"

namespace mindroid {

/**
 * RuntimeException is the superclass of those exceptions that can be thrown during the normal operation.
 */
class RuntimeException : public Exception {
public:
    RuntimeException() = default;

    RuntimeException(const char* message) : Exception(message) {
    }

    RuntimeException(const sp<String>& message) : Exception(message) {
    }
};

} /* namespace mindroid */

#endif /* MINDROID_RUNTIMEEXCEPTION_H_ */
