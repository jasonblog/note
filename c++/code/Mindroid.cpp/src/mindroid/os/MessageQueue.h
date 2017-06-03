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

#ifndef MINDROID_MESSAGEQUEUE_H_
#define MINDROID_MESSAGEQUEUE_H_

#include "mindroid/lang/Object.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"

namespace mindroid {

class Message;
class Handler;
class Runnable;

/**
 * Low-level class holding the list of messages to be dispatched by a {@link Looper}. Messages are
 * not added directly to a MessageQueue, but rather through {@link Handler} objects associated with
 * the Looper.
 *
 * <p>
 * You can retrieve the MessageQueue for the current thread with {@link Looper#myQueue()
 * Looper.myQueue()}.
 */
class MessageQueue final :
        public Object {
public:
    MessageQueue(bool quitAllowed);
    virtual ~MessageQueue();
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;

    bool quit();
    bool enqueueMessage(const sp<Message>& message, uint64_t when);
    sp<Message> dequeueMessage();
    bool hasMessages(const sp<Handler>& handler, int32_t what, const sp<Object>& object);
    bool hasMessages(const sp<Handler>& handler, const sp<Runnable>& runnable, const sp<Object>& object);
    bool removeMessages(const sp<Handler>& handler, int32_t what, const sp<Object>& object);
    bool removeCallbacks(const sp<Handler>& handler, const sp<Runnable>& runnable, const sp<Object>& object);
    bool removeCallbacksAndMessages(const sp<Handler>& handler, const sp<Object>& object);

private:
    static const char* const TAG;

    sp<Message> mMessages;
    sp<ReentrantLock> mLock;
    sp<Condition> mCondition;
    const bool mQuitAllowed;
    bool mQuitting;
};

} /* namespace mindroid */

#endif /* MINDROID_MESSAGEQUEUE_H_ */
