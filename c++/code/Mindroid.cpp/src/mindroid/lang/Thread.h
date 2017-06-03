/*
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

#ifndef MINDROID_THREAD_H_
#define MINDROID_THREAD_H_

#include "mindroid/lang/String.h"
#include "mindroid/lang/Runnable.h"
#include <pthread.h>

namespace mindroid {

class Thread :
        public Runnable {
public:
    Thread() :
            Thread(nullptr, nullptr) {
    }
    Thread(const char* name) :
            Thread(nullptr, String::valueOf(name)) {
    }
    Thread(const sp<String>& name) :
            Thread(nullptr, name) {
    }
    Thread(const sp<Runnable>& runnable) :
            Thread(runnable, nullptr) {
    }
    Thread(const sp<Runnable>& runnable, const char* name) :
            Thread(runnable, String::valueOf(name)) {
    }
    Thread(const sp<Runnable>& runnable, const sp<String>& name);
    virtual ~Thread() = default;
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    /**
     * The maximum priority value allowed for a thread.
     * This corresponds to (but does not have the same value as)
     * {@code android.os.Process.THREAD_PRIORITY_URGENT_DISPLAY}.
     */
    static const int32_t MAX_PRIORITY = 10;

    /**
     * The minimum priority value allowed for a thread.
     * This corresponds to (but does not have the same value as)
     * {@code android.os.Process.THREAD_PRIORITY_LOWEST}.
     */
    static const int32_t MIN_PRIORITY = 1;

    /**
     * The normal (default) priority value assigned to the main thread.
     * This corresponds to (but does not have the same value as)
     * {@code android.os.Process.THREAD_PRIORITY_DEFAULT}.

     */
    static const int32_t NORM_PRIORITY = 5;

    /**
     * Returns the name of the Thread.
     */
    sp<String> getName() { return mName; }

    /**
     * Calls the <code>run()</code> method of the Runnable object the receiver
     * holds. If no Runnable is set, does nothing.
     *
     * @see Thread#start
     */
    virtual void run() { }

    /**
     * Starts the new Thread of execution. The <code>run()</code> method of
     * the receiver will be called by the receiver Thread itself (and not the
     * Thread calling <code>start()</code>).
     *
     * @throws IllegalThreadStateException - if this thread has already started.
     * @see Thread#run
     */
    bool start();

    /**
     * Starts the new Thread of execution. The <code>run()</code> method of
     * the receiver will be called by the receiver Thread itself (and not the
     * Thread calling <code>start()</code>).
     *
     * @throws IllegalThreadStateException - if this thread has already started.
     * @see Thread#run
     */
    static void sleep(uint32_t milliseconds);

    /**
     * Posts an interrupt request to this {@code Thread}. The behavior depends on
     * the state of this {@code Thread}:
     * <ul>
     * <li>
     * {@code Thread}s blocked in one of {@code Object}'s {@code wait()} methods
     * or one of {@code Thread}'s {@code join()} or {@code sleep()} methods will
     * be woken up, their interrupt status will be cleared, and they receive an
     * {@link InterruptedException}.
     * <li>
     * {@code Thread}s blocked in an I/O operation of an
     * {@link java.nio.channels.InterruptibleChannel} will have their interrupt
     * status set and receive an
     * {@link java.nio.channels.ClosedByInterruptException}. Also, the channel
     * will be closed.
     * <li>
     * {@code Thread}s blocked in a {@link java.nio.channels.Selector} will have
     * their interrupt status set and return immediately. They don't receive an
     * exception in this case.
     * <ul>
     *
     * @see Thread#interrupted
     * @see Thread#isInterrupted
     */
    void interrupt();

    /**
     * Returns a <code>boolean</code> indicating whether the receiver has a
     * pending interrupt request (<code>true</code>) or not (
     * <code>false</code>)
     *
     * @return a <code>boolean</code> indicating the interrupt status
     * @see Thread#interrupt
     * @see Thread#interrupted
     */
    bool isInterrupted() const;

    /**
     * Blocks the current Thread (<code>Thread.currentThread()</code>) until
     * the receiver finishes its execution and dies.
     *
     * @throws InterruptedException if the current thread has been interrupted.
     *         The interrupted status of the current thread will be cleared before the exception is
     *         thrown.
     * @see Object#notifyAll
     * @see java.lang.ThreadDeath
     */
    void join() const;

    /**
     * Returns the Thread of the caller, that is, the current Thread.
     */
    static sp<Thread> currentThread();

    /**
     * Returns <code>true</code> if the receiver has already been started and
     * still runs code (hasn't died yet). Returns <code>false</code> either if
     * the receiver hasn't been started yet or if it has already started and run
     * to completion and died.
     *
     * @return a <code>boolean</code> indicating the liveness of the Thread
     * @see Thread#start
     */
    bool isAlive() const;

    /**
     * Returns the thread's identifier. The ID is a positive <code>long</code>
     * generated on thread creation, is unique to the thread, and doesn't change
     * during the lifetime of the thread; the ID may be reused after the thread
     * has been terminated.
     *
     * @return the thread's ID.
     */
    pthread_t getId() const;

    void setSchedulingParams(int32_t policy, int32_t priority);

private:
    Thread(pthread_t thread);
    static void* exec(void* args);

    sp<Thread> mSelf;
    sp<String> mName;
    sp<Runnable> mRunnable;
    pthread_t mThread;
    bool mStarted;
    bool mInterrupted;

    friend class Looper;
};

} /* namespace mindroid */

#endif /* MINDROID_THREAD_H_ */
