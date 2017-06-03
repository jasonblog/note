/*
 * Copyright (C) 2010 The Android Open Source Project
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

#ifndef MINDROID_REMOTECALLBACK_H_
#define MINDROID_REMOTECALLBACK_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/RuntimeException.h"
#include "mindroid/os/IRemoteCallback.h"
#include "mindroid/util/Log.h"

namespace mindroid {

/**
 * @hide
 */
class RemoteCallback :
        public Object {
public:
    class OnResultListener : public Object {
    public:
        virtual void onResult(const sp<Bundle>& result) = 0;

        sp<RemoteCallback> getCallback() {
            return mCallback.lock();
        }

    private:
        wp<RemoteCallback> mCallback;

        friend class RemoteCallback;
    };

    RemoteCallback(const sp<OnResultListener>& listener) :
            RemoteCallback(listener, nullptr) {
    }

    RemoteCallback(const sp<OnResultListener>& listener, const sp<Handler>& handler) {
        if (listener == nullptr) {
            Assert::assertNotNull("Listener cannot be null", listener);
        }
        listener->mCallback = this;
        mCallback = new Stub(listener, handler);
    }

    virtual ~RemoteCallback() {
        mCallback->dispose();
    }

    sp<IRemoteCallback> asInterface() {
        return mCallback;
    }

    bool equals(const sp<Object>& other) const override {
        if (other == nullptr) return false;
        if (other == this) return true;
        if (Class<RemoteCallback>::isInstance(other)) {
            sp<RemoteCallback> o = Class<RemoteCallback>::cast(other);
            return mCallback->asBinder()->equals(o->mCallback->asBinder());
        } else {
            return false;
        }
    }

    size_t hashCode() const override {
        return mCallback->asBinder()->hashCode();
    }

private:
    class Stub : public binder::RemoteCallback::Stub {
    public:
        Stub(const sp<OnResultListener>& listener, const sp<Handler>& handler) :
                mListener(listener), mHandler(handler) {
        }

        virtual void sendResult(const sp<Bundle>& result) {
            if (mHandler != nullptr) {
                mHandler->post([=] { mListener->onResult(result); });
            } else {
                mListener->onResult(result);
            }
        };

    private:
        sp<OnResultListener> mListener;
        sp<Handler> mHandler;
    };

    sp<IRemoteCallback> mCallback;
};

} /* namespace mindroid */

#endif /* MINDROID_REMOTECALLBACK_H_ */
