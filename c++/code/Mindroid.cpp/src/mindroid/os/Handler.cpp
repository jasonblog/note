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

#include "mindroid/os/Handler.h"
#include "mindroid/os/Message.h"
#include "mindroid/os/MessageQueue.h"
#include "mindroid/os/Looper.h"
#include "mindroid/util/Assert.h"

namespace mindroid {

Handler::Handler() : Handler(Looper::myLooper()) {
}

Handler::Handler(const sp<Callback>& callback) : Handler(Looper::myLooper(), callback) {
}

Handler::Handler(const sp<Looper>& looper) {
    mLooper = looper;
    Assert::assertNotNull("Can't create handler inside thread that has not called Looper.prepare()", mLooper);
    mMessageQueue = looper->mMessageQueue;
    mCallback = nullptr;
}

Handler::Handler(const sp<Looper>& looper, const sp<Callback>& callback) {
    mLooper = looper;
    Assert::assertNotNull("Can't create handler inside thread that has not called Looper.prepare()", mLooper);
    mMessageQueue = looper->mMessageQueue;
    mCallback = callback;
}

void Handler::dispatchMessage(const sp<Message>& msg) {
    if (msg->callback != nullptr) {
        handleCallback(msg);
    } else {
        if (mCallback != nullptr) {
            if (mCallback->handleMessage(msg)) {
                return;
            }
        }
        handleMessage(msg);
    }
}

sp<Closure> Handler::post(const std::function<void (void)>& func) {
    if (func) {
        const sp<Message> message = Message::obtain();
        sp<Closure> closure = new Closure(sp<Handler>(this), func);
        message->callback = closure;
        return sendMessage(message) ? closure : nullptr;
    } else {
        return nullptr;
    }
}

sp<Closure> Handler::post(std::function<void (void)>&& func) {
    if (func) {
        const sp<Message> message = Message::obtain();
        sp<Closure> closure = new Closure(sp<Handler>(this), std::move(func));
        message->callback = closure;
        return sendMessage(message) ? closure : nullptr;
    } else {
        return nullptr;
    }
}

sp<Closure> Handler::postAtTime(const std::function<void (void)>& func, uint64_t uptimeMillis) {
    if (func) {
        const sp<Message> message = Message::obtain();
        sp<Closure> closure = new Closure(sp<Handler>(this), func);
        message->callback = closure;
        return sendMessageAtTime(message, uptimeMillis) ? closure : nullptr;
    } else {
        return nullptr;
    }
}

sp<Closure> Handler::postAtTime(std::function<void (void)>&& func, uint64_t uptimeMillis) {
    if (func) {
        const sp<Message> message = Message::obtain();
        sp<Closure> closure = new Closure(sp<Handler>(this), std::move(func));
        message->callback = closure;
        return sendMessageAtTime(message, uptimeMillis) ? closure : nullptr;
    } else {
        return nullptr;
    }
}

sp<Closure> Handler::postDelayed(const std::function<void (void)>& func, uint32_t delayMillis) {
    if (func) {
        const sp<Message> message = Message::obtain();
        sp<Closure> closure = new Closure(sp<Handler>(this), func);
        message->callback = closure;
        return sendMessageDelayed(message, delayMillis) ? closure : nullptr;
    } else {
        return nullptr;
    }
}

sp<Closure> Handler::postDelayed(std::function<void (void)>&& func, uint32_t delayMillis) {
    if (func) {
        const sp<Message> message = Message::obtain();
        sp<Closure> closure = new Closure(sp<Handler>(this), std::move(func));
        message->callback = closure;
        return sendMessageDelayed(message, delayMillis) ? closure : nullptr;
    } else {
        return nullptr;
    }
}

bool Handler::removeCallbacks(const sp<Runnable>& runnable) {
    return mMessageQueue->removeCallbacks(this, runnable, nullptr);
}

bool Handler::removeCallbacks(const sp<Runnable>& runnable, const sp<Object>& token) {
    return mMessageQueue->removeCallbacks(this, runnable, token);
}

bool Handler::sendMessageAtTime(const sp<Message>& message, uint64_t uptimeMillis) {
    message->target = this;
    return mMessageQueue->enqueueMessage(message, uptimeMillis);
}

bool Handler::removeMessages(int32_t what) {
    return mMessageQueue->removeMessages(this, what, nullptr);
}

bool Handler::removeMessages(int32_t what, const sp<Object>& object) {
    return mMessageQueue->removeMessages(this, what, object);
}

bool Handler::removeCallbacksAndMessages(const sp<Object>& token) {
    return mMessageQueue->removeCallbacksAndMessages(this, token);
}

bool Handler::hasMessages(int32_t what) {
    return mMessageQueue->hasMessages(this, what, nullptr);
}

bool Handler::hasMessages(int32_t what, const sp<Object>& object) {
    return mMessageQueue->hasMessages(this, what, object);
}

bool Handler::hasCallbacks(const sp<Runnable>& runnable) {
    return mMessageQueue->hasMessages(this, runnable, nullptr);
}

sp<Message> Handler::getPostMessage(const sp<Runnable>& runnable) {
    const sp<Message> message = Message::obtain();
    message->callback = runnable;
    return message;
}

sp<Message> Handler::getPostMessage(const sp<Runnable>& runnable, const sp<Object>& token) {
    sp<Message> message = Message::obtain();
    message->obj = token;
    message->callback = runnable;
    return message;
}

} /* namespace mindroid */
