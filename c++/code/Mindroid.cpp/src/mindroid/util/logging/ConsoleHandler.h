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

#ifndef MINDROID_CONSOLEHANDLER_H_
#define MINDROID_CONSOLEHANDLER_H_

#include "mindroid/util/logging/LogBuffer.h"

namespace mindroid {

/**
 * A handler that writes log messages to the standard output stream {@code System.out}.
 */
class ConsoleHandler : public Object {
public:
    /**
     * Constructs a {@code ConsoleHandler} object.
     */
    ConsoleHandler() {
    }

    /**
     * Flushes and closes all opened files.
     */
    void close() {
    }

    /**
     * Publish a {@code LogRecord}.
     *
     * @param record The log record.
     */
    void publish(const sp<LogBuffer::LogRecord>& record);

    void setFlag(uint32_t flag);
    void removeFlag(uint32_t flag);

    static const uint32_t FLAG_TIMESTAMP = 1;

private:
    uint32_t mFlags = 0;
};

} /* namespace mindroid */

#endif /* MINDROID_CONSOLEHANDLER_H_ */
