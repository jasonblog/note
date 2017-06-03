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

#include "mindroid/app/Service.h"
#include "mindroid/content/Intent.h"
#include "mindroid/content/ComponentName.h"

namespace mindroid {

void Service::stopSelf(int32_t startId) {
    sp<Intent> intent = new Intent();
    intent->setComponent(new ComponentName(object_cast<Context>(this), mClassName));
    try {
        mProcess->stopService(intent);
    } catch (const RemoteException& e) {
        Assert::fail("System failure");
    }
}

void Service::attach(const sp<Context>& context, const sp<IProcess>& process, const sp<String>& className) {
    mProcess = process;
    mClassName = className;
    attachBaseContext(context);
}

} /* namespace mindroid */
