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

#ifndef MINDROID_RESOLVEINFO_H_
#define MINDROID_RESOLVEINFO_H_

#include "mindroid/lang/Object.h"
#include "mindroid/content/pm/ServiceInfo.h"

namespace mindroid {

/**
 * Information that is returned from resolving an intent. This partially corresponds to information
 * collected from the MindroidManifest.xml.
 */
class ResolveInfo :
        public Object {
public:
    ResolveInfo() = default;

    /**
     * The service that corresponds to this resolution match, if this resolution is for a service.
     * {@link #serviceInfo} will be non-null.
     */
    sp<ServiceInfo> serviceInfo;
};

} /* namespace mindroid */

#endif /* MINDROID_RESOLVEINFO_H_ */
