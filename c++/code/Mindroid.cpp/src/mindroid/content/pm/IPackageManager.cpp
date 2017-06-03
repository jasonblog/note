/*
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

#include "mindroid/content/Intent.h"
#include "mindroid/content/pm/IPackageManager.h"
#include "mindroid/util/concurrent/Promise.h"

namespace mindroid {
namespace binder {

const char* const PackageManager::Stub::DESCRIPTOR = "mindroid.content.pm.IPackageManager";

void PackageManager::Stub::onTransact(int32_t what, int32_t arg1, int32_t arg2, const sp<Object>& obj, const sp<Bundle>& data, const sp<Object>& result) {
    switch (what) {
    case MSG_GET_INSTALLED_PACKAGES: {
        sp<Promise<sp<ArrayList<sp<PackageInfo>>>>> promise = object_cast<Promise<sp<ArrayList<sp<PackageInfo>>>>>(result);
        sp<ArrayList<sp<PackageInfo>>> packages = getInstalledPackages(arg1);
        promise->set(packages);
        break;
    }
    case MSG_RESOLVE_SERVICE: {
        sp<Promise<sp<ResolveInfo>>> promise = object_cast<Promise<sp<ResolveInfo>>>(result);
        sp<Intent> intent = object_cast<Intent>(obj);
        sp<ResolveInfo> serviceInfo = resolveService(intent, arg1);
        promise->set(serviceInfo);
        break;
    }
    default:
        Binder::onTransact(what, arg1, arg2, obj, data, result);
    }
}

sp<ArrayList<sp<PackageInfo>>> PackageManager::Stub::Proxy::getInstalledPackages(int32_t flags) {
    sp<Promise<sp<ArrayList<sp<PackageInfo>>>>> promise = new Promise<sp<ArrayList<sp<PackageInfo>>>>();
    mRemote->transact(MSG_GET_INSTALLED_PACKAGES, flags, 0, promise, 0);
    return promise->get();
}

sp<ResolveInfo> PackageManager::Stub::Proxy::resolveService(const sp<Intent>& intent, int32_t flags) {
    sp<Promise<sp<ResolveInfo>>> promise = new Promise<sp<ResolveInfo>>();
    mRemote->transact(MSG_RESOLVE_SERVICE, flags, 0, object_cast<Object>(intent), promise, 0);
    return promise->get();
}

PackageManager::Stub::SmartProxy::SmartProxy(const sp<IBinder>& remote) {
    mRemote = remote;
    mStub = interface_cast<IPackageManager>(remote->queryLocalInterface(DESCRIPTOR));
    mProxy = new PackageManager::Stub::Proxy(remote);
}

sp<ArrayList<sp<PackageInfo>>> PackageManager::Stub::SmartProxy::getInstalledPackages(int32_t flags) {
    if (mRemote->runsOnSameThread()) {
        return mStub->getInstalledPackages(flags);
    } else {
        return mProxy->getInstalledPackages(flags);
    }
}

sp<ResolveInfo> PackageManager::Stub::SmartProxy::resolveService(const sp<Intent>& intent, int32_t flags) {
    if (mRemote->runsOnSameThread()) {
        return mStub->resolveService(intent, flags);
    } else {
        return mProxy->resolveService(intent, flags);
    }
}

} /* namespace binder */
} /* namespace mindroid */
