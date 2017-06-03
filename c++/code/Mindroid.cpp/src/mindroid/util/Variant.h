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

#ifndef MINDROID_VARIANT_H_
#define MINDROID_VARIANT_H_

#include "mindroid/lang/Object.h"
#include "mindroid/lang/String.h"
#include "mindroid/os/IBinder.h"
#include "mindroid/util/Set.h"
#include "mindroid/util/ArrayList.h"

namespace mindroid {

class Bundle;

class Variant :
        public Object {
public:
    enum Type {
        Null,
        Bool,
        Byte,
        Char,
        Short,
        UnsignedShort,
        Int,
        UnsignedInt,
        Long,
        UnsignedLong,
        Float,
        Double,
        String,
        StringSet,
        StringArrayList,
        IntegerArrayList,
        Object,
        Bundle,
        Binder
    };

    inline Variant() : mType(Null) { }

    inline Variant(bool value) :
            mType(Bool) {
        mValue.boolValue = value;
    }

    inline Variant(uint8_t value) :
            mType(Byte) {
        mValue.byteValue = value;
    }

    inline Variant(char value) :
            mType(Char) {
        mValue.charValue = value;
    }

    inline Variant(int16_t value) :
            mType(Short) {
        mValue.shortValue = value;
    }

    inline Variant(uint16_t value) :
            mType(UnsignedShort) {
        mValue.unsignedShortValue = value;
    }

    inline Variant(int32_t value) :
            mType(Int) {
        mValue.intValue = value;
    }

    inline Variant(uint32_t value) :
            mType(UnsignedInt) {
        mValue.unsignedIntValue = value;
    }

    inline Variant(int64_t value) :
            mType(Long) {
        mValue.longValue = value;
    }

    inline Variant(uint64_t value) :
            mType(UnsignedLong) {
        mValue.unsignedLongValue = value;
    }

    inline Variant(float value) :
            mType(Float) {
        mValue.floatValue = value;
    }

    inline Variant(double value) :
            mType(Double) {
        mValue.doubleValue = value;
    }

    inline Variant(const char* string) :
            mType(String) {
        const sp<mindroid::String> object = String::valueOf(string);
        if (object != nullptr) {
            object->incStrongReference(this);
        }
        mValue.object = object.getPointer();
    }

    inline Variant(const sp<mindroid::String>& string) :
            mType(String) {
        if (string != nullptr) {
            string->incStrongReference(this);
        }
        mValue.object = string.getPointer();
    }

    inline Variant(const sp<mindroid::Set<sp<mindroid::String>>>& set) :
            mType(StringSet) {
        if (set != nullptr) {
            set->incStrongReference(this);
        }
        mValue.object = set.getPointer();
    }

    inline Variant(const sp<mindroid::ArrayList<sp<mindroid::String>>>& stringArrayList) :
            mType(StringArrayList) {
        if (stringArrayList != nullptr) {
            stringArrayList->incStrongReference(this);
        }
        mValue.object = stringArrayList.getPointer();
    }

    inline Variant(const sp<mindroid::ArrayList<int32_t>>& integerArrayList) :
            mType(IntegerArrayList) {
        if (integerArrayList != nullptr) {
            integerArrayList->incStrongReference(this);
        }
        mValue.object = integerArrayList.getPointer();
    }

    inline Variant(const sp<mindroid::Object>& object) :
            mType(Object) {
        if (object != nullptr) {
            object->incStrongReference(this);
        }
        mValue.object = object.getPointer();
    }

    inline Variant(const sp<mindroid::Bundle>& bundle);

    inline Variant(const sp<mindroid::IBinder>& binder) :
            mType(Binder) {
        if (binder != nullptr) {
            binder->incStrongReference(this);
        }
        mValue.binder = binder.getPointer();
    }

    ~Variant();

    Variant(const Variant&) = delete;
    Variant& operator=(const Variant&) = delete;

    bool equals(const sp<mindroid::Object>& other) const override;
    size_t hashCode() const override;

    inline bool isBool() const {
        return mType == Bool;
    }

    inline bool getBool() const {
        return mValue.boolValue;
    }

    inline bool isByte() const {
        return mType == Byte;
    }

    inline uint8_t getByte() const {
        return mValue.byteValue;
    }

    inline bool isChar() const {
        return mType == Char;
    }

    inline char getChar() const {
        return mValue.charValue;
    }

    inline bool isShort() const {
        return mType == Short;
    }

    inline int16_t getShort() const {
        return mValue.shortValue;
    }

    inline bool isUnsignedShort() const {
        return mType == UnsignedShort;
    }

    inline uint16_t getUnsignedShort() const {
        return mValue.unsignedShortValue;
    }

    inline bool isInt() const {
        return mType == Int;
    }

    inline int32_t getInt() const {
        return mValue.intValue;
    }

    inline bool isUnsignedInt() const {
        return mType == UnsignedInt;
    }

    inline uint32_t getUnsignedInt() const {
        return mValue.unsignedIntValue;
    }

    inline bool isLong() const {
        return mType == Long;
    }

    inline int64_t getLong() const {
        return mValue.longValue;
    }

    inline bool isUnsignedLong() const {
        return mType == UnsignedLong;
    }

    inline uint64_t getUnsignedLong() const {
        return mValue.unsignedLongValue;
    }

    inline bool isFloat() const {
        return mType == Float;
    }

    inline float getFloat() const {
        return mValue.floatValue;
    }

    inline bool isDouble() const {
        return mType == Double;
    }

    inline double getDouble() const {
        return mValue.doubleValue;
    }

    inline bool isString() const {
        return mType == String;
    }

    inline sp<mindroid::String> getString() const {
        return static_cast<mindroid::String*>(mValue.object);
    }

    inline bool isStringSet() const {
        return mType == StringSet;
    }

    inline sp<mindroid::Set<sp<mindroid::String>>> getStringSet() const {
        return static_cast<mindroid::Set<sp<mindroid::String>>*>(mValue.object);
    }

    inline bool isStringArrayList() const {
        return mType == StringArrayList;
    }

    inline sp<mindroid::ArrayList<sp<mindroid::String>>> getStringArrayList() const {
        return static_cast<mindroid::ArrayList<sp<mindroid::String>>*>(mValue.object);
    }

    inline bool isIntegerArrayList() const {
        return mType == IntegerArrayList;
    }

    inline sp<mindroid::ArrayList<int32_t>> getIntegerArrayList() const {
        return static_cast<mindroid::ArrayList<int32_t>*>(mValue.object);
    }

    inline bool isObject() const {
        return mType == Object;
    }

    inline sp<mindroid::Object> getObject() const {
        return static_cast<mindroid::Object*>(mValue.object);
    }

    inline bool isBundle() const {
        return mType == Bundle;
    }

    inline sp<mindroid::Bundle> getBundle() const;

    inline bool isBinder() const {
        return mType == Binder;
    }

    inline sp<mindroid::IBinder> getBinder() const {
        if (mType == Binder) {
            return mValue.binder;
        } else {
            return nullptr;
        }
    }

    inline bool isNull() {
        return mType == Null;
    }

    inline Type getType() const {
        return mType;
    }

    inline bool isBasicType() const {
        return ((mType == Null) ||
                (mType == Bool) ||
                (mType == Byte) ||
                (mType == Char) ||
                (mType == Short) ||
                (mType == UnsignedShort) ||
                (mType == Int) ||
                (mType == UnsignedInt) ||
                (mType == Long) ||
                (mType == UnsignedLong) ||
                (mType == Float) ||
                (mType == Double) ||
                (mType == String));
    }

private:
    static size_t hashCode(size_t seed, const void* data, const size_t size);

    typedef union {
        bool boolValue;
        uint8_t byteValue;
        char charValue;
        int16_t shortValue;
        uint16_t unsignedShortValue;
        int32_t intValue;
        uint32_t unsignedIntValue;
        int64_t longValue;
        uint64_t unsignedLongValue;
        float floatValue;
        double doubleValue;
        mindroid::Object* object;
        mindroid::IBinder* binder;
    } Value;

    Type mType;
    Value mValue;
};

} /* namespace mindroid */

#endif /* MINDROID_VARIANT_H_ */
