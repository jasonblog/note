/*
 * Copyright (C) 2016 Daniel Himmelein
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

#include "mindroid/os/IRemoteCallback.h"

namespace mindroid {
namespace binder {

const char* const RemoteCallback::Stub::DESCRIPTOR = "mindroid.os.IRemoteCallback";

void RemoteCallback::Stub::onTransact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Bundle>& data, const sp<Object>& result) {
    switch (what) {
    case MSG_SEND_RESULT: {
        sendResult(data);
        break;
    }
    default:
        Binder::onTransact(what, arg1, arg2, obj, data, result);
        break;
    }
}

void RemoteCallback::Stub::Proxy::sendResult(const sp<Bundle>& data) {
    mRemote->transact(MSG_SEND_RESULT, data, nullptr, FLAG_ONEWAY);
}


RemoteCallback::Stub::SmartProxy::SmartProxy(const sp<IBinder>& remote) {
    mRemote = remote;
    mStub = interface_cast<Stub>(remote->queryLocalInterface(DESCRIPTOR));
    mProxy = new RemoteCallback::Stub::Proxy(remote);
}

void RemoteCallback::Stub::SmartProxy::sendResult(const sp<Bundle>& data) {
    if (mRemote->runsOnSameThread()) {
        mStub->sendResult(data);
    } else {
        mProxy->sendResult(data);
    }
}

} /* namespace binder */
} /* namespace mindroid */
