/*
 * Copyright (C) 2013 Daniel Himmelein
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

#include "mindroid/util/Variant.h"
#include "mindroid/lang/Class.h"
#include "mindroid/os/Bundle.h"

namespace mindroid {

inline Variant::Variant(const sp<mindroid::Bundle>& bundle) :
        mType(Bundle) {
    if (bundle != nullptr) {
        bundle->incStrongReference(this);
    }
    mValue.object = bundle.getPointer();
}

Variant::~Variant() {
    switch (mType) {
    case String:
    case StringSet:
    case StringArrayList:
    case IntegerArrayList:
    case Object:
    case Bundle:
        if (mValue.object != nullptr) {
            mValue.object->decStrongReference(this);
        }
        break;
    case Binder:
        if (mValue.binder != nullptr) {
            mValue.binder->decStrongReference(this);
        }
        break;
    default:
        break;
    }
}

inline sp<mindroid::Bundle> Variant::getBundle() const {
    return static_cast<mindroid::Bundle*>(mValue.object);
}

bool Variant::equals(const sp<mindroid::Object>& other) const {
    if (other != nullptr) {
        if (Class<Variant>::isInstance(other)) {
            sp<Variant> v = Class<Variant>::cast(other);
            if (mType == v->mType) {
                switch (mType) {
                case Null:
                    return true;
                case Bool:
                    return mValue.boolValue == v->mValue.boolValue;
                case Byte:
                    return mValue.byteValue == v->mValue.byteValue;
                case Char:
                    return mValue.charValue == v->mValue.charValue;
                case Short:
                    return mValue.shortValue == v->mValue.shortValue;
                case UnsignedShort:
                    return mValue.unsignedShortValue == v->mValue.unsignedShortValue;
                case Int:
                    return mValue.intValue == v->mValue.intValue;
                case UnsignedInt:
                    return mValue.unsignedIntValue == v->mValue.unsignedIntValue;
                case Long:
                    return mValue.longValue == v->mValue.longValue;
                case UnsignedLong:
                    return mValue.unsignedLongValue == v->mValue.unsignedLongValue;
                case Float:
                    return mValue.floatValue == v->mValue.floatValue;
                case Double:
                    return mValue.doubleValue == v->mValue.doubleValue;
                case String:
                case StringSet:
                case StringArrayList:
                case IntegerArrayList:
                case Object:
                case Bundle:
                    if (mValue.object != nullptr) {
                        return mValue.object->equals(v->mValue.object);
                    } else {
                        return v->mValue.object == nullptr;
                    }
                case Binder:
                    if (mValue.binder != nullptr) {
                        return mValue.binder->equals(v->mValue.binder);
                    } else {
                        return v->mValue.binder == nullptr;
                    }
                default:
                    return false;
                }
            }
        }
    }
    return false;
}

size_t Variant::hashCode() const {
    switch (mType) {
    case Null:
        return mType;
    case Bool:
        return hashCode(mType, &mValue.boolValue, sizeof(mValue.boolValue));
    case Byte:
        return hashCode(mType, &mValue.byteValue, sizeof(mValue.byteValue));
    case Char:
        return hashCode(mType, &mValue.charValue, sizeof(mValue.charValue));
    case Short:
        return hashCode(mType, &mValue.shortValue, sizeof(mValue.shortValue));
    case UnsignedShort:
        return hashCode(mType, &mValue.unsignedShortValue, sizeof(mValue.unsignedShortValue));
    case Int:
        return hashCode(mType, &mValue.intValue, sizeof(mValue.intValue));
    case UnsignedInt:
        return hashCode(mType, &mValue.unsignedIntValue, sizeof(mValue.unsignedIntValue));
    case Long:
        return hashCode(mType, &mValue.longValue, sizeof(mValue.longValue));
    case UnsignedLong:
        return hashCode(mType, &mValue.unsignedLongValue, sizeof(mValue.unsignedLongValue));
    case Float:
        return hashCode(mType, &mValue.floatValue, sizeof(mValue.floatValue));
    case Double:
        return hashCode(mType, &mValue.doubleValue, sizeof(mValue.doubleValue));
    case String:
    case StringSet:
    case StringArrayList:
    case IntegerArrayList:
    case Object:
    case Bundle:
        if (mValue.object != nullptr) {
            return mValue.object->hashCode();
        } else {
            return 0;
        }
    case Binder:
        if (mValue.binder != nullptr) {
            return mValue.binder->hashCode();
        } else {
            return 0;
        }
    default:
        return 0;
    }
}

size_t Variant::hashCode(size_t seed, const void* data, const size_t size) {
    size_t hash = seed;
    for (size_t i = 0; i < size; i++) {
        hash = 31 * hash + static_cast<const uint8_t*>(data)[i];
    }
    return hash;
}

} /* namespace mindroid */
