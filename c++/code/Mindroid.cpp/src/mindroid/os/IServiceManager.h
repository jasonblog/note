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

#ifndef MINDROID_ISERVICEMANAGER_H_
#define MINDROID_ISERVICEMANAGER_H_

#include "mindroid/util/concurrent/Promise.h"
#include "mindroid/lang/Object.h"
#include "mindroid/lang/Class.h"
#include "mindroid/os/Binder.h"
#include "mindroid/content/ComponentName.h"
#include "mindroid/content/Intent.h"
#include "mindroid/content/ServiceConnection.h"

namespace mindroid {

class IRemoteCallback;

class IServiceManager :
        public IInterface {
public:
    virtual sp<ComponentName> startService(const sp<Intent>& service) = 0;
    virtual bool stopService(const sp<Intent>& service) = 0;
    virtual bool bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) = 0;
    virtual void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn) = 0;
    virtual void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback) = 0;
    virtual sp<ComponentName> startSystemService(const sp<Intent>& service) = 0;
    virtual bool stopSystemService(const sp<Intent>& service) = 0;
};

namespace binder {

class ServiceManager {
public:
    class Stub : public Binder, public IServiceManager {
    public:
        Stub() {
            this->attachInterface(this, String::valueOf(DESCRIPTOR));
        }

        static sp<IServiceManager> asInterface(const sp<IBinder>& binder) {
            if (binder == nullptr) {
                return nullptr;
            }
            return new ServiceManager::Stub::SmartProxy(binder);
        }

        virtual sp<IBinder> asBinder() {
            return this;
        }

        virtual void onTransact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Bundle>& data, const sp<Object>& result);

        class Proxy : public IServiceManager {
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

            virtual sp<ComponentName> startService(const sp<Intent>& service) override;
            virtual bool stopService(const sp<Intent>& service) override;
            virtual bool bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) override;
            virtual void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn) override;
            virtual void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback) override;
            virtual sp<ComponentName> startSystemService(const sp<Intent>& service) override;
            virtual bool stopSystemService(const sp<Intent>& service) override;

        private:
            sp<IBinder> mRemote;
        };

        class SmartProxy : public IServiceManager {
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

            virtual sp<ComponentName> startService(const sp<Intent>& service) override;
            virtual bool stopService(const sp<Intent>& service) override;
            virtual bool bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) override;
            virtual void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn) override;
            virtual void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback) override;
            virtual sp<ComponentName> startSystemService(const sp<Intent>& service) override;
            virtual bool stopSystemService(const sp<Intent>& service) override;

        private:
            sp<IBinder> mRemote;
            sp<IServiceManager> mStub;
            sp<IServiceManager> mProxy;
        };

    private:
        static const char* const DESCRIPTOR;
        static const int32_t MSG_START_SERVICE = 1;
        static const int32_t MSG_STOP_SERVICE = 2;
        static const int32_t MSG_BIND_SERVICE = 3;
        static const int32_t MSG_UNBIND_SERVICE = 4;
        static const int32_t MSG_START_SYSTEM_SERVICE = 5;
        static const int32_t MSG_STOP_SYSTEM_SERVICE = 6;
    };
};

} /* namespace binder */
} /* namespace mindroid */

#endif /* MINDROID_ISERVICEMANAGER_H_ */
