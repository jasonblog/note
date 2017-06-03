/*
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

#ifndef MINDROID_CLASS_H_
#define MINDROID_CLASS_H_

#include "mindroid/lang/String.h"
#include "mindroid/util/concurrent/locks/ReentrantLock.h"
#include "mindroid/util/HashMap.h"
#include <pthread.h>

namespace mindroid {

class Factory;

class Classes {
public:
    void put(const sp<String>& name, Factory* factory);
    Factory* get(const sp<String>& name);

    static Classes* getInstance();

private:
    Classes() : mClasses(new HashMap<sp<String>, Factory*>()) { }

    static pthread_mutex_t sMutex;
    static Classes* sInstance;

    sp<HashMap<sp<String>, Factory*>> mClasses;
};

class Factory {
public:
    Factory(const char* name) {
        Classes::getInstance()->put(String::valueOf(name), this);
    }

    virtual ~Factory() = default;

    virtual sp<Object> newInstance() = 0;
};

template<typename T>
class Class final :
        public Object {
public:
    static bool isInstance(const sp<Object>& o) {
        if (o != nullptr) {
            sp<T> t = dynamic_cast<T*>(o.getPointer());
            return t != nullptr;
        } else {
            return false;
        }
    }

    static sp<T> cast(const sp<Object>& o) {
        if (o != nullptr) {
            sp<T> t = dynamic_cast<T*>(o.getPointer());
            return t;
        } else {
            return nullptr;
        }
    }

    static sp<Class> forName(const char* className) {
        return forName(String::valueOf(className));
    }

    static sp<Class> forName(const sp<String>& className) {
        return new Class(className);
    }

    sp<String> getName() {
        return mName;
    }

    sp<T> newInstance() {
        Factory* factory = Classes::getInstance()->get(mName);
        if (factory != nullptr) {
            return object_cast<T>(factory->newInstance());
        } else {
            return nullptr;
        }
    }

private:
    Class(const sp<String>& name) : mName(name) {
    }

    sp<String> mName;
};

#define CLASS(Package, Clazz) \
class Clazz##Factory : public Factory { \
public: \
    Clazz##Factory() : Factory(#Package"::"#Clazz) { \
    } \
    \
    virtual sp<Object> newInstance() { \
        return new Package::Clazz(); \
    } \
}; \
static volatile Clazz##Factory s##Clazz##Factory;

} /* namespace mindroid */

#endif /* MINDROID_CLASS_H_ */
