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

#ifndef MINDROID_CONTEXTIMPL_H_
#define MINDROID_CONTEXTIMPL_H_

#include "mindroid/content/Context.h"
#include "mindroid/content/Intent.h"
#include "mindroid/content/ServiceConnection.h"
#include "mindroid/content/SharedPreferences.h"
#include "mindroid/io/File.h"
#include "mindroid/os/HandlerThread.h"
#include "mindroid/os/RemoteCallback.h"

namespace mindroid {

class IServiceManager;
class PackageManager;

/**
 * Common implementation of Context API, which provides the base
 * context object for Activity and other application components.
 */
class ContextImpl :
        public Context {
public:
    ContextImpl(const sp<HandlerThread>& mainThread, const sp<ComponentName>& component);

    sp<PackageManager> getPackageManager();

    sp<Looper> getMainLooper() {
        return mMainThread->getLooper();
    }

    sp<String> getPackageName();

    sp<File> getSharedPrefsFile(const sp<String>& name) {
        return makeFilename(getPreferencesDir(), String::format("%s.xml", name->c_str()));
    }

    sp<SharedPreferences> getSharedPreferences(const char* name, int32_t mode) {
        return getSharedPreferences(String::valueOf(name), mode);
    }

    sp<SharedPreferences> getSharedPreferences(const sp<String>& name, int32_t mode);

    sp<IBinder> getSystemService(const char* name) {
        return getSystemService(String::valueOf(name));
    }

    sp<IBinder> getSystemService(const sp<String>& name);

    sp<ComponentName> startService(const sp<Intent>& service);
    bool stopService(const sp<Intent>& service);
    bool bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int32_t flags);
    void unbindService(const sp<ServiceConnection>& conn);

private:
    sp<File> getPreferencesDir();
    sp<File> makeFilename(const sp<File>& baseDir, const sp<String>& name);
    virtual void cleanup();

    static const char* const TAG;
    sp<IServiceManager> mServiceManager;
    sp<HandlerThread> mMainThread;
    sp<Handler> mHandler;
    sp<ComponentName> mComponent;
    sp<HashMap<sp<ServiceConnection>, sp<Intent>>> mServiceConnections;
    sp<ArrayList<sp<RemoteCallback>>> mServiceConnectionCallbacks;
    sp<PackageManager> mPackageManager;
};

} /* namespace mindroid */

#endif /* MINDROID_CONTEXTIMPL_H_ */
