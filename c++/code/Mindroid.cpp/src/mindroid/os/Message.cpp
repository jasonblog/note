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

#include "mindroid/os/Message.h"
#include "mindroid/os/Handler.h"
#include "mindroid/util/Assert.h"
#include "mindroid/util/concurrent/Cancellable.h"

namespace mindroid {

MessagePool Message::sMessagePool;

MessagePool::MessagePool() :
        MAX_SIZE(42),
        size(0),
        lock(new ReentrantLock()) {
}

MessagePool::~MessagePool() {
    AutoLock autoLock(lock);
    MAX_SIZE = 0;
    size = 0;
    pool = nullptr;
}

Message::Message() :
        what(0),
        arg1(0),
        arg2(0),
        obj(nullptr),
        flags(0),
        when(0),
        data(nullptr),
        target(nullptr),
        callback(nullptr),
        nextMessage(nullptr) {
}

sp<Message> Message::obtain() {
    {
        AutoLock autoLock(sMessagePool.lock);
        if (sMessagePool.pool != nullptr) {
            sp<Message> message = sMessagePool.pool;
            sMessagePool.pool = message->nextMessage;
            message->nextMessage = nullptr;
            message->flags = 0;
            sMessagePool.size--;
            return message;
        }
    }
    Message* message = new Message();
    Assert::assertNotNull(message);
    return message;
}

sp<Message> Message::obtain(const sp<Message>& origMessage) {
    sp<Message> message = obtain();
    message->what = origMessage->what;
    message->arg1 = origMessage->arg1;
    message->arg2 = origMessage->arg2;
    message->obj = origMessage->obj;
    if (origMessage->data != nullptr) {
        message->data = new Bundle(origMessage->data);
    }
    message->target = origMessage->target;
    message->callback = origMessage->callback;

    return message;
}

sp<Message> Message::obtain(const sp<Handler>& handler) {
    sp<Message> message = obtain();
    message->target = handler;
    return message;
}

sp<Message> Message::obtain(const sp<Handler>& handler, const sp<Runnable>& callback) {
    sp<Message> message = obtain();
    message->target = handler;
    message->callback = callback;
    return message;
}

sp<Message> Message::obtain(const sp<Handler>& handler, int32_t what) {
    sp<Message> message = obtain();
    message->target = handler;
    message->what = what;
    return message;
}

sp<Message> Message::obtain(const sp<Handler>& handler, int32_t what, const sp<Object>& obj) {
    sp<Message> message = obtain();
    message->target = handler;
    message->what = what;
    message->obj = obj;
    return message;
}

sp<Message> Message::obtain(const sp<Handler>& handler, int32_t what, int32_t arg1, int32_t arg2) {
    sp<Message> message = obtain();
    message->target = handler;
    message->what = what;
    message->arg1 = arg1;
    message->arg2 = arg2;
    return message;
}

sp<Message> Message::obtain(const sp<Handler>& handler, int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj) {
    sp<Message> message = obtain();
    message->target = handler;
    message->what = what;
    message->arg1 = arg1;
    message->arg2 = arg2;
    message->obj = obj;
    return message;
}

/**
 * Return a Message instance to the global pool. You MUST NOT touch the Message after calling
 * this function -- it has effectively been freed.
 */
void Message::recycle() {
    if (result != nullptr) {
        result->cancel();
    }

    flags = FLAG_IN_USE;
    what = 0;
    arg1 = 0;
    arg2 = 0;
    obj = nullptr;
    when = 0;
    target = nullptr;
    callback = nullptr;
    data = nullptr;
    result = nullptr;

    {
        AutoLock autoLock(sMessagePool.lock);
        if (sMessagePool.size < sMessagePool.MAX_SIZE) {
            nextMessage = sMessagePool.pool;
            sMessagePool.pool = this;
            sMessagePool.size++;
        }
    }
}

void Message::copyFrom(const sp<Message>& otherMessage) {
    this->what = otherMessage->what;
    this->arg1 = otherMessage->arg1;
    this->arg2 = otherMessage->arg2;
    this->obj = otherMessage->obj;
    if (otherMessage->data != nullptr) {
        this->data = new Bundle(otherMessage->data);
    } else {
        this->data = nullptr;
    }
}

void Message::sendToTarget() {
    target->sendMessage(this);
}

} /* namespace mindroid */
