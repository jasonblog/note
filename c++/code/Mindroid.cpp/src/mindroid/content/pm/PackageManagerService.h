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

#ifndef MINDROID_PACKAGEMANAGERSERVICE_H_
#define MINDROID_PACKAGEMANAGERSERVICE_H_

#include "mindroid/app/Service.h"
#include "mindroid/content/ComponentName.h"
#include "mindroid/content/Intent.h"
#include "mindroid/content/pm/ServiceInfo.h"
#include "mindroid/content/pm/ResolveInfo.h"
#include "mindroid/content/pm/PackageInfo.h"

namespace tinyxml2 {
class XMLElement;
}

namespace mindroid {

class File;

class PackageManagerService :
        public Service {
public:
    PackageManagerService() :
        mPackages(new HashMap<sp<String>, sp<PackageInfo>>()),
        mComponents(new HashMap<sp<ComponentName>, sp<ComponentInfo>>()) {
    }

    virtual ~PackageManagerService() = default;

    virtual void onCreate();
    virtual int32_t onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId);
    virtual void onDestroy();

    virtual sp<IBinder> onBind(const sp<Intent>& intent) {
        return mBinder;
    }

private:
    class PackageManagerImpl : public binder::PackageManager::Stub {
    public:
        PackageManagerImpl(const sp<PackageManagerService>& packageManagerService) : mPackageManagerService(packageManagerService) { }

        virtual sp<ArrayList<sp<PackageInfo>>> getInstalledPackages(int32_t flags);
        virtual sp<ResolveInfo> resolveService(const sp<Intent>& intent, int32_t flags);

    private:
        sp<PackageManagerService> mPackageManagerService;
    };

    sp<binder::PackageManager::Stub> mBinder = new PackageManagerImpl(this);

    void parseManifest(const sp<File>& file);
    sp<ArrayList<sp<ServiceInfo>>> parseApplication(sp<ApplicationInfo>& ai, const tinyxml2::XMLElement* applicationNode);
    sp<ServiceInfo> parseService(sp<ApplicationInfo>& ai, const tinyxml2::XMLElement* serviceNode);

    static const char* const TAG;
    static const char* MANIFEST_TAG;
    static const char* APPLICATION_TAG;
    static const char* SERVICE_TAG;
    sp<HashMap<sp<String>, sp<PackageInfo>>> mPackages;
    sp<HashMap<sp<ComponentName>, sp<ComponentInfo>>> mComponents;
};

} /* namespace mindroid */

#endif /* MINDROID_PACKAGEMANAGERSERVICE_H_ */
