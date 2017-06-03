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

#ifndef MINDROID_MESSAGE_H_
#define MINDROID_MESSAGE_H_

#include "mindroid/lang/Object.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"
#include "mindroid/util/concurrent/Cancellable.h"
#include "mindroid/os/Bundle.h"

namespace mindroid {

class Runnable;
class Handler;
class Message;

struct MessagePool {
    MessagePool();
    ~MessagePool();

    uint32_t MAX_SIZE;
    sp<Message> pool;
    uint32_t size;
    sp<ReentrantLock> lock;
};

/**
 * Defines a message containing a description and arbitrary data object that can be sent to a
 * {@link Handler}. This object contains two extra int fields and an extra object field that allow
 * you to not do allocations in many cases.
 *
 * <p class="note">
 * While the constructor of Message is public, the best way to get one of these is to call
 * {@link #obtain Message.obtain()} or one of the {@link Handler#obtainMessage
 * Handler.obtainMessage()} methods, which will pull them from a pool of recycled objects.
 * </p>
 */
class Message final :
        public Object {
public:
    /**
     * User-defined message code so that the recipient can identify what this message is about. Each
     * {@link Handler} has its own name-space for message codes, so you do not need to worry about
     * yours conflicting with other handlers.
     */
    int32_t what;

    /**
     * arg1 and arg2 are lower-cost alternatives to using {@link #setData(Bundle) setData()} if you
     * only need to store a few integer values.
     */
    int32_t arg1;

    /**
     * arg1 and arg2 are lower-cost alternatives to using {@link #setData(Bundle) setData()} if you
     * only need to store a few integer values.
     */
    int32_t arg2;

    /**
     * An arbitrary object to send to the recipient.
     */
    sp<Object> obj;

    /**
     * Constructor (but the preferred way to get a Message is to call {@link #obtain()
     * Message.obtain()}).
     */
    Message();

    virtual ~Message() {
    }

    /**
     * Return a new Message instance from the global pool. Allows us to avoid allocating new objects
     * in many cases.
     */
    static sp<Message> obtain();

    /**
     * Same as {@link #obtain()}, but copies the values of an existing message (including its
     * target) into the new one.
     *
     * @param origMessage Original message to copy.
     * @return A Message object from the global pool.
     */
    static sp<Message> obtain(const sp<Message>& origMessage);

    /**
     * Same as {@link #obtain()}, but sets the value for the <em>target</em> member on the Message
     * returned.
     *
     * @param handler Handler to assign to the returned Message object's <em>target</em> member.
     * @return A Message object from the global pool.
     */
    static sp<Message> obtain(const sp<Handler>& handler);

    /**
     * Same as {@link #obtain(Handler)}, but assigns a callback Runnable on the Message that is
     * returned.
     *
     * @param handler Handler to assign to the returned Message object's <em>target</em> member.
     * @param callback Runnable that will execute when the message is handled.
     * @return A Message object from the global pool.
     */
    static sp<Message> obtain(const sp<Handler>& handler, const sp<Runnable>& callback);

    /**
     * Same as {@link #obtain()}, but sets the values for both <em>target</em> and <em>what</em>
     * members on the Message.
     *
     * @param handler Value to assign to the <em>target</em> member.
     * @param what Value to assign to the <em>what</em> member.
     * @return A Message object from the global pool.
     */
    static sp<Message> obtain(const sp<Handler>& handler, int32_t what);

    /**
     * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>, and
     * <em>obj</em> members.
     *
     * @param handler The <em>target</em> value to set.
     * @param what The <em>what</em> value to set.
     * @param obj The <em>object</em> method to set.
     * @return A Message object from the global pool.
     */
    static sp<Message> obtain(const sp<Handler>& handler, int32_t what, const sp<Object>& obj);

    /**
     * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>,
     * <em>arg1</em>, and <em>arg2</em> members.
     *
     * @param handler The <em>target</em> value to set.
     * @param what The <em>what</em> value to set.
     * @param arg1 The <em>arg1</em> value to set.
     * @param arg2 The <em>arg2</em> value to set.
     * @return A Message object from the global pool.
     */
    static sp<Message> obtain(const sp<Handler>& handler, int32_t what, int32_t arg1, int32_t arg2);

    /**
     * Same as {@link #obtain()}, but sets the values of the <em>target</em>, <em>what</em>,
     * <em>arg1</em>, <em>arg2</em>, and <em>obj</em> members.
     *
     * @param handler The <em>target</em> value to set.
     * @param what The <em>what</em> value to set.
     * @param arg1 The <em>arg1</em> value to set.
     * @param arg2 The <em>arg2</em> value to set.
     * @param obj The <em>obj</em> value to set.
     * @return A Message object from the global pool.
     */
    static sp<Message> obtain(const sp<Handler>& handler, int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj);

    /**
     * Make this message like otherMessage. Performs a shallow copy of the data field. Does not copy the linked
     * list fields, nor the timestamp or target/callback of the original message.
     */
    void copyFrom(const sp<Message>& otherMessage);

    /**
     * Return the targeted delivery time of this message, in milliseconds.
     */
    uint64_t getWhen() const {
        return when;
    }

    void setTarget(const sp<Handler>& target) {
        this->target = target;
    }

    /**
     * Retrieve the a {@link mindroid.os.Handler Handler} implementation that will receive this
     * message. The object must implement
     * {@link mindroid.os.Handler#handleMessage(mindroid.os.Message) Handler.handleMessage()}. Each
     * Handler has its own name-space for message codes, so you do not need to worry about yours
     * conflicting with other handlers.
     */
    sp<Handler> getTarget() const {
        return target;
    }

    /**
     * Retrieve callback object that will execute when this message is handled. This object must
     * implement Runnable. This is called by the <em>target</em> {@link Handler} that is receiving
     * this Message to dispatch it. If not set, the message will be dispatched to the receiving
     * Handler's {@link Handler#handleMessage(Message Handler.handleMessage())}.
     */
    sp<Runnable> getCallback() const {
        return callback;
    }

    /**
     * Obtains a Bundle of arbitrary data associated with this event, lazily creating it if
     * necessary. Set this value by calling {@link #setData(Bundle)}.
     *
     * @see #peekData()
     * @see #setData(Bundle)
     */
    sp<Bundle> getData() {
        if (data == nullptr) {
            data = new Bundle();
        }
        return data;
    }

    /**
     * Like getData(), but does not lazily create the Bundle. A null is returned if the Bundle does
     * not already exist. See {@link #getData} for further information on this.
     *
     * @see #getData()
     * @see #setData(Bundle)
     */
    sp<Bundle> peekData() {
        return data;
    }

    /**
     * Sets a Bundle of arbitrary data values. Use arg1 and arg1 members as a lower cost way to send
     * a few simple integer values, if you can.
     *
     * @see #getData()
     * @see #peekData()
     */
    void setData(const sp<Bundle>& data) {
        this->data = data;
    }

    /**
     * Sends this Message to the Handler specified by {@link #getTarget}. Throws a null pointer
     * exception if this field has not been set.
     */
    void sendToTarget();

private:
    bool isInUse() {
        return ((flags & FLAG_IN_USE) == FLAG_IN_USE);
    }

    void markInUse() {
        flags |= FLAG_IN_USE;
    }

    /**
     * Return a Message instance to the global pool. You MUST NOT touch the Message after calling
     * this function -- it has effectively been freed.
     */
    void recycle();

    static const int32_t FLAG_IN_USE = 1 << 0;

    int32_t flags;
    uint64_t when;
    sp<Bundle> data;
    sp<Handler> target;
    sp<Runnable> callback;
    sp<Message> nextMessage;
    static MessagePool sMessagePool;

    /**
     * Optional result. The semantics of exactly how this is used are up to the sender and receiver.
     *
     * @hide
     */
    sp<Cancellable> result;

    friend class MessageQueue;
    friend class Looper;
    friend class Handler;
    friend struct MessagePool;
    friend class Binder;
};

} /* namespace mindroid */

#endif /* MINDROID_MESSAGE_H_ */
