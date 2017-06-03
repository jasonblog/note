/*
 * Copyright (C) 2013 Daniel Himmelein
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

#ifndef MINDROID_LOGBUFFER_H_
#define MINDROID_LOGBUFFER_H_

#include "mindroid/lang/String.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"
#include "mindroid/util/concurrent/locks/Condition.h"

namespace mindroid {

class LogBuffer : public Object {
public:
    class LogRecord : public Object {
    public:
        LogRecord(const int32_t logId, const uint64_t timestamp, const int32_t threadId, const int32_t priority, const sp<String>& tag, const sp<String>& message);

        sp<String> toShortString();
        sp<String> toString();

        int32_t getLogId() {
            return mLogId;
        }

        uint64_t getTimestamp() {
            return mTimestamp;
        }

        int32_t getThreadId() {
            return mThreadId;
        }

        int32_t getPriority() {
            return mPriority;
        }

        sp<String> getTag() {
            return mTag;
        }

        sp<String> getMessage() {
            return mMessage;
        }

    private:
        int32_t mLogId;
        uint64_t mTimestamp;
        int32_t mThreadId;
        int32_t mPriority;
        sp<String> mTag;
        sp<String> mMessage;
    };

    LogBuffer(const int32_t id, const size_t size);

    virtual ~LogBuffer() {
        delete [] mData;
    }

    bool offer(const int32_t priority, const sp<String>& tag, const sp<String>& message);
    bool offer(const uint64_t timestamp, const int32_t threadId, const int32_t priority, const sp<String>& tag, const sp<String>& message);
    sp<LogRecord> take(const int32_t minPriority);
    sp<LogRecord> poll(const int32_t minPriority);

    void quit();
    bool isEmpty();
    bool isFull();
    void reset();

private:
    size_t remainingCapacity();
    void free(const size_t size);
    void write(const void* data, const size_t size);
    void read(const void* data, const size_t size);

    static const int32_t TIMESTAMP_SIZE = 8;
    static const int32_t PRIO_SIZE = 4;
    static const int32_t THREAD_ID_SIZE = 4;
    static const int32_t TAG_SIZE = 4;
    static const int32_t MESSAGE_SIZE = 4;
    static char sProrities[];

    const int32_t mId;
    const size_t mSize;
    size_t mReadIndex;
    size_t mWriteIndex;
    uint8_t* mData;
    bool mQuitting = false;
    sp<ReentrantLock> mLock;
    sp<Condition> mCondition;
};

} /* namespace mindroid */

#endif /* MINDROID_LOGBUFFER_H_ */
