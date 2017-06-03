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

#include "mindroid/util/logging/ConsoleHandler.h"
#include "mindroid/util/Log.h"
#include <cstdio>

namespace mindroid {

void ConsoleHandler::publish(const sp<LogBuffer::LogRecord>& record) {
    sp<String> message;
    if ((mFlags & FLAG_TIMESTAMP) == FLAG_TIMESTAMP) {
        message = record->toString();
    } else {
        message = record->toShortString();
    }

    printf("%s\n", message->c_str());
    fflush(stdout);
}

void ConsoleHandler::setFlag(uint32_t flag) {
    mFlags |= flag;
}

void ConsoleHandler::removeFlag(uint32_t flag) {
    mFlags &= ~flag;
}

} /* namespace mindroid */
