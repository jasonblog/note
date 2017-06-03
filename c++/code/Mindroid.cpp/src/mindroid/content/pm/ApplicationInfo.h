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

#ifndef MINDROID_APPLICATIONINFO_H_
#define MINDROID_APPLICATIONINFO_H_

#include "mindroid/lang/Object.h"

namespace mindroid {

/**
 * Information you can retrieve about a particular application. This corresponds to information
 * collected from the MindroidManifest.xml's &lt;application&gt; tag.
 */
class ApplicationInfo :
        public Object {
public:
    ApplicationInfo() = default;

    /**
     * Public name of this item. From the "mindroid:name" attribute.
     */
    sp<String> name;

    /**
     * Name of the package that this item is in.
     */
    sp<String> packageName;

    /**
     * The name of the process this application should run in. From the "process" attribute or, if
     * not set, the same as <var>packageName</var>.
     */
    sp<String> processName;

    /**
     * The app's declared version code.
     *
     * @hide
     */
    int32_t versionCode;

    /**
     * When false, indicates that all components within this application are considered disabled,
     * regardless of their individually set enabled status.
     */
    bool enabled = true;
};

} /* namespace mindroid */

#endif /* MINDROID_APPLICATIONINFO_H_ */
