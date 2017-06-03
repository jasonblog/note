/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2013 Daniel Himmelein
 * Copyright (C) 2016 E.S.R.Labs
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

#include "mindroid/app/ContextImpl.h"
#include "mindroid/app/SharedPreferencesImpl.h"
#include "mindroid/content/pm/PackageManager.h"
#include "mindroid/os/ServiceManager.h"
#include "mindroid/os/Environment.h"
#include "mindroid/util/Log.h"

namespace mindroid {

const char* const ContextImpl::TAG = "ContextImpl";

ContextImpl::ContextImpl(const sp<HandlerThread>& mainThread, const sp<ComponentName>& component) :
        mServiceConnections(new HashMap<sp<ServiceConnection>, sp<Intent>>()),
        mServiceConnectionCallbacks(new ArrayList<sp<RemoteCallback>>()) {
    mServiceManager = ServiceManager::getServiceManager();
    mMainThread = mainThread;
    mHandler = new Handler(mainThread->getLooper());
    mComponent = component;
}

sp<PackageManager> ContextImpl::getPackageManager() {
    if (mPackageManager != nullptr) {
        return mPackageManager;
    }

    return (mPackageManager = new PackageManager(this));
}

sp<String> ContextImpl::getPackageName() {
    if (mComponent != nullptr) {
        return mComponent->getPackageName();
    }
    return String::valueOf("mindroid");
}

sp<SharedPreferences> ContextImpl::getSharedPreferences(const sp<String>& name, int32_t mode) {
    return Environment::getSharedPreferences(getSharedPrefsFile(name),mode);
}

sp<IBinder> ContextImpl::getSystemService(const sp<String>& name) {
    if (name != nullptr) {
        return ServiceManager::getSystemService(name);
    } else {
        return nullptr;
    }
}

sp<ComponentName> ContextImpl::startService(const sp<Intent>& service) {
    if (service != nullptr) {
        try {
            return mServiceManager->startService(service);
        } catch (const RemoteException& e) {
            Assert::fail("System failure");
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

bool ContextImpl::stopService(const sp<Intent>& service) {
    if (service != nullptr) {
        try {
            return mServiceManager->stopService(service);
        } catch (const RemoteException& e) {
            Assert::fail("System failure");
            return false;
        }
    } else {
        return false;
    }
}

bool ContextImpl::bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags) {
    if (service != nullptr && conn != nullptr) {
        if (mServiceConnections->containsKey(conn)) {
            return true;
        }
        mServiceConnections->put(conn, service);

        class OnResultListener : public RemoteCallback::OnResultListener {
        public:
            OnResultListener(const wp<ContextImpl>& context, const sp<Intent>& service, const sp<ServiceConnection>& conn) :
                mContext(context),
                mService(service),
                mConn(conn) {
            }

        protected:
            void onResult(const sp<Bundle>& data) {
                sp<ContextImpl> context = mContext.lock();
                if (context != nullptr) {
                    bool result = data->getBoolean("result");
                    if (result) {
                        sp<IBinder> binder = data->getBinder("binder");
                        mConn->onServiceConnected(mService->getComponent(), binder);
                    } else {
                        Log::e(ContextImpl::TAG, "Cannot bind to service %s", mService->getComponent()->toShortString()->c_str());
                    }

                    context->mServiceConnectionCallbacks->remove(getCallback());
                }
            }

        private:
            wp<ContextImpl> mContext;
            const sp<Intent> mService;
            const sp<ServiceConnection> mConn;
        };

        sp<RemoteCallback> callback = new RemoteCallback(new OnResultListener(this, service, conn), mHandler);
        mServiceConnectionCallbacks->add(callback);
        try {
            return mServiceManager->bindService(service, conn, flags, callback->asInterface());
        } catch (const RemoteException& e) {
            Assert::fail("System failure");
            return false;
        }
    } else {
        return false;
    }
}

void ContextImpl::unbindService(const sp<ServiceConnection>& conn) {
    if (conn != nullptr) {
        if (mServiceConnections->containsKey(conn)) {
            sp<Intent> service = mServiceConnections->get(conn);
            mServiceConnections->remove(conn);
            try {
                mServiceManager->unbindService(service, conn);
            } catch (const RemoteException& e) {
            }
        }
    }
}

sp<File> ContextImpl::getPreferencesDir() {
    if (!Environment::getPreferencesDirectory()->exists()) {
        Environment::getPreferencesDirectory()->mkdir();
    }
    return Environment::getPreferencesDirectory();
}

sp<File> ContextImpl::makeFilename(const sp<File>& baseDir, const sp<String>& name) {
    if (name->indexOf(File::separatorChar) < 0) {
        return new File(baseDir, name);
    }
    Assert::assertTrue(String::format("File %s contains a path separator", name->c_str())->c_str(), false);
    return nullptr;
}

void ContextImpl::cleanup() {
    auto itr = mServiceConnections->iterator();
    while (itr.hasNext()) {
        auto entry = itr.next();
        sp<ServiceConnection> conn = entry.getKey();
        sp<Intent> service = entry.getValue();
        itr.remove();
        mServiceManager->unbindService(service, conn);
        Log::w(TAG, "Service %s is leaking a ServiceConnection to %s", mComponent->toString()->c_str(), service->getComponent()->toString()->c_str());
    }
}

} /* namespace mindroid */
