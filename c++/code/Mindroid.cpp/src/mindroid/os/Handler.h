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

#ifndef MINDROID_HANDLER_H_
#define MINDROID_HANDLER_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/Closure.h"
#include "mindroid/os/Message.h"
#include "mindroid/os/SystemClock.h"
#include "mindroid/os/Looper.h"
#include <functional>

namespace mindroid {

class Looper;
class MessageQueue;

/**
 * A Handler allows you to send and process {@link Message} and Runnable objects associated with a
 * thread's {@link MessageQueue}. Each Handler instance is associated with a single thread and that
 * thread's message queue. When you create a new Handler, it is bound to the thread / message queue
 * of the thread that is creating it -- from that point on, it will deliver messages and runnables
 * to that message queue and execute them as they come out of the message queue.
 *
 * <p>
 * There are two main uses for a Handler: (1) to schedule messages and runnables to be executed as
 * some point in the future; and (2) to enqueue an action to be performed on a different thread than
 * your own.
 *
 * <p>
 * Scheduling messages is accomplished with the {@link #post}, {@link #postAtTime(Runnable, long)},
 * {@link #postDelayed}, {@link #sendEmptyMessage}, {@link #sendMessage}, {@link #sendMessageAtTime}
 * , and {@link #sendMessageDelayed} methods. The <em>post</em> versions allow you to enqueue
 * Runnable objects to be called by the message queue when they are received; the
 * <em>sendMessage</em> versions allow you to enqueue a {@link Message} object containing a bundle
 * of data that will be processed by the Handler's {@link #handleMessage} method (requiring that you
 * implement a subclass of Handler).
 *
 * <p>
 * When posting or sending to a Handler, you can either allow the item to be processed as soon as
 * the message queue is ready to do so, or specify a delay before it gets processed or absolute time
 * for it to be processed. The latter two allow you to implement timeouts, ticks, and other
 * timing-based behavior.
 *
 * <p>
 * When a process is created for your application, its main thread is dedicated to running a message
 * queue that takes care of managing the top-level application objects (activities, broadcast
 * receivers, etc) and any windows they create. You can create your own threads, and communicate
 * back with the main application thread through a Handler. This is done by calling the same
 * <em>post</em> or <em>sendMessage</em> methods as before, but from your new thread. The given
 * Runnable or Message will then be scheduled in the Handler's message queue and processed when
 * appropriate.
 */
class Handler :
        public Object {
public:
    /**
     * Callback interface you can use when instantiating a Handler to avoid having to implement your
     * own subclass of Handler.
     */
    class Callback :
            public Object {
    public:
        virtual bool handleMessage(const sp<Message>& msg) = 0;
    };

    /**
     * Default constructor associates this handler with the queue for the current thread.
     *
     * If there isn't one, this handler won't be able to receive messages.
     */
    Handler();

    /**
     * Constructor associates this handler with the queue for the current thread and takes a
     * callback interface in which you can handle messages.
     */
    Handler(const sp<Callback>& callback);

    /**
     * Use the provided queue instead of the default one.
     */
    Handler(const sp<Looper>& looper);

    /**
     * Use the provided queue instead of the default one and take a callback interface in which to
     * handle messages.
     */
    Handler(const sp<Looper>& looper, const sp<Callback>& callback);

    virtual ~Handler() {
    }

    Handler(const Handler&) = delete;
    Handler& operator=(const Handler&) = delete;

    /**
     * Subclasses must implement this to receive messages.
     */
    virtual void handleMessage(const sp<Message>& msg) {
    }

    /**
     * Handle system messages here.
     */
    void dispatchMessage(const sp<Message>& msg);

    /**
     * Returns a new {@link mindroid.os.Message Message} from the global message pool. More
     * efficient than creating and allocating new instances. The retrieved message has its handler
     * set to this instance (Message.target == this). If you don't want that facility, just call
     * Message.obtain() instead.
     */
    sp<Message> obtainMessage() {
        return Message::obtain(sp<Handler>(this));
    }

    /**
     * Same as {@link #obtainMessage()}, except that it also sets the what member of the returned
     * Message.
     *
     * @param what Value to assign to the returned Message.what field.
     * @return A Message from the global message pool.
     */
    sp<Message> obtainMessage(int32_t what) {
        return Message::obtain(sp<Handler>(this), what);
    }

    /**
     *
     * Same as {@link #obtainMessage()}, except that it also sets the what and obj members of the
     * returned Message.
     *
     * @param what Value to assign to the returned Message.what field.
     * @param obj Value to assign to the returned Message.obj field.
     * @return A Message from the global message pool.
     */
    sp<Message> obtainMessage(int32_t what, const sp<Object>& obj) {
        return Message::obtain(sp<Handler>(this), what, obj);
    }

    /**
     *
     * Same as {@link #obtainMessage()}, except that it also sets the what, arg1 and arg2 members of
     * the returned Message.
     *
     * @param what Value to assign to the returned Message.what field.
     * @param arg1 Value to assign to the returned Message.arg1 field.
     * @param arg2 Value to assign to the returned Message.arg2 field.
     * @return A Message from the global message pool.
     */
    sp<Message> obtainMessage(int32_t what, int32_t arg1, int32_t arg2) {
        return Message::obtain(sp<Handler>(this), what, arg1, arg2);
    }

    /**
     *
     * Same as {@link #obtainMessage()}, except that it also sets the what, obj, arg1,and arg2
     * values on the returned Message.
     *
     * @param what Value to assign to the returned Message.what field.
     * @param arg1 Value to assign to the returned Message.arg1 field.
     * @param arg2 Value to assign to the returned Message.arg2 field.
     * @param obj Value to assign to the returned Message.obj field.
     * @return A Message from the global message pool.
     */
    sp<Message> obtainMessage(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj) {
        return Message::obtain(sp<Handler>(this), what, arg1, arg2, obj);
    }

    sp<Message> obtainMessage(const sp<Runnable>& callback) {
        return Message::obtain(sp<Handler>(this), callback);
    }

    sp<Message> obtainMessage(const std::function<void (void)>& func) {
        return Message::obtain(sp<Handler>(this), new Closure(sp<Handler>(this), func));
    }

    /**
     * Causes the Runnable r to be added to the message queue. The runnable will be run on the
     * thread to which this handler is attached.
     *
     * @param runnable The Runnable that will be executed.
     *
     * @return Returns true if the Runnable was successfully placed in to the message queue. Returns
     * false on failure, usually because the looper processing the message queue is exiting.
     */
    bool post(const sp<Runnable>& runnable) {
        return sendMessageDelayed(getPostMessage(runnable), 0);
    }

    /**
     * Causes the Runnable r to be added to the message queue, to be run at a specific time given by
     * <var>uptimeMillis</var>. <b>The time-base is {@link mindroid.os.SystemClock#uptimeMillis}
     * .</b> The runnable will be run on the thread to which this handler is attached.
     *
     * @param runnable The Runnable that will be executed.
     * @param uptimeMillis The absolute time at which the callback should run, using the
     * {@link mindroid.os.SystemClock#uptimeMillis} time-base.
     *
     * @return Returns true if the Runnable was successfully placed in to the message queue. Returns
     * false on failure, usually because the looper processing the message queue is exiting. Note
     * that a result of true does not mean the Runnable will be processed -- if the looper is quit
     * before the delivery time of the message occurs then the message will be dropped.
     */
    bool postAtTime(const sp<Runnable>& runnable, uint64_t uptimeMillis) {
        return sendMessageAtTime(getPostMessage(runnable), uptimeMillis);
    }

    /**
     * Causes the Runnable r to be added to the message queue, to be run at a specific time given by
     * <var>uptimeMillis</var>. <b>The time-base is {@link mindroid.os.SystemClock#uptimeMillis}
     * .</b> The runnable will be run on the thread to which this handler is attached.
     *
     * @param runnable The Runnable that will be executed.
     * @param uptimeMillis The absolute time at which the callback should run, using the
     * {@link mindroid.os.SystemClock#uptimeMillis} time-base.
     *
     * @return Returns true if the Runnable was successfully placed in to the message queue. Returns
     * false on failure, usually because the looper processing the message queue is exiting. Note
     * that a result of true does not mean the Runnable will be processed -- if the looper is quit
     * before the delivery time of the message occurs then the message will be dropped.
     *
     * @see mindroid.os.SystemClock#uptimeMillis
     */
    bool postAtTime(const sp<Runnable>& runnable, const sp<Object>& token, uint64_t uptimeMillis) {
        return sendMessageAtTime(getPostMessage(runnable, token), uptimeMillis);
    }

    /**
     * Causes the Runnable r to be added to the message queue, to be run after the specified amount
     * of time elapses. The runnable will be run on the thread to which this handler is attached.
     *
     * @param runnable The Runnable that will be executed.
     * @param delayMillis The delay (in milliseconds) until the Runnable will be executed.
     *
     * @return Returns true if the Runnable was successfully placed in to the message queue. Returns
     * false on failure, usually because the looper processing the message queue is exiting. Note
     * that a result of true does not mean the Runnable will be processed -- if the looper is quit
     * before the delivery time of the message occurs then the message will be dropped.
     */
    bool postDelayed(const sp<Runnable>& runnable, uint64_t delayMillis) {
        return sendMessageDelayed(getPostMessage(runnable), delayMillis);
    }

    /**
     * Causes the std::function func to be added to the message queue. The runnable will be run on the
     * thread to which this handler is attached.
     *
     * @param runnable The std::function that will be executed.
     *
     * @return Returns a Closure if the std::function was successfully placed in to the message queue. Returns
     * nullptr on failure, usually because the looper processing the message queue is exiting.
     */
    sp<Closure> post(const std::function<void (void)>& func);
    sp<Closure> post(std::function<void (void)>&& func);

    /**
     * Causes the std::function func to be added to the message queue, to be run at a specific time given by
     * <var>uptimeMillis</var>. <b>The time-base is {@link mindroid.os.SystemClock#uptimeMillis}
     * .</b> The runnable will be run on the thread to which this handler is attached.
     *
     * @param func The std::function that will be executed.
     * @param uptimeMillis The absolute time at which the callback should run, using the
     * {@link mindroid.os.SystemClock#uptimeMillis} time-base.
     *
     * @return Returns a Closure if the std::function was successfully placed in to the message queue. Returns
     * nullptr on failure, usually because the looper processing the message queue is exiting. Note
     * that a result of true does not mean the std::function will be processed -- if the looper is quit
     * before the delivery time of the message occurs then the message will be dropped.
     */
    sp<Closure> postAtTime(const std::function<void (void)>& func, uint64_t uptimeMillis);
    sp<Closure> postAtTime(std::function<void (void)>&& func, uint64_t uptimeMillis);

    /**
     * Causes the std::function func to be added to the message queue, to be run after the specified amount
     * of time elapses. The std::function will be run on the thread to which this handler is attached.
     *
     * @param func The std::function that will be executed.
     * @param delayMillis The delay (in milliseconds) until the std::function will be executed.
     *
     * @return Returns a Closure if the std::function was successfully placed in to the message queue. Returns
     * nullptr on failure, usually because the looper processing the message queue is exiting. Note
     * that a result of true does not mean the std::function will be processed -- if the looper is quit
     * before the delivery time of the message occurs then the message will be dropped.
     */
    sp<Closure> postDelayed(const std::function<void (void)>& func, uint32_t delayMillis);
    sp<Closure> postDelayed(std::function<void (void)>&& func, uint32_t delayMillis);

    /**
     * Remove any pending posts of Runnable r that are in the message queue.
     */
    bool removeCallbacks(const sp<Runnable>& runnable);

    /**
     * Remove any pending posts of Runnable <var>r</var> with Object <var>token</var> that are in
     * the message queue. If <var>token</var> is null, all callbacks will be removed.
     */
    bool removeCallbacks(const sp<Runnable>& runnable, const sp<Object>& token);

    /**
     * Pushes a message onto the end of the message queue after all pending messages before the
     * current time. It will be received in {@link #handleMessage}, in the thread attached to this
     * handler.
     *
     * @return Returns true if the message was successfully placed in to the message queue. Returns
     * false on failure, usually because the looper processing the message queue is exiting.
     */
    bool sendMessage(const sp<Message>& message) {
        return sendMessageDelayed(message, 0);
    }

    /**
     * Sends a Message containing only the what value.
     *
     * @return Returns true if the message was successfully placed in to the message queue. Returns
     * false on failure, usually because the looper processing the message queue is exiting.
     */
    bool sendEmptyMessage(int32_t what) {
        return sendEmptyMessageDelayed(what, 0);
    }

    /**
     * Sends a Message containing only the what value, to be delivered after the specified amount of
     * time elapses.
     *
     * @see #sendMessageDelayed(mindroid.os.Message, long)
     *
     * @return Returns true if the message was successfully placed in to the message queue. Returns
     * false on failure, usually because the looper processing the message queue is exiting.
     */
    bool sendEmptyMessageDelayed(int32_t what, uint64_t delayMillis) {
        sp<Message> message = Message::obtain();
        message->what = what;
        return sendMessageDelayed(message, delayMillis);
    }

    /**
     * Sends a Message containing only the what value, to be delivered at a specific time.
     *
     * @see #sendMessageAtTime(mindroid.os.Message, long)
     *
     * @return Returns true if the message was successfully placed in to the message queue. Returns
     * false on failure, usually because the looper processing the message queue is exiting.
     */
    bool sendEmptyMessageAtTime(int32_t what, uint64_t uptimeMillis) {
        sp<Message> message = Message::obtain();
        message->what = what;
        return sendMessageAtTime(message, uptimeMillis);
    }

    /**
     * Enqueue a message into the message queue after all pending messages before (current time +
     * delayMillis). You will receive it in {@link #handleMessage}, in the thread attached to this
     * handler.
     *
     * @return Returns true if the message was successfully placed in to the message queue. Returns
     * false on failure, usually because the looper processing the message queue is exiting. Note
     * that a result of true does not mean the message will be processed -- if the looper is quit
     * before the delivery time of the message occurs then the message will be dropped.
     */
    bool sendMessageDelayed(const sp<Message>& message, uint32_t delayMillis) {
        return sendMessageAtTime(message, SystemClock::uptimeMillis() + delayMillis);
    }

    /**
     * Enqueue a message into the message queue after all pending messages before the absolute time
     * (in milliseconds) <var>uptimeMillis</var>. <b>The time-base is
     * {@link mindroid.os.SystemClock#uptimeMillis}.</b> You will receive it in
     * {@link #handleMessage}, in the thread attached to this handler.
     *
     * @param uptimeMillis The absolute time at which the message should be delivered, using the
     * {@link mindroid.os.SystemClock#uptimeMillis} time-base.
     *
     * @return Returns true if the message was successfully placed in to the message queue. Returns
     * false on failure, usually because the looper processing the message queue is exiting. Note
     * that a result of true does not mean the message will be processed -- if the looper is quit
     * before the delivery time of the message occurs then the message will be dropped.
     */
    bool sendMessageAtTime(const sp<Message>& message, uint64_t uptimeMillis);

    /**
     * Remove any pending posts of messages with code 'what' that are in the message queue.
     */
    bool removeMessages(int32_t what);

    /**
     * Remove any pending posts of messages with code 'what' and whose obj is 'object' that are in
     * the message queue. If <var>object</var> is null, all messages will be removed.
     */
    bool removeMessages(int32_t what, const sp<Object>& object);

    /**
     * Remove any pending posts of callbacks and sent messages whose <var>obj</var> is
     * <var>token</var>. If <var>token</var> is null, all callbacks and messages will be removed.
     */
    bool removeCallbacksAndMessages(const sp<Object>& token);

    /**
     * Check if there are any pending posts of messages with code 'what' in the message queue.
     */
    bool hasMessages(int32_t what);

    /**
     * Check if there are any pending posts of messages with code 'what' and whose obj is 'object'
     * in the message queue.
     */
    bool hasMessages(int32_t what, const sp<Object>& object);

    /**
     * Check if there are any pending posts of messages with callback r in the message queue.
     *
     * @hide
     */
    bool hasCallbacks(const sp<Runnable>& runnable);

    // If we can get rid of this method, the handler need not remember its loop
    // we could instead export a getMessageQueue() method...
    sp<Looper> getLooper() {
        return mLooper;
    }

private:
    sp<Message> getPostMessage(const sp<Runnable>& runnable);
    sp<Message> getPostMessage(const sp<Runnable>& runnable, const sp<Object>& token);

    static void handleCallback(const sp<Message>& message) {
        message->callback->run();
    }

    sp<MessageQueue> mMessageQueue;
    sp<Looper> mLooper;
    sp<Callback> mCallback;
};

} /* namespace mindroid */

#endif /* MINDROID_HANDLER_H_ */
