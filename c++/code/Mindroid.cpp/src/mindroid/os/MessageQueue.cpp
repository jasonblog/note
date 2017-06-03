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

#include "mindroid/lang/Runnable.h"
#include "mindroid/lang/Integer.h"
#include "mindroid/lang/Math.h"
#include "mindroid/os/SystemClock.h"
#include "mindroid/os/MessageQueue.h"
#include "mindroid/os/Message.h"
#include "mindroid/os/Handler.h"
#include "mindroid/util/Log.h"
#include <climits>

namespace mindroid {

const char* const MessageQueue::TAG = "MessageQueue";

MessageQueue::MessageQueue(bool quitAllowed) :
        mMessages(nullptr),
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()),
        mQuitAllowed(quitAllowed),
        mQuitting(false) {
}

MessageQueue::~MessageQueue() { }

bool MessageQueue::quit() {
    if (!mQuitAllowed) {
        return false;
    }

    AutoLock autoLock(mLock);
    if (mQuitting) {
        return true;
    }
    mQuitting = true;

    sp<Message> curMessage = mMessages;
    while (curMessage != nullptr) {
        sp<Message> nextMessage = curMessage->nextMessage;
        curMessage->recycle();
        curMessage = nextMessage;
    }
    mMessages = nullptr;

    mCondition->signal();
    return true;
}

bool MessageQueue::enqueueMessage(const sp<Message>& message, uint64_t when) {
    if (message->target == nullptr) {
        Assert::assertNotNull("Message must have a target", message->target);
        return false;
    }

    AutoLock autoLock(mLock);

    if (message->isInUse()) {
        Assert::assertFalse("Message is already in use", message->isInUse());
        return false;
    }

    if (mQuitting) {
        Log::w(TAG, "%p is sending a message to a Handler on a dead thread", message->target.getPointer());
        message->recycle();
        return false;
    }

    message->markInUse();
    message->when = when;
    sp<Message> curMessage = mMessages;
    if (curMessage == nullptr || when == 0 || when < curMessage->when) {
        message->nextMessage = curMessage;
        mMessages = message;
        mCondition->signal();
    } else {
        sp<Message> prevMessage;
        for (;;) {
            prevMessage = curMessage;
            curMessage = curMessage->nextMessage;
            if (curMessage == nullptr || when < curMessage->when) {
                break;
            }
        }
        message->nextMessage = curMessage;
        prevMessage->nextMessage = message;
        mCondition->signal();
    }
    return true;
}

sp<Message> MessageQueue::dequeueMessage() {
    for (;;) {
        AutoLock autoLock(mLock);
        if (mQuitting) {
            return nullptr;
        }

        const uint64_t now = SystemClock::uptimeMillis();
        sp<Message> message = mMessages;

        if (message != nullptr) {
            if (now < message->when) {
                mCondition->await(Math::min(message->when - now, (uint64_t) Integer::MAX_VALUE));
            } else {
                mMessages = message->nextMessage;
                message->nextMessage = nullptr;
                return message;
            }
        } else {
            mCondition->await();
        }
    }
}

bool MessageQueue::hasMessages(const sp<Handler>& handler, int32_t what, const sp<Object>& object) {
    if (handler == nullptr) {
        return false;
    }

    AutoLock autoLock(mLock);
    sp<Message> curMessage = mMessages;
    while (curMessage != nullptr) {
        if (curMessage->target == handler && curMessage->what == what && (object == nullptr || curMessage->obj == object)) {
            return true;
        }
        curMessage = curMessage->nextMessage;
    }
    return false;
}

bool MessageQueue::hasMessages(const sp<Handler>& handler, const sp<Runnable>& runnable, const sp<Object>& object) {
    if (handler == nullptr) {
        return false;
    }

    AutoLock autoLock(mLock);
    sp<Message> curMessage = mMessages;
    while (curMessage != nullptr) {
        if (curMessage->target == handler && curMessage->callback == runnable && (object == nullptr || curMessage->obj == object)) {
            return true;
        }
        curMessage = curMessage->nextMessage;
    }
    return false;
}

