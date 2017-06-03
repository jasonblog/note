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

#include "mindroid/util/logging/Logger.h"
#include "mindroid/content/Intent.h"
#include "mindroid/os/Environment.h"

namespace mindroid {

const char* const Logger::TAG = "Logger";

Logger::LoggerThread::LoggerThread(const sp<String>& name, int32_t logBufferId, bool consoleLogging, bool fileLogging, bool timestamps,
        int32_t priority, const sp<String>& logDirectory, const sp<String>& logFileName, uint32_t logFileSizeLimit, uint32_t logFileCount) :
        Thread(name) {
    mLogBuffer = Log::getLogBuffer(logBufferId);
    mConsoleLogging = consoleLogging;
    mFileLogging = fileLogging;
    mTimestamps = timestamps;
    mPriority = priority;
    mLogDirectory = logDirectory;
    mLogFileName = logFileName;
    mLogFileSizeLimit = logFileSizeLimit;
    mLogFileCount = logFileCount;
}

void Logger::LoggerThread::run() {
    open();

    while (!isInterrupted()) {
        sp<LogBuffer::LogRecord> logRecord = mLogBuffer->take(mPriority);
        if (logRecord != nullptr) {
            if (mConsoleHandler != nullptr) {
                mConsoleHandler->publish(logRecord);
            }
            if (mFileHandler != nullptr) {
                mFileHandler->publish(logRecord);
            }
        }
    }

    close();
}

void Logger::LoggerThread::open() {
    if (mConsoleLogging) {
        mConsoleHandler = new ConsoleHandler();
        if (mTimestamps) {
            mConsoleHandler->setFlag(ConsoleHandler::FLAG_TIMESTAMP);
        }
    }

    if (mFileLogging) {
        mFileHandler = new FileHandler(String::format("%s%c%s", mLogDirectory->c_str(), File::separatorChar, mLogFileName->c_str()),
                mLogFileSizeLimit, mLogFileCount, true);
    }
}

void Logger::LoggerThread::close() {
    if (mConsoleHandler != nullptr) {
        mConsoleHandler->close();
        mConsoleHandler = nullptr;
    }

    if (mFileHandler != nullptr) {
        mFileHandler->close();
        mFileHandler = nullptr;
    }
}

void Logger::LoggerThread::quit() {
    interrupt();
    mLogBuffer->quit();
    join();
}

void Logger::onCreate() {
}

int32_t Logger::onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) {
    sp<ArrayList<sp<String>>> logBuffers = intent->getStringArrayListExtra("logBuffers");

    if (logBuffers != nullptr) {
        if (logBuffers->contains(String::valueOf("main"))) {
            bool timestamps = intent->getBooleanExtra("timestamps", false);
            int32_t priority = intent->getIntExtra("priority", Log::ERROR);
            sp<String> logDirectory = Environment::getLogDirectory()->getPath();
            sp<String> logFileName = intent->getStringExtra("logFileName");
            int32_t logFileSizeLimit = intent->getIntExtra("logFileSizeLimit", 0);
            int32_t logFileCount = intent->getIntExtra("logFileCount", 2);
            bool fileLogging = (logDirectory != nullptr && logDirectory->length() > 0 && logFileName != nullptr && logFileName->length() > 0);

            mMainLoggerThread = new LoggerThread(String::format("%s {main}", TAG), Log::LOG_ID_MAIN, true, fileLogging, timestamps, priority,
                    logDirectory, logFileName, logFileSizeLimit, logFileCount);
            mMainLoggerThread->start();
        }

        if (logBuffers->contains(String::valueOf("debug"))) {
            if (mDebugLoggerThread == nullptr) {
                mDebugLoggerThread = new LoggerThread(String::format("%s {debug}", TAG), Log::LOG_ID_DEBUG, true, false, true, Log::DEBUG,
                        nullptr, nullptr, 0, 0);
                mDebugLoggerThread->start();
            }
        }
    }

    return 0;
}

void Logger::onDestroy() {
    if (mMainLoggerThread != nullptr) {
        mMainLoggerThread->quit();
    }

    if (mDebugLoggerThread != nullptr) {
        mDebugLoggerThread->quit();
    }
}

sp<IBinder> Logger::onBind(const sp<Intent>& intent) {
    return nullptr;
}

} /* namespace mindroid */
