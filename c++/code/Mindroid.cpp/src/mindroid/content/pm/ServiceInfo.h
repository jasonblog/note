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

#ifndef MINDROID_SERVICEINFO_H_
#define MINDROID_SERVICEINFO_H_

#include "mindroid/content/pm/ComponentInfo.h"

namespace mindroid {

/**
 * Information you can retrieve about a particular application service. This corresponds to
 * information collected from the MindroidManifest.xml's &lt;service&gt; tags.
 */
class ServiceInfo :
        public ComponentInfo {
public:
    ServiceInfo() = default;

    /**
     * Bit in {@link #flags}: If set, the service will automatically be started by the system after
     * boot completed.
     */
    static const int32_t FLAG_AUTO_START = 0x0001;

    /**
     * Bit in {@link #flags}: If set, the service is marked as system service.
     */
    static const int32_t FLAG_SYSTEM_SERVICE = 0x40000000;

    /**
     * Options that have been set in the service declaration in the manifest. These include:
     * {@link #FLAG_AUTO_START}.
     */
    int32_t flags = 0;

    bool hasFlag(int32_t flag) {
        return (flags & flag) == flag;
    }
};

} /* namespace mindroid */

#endif /* MINDROID_SERVICEINFO_H_ */
