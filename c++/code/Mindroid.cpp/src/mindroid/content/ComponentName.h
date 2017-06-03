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

#ifndef MINDROID_COMPONENTNAME_H_
#define MINDROID_COMPONENTNAME_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"
#include "mindroid/lang/Class.h"

namespace mindroid {

class Context;

/**
 * Identifier for a specific application component {@link mindroid.app.Service}, Two pieces of
 * information, encapsulated here, are required to identify a component: the package (a String) it
 * exists in, and the class (a String) name inside of that package.
 *
 */
class ComponentName final :
        public Object {
public:
    /**
     * Create a new component identifier.
     *
     * @param pkg The name of the package that the component exists in. Can not be null.
     * @param cls The name of the class inside of <var>pkg</var> that implements the component. Can
     * not be null.
     */
    ComponentName(const char* pkg, const char* cls);
    ComponentName(const sp<String>& pkg, const sp<String>& cls);

    /**
     * Create a new component identifier from a Context and class name.
     *
     * @param pkg A Context for the package implementing the component, from which the actual
     * package name will be retrieved.
     * @param cls The name of the class inside of <var>pkg</var> that implements the component.
     */
    ComponentName(const sp<Context>& pkg, const char* cls);
    ComponentName(const sp<Context>& pkg, const sp<String>& cls);

    /**
     * Return the package name of this component.
     */
    sp<String> getPackageName() {
        return mPackage;
    }

    /**
     * Return the class name of this component.
     */
    sp<String> getClassName() {
        return mClass;
    }

    /**
     * Return string representation of this class without the class's name
     * as a prefix.
     */
    sp<String> toShortString() {
        return String::format("{%s/%s}", mPackage->c_str(), mClass->c_str());
    }

    sp<String> toString() {
        return String::format("ComponentInfo{%s/%s}", mPackage->c_str(), mClass->c_str());
    }

    bool equals(const sp<ComponentName>& other) {
        if (other != nullptr) {
            return mPackage->equals(other->mPackage)
                    && mClass->equals(other->mClass);
        }
        return false;
    }

    bool equals(const sp<Object>& obj) const override {
        if (obj != nullptr) {
            if (Class<ComponentName>::isInstance(obj)) {
                sp<ComponentName> other = Class<ComponentName>::cast(obj);
                return mPackage->equals(other->mPackage) && mClass->equals(other->mClass);
            } else {
                return false;
            }
        }
        return false;
    }

    size_t hashCode() const override {
        return mPackage->hashCode() + mClass->hashCode();
    }

private:
    sp<String> mPackage;
    sp<String> mClass;
};

} /* namespace mindroid */

#endif /* MINDROID_COMPONENTNAME_H_ */
