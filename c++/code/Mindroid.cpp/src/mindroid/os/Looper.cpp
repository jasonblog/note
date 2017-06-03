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

#include "mindroid/os/Looper.h"
#include "mindroid/os/Handler.h"

namespace mindroid {

pthread_once_t Looper::sTlsOneTimeInitializer = PTHREAD_ONCE_INIT;
pthread_key_t Looper::sTlsKey;

Looper::Looper(bool quitAllowed) {
    mMessageQueue = new MessageQueue(quitAllowed);
    mThread = Thread::currentThread();
}

void Looper::init() {
    pthread_key_create(&sTlsKey, Looper::finalize);
}

void Looper::finalize(void* l) {
    sp<Looper> looper((Looper*) l);
    looper->mSelf = nullptr;
}

bool Looper::prepare(bool quitAllowed) {
    pthread_once(&sTlsOneTimeInitializer, Looper::init);
    Looper* l = (Looper*) pthread_getspecific(sTlsKey);
    if (l == nullptr) {
        sp<Looper> looper = new Looper(quitAllowed);
        if (looper != nullptr) {
            if (pthread_setspecific(sTlsKey, looper.getPointer()) == 0) {
                looper->mSelf = looper;
                return true;
            } else {
                looper = nullptr;
                return false;
            }
        } else {
            return false;
        }
    } else {
        Assert::assertNull("Only one Looper may be created per thread", l);
        return false;
    }
}

void Looper::loop() {
    sp<Looper> me = myLooper();
    Assert::assertNotNull("No Looper; Looper.prepare() wasn't called on this thread", me);

    sp<MessageQueue> mq = me->mMessageQueue;
    for (;;) {
        sp<Message> message = mq->dequeueMessage();
        if (message == nullptr) {
            return;
        }
        message->target->dispatchMessage(message);
        message->recycle();
    }
}

sp<Looper> Looper::myLooper() {
    pthread_once(&sTlsOneTimeInitializer, Looper::init);
    Looper* looper = (Looper*) pthread_getspecific(sTlsKey);
    return looper;
}

void Looper::quit() {
    mMessageQueue->quit();
}

} /* namespace mindroid */
