/*
 * Copyright (C) 2007 The Android Open Source Project
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

#ifndef MINDROID_PACKAGEINFO_H_
#define MINDROID_PACKAGEINFO_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"
#include "mindroid/content/pm/ApplicationInfo.h"
#include "mindroid/content/pm/ServiceInfo.h"
#include "mindroid/util/ArrayList.h"

namespace mindroid {

/**
 * Overall information about the contents of a package. This corresponds to all of the information
 * collected from MindroidManifest.xml.
 */
class PackageInfo :
        public Object {
public:
    PackageInfo() = default;

    /**
     * The name of this package. From the &lt;manifest&gt; tag's "name" attribute.
     */
    sp<String> packageName;

    /**
     * The version number of this package, as specified by the &lt;manifest&gt; tag's
     * {@link MindroidManifest_versionCode versionCode} attribute.
     */
    int32_t versionCode;

    /**
     * The version name of this package, as specified by the &lt;manifest&gt; tag's
     * {@link MindroidManifest_versionName versionName} attribute.
     */
    sp<String> versionName;

    /**
     * Information collected from the &lt;application&gt; tag, or null if there was none.
     */
    sp<ApplicationInfo> applicationInfo;

    /**
     * Array of all {@link MindroidManifestService &lt;service&gt;} tags included under
     * &lt;application&gt;, or null if there were none. This is only filled in if the flag
     * {@link PackageManager#GET_SERVICES} was set.
     */
    sp<ArrayList<sp<ServiceInfo>>> services;
};

} /* namespace mindroid */

#endif /* MINDROID_PACKAGEINFO_H_ */
