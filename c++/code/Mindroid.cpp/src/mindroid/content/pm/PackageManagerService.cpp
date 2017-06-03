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

#include "mindroid/content/pm/PackageManagerService.h"
#include "mindroid/os/Environment.h"
#include "mindroid/os/ServiceManager.h"
#include "mindroid/io/File.h"
#include "mindroid/io/FilenameFilter.h"
#include "mindroid/lang/Class.h"
#include "mindroid/util/Log.h"
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

namespace mindroid {

const char* const PackageManagerService::TAG = "PackageManager";
const char* PackageManagerService::MANIFEST_TAG = "manifest";
const char* PackageManagerService::APPLICATION_TAG = "application";
const char* PackageManagerService::SERVICE_TAG = "service";

void PackageManagerService::onCreate() {
    ServiceManager::addService(Context::PACKAGE_MANAGER, mBinder);
}

int32_t PackageManagerService::onStartCommand(const sp<Intent>& intent, int32_t flags, int32_t startId) {
    class ManifestFilenameFilter : public FilenameFilter {
    public:
        bool accept(const sp<File>& dir, const sp<String>& filename) override {
            return filename->toLowerCase()->endsWith(".xml");
        }
    };
    sp<ArrayList<sp<File>>> apps = Environment::getAppsDirectory()->listFiles(new ManifestFilenameFilter());
    if (apps != nullptr) {
        auto itr = apps->iterator();
        while (itr.hasNext()) {
            sp<File> manifest = itr.next();
            parseManifest(manifest);
        }
    }

    return 0;
}

void PackageManagerService::onDestroy() {
    ServiceManager::removeService(Context::PACKAGE_MANAGER);
}

sp<ArrayList<sp<PackageInfo>>> PackageManagerService::PackageManagerImpl::getInstalledPackages(int32_t flags) {
    if ((flags & PackageManager::GET_SERVICES) == PackageManager::GET_SERVICES) {
        sp<ArrayList<sp<PackageInfo>>> packages = new ArrayList<sp<PackageInfo>>();
        auto itr = mPackageManagerService->mPackages->iterator();
        while (itr.hasNext()) {
            auto entry = itr.next();
            sp<PackageInfo> p = entry.getValue();
            packages->add(p);
        }
        return packages->isEmpty() ? nullptr : packages;
    } else {
        return nullptr;
    }
}

sp<ResolveInfo> PackageManagerService::PackageManagerImpl::resolveService(const sp<Intent>& intent, int32_t flags) {
    sp<ResolveInfo> resolveInfo = nullptr;
    sp<ComponentInfo> componentInfo = mPackageManagerService->mComponents->get(intent->getComponent());
    if (componentInfo != nullptr) {
        resolveInfo = new ResolveInfo();
        resolveInfo->serviceInfo = object_cast<ServiceInfo>(componentInfo);
    }
    return resolveInfo;
}

void PackageManagerService::parseManifest(const sp<File>& file) {
    XMLDocument doc;
    if (doc.LoadFile(file->getPath()->c_str()) == XML_SUCCESS) {
        const XMLElement* rootNode = doc.FirstChildElement(MANIFEST_TAG);
        if (rootNode != nullptr) {
            sp<ApplicationInfo> ai = new ApplicationInfo();

            sp<String> packageName;
            const XMLAttribute* attribute = rootNode->FindAttribute("package");
            if (attribute != nullptr) {
                packageName = String::valueOf(attribute->Value());
            }
            if (packageName == nullptr || packageName->length() == 0) {
                Assert::fail("Manifest is missing a package name");
            }
            ai->packageName = packageName;

            const XMLElement* element;
            for (element = rootNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
                if (XMLUtil::StringEqual(APPLICATION_TAG, element->Name())) {
                    sp<ArrayList<sp<ServiceInfo>>> services = parseApplication(ai, element);

                    if (services != nullptr && !services->isEmpty()) {
                        sp<PackageInfo> packageInfo = new PackageInfo();
                        packageInfo->packageName = packageName;
                        packageInfo->applicationInfo = ai;
                        packageInfo->services = services;
                        mPackages->put(packageName, packageInfo);

                        auto itr = services->iterator();
                        while (itr.hasNext()) {
                            sp<ServiceInfo> si = itr.next();
                            mComponents->put(new ComponentName(si->packageName, si->name), si);
                        }
                    }
                }
            }
        }
    } else {
        Log::w(TAG, "Cannot read %s", file->getPath()->c_str());
    }
}

sp<ArrayList<sp<ServiceInfo>>> PackageManagerService::parseApplication(sp<ApplicationInfo>& ai, const XMLElement* applicationNode) {
    sp<ArrayList<sp<ServiceInfo>>> services = new ArrayList<sp<ServiceInfo>>();

    const XMLAttribute* attribute = applicationNode->FindAttribute("process");
    if (attribute != nullptr) {
        ai->processName = String::valueOf(attribute->Value());
    }
    attribute = applicationNode->FindAttribute("enabled");
    if (attribute != nullptr) {
        ai->enabled = attribute->BoolValue();
    }

    const XMLElement* element;
    for (element = applicationNode->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
        if (XMLUtil::StringEqual(SERVICE_TAG, element->Name())) {
            sp<ServiceInfo> si = parseService(ai, element);
            if (si != nullptr) {
                services->add(si);
            }
        }
    }

    return services;
}

sp<ServiceInfo> PackageManagerService::parseService(sp<ApplicationInfo>& ai, const XMLElement* serviceNode) {
    sp<String> name;
    sp<String> processName = ai->packageName;
    bool enabled = true;
    bool autostart = false;

    const XMLAttribute* attribute = serviceNode->FindAttribute("mindroid:name");
    if (attribute != nullptr) {
        name = String::valueOf(attribute->Value());
        if (name->startsWith(".")) {
            name = name->substring(1);
        } else {
            if (name->startsWith(ai->packageName)) {
                name = name->substring(ai->packageName->length() + 1);
            } else {
                name = nullptr;
            }
        }
    }
    attribute = serviceNode->FindAttribute("mindroid:process");
    if (attribute != nullptr) {
        processName = String::valueOf(attribute->Value());
    }
    attribute = serviceNode->FindAttribute("mindroid:enabled");
    if (attribute != nullptr) {
        enabled = attribute->BoolValue();
    }
    attribute = serviceNode->FindAttribute("mindroid:autostart");
    if (attribute != nullptr) {
        autostart = attribute->BoolValue();
    }

    if (name == nullptr || name->length() == 0) {
        return nullptr;
    }

    sp<ServiceInfo> si = new ServiceInfo();
    si->name = name;
    si->packageName = ai->packageName;
    si->applicationInfo = ai;
    si->processName = processName;
    si->enabled = enabled;
    if (autostart) {
        si->flags |= ServiceInfo::FLAG_AUTO_START;
    }

    return si;
}

} /* namespace mindroid */
