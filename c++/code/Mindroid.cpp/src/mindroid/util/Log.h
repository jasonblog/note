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

#ifndef MINDROID_LOG_H_
#define MINDROID_LOG_H_

#include "mindroid/util/logging/LogBuffer.h"
#if defined(ANDROID)
#include <android/log.h>
#endif

namespace mindroid {

class Log {
public:
    static const int32_t VERBOSE = 0;
    static const int32_t DEBUG = 1;
    static const int32_t INFO = 2;
    static const int32_t WARN = 3;
    static const int32_t ERROR = 4;
    static const int32_t WTF = 5;

    Log() noexcept = delete;
    ~Log() noexcept = delete;
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    /**
     * Send a {@link #VERBOSE} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    static int v(const char* tag, const char* format, ...);

    /**
     * Send a {@link #DEBUG} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    static int d(const char* tag, const char* format, ...);

    /**
     * Send an {@link #INFO} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    static int i(const char* tag, const char* format, ...);

    /**
     * Send a {@link #WARN} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    static int w(const char* tag, const char* format, ...);

    /**
     * Send an {@link #ERROR} log message.
     * @param tag Used to identify the source of a log message.  It usually identifies
     *        the class or activity where the log call occurs.
     * @param msg The message you would like logged.
     */
    static int e(const char* tag, const char* format, ...);

    /**
     * What a Terrible Failure: Report a condition that should never happen.
     * The error will always be logged at level ASSERT with the call stack.
     * Depending on system configuration, a report may be added to the
     * {@link android.os.DropBoxManager} and/or the process may be terminated
     * immediately with an error dialog.
     * @param tag Used to identify the source of a log message.
     * @param msg The message you would like logged.
     */
    static int wtf(const char* tag, const char* format, ...);

    /** @hide */
    static int println(int32_t logId, int32_t priority, const char* tag, const char* msg) {
        return println(logId, priority, String::valueOf(tag), String::valueOf(msg));
    }

    /** @hide */
    static int println(int32_t logId, int32_t priority, const sp<String>& tag, const sp<String>& msg) {
#ifdef ANDROID
        int androidPriority = ANDROID_LOG_DEFAULT;
        switch (priority) {
        case Log::VERBOSE:
            androidPriority = ANDROID_LOG_VERBOSE;
            break;
        case Log::DEBUG:
            androidPriority = ANDROID_LOG_DEBUG;
            break;
        case Log::INFO:
            androidPriority = ANDROID_LOG_INFO;
            break;
        case Log::WARN:
            androidPriority = ANDROID_LOG_WARN;
            break;
        case Log::ERROR:
            androidPriority = ANDROID_LOG_ERROR;
            break;
        case Log::WTF:
            androidPriority = ANDROID_LOG_FATAL;
            break;
        }
        __android_log_write(androidPriority, tag->c_str(), msg->c_str());
#endif

        switch (logId) {
        case LOG_ID_MAIN:
            sMainLogBuffer->offer(priority, tag, msg);
            return 0;
        case LOG_ID_EVENTS:
            sEventLogBuffer->offer(priority, tag, msg);
            return 0;
        case LOG_ID_DEBUG:
            sDebugLogBuffer->offer(priority, tag, msg);
            return 0;
        default:
            return -1;
        }
    }

    /** @hide */
    static int32_t parsePriority(const sp<String>& priority) {
        char c;
        if (priority->length() > 1 && priority->toUpperCase()->equals("WTF")) {
            c = 'A';
        } else {
            c = priority->charAt(0);
        }

        switch (c) {
        case 'V':
            return Log::VERBOSE;
        case 'D':
            return Log::DEBUG;
        case 'I':
            return Log::INFO;
        case 'W':
            return Log::WARN;
        case 'E':
            return Log::ERROR;
        case 'A':
            return Log::WTF;
        default:
            return -1;
        }
    }

    /** @hide */
    static sp<String> toPriority(int32_t priority) {
        const char logLevels[] = { 'V', 'D', 'I', 'W', 'E', 'A' };
        if (priority >= 0 && size_t(priority) < sizeof(logLevels)) {
            return String::valueOf(logLevels[priority]);
        } else {
            return nullptr;
        }
    }

    /** @hide */
    static sp<LogBuffer> getLogBuffer(int32_t logId) {
        switch (logId) {
        case LOG_ID_MAIN:
            return sMainLogBuffer;
        case LOG_ID_EVENTS:
            return sEventLogBuffer;
        case LOG_ID_DEBUG:
            return sDebugLogBuffer;
        default:
            return nullptr;
        }
    }

    /** @hide */
    static const int32_t LOG_ID_MAIN = 0;
    /** @hide */
    static const int32_t LOG_ID_EVENTS = 1;
    /** @hide */
    static const int32_t LOG_ID_DEBUG = 2;

private:
    static const int LOG_MESSAGE_SIZE = 256;

    static sp<LogBuffer> sMainLogBuffer;
    static sp<LogBuffer> sEventLogBuffer;
    static sp<LogBuffer> sDebugLogBuffer;
};

} /* namespace mindroid */

#endif /* MINDROID_LOG_H_ */
