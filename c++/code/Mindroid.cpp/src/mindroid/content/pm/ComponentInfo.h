/*
 * Copyright (C) 2008 The Android Open Source Project
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

#ifndef MINDROID_COMPONENTINFO_H_
#define MINDROID_COMPONENTINFO_H_

#include "mindroid/lang/Object.h"
#include "mindroid/content/pm/ApplicationInfo.h"

namespace mindroid {

/**
 * Base class containing information common to all application components ({@link ServiceInfo}).
 * This class is not intended to be used by itself; it is simply here to share common definitions
 * between all application components.
 */
class ComponentInfo :
        public Object {
public:
    ComponentInfo() = default;

    /**
     * Public name of this item. From the "mindroid:name" attribute.
     */
    sp<String> name;

    /**
     * Name of the package that this item is in.
     */
    sp<String> packageName;

    /**
     * Global information about the application/package this component is a part of.
     */
    sp<ApplicationInfo> applicationInfo;

    /**
     * The name of the process this component should run in. From the "mindroid:process" attribute
     * or, if not set, the same as <var>applicationInfo.processName</var>.
     */
    sp<String> processName;

    /**
     * Indicates whether or not this component may be instantiated. Note that this value can be
     * overriden by the one in its parent {@link ApplicationInfo}.
     */
    bool enabled = true;

    /**
     * Return whether this component and its enclosing application are enabled.
     */
    bool isEnabled() {
        return enabled && applicationInfo->enabled;
    }

protected:
    /**
     * @hide
     */
    sp<ApplicationInfo> getApplicationInfo() {
        return applicationInfo;
    }
};

} /* namespace mindroid */

#endif /* MINDROID_COMPONENTINFO_H_ */
