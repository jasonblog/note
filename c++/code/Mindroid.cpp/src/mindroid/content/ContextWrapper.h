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

#ifndef MINDROID_CONTEXTWRAPPER_H_
#define MINDROID_CONTEXTWRAPPER_H_

#include "mindroid/content/Context.h"
#include "mindroid/content/ComponentName.h"
#include "mindroid/content/SharedPreferences.h"
#include "mindroid/content/pm/PackageManager.h"
#include "mindroid/lang/String.h"
#include "mindroid/os/Looper.h"
#include "mindroid/io/File.h"

namespace mindroid {

/**
 * Proxying implementation of Context that simply delegates all of its calls to another Context. Can
 * be subclassed to modify behavior without changing the original Context.
 */
class ContextWrapper :
        public Context {
public:
    ContextWrapper(const sp<Context>& baseContext) {
        mBaseContext = baseContext;
    }

    /**
     * @return the base context as set by the constructor or setBaseContext
     */
    virtual sp<Context> getBaseContext() {
        return mBaseContext;
    }

    virtual sp<PackageManager> getPackageManager() {
        return mBaseContext->getPackageManager();
    }

    virtual sp<Looper> getMainLooper() {
        return mBaseContext->getMainLooper();
    }

    virtual sp<String> getPackageName() {
        return mBaseContext->getPackageName();
    }

    virtual sp<File> getSharedPrefsFile(const sp<String>& name) {
        return mBaseContext->getSharedPrefsFile(name);
    }

    virtual sp<SharedPreferences> getSharedPreferences(const char* name, int32_t mode) {
        return getSharedPreferences(String::valueOf(name), mode);
    }

    virtual sp<SharedPreferences> getSharedPreferences(const sp<String>& name, int32_t mode) {
        return mBaseContext->getSharedPreferences(name, mode);
    }

    virtual sp<IBinder> getSystemService(const char* name) {
        return getSystemService(String::valueOf(name));
    }

    virtual sp<IBinder> getSystemService(const sp<String>& name) {
        return mBaseContext->getSystemService(name);
    }

    virtual sp<ComponentName> startService(const sp<Intent>& service) {
        return mBaseContext->startService(service);
    }

    virtual bool stopService(const sp<Intent>& service) {
        return mBaseContext->stopService(service);
    }

    virtual bool bindService(const sp<Intent>& service, const sp<ServiceConnection>& conn, int flags) {
        return mBaseContext->bindService(service, conn, flags);
    }

    virtual void unbindService(const sp<ServiceConnection>& conn) {
        mBaseContext->unbindService(conn);
    }

protected:
    /**
     * Set the base context for this ContextWrapper. All calls will then be delegated to the base
     * context. Throws IllegalStateException if a base context has already been set.
     *
     * @param baseContext The new base context for this wrapper.
     */
    void attachBaseContext(const sp<Context> baseContext);

private:
    sp<Context> mBaseContext;
};

} /* namespace mindroid */

#endif /* MINDROID_CONTEXTWRAPPER_H_ */
