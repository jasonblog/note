/*
 * Copyright (C) 2012 Daniel Himmelein
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

#ifndef MINDROID_LOGGER_H_
#define MINDROID_LOGGER_H_

#include "mindroid/app/Service.h"
#include "mindroid/lang/Thread.h"
#include "mindroid/util/Log.h"
#include "mindroid/util/logging/ConsoleHandler.h"
#include "mindroid/util/logging/FileHandler.h"

namespace mindroid {

class Logger : public Service {
public:
    class LoggerThread : public Thread {
    public:
        LoggerThread(const sp<String>& name, int32_t logBufferId, bool consoleLogging, bool fileLogging, bool timestamps, int32_t priority,
                const sp<String>& logDirectory, const sp<String>& logFileName, uint32_t logFileSizeLimit, uint32_t logFileCount);

        void run() override;
        void open();
        void close();
        void quit();

    private:
        sp<LogBuffer> mLogBuffer;
        bool mConsoleLogging;
        bool mFileLogging;
        bool mTimestamps;
        int32_t mPriority = Log::VERBOSE;
        sp<String> mLogDirectory;
        sp<String> mLogFileName;
        uint32_t mLogFileSizeLimit;
        uint32_t mLogFileCount;
        sp<ConsoleHandler> mConsoleHandler;
        sp<FileHandler> mFileHandler;
    };

    void onCreate() override;
    int32_t onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) override;
    void onDestroy() override;
    sp<IBinder> onBind(const sp<Intent>& intent) override;

private:
    static const char* const TAG;
    sp<LoggerThread> mMainLoggerThread;
    sp<LoggerThread> mDebugLoggerThread;
};

} /* namespace mindroid */

#endif /* MINDROID_LOGGER_H_ */
