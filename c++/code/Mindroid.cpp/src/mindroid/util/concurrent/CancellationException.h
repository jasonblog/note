/*
 * Copyright (C) 2014 Daniel Himmelein
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

#ifndef MINDROID_CANCELLATIONEXCEPTION_H_
#define MINDROID_CANCELLATIONEXCEPTION_H_

#include "mindroid/lang/Exception.h"

namespace mindroid {

/**
 * Exception indicating that the result of a future cannot be retrieved because the task was
 * cancelled.
 */
class CancellationException : public Exception {
public:
    CancellationException() = default;

    CancellationException(const char* message) : Exception(message) {
    }

    CancellationException(const sp<String>& message) : Exception(message) {
    }
};

} /* namespace mindroid */

#endif /* MINDROID_CANCELLATIONEXCEPTION_H_ */
