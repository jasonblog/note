/*
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

#include "mindroid/content/Context.h"
#include "mindroid/content/pm/ServiceInfo.h"
#include "mindroid/content/pm/ApplicationInfo.h"
#include "mindroid/content/pm/ResolveInfo.h"
#include "mindroid/content/pm/IPackageManager.h"
#include "mindroid/os/HandlerThread.h"
#include "mindroid/os/Process.h"
#include "mindroid/os/IProcess.h"
#include "mindroid/os/ServiceManager.h"
#include "mindroid/util/concurrent/Promise.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"
#include "mindroid/util/Log.h"

namespace mindroid {

const char* const ServiceManager::TAG = "ServiceManager";
const char* ServiceManager::SYSTEM_SERVICE = "systemService";
sp<ReentrantLock> ServiceManager::sLock = new ReentrantLock();
sp<Condition> ServiceManager::sCondition = sLock->newCondition();
sp<IServiceManager> ServiceManager::sServiceManager = nullptr;
sp<binder::ServiceManager::Stub> ServiceManager::sStub = nullptr;
sp<HashMap<sp<String>, sp<IBinder>>> ServiceManager::sSystemServices = new HashMap<sp<String>, sp<IBinder>>();

ServiceManager::ProcessManager::ProcessManager() :
        mLock(new ReentrantLock()),
        mProcesses(new HashMap<sp<String>, sp<Pair<sp<Process>, sp<IProcess>>>>()) {
    mThread = new HandlerThread("ProcessManager");
}

void ServiceManager::ProcessManager::start() {
    mThread->start();
    mHandler = new Handler(mThread->getLooper());
}

void ServiceManager::ProcessManager::shutdown() {
    mThread->quit();
    mThread->join();
}

sp<IProcess> ServiceManager::ProcessManager::startProcess(const sp<String>& name) {
    AutoLock autoLock(mLock);
    if (!mProcesses->containsKey(name)) {
        sp<Process> process = new Process(name);
        sp<IProcess> p = process->start();
        mProcesses->put(name, new Pair<sp<Process>, sp<IProcess>>(process, p));
        return p;
    } else {
        auto pair = mProcesses->get(name);
        return pair->second;
    }
}

bool ServiceManager::ProcessManager::stopProcess(const sp<String>& name) {
    AutoLock autoLock(mLock);
    auto pair = mProcesses->remove(name);
    if (pair != nullptr) {
        sp<Process> process = pair->first;
        mHandler->post([=] {
            process->stop(SHUTDOWN_TIMEOUT);
        });
        return true;
    } else {
        return false;
    }
}

sp<Future<bool>> ServiceManager::ProcessManager::stopProcess(const sp<String>& name, uint64_t timeout) {
    AutoLock autoLock(mLock);
    sp<Promise<bool>> promise = new Promise<bool>();
    auto pair = mProcesses->remove(name);
    if (pair != nullptr) {
        sp<Process> process = pair->first;
        mHandler->post([=] {
            process->stop(timeout);
            promise->set(true);
        });
        return promise;
    } else {
        promise->set(false);
        return promise;
    }
}

ServiceManager::ProcessRecord::ProcessRecord(const sp<String>& name, const sp<IProcess>& process) :
        name(name),
        process(process),
        services(new HashMap<sp<ComponentName>, sp<ServiceRecord>>) {
}

void ServiceManager::ProcessRecord::addService(const sp<ComponentName>& component, const sp<ServiceRecord>& service) {
    services->put(component, service);
}

void ServiceManager::ProcessRecord::removeService(const sp<ComponentName>& component) {
    services->remove(component);
}

sp<ServiceManager::ServiceRecord> ServiceManager::ProcessRecord::getService(const sp<ComponentName>& component) {
    return services->get(component);
}

bool ServiceManager::ProcessRecord::containsService(const sp<ComponentName>& component) {
    return services->containsKey(component);
}

size_t ServiceManager::ProcessRecord::numServices() const {
    return services->size();
}

ServiceManager::ServiceRecord::ServiceRecord(const sp<String>& name, const sp<ProcessRecord>& processRecord, bool systemService) :
        name(name),
        processRecord(processRecord),
        systemService(systemService),
        alive(false),
        running(false),
        serviceConnections(new ArrayList<sp<ServiceConnection>>()) {
}

void ServiceManager::ServiceRecord::addServiceConnection(const sp<ServiceConnection>& conn) {
    serviceConnections->add(conn);
}

void ServiceManager::ServiceRecord::removeServiceConnection(const sp<ServiceConnection> conn) {
    serviceConnections->remove(conn);
}

size_t ServiceManager::ServiceRecord::getNumServiceConnections() const {
    return serviceConnections->size();
}

bool ServiceManager::ServiceRecord::hasServiceConnection(const sp<ServiceConnection>& conn) const {
    return serviceConnections->contains(conn);
}

ServiceManager::ServiceManager() :
            mLock(new ReentrantLock()),
            mProcesses(new HashMap<sp<String>, sp<ProcessRecord>>()),
            mServices(new HashMap<sp<ComponentName>, sp<ServiceRecord>>()),
            mServiceCallbacks(new ArrayList<sp<RemoteCallback>>()) {
    mProcessManager = new ProcessManager();
    mMainThread = new HandlerThread(TAG);
}

void ServiceManager::start() {
    mProcessManager->start();

    mMainThread->start();
    mMainHandler = new Handler(mMainThread->getLooper());
    sp<Promise<sp<binder::ServiceManager::Stub>>> promise = new Promise<sp<binder::ServiceManager::Stub>>();
    mMainHandler->post([=] {
        promise->set(new ServiceManagerImpl(this));
    });
    promise->done([&] {
        sStub = promise->get();
    })->fail([] {
        Assert::fail("System failure");
    });

    addService(Context::SERVICE_MANAGER, sStub);
}

void ServiceManager::shutdown() {
    {
        AutoLock autoLock(mLock);
        auto itr = mProcesses->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            sp<ProcessRecord> processRecord = entry.getValue();
            sp<Future<bool>> future = mProcessManager->stopProcess(processRecord->name, SHUTDOWN_TIMEOUT);
            future->await();
        }
    }

    removeService(Context::SERVICE_MANAGER);

    mMainThread->quit();
    mMainThread->join();

    mProcessManager->shutdown();

    sServiceManager = nullptr;
}

sp<ComponentName> ServiceManager::ServiceManagerImpl::startService(const sp<Intent>& intent) {
    intent->putExtra(SYSTEM_SERVICE, false);
    return mServiceManager->startService(intent);
}

bool ServiceManager::ServiceManagerImpl::stopService(const sp<Intent>& service) {
    if (mServiceManager->mServices->containsKey(service->getComponent())) {
        sp<ServiceRecord> serviceRecord = mServiceManager->mServices->get(service->getComponent());
        sp<ProcessRecord> processRecord = serviceRecord->processRecord;
        sp<IProcess> process = processRecord->process;

        if (!serviceRecord->alive) {
            return false;
        }

        if (serviceRecord->getNumServiceConnections() != 0) {
            Log::d(ServiceManager::TAG, "Cannot stop service %s due to active bindings", service->getComponent()->toShortString()->c_str());
            return false;
        }

        class OnResultListener : public RemoteCallback::OnResultListener {
        public:
            OnResultListener(const wp<ServiceManager>& serviceManager, const sp<String>& serviceName) :
                    mServiceManager(serviceManager),
                    mServiceName(serviceName) {
            }

        protected:
            void onResult(const sp<Bundle>& data) {
                sp<ServiceManager> serviceManager = mServiceManager.lock();
                if (serviceManager != nullptr) {
                    bool result = data->getBoolean("result");
                    if (result) {
                        Log::d(ServiceManager::TAG, "Service %s has been stopped", mServiceName->c_str());
                    } else {
                        Log::w(ServiceManager::TAG, "Service %s cannot be stopped", mServiceName->c_str());
                    }

                    serviceManager->mServiceCallbacks->remove(getCallback());
                }
            }

        private:
            const wp<ServiceManager> mServiceManager;
            const sp<String> mServiceName;
        };

        sp<RemoteCallback> callback = new RemoteCallback(new OnResultListener(mServiceManager, serviceRecord->name));
        mServiceManager->mServiceCallbacks->add(callback);
        try {
            process->stopService(service, callback->asInterface());
        } catch (const RemoteException& e) {
            Assert::fail("System failure");
        }

        serviceRecord->alive = false;
        serviceRecord->running = false;
        mServiceManager->mServices->remove(service->getComponent());
        processRecord->removeService(service->getComponent());

        if (processRecord->numServices() == 0) {
            mServiceManager->mMainHandler->post([=] {
                if (processRecord->numServices() == 0) {
                    mServiceManager->mProcessManager->stopProcess(processRecord->name);
                    {
                        AutoLock autoLock(mServiceManager->mLock);
                        mServiceManager->mProcesses->remove(processRecord->name);
                    }
                }
            });
        }

        return true;
    } else {
        Log::d(ServiceManager::TAG, "Cannot find and stop service %s", service->getComponent()->toShortString()->c_str());
        return false;
    }
}

bool ServiceManager::ServiceManagerImpl::bindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback) {
    intent->putExtra(SYSTEM_SERVICE, false);

    if (!mServiceManager->prepareService(intent)) {
        return false;
    }

    if (mServiceManager->mServices->containsKey(intent->getComponent())) {
        sp<ServiceRecord> serviceRecord = mServiceManager->mServices->get(intent->getComponent());
        if (!serviceRecord->hasServiceConnection(conn)) {
            serviceRecord->addServiceConnection(conn);

            try {
                serviceRecord->processRecord->process->bindService(intent, conn, flags, callback);
            } catch (const RemoteException& e) {
                Assert::fail("System failure");
            }

            Log::d(ServiceManager::TAG, "Bound to service %s in process %s", serviceRecord->name->c_str(), serviceRecord->processRecord->name->c_str());
        }

        return true;
    } else {
        Log::d(ServiceManager::TAG, "Cannot find and bind service %s", intent->getComponent()->toShortString()->c_str());
        return false;
    }
}

void ServiceManager::ServiceManagerImpl::unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn) {
    unbindService(service, conn, nullptr);
}

void ServiceManager::ServiceManagerImpl::unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback) {
    sp<ServiceRecord> serviceRecord = mServiceManager->mServices->get(service->getComponent());
    if (serviceRecord != nullptr) {
        sp<IProcess> process = serviceRecord->processRecord->process;
        try {
            if (callback != nullptr) {
                process->unbindService(service, callback);
            } else {
                process->unbindService(service);
            }
        } catch (const RemoteException& e) {
            Assert::fail("System failure");
        }
        Log::d(ServiceManager::TAG, "Unbound from service %s in process %s", serviceRecord->name->c_str(), serviceRecord->processRecord->name->c_str());

        serviceRecord->removeServiceConnection(conn);
        if (serviceRecord->getNumServiceConnections() == 0) {
            try {
                sServiceManager->stopService(service);
            } catch (const RemoteException& e) {
                Assert::fail("System failure");
            }
        }
    }
}

sp<ComponentName> ServiceManager::ServiceManagerImpl::startSystemService(const sp<Intent>& service) {
    if (!service->hasExtra("name")) {
        service->putExtra("name", service->getComponent()->getClassName()->c_str());
    }
    if (!service->hasExtra("process")) {
        service->putExtra("process", service->getComponent()->getPackageName()->c_str());
    }
    service->putExtra(SYSTEM_SERVICE, true);
    return mServiceManager->startService(service);
}

bool ServiceManager::ServiceManagerImpl::stopSystemService(const sp<Intent>& service) {
    service->putExtra(SYSTEM_SERVICE, true);
    return stopService(service);
}

sp<IProcess> ServiceManager::prepareProcess(const sp<String>& name) {
    sp<IProcess> process;
    {
        AutoLock autoLock(mLock);
        if (mProcesses->containsKey(name)) {
            sp<ProcessRecord> processRecord = mProcesses->get(name);
            process = processRecord->process;
        } else {
            process = mProcessManager->startProcess(name);
            mProcesses->put(name, new ProcessRecord(name, process));
        }
    }
    return process;
}

bool ServiceManager::prepareService(const sp<Intent>& service) {
    sp<ServiceInfo> serviceInfo;
    if (service->getBooleanExtra(SYSTEM_SERVICE, false)) {
        sp<ApplicationInfo> ai = new ApplicationInfo();
        ai->processName = service->getStringExtra("process");
        sp<ServiceInfo> si = new ServiceInfo();
        si->name = service->getComponent()->getClassName();
        si->packageName = service->getComponent()->getPackageName();
        si->applicationInfo = ai;
        si->processName = ai->processName;
        si->flags |= ServiceInfo::FLAG_SYSTEM_SERVICE;

        serviceInfo = si;
    } else {
        if (mPackageManager == nullptr) {
            mPackageManager = binder::PackageManager::Stub::asInterface(getSystemService(Context::PACKAGE_MANAGER));
            Assert::assertNotNull("System failure", mPackageManager);
        }
        sp<ResolveInfo> resolveInfo = nullptr;
        try {
            resolveInfo = mPackageManager->resolveService(service, 0);
        } catch (const RemoteException& e) {
            Assert::fail("System failure");
        }

        if (resolveInfo == nullptr || resolveInfo->serviceInfo == nullptr) {
            return false;
        }

        serviceInfo = resolveInfo->serviceInfo;
    }

    sp<IProcess> process = prepareProcess(serviceInfo->processName);
    if (process == nullptr) {
        return false;
    }

    sp<ServiceRecord> serviceRecord;
    sp<ProcessRecord> processRecord = mProcesses->get(serviceInfo->processName);
    if (mServices->containsKey(service->getComponent())) {
        serviceRecord = mServices->get(service->getComponent());
    } else {
        bool systemService = service->getBooleanExtra(SYSTEM_SERVICE, false);
        sp<String> name;
        if (systemService) {
            name = service->getStringExtra("name");
        } else {
            name = String::format("%s::%s", serviceInfo->packageName->c_str(), serviceInfo->name->c_str());
        }
        serviceRecord = new ServiceRecord(name, processRecord, systemService);
        mServices->put(service->getComponent(), serviceRecord);
    }
    if (!processRecord->containsService(service->getComponent())) {
        processRecord->addService(service->getComponent(), serviceRecord);
    }

    if (!serviceRecord->alive) {
        serviceRecord->alive = true;

        class OnResultListener : public RemoteCallback::OnResultListener {
        public:
            OnResultListener(const wp<ServiceManager>& serviceManager, const sp<Intent>& service) :
                    mServiceManager(serviceManager),
                    mService(service) {
            }

        protected:
            void onResult(const sp<Bundle>& data) {
                sp<ServiceManager> serviceManager = mServiceManager.lock();
                if (serviceManager != nullptr) {
                    sp<ComponentName> component = mService->getComponent();
                    if (serviceManager->mServices->containsKey(component)) {
                        sp<ServiceRecord> serviceRecord = serviceManager->mServices->get(component);
                        bool result = data->getBoolean("result");
                        if (result) {
                            Log::d(TAG, "Service %s has been created in process %s", serviceRecord->name->c_str(), serviceRecord->processRecord->name->c_str());
                        } else {
                            Log::w(TAG, "Service %s cannot be created in process %s. Cleaning up", serviceRecord->name->c_str(), serviceRecord->processRecord->name->c_str());
                            serviceManager->cleanupService(mService);
                        }
                    }

                    serviceManager->mServiceCallbacks->remove(getCallback());
                }
            }

        private:
            const wp<ServiceManager> mServiceManager;
            const sp<Intent> mService;
        };

        sp<RemoteCallback> callback = new RemoteCallback(new OnResultListener(this, service));
        mServiceCallbacks->add(callback);
        try {
            process->createService(service, callback->asInterface());
        } catch (const RemoteException& e) {
            Assert::fail("System failure");
        }
    }

    return true;
}

sp<ComponentName> ServiceManager::startService(const sp<Intent>& service) {
    if (!prepareService(service)) {
        return nullptr;
    }

    sp<ServiceRecord> serviceRecord = mServices->get(service->getComponent());
    serviceRecord->running = true;

    class OnResultListener : public RemoteCallback::OnResultListener {
    public:
        OnResultListener(const wp<ServiceManager>& serviceManager, const sp<String>& serviceName, const sp<String>& processName) :
                mServiceManager(serviceManager),
                mServiceName(serviceName),
                mProcessName(processName) {
        }

    protected:
        void onResult(const sp<Bundle>& data) {
            sp<ServiceManager> serviceManager = mServiceManager.lock();
            if (serviceManager != nullptr) {
                bool result = data->getBoolean("result");
                if (result) {
                    Log::d(TAG, "Service %s has been started in process %s", mServiceName->c_str(), mProcessName->c_str());
                } else {
                    Log::w(TAG, "Service %s cannot be started in process %s", mServiceName->c_str(), mProcessName->c_str());
                }

                serviceManager->mServiceCallbacks->remove(getCallback());
            }
        }

    private:
        const wp<ServiceManager> mServiceManager;
        const sp<String> mServiceName;
        const sp<String> mProcessName;
    };

    sp<RemoteCallback> callback = new RemoteCallback(new OnResultListener(this, serviceRecord->name, serviceRecord->processRecord->name));
    mServiceCallbacks->add(callback);
    try {
        serviceRecord->processRecord->process->startService(service, 0, mStartId++, callback->asInterface());
    } catch (const RemoteException& e) {
        Assert::fail("System failure");
    }

    return service->getComponent();
}

bool ServiceManager::cleanupService(const sp<Intent>& service) {
    if (mServices->containsKey(service->getComponent())) {
        sp<ServiceRecord> serviceRecord = mServices->get(service->getComponent());
        sp<ProcessRecord> processRecord = serviceRecord->processRecord;

        if (serviceRecord->alive) {
            serviceRecord->alive = false;
        }
        if (serviceRecord->running) {
            serviceRecord->running = false;
        }

        mServices->remove(service->getComponent());
        processRecord->removeService(service->getComponent());

        if (processRecord->numServices() == 0) {
            mMainHandler->post([=] {
                if (processRecord->numServices() == 0) {
                    mProcessManager->stopProcess(processRecord->name);
                    {
                        AutoLock autoLock(mLock);
                        mProcesses->remove(processRecord->name);
                    }
                }
            });
        }

        return true;
    } else {
        Log::d(TAG, "Cannot find and clean up service %s", service->getComponent()->toShortString()->c_str());
        return false;
    }
}

sp<IServiceManager> ServiceManager::getServiceManager() {
    AutoLock autoLock(sLock);
    if (sServiceManager != nullptr) {
        return sServiceManager;
    }

    sServiceManager = binder::ServiceManager::Stub::asInterface(sStub);
    return sServiceManager;
}

sp<IBinder> ServiceManager::getSystemService(const sp<String>& name) {
    AutoLock autoLock(sLock);
    sp<IBinder> service = sSystemServices->get(name);
    return service;
}

void ServiceManager::addService(const sp<String>& name, const sp<IBinder>& service) {
    AutoLock autoLock(sLock);
    if (!sSystemServices->containsKey(name)) {
        sSystemServices->put(name, service);
        sCondition->signalAll();
    }
}

void ServiceManager::removeService(const sp<String>& name) {
    AutoLock autoLock(sLock);
    sSystemServices->remove(name);
    sCondition->signalAll();
}

void ServiceManager::waitForSystemService(const sp<String>& name) {
    AutoLock autoLock(sLock);
    const uint64_t TIMEOUT = 10000;
    uint64_t start = SystemClock::uptimeMillis();
    uint64_t duration = TIMEOUT;
    while (!sSystemServices->containsKey(name)) {
        sCondition->await(duration);
        if (!sSystemServices->containsKey(name)) {
            duration = start + TIMEOUT - SystemClock::uptimeMillis();
            if (duration <= 0) {
                Log::w(TAG, "Starting %s takes very long", name->c_str());
                start = SystemClock::uptimeMillis();
                duration = TIMEOUT;
            }
        }
    }
}

void ServiceManager::waitForSystemServiceShutdown(const sp<String>& name) {
    AutoLock autoLock(sLock);
    const uint64_t TIMEOUT = 10000;
    uint64_t start = SystemClock::uptimeMillis();
    uint64_t duration = TIMEOUT;
    while (sSystemServices->containsKey(name)) {
        sCondition->await(duration);
        if (sSystemServices->containsKey(name)) {
            duration = start + TIMEOUT - SystemClock::uptimeMillis();
            if (duration <= 0) {
                Log::w(TAG, "Stopping %s takes very long", name->c_str());
                start = SystemClock::uptimeMillis();
                duration = TIMEOUT;
            }
        }
    }
}

} /* namespace mindroid */
