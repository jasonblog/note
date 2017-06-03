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

#ifndef MINDROID_EXECUTIONEXCEPTION_H_
#define MINDROID_EXECUTIONEXCEPTION_H_

#include "mindroid/lang/Exception.h"

namespace mindroid {

/**
 * Exception indicating that the result of a future cannot be retrieved because the task was
 * stopped.
 */
class ExecutionException : public Exception {
public:
    ExecutionException() = default;

    ExecutionException(const char* message) : Exception(message) {
    }

    ExecutionException(const sp<String>& message) : Exception(message) {
    }
};

} /* namespace mindroid */

#endif /* MINDROID_EXECUTIONEXCEPTION_H_ */
