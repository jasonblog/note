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

#include "mindroid/os/Binder.h"
#include "mindroid/util/Log.h"
#include "mindroid/util/concurrent/Awaitable.h"

namespace mindroid {

const char* const Binder::TAG = "Binder";
const sp<String> Binder::EXCEPTION_MESSAGE = String::valueOf("Binder transaction failure");

void Binder::transact(int32_t what, const sp<Awaitable>& result, int32_t flags) {
    sp<Message> message = Message::obtain();
    message->what = what;
    if (flags == FLAG_ONEWAY) {
        mTarget->send(message);
    } else {
        message->result = result;
        mTarget->send(message);
        try {
            result->await();
        } catch (const CancellationException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        } catch (const ExecutionException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        }
    }
}

void Binder::transact(int32_t what, const sp<Object>& obj, const sp<Awaitable>& result, int32_t flags) {
    sp<Message> message = Message::obtain();
    message->what = what;
    message->obj = obj;
    if (flags == FLAG_ONEWAY) {
        mTarget->send(message);
    } else {
        message->result = result;
        mTarget->send(message);
        try {
            result->await();
        } catch (const CancellationException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        } catch (const ExecutionException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        }
    }
}

void Binder::transact(int32_t what, int32_t arg1, int32_t arg2, const sp<Awaitable>& result, int32_t flags) {
    sp<Message> message = Message::obtain();
    message->what = what;
    message->arg1 = arg1;
    message->arg2 = arg2;
    if (flags == FLAG_ONEWAY) {
        mTarget->send(message);
    } else {
        message->result = result;
        mTarget->send(message);
        try {
            result->await();
        } catch (const CancellationException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        } catch (const ExecutionException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        }
    }
}

void Binder::transact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Awaitable>& result, int32_t flags) {
    sp<Message> message = Message::obtain();
    message->what = what;
    message->arg1 = arg1;
    message->arg2 = arg2;
    message->obj = obj;
    if (flags == FLAG_ONEWAY) {
        mTarget->send(message);
    } else {
        message->result = result;
        mTarget->send(message);
        try {
            result->await();
        } catch (const CancellationException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        } catch (const ExecutionException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        }
    }
}

void Binder::transact(int32_t what, const sp<Bundle>& data, const sp<Awaitable>& result, int32_t flags) {
    sp<Message> message = Message::obtain();
    message->what = what;
    message->setData(data);
    if (flags == FLAG_ONEWAY) {
        mTarget->send(message);
    } else {
        message->result = result;
        mTarget->send(message);
        try {
            result->await();
        } catch (const CancellationException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        } catch (const ExecutionException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        }
    }
}

void Binder::transact(int32_t what, int32_t arg1, int32_t arg2, const sp<Bundle>& data, const sp<Awaitable>& result, int32_t flags) {
    sp<Message> message = Message::obtain();
    message->what = what;
    message->arg1 = arg1;
    message->arg2 = arg2;
    message->setData(data);
    if (flags == FLAG_ONEWAY) {
        mTarget->send(message);
    } else {
        message->result = result;
        mTarget->send(message);
        try {
            result->await();
        } catch (const CancellationException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        } catch (const ExecutionException& e) {
            throw RemoteException(EXCEPTION_MESSAGE);
        }
    }
}

} /* namespace mindroid */
