/*
 * Copyright (C) 2012 Daniel Himmelein
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

#include "mindroid/app/Service.h"
#include "mindroid/app/ContextImpl.h"
#include "mindroid/content/ComponentName.h"
#include "mindroid/content/Intent.h"
#include "mindroid/content/pm/IPackageManager.h"
#include "mindroid/os/Process.h"
#include "mindroid/os/RemoteCallback.h"
#include "mindroid/os/ServiceManager.h"
#include "mindroid/lang/Class.h"
#include "mindroid/util/concurrent/Promise.h"
#include "mindroid/util/Log.h"

namespace mindroid {

const char* const Process::TAG = "Process";

Process::Process(const sp<String>& name) :
        mName(name),
        mLock(new ReentrantLock()),
        mCondition(mLock->newCondition()) {
    mMainThread = new HandlerThread(String::format("Process {%s}", name->c_str()));
    mServices = new HashMap<sp<ComponentName>, sp<Service>>();
}

sp<IProcess> Process::start() {
    Log::d(TAG, "Starting process %s", mName->c_str());
    mMainThread->start();
    mMainHandler = new Handler(mMainThread->getLooper());
    sp<Promise<sp<binder::Process::Stub>>> promise = new Promise<sp<binder::Process::Stub>>();
    mMainHandler->post([=] { promise->set(new ProcessImpl(this)); });
    promise->done([&] {
        mStub = promise->get();
    })->fail([] {
        Assert::fail("System failure");
    });

    return binder::Process::Stub::asInterface(mStub);
}

void Process::stop(uint64_t timeout) {
    Log::d(TAG, "Stopping process %s", mName->c_str());

    {
        AutoLock autoLock(mLock);
        if (!mServices->isEmpty()) {
            sp<IProcess> process = binder::Process::Stub::asInterface(mStub);

            auto itr = mServices->iterator();
            while (itr.hasNext()) {
                auto entry = itr.next();
                sp<ComponentName> component = entry.getKey();
                sp<Intent> intent = new Intent();
                intent->setComponent(component);

                try {
                    process->stopService(intent);
                } catch (const RemoteException& e) {
                    Assert::fail("System failure");
                }
            }

            uint64_t start = SystemClock::uptimeMillis();
            uint64_t duration = timeout;
            while (!mServices->isEmpty() && (duration > 0)) {
                mCondition->await(duration);
                duration = start + timeout - SystemClock::uptimeMillis();
            }
        }
    }

    mMainThread->quit();
    mMainThread->join();

    Log::d(TAG, "Process %s has been stopped", mName->c_str());
}

bool Process::isAlive() const {
    return mMainThread->isAlive();
}

void Process::ProcessImpl::createService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) {
    sp<Service> service = nullptr;
    sp<Bundle> result = new Bundle();

    sp<String> componentName = String::format("%s::%s", intent->getComponent()->getPackageName()->c_str(), intent->getComponent()->getClassName()->c_str());
    if (intent->getBooleanExtra("systemService", false)) {
        sp<Class<Service>> clazz = Class<Service>::forName(componentName);
        service = clazz->newInstance();
    } else {
        if (mProcess->mPackageManager == nullptr) {
            mProcess->mPackageManager = binder::PackageManager::Stub::asInterface(ServiceManager::getSystemService(Context::PACKAGE_MANAGER));
            Assert::assertNotNull("System failure", mProcess->mPackageManager);
        }
        sp<ResolveInfo> resolveInfo = nullptr;
        sp<ServiceInfo> serviceInfo = nullptr;
        try {
            resolveInfo = mProcess->mPackageManager->resolveService(intent, 0);
        } catch (const RemoteException& e) {
            Assert::fail("System failure");
        }

        if (resolveInfo != nullptr && resolveInfo->serviceInfo != nullptr) {
            serviceInfo = resolveInfo->serviceInfo;
            if (serviceInfo->isEnabled()) {
                sp<Class<Service>> clazz = Class<Service>::forName(componentName);
                service = clazz->newInstance();
            } else {
                Log::e(Process::TAG, "Service not enabled %s", intent->getComponent()->toShortString()->c_str());
            }
        } else {
            Log::e(Process::TAG, "Unknown service %s", intent->getComponent()->toShortString()->c_str());
        }
    }

    if (service != nullptr) {
        service->attach(new ContextImpl(mProcess->mMainThread, intent->getComponent()), binder::Process::Stub::asInterface(this), intent->getComponent()->getClassName());
        service->onCreate();
        result->putBoolean("result", true);

        {
            AutoLock autoLock(mProcess->mLock);
            mProcess->mServices->put(intent->getComponent(), service);
        }
    } else {
        Log::e(Process::TAG, "Cannot find and instantiate class %s", componentName->c_str());
        result->putBoolean("result", false);
    }

    callback->sendResult(result);
}

void Process::ProcessImpl::startService(const sp<Intent>& intent, int32_t flags, int32_t startId, const sp<IRemoteCallback>& callback) {
    sp<Service> service;
    sp<Bundle> result = new Bundle();

    {
        AutoLock autoLock(mProcess->mLock);
        service = mProcess->mServices->get(intent->getComponent());
    }
    if (service != nullptr) {
        service->onStartCommand(intent, flags, startId);
        result->putBoolean("result", true);
    } else {
        result->putBoolean("result", false);
    }

    callback->sendResult(result);
}

void Process::ProcessImpl::stopService(const sp<Intent>& intent) {
    stopService(intent, nullptr);
}

void Process::ProcessImpl::stopService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) {
    sp<Service> service;
    sp<Bundle> result = new Bundle();

    {
        AutoLock autoLock(mProcess->mLock);
        service = mProcess->mServices->get(intent->getComponent());
    }
    if (service != nullptr) {
        service->onDestroy();
        result->putBoolean("result", true);

        sp<Context> context = service->getBaseContext();
        context->cleanup();
        {
            AutoLock autoLock(mProcess->mLock);
            mProcess->mServices->remove(intent->getComponent());
            mProcess->mCondition->signalAll();
        }
    } else {
        result->putBoolean("result", false);
    }

    if (callback != nullptr) {
        callback->sendResult(result);
    }
}

void Process::ProcessImpl::bindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) {
    sp<Service> service;
    sp<Bundle> result = new Bundle();

    {
        AutoLock autoLock(mProcess->mLock);
        service = mProcess->mServices->get(intent->getComponent());
    }
    if (service != nullptr) {
        sp<IBinder> binder = service->onBind(intent);
        result->putBoolean("result", true);
        result->putBinder("binder", binder);
    } else {
        result->putBoolean("result", false);
    }

    callback->sendResult(result);
}

void Process::ProcessImpl::unbindService(const sp<Intent>& intent) {
    unbindService(intent, nullptr);
}

void Process::ProcessImpl::unbindService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback) {
    sp<Service> service;
    sp<Bundle> result = new Bundle();

    {
        AutoLock autoLock(mProcess->mLock);
        service = mProcess->mServices->get(intent->getComponent());
    }
    if (service != nullptr) {
        service->onUnbind(intent);
        result->putBoolean("result", true);
    } else {
        result->putBoolean("result", false);
    }

    if (callback != nullptr) {
        callback->sendResult(result);
    }
}

} /* namespace mindroid */
