/*
 * Copyright (C) 2006 The Android Open Source Project
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

#ifndef MINDROID_HANDLERTHREAD_H_
#define MINDROID_HANDLERTHREAD_H_

#include "mindroid/lang/Thread.h"

namespace mindroid {

class Looper;
class ReentrantLock;
class Condition;

/**
 * Handy class for starting a new thread that has a looper. The looper can then be used to create
 * handler classes. Note that start() must still be called.
 */
class HandlerThread :
        public Thread {
public:
    HandlerThread();
    HandlerThread(const char* name);
    HandlerThread(const sp<String>& name);
    virtual ~HandlerThread() = default;

    HandlerThread(const HandlerThread&) = delete;
    HandlerThread& operator=(const HandlerThread&) = delete;

    virtual void run();

    /**
     * This method returns the Looper associated with this thread. If this thread not been started
     * or for any reason is isAlive() returns false, this method will return null. If this thread
     * has been started, this method will block until the looper has been initialized.
     *
     * @return The looper.
     */
    sp<Looper> getLooper();

    /**
     * Ask the currently running looper to quit. If the thread has not been started or has finished
     * (that is if {@link #getLooper} returns null), then false is returned. Otherwise the looper is
     * asked to quit and true is returned.
     */
    bool quit();

protected:
    /**
     * Call back method that can be explicitly overridden if needed to execute some setup before
     * Looper loops.
     */
    virtual void onLooperPrepared() {
    }

private:
    sp<ReentrantLock> mLock;
    sp<Condition> mCondition;
    sp<Looper> mLooper;
};

} /* namespace mindroid */

#endif /* MINDROID_HANDLERTHREAD_H_ */
