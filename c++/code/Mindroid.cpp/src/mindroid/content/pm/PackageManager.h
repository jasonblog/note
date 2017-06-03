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

#ifndef MINDROID_PACKAGEMANAGER_H_
#define MINDROID_PACKAGEMANAGER_H_

#include "mindroid/content/Context.h"
#include "mindroid/content/pm/ResolveInfo.h"
#include "mindroid/content/pm/IPackageManager.h"
#include "mindroid/lang/Object.h"
#include "mindroid/util/ArrayList.h"

namespace mindroid {

/**
 * Class for retrieving various kinds of information related to the application packages that are
 * currently installed on the device.
 *
 * You can find this class through {@link Context#getPackageManager}.
 */
class PackageManager :
        public Object {
public:
    /**
     * {@link PackageInfo} flag: return information about services in the package in
     * {@link PackageInfo#services}.
     */
    static const int32_t GET_SERVICES = 0x00000004;

    PackageManager(const sp<Context>& context) {
        mService = binder::PackageManager::Stub::asInterface(context->getSystemService(Context::PACKAGE_MANAGER));
    }

    /**
     * Return a List of all packages that are installed on the device.
     *
     * @param flags Additional option flags. Use any combination of {@link #GET_SERVICES}
     *
     * @return A List of PackageInfo objects, one for each package that is installed on the device.
     * In the unlikely case of there being no installed packages, an empty list is returned.
     *
     * @see #GET_SERVICES
     */
    sp<ArrayList<sp<PackageInfo>>> getInstalledPackages(int32_t flags) {
        try {
            return mService->getInstalledPackages(flags);
        } catch (const RemoteException& e) {
            Assert::fail("System failure");
            return nullptr;
        }
    }

    /**
     * Determine the best service to handle for a given Intent.
     *
     * @param intent An intent containing all of the desired specification (action, data, type,
     * category, and/or component).
     * @param flags Additional option flags.
     *
     * @return Returns a ResolveInfo containing the final service intent that was determined to be
     * the best action. Returns null if no matching service was found.
     */
    sp<ResolveInfo> resolveService(const sp<Intent>& intent, int32_t flags) {
        try {
            return mService->resolveService(intent, flags);
        } catch (const RemoteException& e) {
            Assert::fail("System failure");
            return nullptr;
        }
    }

private:
    sp<IPackageManager> mService;
};

} /* namespace mindroid */

#endif /* MINDROID_PACKAGEMANAGER_H_ */
