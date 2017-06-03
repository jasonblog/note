/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2011 Daniel Himmelein
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

#ifndef MINDROID_LOOPER_H_
#define MINDROID_LOOPER_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/Thread.h"
#include "mindroid/os/MessageQueue.h"

namespace mindroid {

class Runnable;

/**
 * Class used to run a message loop for a thread. Threads by default do not have a message loop
 * associated with them; to create one, call {@link #prepare} in the thread that is to run the loop,
 * and then {@link #loop} to have it process messages until the loop is stopped.
 *
 * <p>
 * Most interaction with a message loop is through the {@link Handler} class.
 *
 * <p>
 * This is a typical example of the implementation of a Looper thread, using the separation of
 * {@link #prepare} and {@link #loop} to create an initial Handler to communicate with the Looper.
 *
 * <pre>
 *  class LooperThread extends Thread {
 *      public Handler mHandler;
 *
 *      public void run() {
 *          Looper.prepare();
 *
 *          mHandler = new Handler() {
 *              public void handleMessage(Message msg) {
 *                  // process incoming messages here
 *              }
 *          };
 *
 *          Looper.loop();
 *      }
 *  }
 * </pre>
 */
class Looper final :
        public Object {
public:
    virtual ~Looper() {
    }

    Looper(const Looper&) = delete;
    Looper& operator=(const Looper&) = delete;

    /**
     * Initialize the current thread as a looper. This gives you a chance to create handlers that
     * then reference this looper, before actually starting the loop. Be sure to call
     * {@link #loop()} after calling this method, and end it by calling {@link #quit()}.
     */
    static bool prepare() {
        return prepare(true);
    }

    static bool prepare(bool quitAllowed);

    /**
     * Run the message queue in this thread. Be sure to call {@link #quit()} to end the loop.
     */
    static void loop();

    /**
     * Return the Looper object associated with the current thread. Returns null if the calling
     * thread is not associated with a Looper.
     */
    static sp<Looper> myLooper();

    /**
     * Return the {@link MessageQueue} object associated with the current thread. This must be
     * called from a thread running a Looper, or a NullPointerException will be thrown.
     */
    static sp<MessageQueue> myQueue() { return myLooper()->mMessageQueue; }

    /**
     * Returns true if the current thread is this looper's thread.
     */
    bool isCurrentThread() {
        return pthread_equal(pthread_self(), mThread->mThread) != 0;
    }

    /**
     * Quits the looper.
     *
     * Causes the {@link #loop} method to terminate as soon as possible.
     */
    void quit();

    /**
     * Return the Thread associated with this Looper.
     */
    sp<Thread> getThread() { return mThread; }

    /** @hide */
    sp<MessageQueue> getQueue() {
        return mMessageQueue;
    }

private:
    Looper(bool quitAllowed);
    static void init();
    static void finalize(void* looper);

    sp<Looper> mSelf;
    sp<MessageQueue> mMessageQueue;
    sp<Thread> mThread;

    static pthread_once_t sTlsOneTimeInitializer;
    static pthread_key_t sTlsKey;

    friend class Handler;
};

} /* namespace mindroid */

#endif /* MINDROID_LOOPER_H_ */
