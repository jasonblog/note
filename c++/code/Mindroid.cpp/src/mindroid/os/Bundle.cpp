/*
 * Copyright (C) 2007 The Android Open Source Project
 * Copyright (C) 2012 Daniel Himmelein
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

#include "mindroid/os/Bundle.h"

namespace mindroid {

Bundle::Bundle() {
    mMap = new HashMap<sp<String>, sp<Variant>>();
}

Bundle::~Bundle() {
    clear();
}

Bundle::Bundle(const sp<Bundle>& other) : Bundle() {
    putAll(other);
}

sp<Set<sp<String>>> Bundle::keySet() const {
    if (mMap->isEmpty()) {
        return nullptr;
    }

    sp<Set<sp<String>>> keys = new Set<sp<String>>();
    auto itr = mMap->iterator();
    while (itr.hasNext()) {
        auto pair = itr.next();
        keys->add(pair.getKey());
    }
    return keys;
}

bool Bundle::containsKey(const sp<String>& key) {
    return mMap->containsKey(key);
}

void Bundle::remove(const sp<String>& key) {
    mMap->remove(key);
}

void Bundle::putAll(const sp<Bundle>& bundle) {
    mMap->putAll(bundle->mMap);
}

void Bundle::putBinder(const sp<String>& key, const sp<IBinder>& binder) {
    mMap->put(key, new Variant(binder));
}

void Bundle::putBoolean(const sp<String>& key, bool value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putBundle(const sp<String>& key, const sp<Bundle>& value) {
    mMap->put(key, new Variant(sp<Object>(value)));
}

void Bundle::putByte(const sp<String>& key, uint8_t value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putChar(const sp<String>& key, char value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putDouble(const sp<String>& key, double value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putFloat(const sp<String>& key, float value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putInt(const sp<String>& key, int32_t value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putUnsignedInt(const sp<String>& key, uint32_t value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putIntegerArrayList(const sp<String>& key, const sp<ArrayList<int32_t>>& value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putLong(const sp<String>& key, int64_t value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putUnsignedLong(const sp<String>& key, uint64_t value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putObject(const sp<String>& key, const sp<Object>& object) {
    mMap->put(key, new Variant(object));
}

void Bundle::putShort(const sp<String>& key, int16_t value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putUnsignedShort(const sp<String>& key, uint16_t value) {
    mMap->put(key, new Variant(value));
}

void Bundle::putString(const sp<String>& key, const sp<String>& string) {
    mMap->put(key, new Variant(string));
}

void Bundle::putStringArrayList(const sp<String>& key, const sp<ArrayList<sp<String>>>& value) {
    mMap->put(key, new Variant(value));
}

sp<IBinder> Bundle::getBinder(const sp<String>& key) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isBinder()) {
            return value->getBinder();
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

bool Bundle::getBoolean(const sp<String>& key, const bool defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isBool()) {
            return value->getBool();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

sp<Bundle> Bundle::getBundle(const sp<String>& key) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isObject()) {
            return object_cast<Bundle>(value->getObject());
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

uint8_t Bundle::getByte(const sp<String>& key, const uint8_t defaultValue) const {
    sp<Variant> value = mMap->get(key);
        if (value != nullptr) {
        if (value->isByte()) {
            return value->getByte();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

char Bundle::getChar(const sp<String>& key, const char defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isChar()) {
            return value->getChar();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

double Bundle::getDouble(const sp<String>& key, const double defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isDouble()) {
            return value->getDouble();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

float Bundle::getFloat(const sp<String>& key, const float defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isFloat()) {
            return value->getFloat();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

int32_t Bundle::getInt(const sp<String>& key, const int32_t defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isInt()) {
            return value->getInt();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

uint32_t Bundle::getUnsignedInt(const sp<String>& key, const uint32_t defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isUnsignedInt()) {
            return value->getUnsignedInt();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

sp<ArrayList<int32_t>> Bundle::getIntegerArrayList(const sp<String>& key) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isIntegerArrayList()) {
            return value->getIntegerArrayList();
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

int64_t Bundle::getLong(const sp<String>& key, const int64_t defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isLong()) {
            return value->getLong();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

uint64_t Bundle::getUnsignedLong(const sp<String>& key, const uint64_t defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isUnsignedLong()) {
            return value->getUnsignedLong();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

sp<Object> Bundle::getObject(const sp<String>& key) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isObject()) {
            return value->getObject();
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

sp<Object> Bundle::getObject(const sp<String>& key, const sp<Object>& defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isObject()) {
            return value->getObject();
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

int16_t Bundle::getShort(const sp<String>& key, const int16_t defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isShort()) {
            return value->getShort();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

uint16_t Bundle::getUnsignedShort(const sp<String>& key, const uint16_t defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isUnsignedShort()) {
            return value->getUnsignedShort();
        } else {
            return defaultValue;
        }
    }
    return defaultValue;
}

sp<String> Bundle::getString(const sp<String>& key) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isString()) {
            return value->getString();
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

sp<String> Bundle::getString(const sp<String>& key, const char* defaultValue) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isString()) {
            return value->getString();
        } else {
            return String::valueOf(defaultValue);
        }
    }
    return String::valueOf(defaultValue);
}

sp<ArrayList<sp<String>>> Bundle::getStringArrayList(const sp<String>& key) const {
    sp<Variant> value = mMap->get(key);
    if (value != nullptr) {
        if (value->isStringArrayList()) {
            return value->getStringArrayList();
        } else {
            return nullptr;
        }
    }
    return nullptr;
}

/**
 * Retain only basic types within the Bundle.
 *
 * @hide
 */
void Bundle::retainBasicTypes() {
    if (mMap != nullptr) {
        auto itr = mMap->iterator();
        while (itr.hasNext()) {
            auto pair = itr.next();
            sp<Variant> value = pair.getValue();

            if (value->isBasicType()) {
                continue;
            }
            if (value->isBundle()) {
                object_cast<Bundle>(value)->retainBasicTypes();
                continue;
            }
            itr.remove();
        }
    }
}

} /* namespace mindroid */
