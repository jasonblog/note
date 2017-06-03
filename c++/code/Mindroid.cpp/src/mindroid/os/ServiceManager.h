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

#ifndef MINDROID_SERVICEMANAGER_H_
#define MINDROID_SERVICEMANAGER_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"
#include "mindroid/os/IServiceManager.h"
#include "mindroid/os/RemoteCallback.h"
#include "mindroid/util/Pair.h"

namespace mindroid {

class IPackageManager;
class Process;
class IProcess;
class HandlerThread;
class ReentrantLock;

class ServiceManager final :
        public Object {
private:
    class ProcessManager :
            public Object {
    public:
        ProcessManager();

        void start();
        void shutdown();

        sp<IProcess> startProcess(const sp<String>& name);
        bool stopProcess(const sp<String>& name);
        sp<Future<bool>> stopProcess(const sp<String>& name, uint64_t timeout);

        sp<HandlerThread> mThread;
        sp<Handler> mHandler;
        sp<ReentrantLock> mLock;
        sp<HashMap<sp<String>, sp<Pair<sp<Process>, sp<IProcess>>>>> mProcesses;
    };

    class ServiceRecord;

    class ProcessRecord :
            public Object {
    public:
        ProcessRecord(const sp<String>& name, const sp<IProcess>& process);
        void addService(const sp<ComponentName>& component, const sp<ServiceRecord>& service);
        void removeService(const sp<ComponentName>& component);
        sp<ServiceRecord> getService(const sp<ComponentName>& component);
        bool containsService(const sp<ComponentName>& component);
        size_t numServices() const;

        sp<String> name;
        sp<IProcess> process;
        sp<HashMap<sp<ComponentName>, sp<ServiceRecord>>> services;
    };

    class ServiceRecord :
            public Object {
    public:
        ServiceRecord(const sp<String>& name, const sp<ProcessRecord>& processRecord, bool systemService);
        void addServiceConnection(const sp<ServiceConnection>& conn);
        void removeServiceConnection(const sp<ServiceConnection> conn);
        size_t getNumServiceConnections() const;
        bool hasServiceConnection(const sp<ServiceConnection>& conn) const;

        sp<String> name;
        sp<ProcessRecord> processRecord;
        bool systemService;
        bool alive;
        bool running;
        sp<ArrayList<sp<ServiceConnection>>> serviceConnections;
    };

    class ServiceManagerImpl :
            public binder::ServiceManager::Stub {
    public:
        ServiceManagerImpl(const sp<ServiceManager>& serviceManager) :
                mServiceManager(serviceManager) {
        }

        sp<ComponentName> startService(const sp<Intent>& intent);
        bool stopService(const sp<Intent>& service);
        bool bindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback);
        void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn);
        void unbindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, const sp<IRemoteCallback>& callback);
        sp<ComponentName> startSystemService(const sp<Intent>& service);
        bool stopSystemService(const sp<Intent>& service);

    private:
        sp<ServiceManager> mServiceManager;
    };

    sp<IProcess> prepareProcess(const sp<String>& name);
    bool prepareService(const sp<Intent>& service);
    sp<ComponentName> startService(const sp<Intent>& service);
    bool cleanupService(const sp<Intent>& service);

public:
    ServiceManager();

    void start();
    void shutdown();

    static sp<IServiceManager> getServiceManager();

    /**
     * Returns a reference to a service with the given name.
     *
     * @param name the name of the service to get
     * @return a reference to the service, or <code>null</code> if the service doesn't exist
     */
    static sp<IBinder> getSystemService(const sp<String>& name);

    /**
     * @hide
     */
    static void addService(const sp<String>& name, const sp<IBinder>& service);

    /**
     * @hide
     */
    static void removeService(const sp<String>& name);

    /**
     * @hide
     */
    static void waitForSystemService(const sp<String>& name);

    /**
     * @hide
     */
    static void waitForSystemServiceShutdown(const sp<String>& name);

private:
    static const char* const TAG;
    static const char* SYSTEM_SERVICE;
    static sp<ReentrantLock> sLock;
    static sp<Condition> sCondition;
    static sp<IServiceManager> sServiceManager;
    static sp<binder::ServiceManager::Stub> sStub;
    static sp<HashMap<sp<String>, sp<IBinder>>> sSystemServices;
    static const int32_t SHUTDOWN_TIMEOUT = 10000; //ms

    sp<ReentrantLock> mLock;
    sp<ProcessManager> mProcessManager;
    sp<HandlerThread> mMainThread;
    sp<Handler> mMainHandler;
    sp<HashMap<sp<String>, sp<ProcessRecord>>> mProcesses;
    sp<HashMap<sp<ComponentName>, sp<ServiceRecord>>> mServices;
    sp<ArrayList<sp<RemoteCallback>>> mServiceCallbacks;
    int32_t mStartId = 0;
    sp<IPackageManager> mPackageManager;
};

} /* namespace mindroid */

#endif /* MINDROID_SERVICEMANAGER_H_ */
