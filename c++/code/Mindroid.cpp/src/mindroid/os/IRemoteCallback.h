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

#ifndef MINDROID_IREMOTECALLBACK_H_
#define MINDROID_IREMOTECALLBACK_H_

#include "mindroid/os/Binder.h"
#include "mindroid/lang/Class.h"

namespace mindroid {

class IRemoteCallback :
        public IInterface {
public:
    virtual void sendResult(const sp<Bundle>& data) = 0;
};

namespace binder {

class RemoteCallback {
public:
    class Stub : public Binder, public IRemoteCallback {
    public:
        Stub() {
            this->attachInterface(this, String::valueOf(DESCRIPTOR));
        }

        static sp<IRemoteCallback> asInterface(const sp<IBinder>& binder) {
            if (binder == nullptr) {
                return nullptr;
            }
            return new RemoteCallback::Stub::SmartProxy(binder);
        }

        virtual sp<IBinder> asBinder() {
            return this;
        }

    protected:
        virtual void onTransact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Bundle>& data, const sp<Object>& result);

    private:
        class Proxy : public IRemoteCallback {
        public:
            Proxy(const sp<IBinder>& remote) {
                mRemote = remote;
            }

            virtual sp<IBinder> asBinder() override {
                return mRemote;
            }

            bool equals(const sp<Object>& obj) const override {
                if (obj == nullptr) return false;
                if (obj == this) return true;
                if (Class<Proxy>::isInstance(obj)) {
                    sp<Proxy> other = Class<Proxy>::cast(obj);
                    return mRemote->equals(other->mRemote);
                } else {
                    return false;
                }
            }

            size_t hashCode() const override {
                return mRemote->hashCode();
            }

            virtual void sendResult(const sp<Bundle>& data) override;

        private:
            sp<IBinder> mRemote;
        };

        class SmartProxy : public IRemoteCallback {
        public:
            SmartProxy(const sp<IBinder>& remote);

            virtual sp<IBinder> asBinder() override {
                return mRemote;
            }

            bool equals(const sp<Object>& obj) const override {
                if (obj == nullptr) return false;
                if (obj == this) return true;
                if (Class<SmartProxy>::isInstance(obj)) {
                    sp<SmartProxy> other = Class<SmartProxy>::cast(obj);
                    return mRemote->equals(other->mRemote);
                } else {
                    return false;
                }
            }

            size_t hashCode() const override {
                return mRemote->hashCode();
            }

            virtual void sendResult(const sp<Bundle>& data) override;

        private:
            sp<IBinder> mRemote;
            sp<IRemoteCallback> mStub;
            sp<IRemoteCallback> mProxy;
        };

        static const char* const DESCRIPTOR;
        static const int32_t MSG_SEND_RESULT = 1;
    };
};

} /* namespace binder */
} /* namespace mindroid */

#endif /* MINDROID_IREMOTECALLBACK_H_ */