bool MessageQueue::removeMessages(const sp<Handler>& handler, int32_t what, const sp<Object>& object) {
    if (handler == nullptr) {
        return false;
    }

    bool foundMessage = false;

    AutoLock autoLock(mLock);
    sp<Message> curMessage = mMessages;

    // Remove all matching messages at the front of the message queue.
    while (curMessage != nullptr && curMessage->target == handler && curMessage->what == what && (object == nullptr || curMessage->obj == object)) {
        foundMessage = true;
        sp<Message> nextMessage = curMessage->nextMessage;
        mMessages = nextMessage;
        curMessage->recycle();
        curMessage = nextMessage;
    }

    // Remove all matching messages after the front of the message queue.
    while (curMessage != nullptr) {
        sp<Message> nextMessage = curMessage->nextMessage;
        if (nextMessage != nullptr) {
            if (nextMessage->target == handler && nextMessage->what == what && (object == nullptr || nextMessage->obj == object)) {
                foundMessage = true;
                sp<Message> nextButOneMessage = nextMessage->nextMessage;
                nextMessage->recycle();
                curMessage->nextMessage = nextButOneMessage;
                continue;
            }
        }
        curMessage = nextMessage;
    }

    return foundMessage;
}

bool MessageQueue::removeCallbacks(const sp<Handler>& handler, const sp<Runnable>& runnable, const sp<Object>& object) {
    if (handler == nullptr || runnable == nullptr) {
        return false;
    }

    bool foundMessage = false;

    AutoLock autoLock(mLock);
    sp<Message> curMessage = mMessages;

    // Remove all matching messages at the front of the message queue.
    while (curMessage != nullptr && curMessage->target == handler && curMessage->callback == runnable && (object == nullptr || curMessage->obj == object)) {
        foundMessage = true;
        sp<Message> nextMessage = curMessage->nextMessage;
        mMessages = nextMessage;
        curMessage->recycle();
        curMessage = nextMessage;
    }

    // Remove all matching messages after the front of the message queue.
    while (curMessage != nullptr) {
        sp<Message> nextMessage = curMessage->nextMessage;
        if (nextMessage != nullptr) {
            if (nextMessage->target == handler && nextMessage->callback == runnable && (object == nullptr || nextMessage->obj == object)) {
                foundMessage = true;
                sp<Message> nextButOneMessage = nextMessage->nextMessage;
                nextMessage->recycle();
                curMessage->nextMessage = nextButOneMessage;
                continue;
            }
        }
        curMessage = nextMessage;
    }

    return foundMessage;
}

bool MessageQueue::removeCallbacksAndMessages(const sp<Handler>& handler, const sp<Object>& object) {
    if (handler == nullptr) {
        return false;
    }

    bool foundMessage = false;

    AutoLock autoLock(mLock);
    sp<Message> curMessage = mMessages;

    // Remove all matching messages at the front of the message queue.
    while (curMessage != nullptr && curMessage->target == handler && (object == nullptr || curMessage->obj == object)) {
        foundMessage = true;
        sp<Message> nextMessage = curMessage->nextMessage;
        mMessages = nextMessage;
        curMessage->recycle();
        curMessage = nextMessage;
    }

    // Remove all matching messages after the front of the message queue.
    while (curMessage != nullptr) {
        sp<Message> nextMessage = curMessage->nextMessage;
        if (nextMessage != nullptr) {
            if (nextMessage->target == handler && (object == nullptr || nextMessage->obj == object)) {
                foundMessage = true;
                sp<Message> nextButOneMessage = nextMessage->nextMessage;
                nextMessage->recycle();
                curMessage->nextMessage = nextButOneMessage;
                continue;
            }
        }
        curMessage = nextMessage;
    }

    return foundMessage;
}

} /* namespace mindroid */
