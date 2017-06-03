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

#include "mindroid/content/ComponentName.h"
#include "mindroid/content/Context.h"
#include "mindroid/util/Assert.h"

namespace mindroid {

ComponentName::ComponentName(const char* pkg, const char* cls) {
    Assert::assertTrue("package name is null", pkg != nullptr);
    Assert::assertTrue("class name is null", cls != nullptr);
    mPackage = String::valueOf(pkg);
    mClass = String::valueOf(cls);
}

ComponentName::ComponentName(const sp<String>& pkg, const sp<String>& cls) {
    Assert::assertTrue("package name is null", pkg != nullptr);
    Assert::assertTrue("class name is null", cls != nullptr);
    mPackage = pkg;
    mClass = cls;
}

ComponentName::ComponentName(const sp<Context>& pkg, const char* cls) {
    Assert::assertTrue("class name is null", cls != nullptr);
    mPackage = pkg->getPackageName();
    mClass = String::valueOf(cls);
}

ComponentName::ComponentName(const sp<Context>& pkg, const sp<String>& cls) {
    Assert::assertTrue("class name is null", cls != nullptr);
    mPackage = pkg->getPackageName();
    mClass = cls;
}

} /* namespace mindroid */
