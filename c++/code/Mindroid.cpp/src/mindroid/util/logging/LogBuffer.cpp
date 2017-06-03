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

#include "mindroid/util/logging/LogBuffer.h"
#include "mindroid/lang/System.h"
#include "mindroid/lang/Thread.h"
#include <cstdio>
#include <cstring>
#include <pthread.h>

namespace mindroid {

char LogBuffer::sProrities[] = { 'V', 'D', 'I', 'W', 'E', 'A' };

LogBuffer::LogRecord::LogRecord(const int32_t logId, const uint64_t timestamp, const int32_t threadId, const int32_t priority, const sp<String>& tag, const sp<String>& message) :
        mLogId(logId),
        mTimestamp(timestamp),
        mThreadId(threadId),
        mPriority(priority),
        mTag(tag),
        mMessage(message) {
}

sp<String> LogBuffer::LogRecord::toShortString() {
    return String::format("%c/%s(%08x): %s", sProrities[mPriority], mTag->c_str(), mThreadId, mMessage->c_str());
}

sp<String> LogBuffer::LogRecord::toString() {
    struct timespec ts;
    ts.tv_nsec = (mTimestamp % 1000) * 1000000;
    ts.tv_sec = mTimestamp / 1000;

    char time[64];
    struct tm t;
    tzset();
    if (localtime_r(&(ts.tv_sec), &t) != NULL) {
        size_t s = strftime(time, sizeof(time), "%F %T.000", &t);
        if (s > 0) {
            snprintf(&time[strlen(time) - 4], 5, ".%03ld", ts.tv_nsec / 1000000);
        } else {
            time[0] = '\0';
        }
    } else {
        time[0] = '\0';
    }

    return String::format("%s  %08x  %c %s: %s", time, mThreadId, sProrities[mPriority], mTag->c_str(), mMessage->c_str());
}

LogBuffer::LogBuffer(const int32_t id, const size_t size) :
        mId(id),
        mSize(size),
        mData(new uint8_t[mSize]),
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()) {
    reset();
}

bool LogBuffer::offer(const int32_t priority, const sp<String>& tag, const sp<String>& message) {
    return offer(System::currentTimeMillis(), Thread::currentThread()->getId(), priority, tag, message);
}

bool LogBuffer::offer(const uint64_t timestamp, const int32_t threadId, const int32_t priority, const sp<String>& tag, const sp<String>& message) {
    if (tag == nullptr) {
        return false;
    }
    if (message == nullptr) {
        return false;
    }

    const size_t tagSize = tag->length();
    const size_t messageSize = message->length();
    uint32_t logMessageSize = TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE + TAG_SIZE + tagSize + MESSAGE_SIZE + messageSize;
    if ((logMessageSize + 4) > mSize) {
        return false;
    }

    uint8_t logMessage[logMessageSize];
    System::arraycopy(&timestamp, 0, TIMESTAMP_SIZE, logMessage, 0, logMessageSize, TIMESTAMP_SIZE);
    System::arraycopy(&threadId, 0, THREAD_ID_SIZE, logMessage, TIMESTAMP_SIZE, logMessageSize, THREAD_ID_SIZE);
    System::arraycopy(&priority, 0, PRIO_SIZE, logMessage, TIMESTAMP_SIZE + THREAD_ID_SIZE, logMessageSize, PRIO_SIZE);
    System::arraycopy(&tagSize, 0, TAG_SIZE, logMessage, TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE, logMessageSize, TAG_SIZE);
    System::arraycopy(tag->c_str(), 0, tagSize, logMessage, TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE + TAG_SIZE, logMessageSize, tagSize);
    System::arraycopy(&messageSize, 0, MESSAGE_SIZE, logMessage, TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE + TAG_SIZE + tagSize, logMessageSize, MESSAGE_SIZE);
    System::arraycopy(message->c_str(), 0, messageSize, logMessage, TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE + TAG_SIZE + tagSize + MESSAGE_SIZE, logMessageSize, messageSize);

    {
        AutoLock autoLock(mLock);
        free(4 + logMessageSize);
        write(&logMessageSize, 4);
        write(logMessage, logMessageSize);
        mCondition->signal();
    }

    return true;
}

