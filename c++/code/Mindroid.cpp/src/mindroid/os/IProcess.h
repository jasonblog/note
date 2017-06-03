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

#ifndef MINDROID_IPROCESS_H_
#define MINDROID_IPROCESS_H_

#include "mindroid/os/Binder.h"
#include "mindroid/lang/Class.h"

namespace mindroid {

class Intent;
class IRemoteCallback;
class ServiceConnection;

class IProcess :
        public IInterface {
public:
    virtual void createService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) = 0;
    virtual void startService(const sp<Intent>& intent, int32_t flags, int32_t startId, const sp<IRemoteCallback>& callback) = 0;
    virtual void stopService(const sp<Intent>& intent) = 0;
    virtual void stopService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) = 0;
    virtual void bindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) = 0;
    virtual void unbindService(const sp<Intent>& intent) = 0;
    virtual void unbindService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) = 0;
};

namespace binder {

class Process {
public:
    class Stub : public Binder, public IProcess {
    public:
        Stub() {
            this->attachInterface(this, String::valueOf(DESCRIPTOR));
        }

        static sp<IProcess> asInterface(const sp<IBinder>& binder) {
            if (binder == nullptr) {
                return nullptr;
            }
            return new Process::Stub::SmartProxy(binder);
        }

        virtual sp<IBinder> asBinder() {
            return this;
        }

    protected:
        virtual void onTransact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Bundle>& data, const sp<Object>& result);

    private:
        class Proxy : public IProcess {
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

            virtual void createService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) override;
            virtual void startService(const sp<Intent>& intent, int32_t flags, int32_t startId, const sp<IRemoteCallback>& callback) override;
            virtual void stopService(const sp<Intent>& intent) override;
            virtual void stopService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) override;
            virtual void bindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) override;
            virtual void unbindService(const sp<Intent>& intent) override;
            virtual void unbindService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) override;

        private:
            sp<IBinder> mRemote;
        };

        class SmartProxy : public IProcess {
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

            virtual void createService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) override;
            virtual void startService(const sp<Intent>& intent, int32_t flags, int32_t startId, const sp<IRemoteCallback>& callback) override;
            virtual void stopService(const sp<Intent>& intent) override;
            virtual void stopService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) override;
            virtual void bindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) override;
            virtual void unbindService(const sp<Intent>& intent) override;
            virtual void unbindService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) override;

        private:
            sp<IBinder> mRemote;
            sp<IProcess> mStub;
            sp<IProcess> mProxy;
        };

        static const char* const DESCRIPTOR;
        static const int32_t MSG_CREATE_SERVICE = 1;
        static const int32_t MSG_START_SERVICE = 2;
        static const int32_t MSG_STOP_SERVICE = 3;
        static const int32_t MSG_BIND_SERVICE = 4;
        static const int32_t MSG_UNBIND_SERVICE = 5;
    };
};

} /* namespace binder */
} /* namespace mindroid */

#endif /* MINDROID_IPROCESS_H_ */
