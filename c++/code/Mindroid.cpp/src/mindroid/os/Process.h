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

#ifndef MINDROID_PROCESS_H_
#define MINDROID_PROCESS_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"
#include "mindroid/os/HandlerThread.h"
#include "mindroid/os/IProcess.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"

namespace mindroid {

class Service;
class IRemoteCallback;
class ComponentName;
class IPackageManager;

class Process :
        public Object {
public:
    Process(const sp<String>& name);

    sp<IProcess> start();
    void stop(uint64_t timeout);

    sp<String> getName() {
        return mName;
    }

    sp<HandlerThread> getMainThread() {
        return mMainThread;
    }

    bool isAlive() const;

private:
    class ProcessImpl : public binder::Process::Stub {
    public:
        ProcessImpl(sp<Process> process) : mProcess(process) {
        }

        void createService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback);
        void startService(const sp<Intent>& intent, int32_t flags, int32_t startId, const sp<IRemoteCallback>& callback);
        void stopService(const sp<Intent>& intent);
        void stopService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback);
        void bindService(const sp<Intent>& intent, const sp<ServiceConnection>& conn, int32_t flags, const sp<IRemoteCallback>& callback);
        void unbindService(const sp<Intent>& intent);
        void unbindService(const sp<Intent>& intent, const sp<IRemoteCallback>& callback);

    private:
        sp<Process> mProcess;
    };

    static const char* const TAG;
    sp<String> mName;
    sp<HandlerThread> mMainThread;
    sp<Handler> mMainHandler;
    sp<binder::Process::Stub> mStub;
    sp<IPackageManager> mPackageManager;
    sp<HashMap<sp<ComponentName>, sp<Service>>> mServices;
    sp<ReentrantLock> mLock;
    sp<Condition> mCondition;
};

} /* namespace mindroid */

#endif /* MINDROID_PROCESS_H_ */