sp<LogBuffer::LogRecord> LogBuffer::take(const int32_t minPriority) {
    while (true) {
        mLock->lock();
        while (isEmpty()) {
            mCondition->await();
            if (mQuitting) {
                mQuitting = false;
                mLock->unlock();
                return nullptr;
            }
        }
        uint32_t size = 0;
        read(&size, 4);
        uint8_t record[size];
        read(record, size);
        mLock->unlock();

        uint64_t timestamp = *reinterpret_cast<uint64_t*>(record);
        int32_t threadId = *reinterpret_cast<int32_t*>(record + TIMESTAMP_SIZE);
        int32_t priority = *reinterpret_cast<int32_t*>(record + TIMESTAMP_SIZE + THREAD_ID_SIZE);
        uint32_t tagSize = *reinterpret_cast<uint32_t*>(record + TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE);
        sp<String> tag = new String(record + TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE + TAG_SIZE, tagSize);
        uint32_t messageSize = *reinterpret_cast<uint32_t*>(record + TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE + TAG_SIZE + tagSize);
        sp<String> message = new String(record + TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE + TAG_SIZE + tagSize + MESSAGE_SIZE, messageSize);
        if (priority >= minPriority) {
            return new LogRecord(mId, timestamp, threadId, priority, tag, message);
        }
    }
}

sp<LogBuffer::LogRecord> LogBuffer::poll(const int32_t minPriority) {
    while (true) {
        mLock->lock();
        while (isEmpty()) {
            return nullptr;
        }
        uint32_t size = 0;
        read(&size, 4);
        uint8_t record[size];
        read(record, size);
        mLock->unlock();

        uint64_t timestamp = *reinterpret_cast<uint64_t*>(record);
        int32_t threadId = *reinterpret_cast<int32_t*>(record + TIMESTAMP_SIZE);
        int32_t priority = *reinterpret_cast<int32_t*>(record + TIMESTAMP_SIZE + THREAD_ID_SIZE);
        uint32_t tagSize = *reinterpret_cast<uint32_t*>(record + TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE);
        sp<String> tag = new String(record + TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE + TAG_SIZE, tagSize);
        uint32_t messageSize = *reinterpret_cast<uint32_t*>(record + TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE + TAG_SIZE + tagSize);
        sp<String> message = new String(record + TIMESTAMP_SIZE + THREAD_ID_SIZE + PRIO_SIZE + TAG_SIZE + tagSize + MESSAGE_SIZE, messageSize);
        if (priority >= minPriority) {
            return new LogRecord(mId, timestamp, threadId, priority, tag, message);
        }
    }
}

void LogBuffer::quit() {
    AutoLock autoLock(mLock);
    mQuitting = true;
    mCondition->signal();
}

bool LogBuffer::isEmpty() {
    AutoLock autoLock(mLock);
    return mReadIndex == mWriteIndex;
}

bool LogBuffer::isFull() {
    AutoLock autoLock(mLock);
    return (mWriteIndex + 1) % mSize == mReadIndex;
}

void LogBuffer::reset() {
    AutoLock autoLock(mLock);
    mReadIndex = 0;
    mWriteIndex = 0;
}

size_t LogBuffer::remainingCapacity() {
    if (mWriteIndex >= mReadIndex) {
        return (mSize - (mWriteIndex - mReadIndex));
    } else {
        return (mReadIndex - mWriteIndex);
    }
}

void LogBuffer::free(const size_t size) {
    size_t freeCapacity = remainingCapacity();
    while (freeCapacity < size) {
        uint32_t curSize = 0;
        read(&curSize, 4);
        mReadIndex = (mReadIndex + curSize) % mSize;
        freeCapacity += (curSize + 4);
    }
}

void LogBuffer::write(const void* data, const size_t size) {
    if (mWriteIndex + size < mSize) {
        System::arraycopy(data, 0, size, mData, mWriteIndex, mSize, size);
        mWriteIndex = (mWriteIndex + size) % mSize;
    } else {
        size_t partialSize = (mSize - mWriteIndex);
        System::arraycopy(data, 0, size, mData, mWriteIndex, mSize, partialSize);
        System::arraycopy(data, partialSize, size, mData, 0, mSize, size - partialSize);
        mWriteIndex = (mWriteIndex + size) % mSize;
    }
}

void LogBuffer::read(const void* data, const size_t size) {
    if (mReadIndex + size < mSize) {
        System::arraycopy(mData, mReadIndex, mSize, data, 0, size, size);
        mReadIndex = (mReadIndex + size) % mSize;
    } else {
        size_t partialSize = (mSize - mReadIndex);
        System::arraycopy(mData, mReadIndex, mSize, data, 0, size, partialSize);
        System::arraycopy(mData, 0, mSize, data, partialSize, size, size - partialSize);
        mReadIndex = (mReadIndex + size) % mSize;
    }
}

} /* namespace mindroid */
