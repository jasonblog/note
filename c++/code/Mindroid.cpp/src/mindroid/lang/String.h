/*
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

#ifndef MINDROID_STRING_H_
#define MINDROID_STRING_H_

#include "mindroid/lang/Object.h"
#include "mindroid/util/Assert.h"
#include "mindroid/util/ArrayList.h"
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cctype>

namespace mindroid {

class String :
        public Object {
public:
    static const sp<String> EMPTY_STRING;

    String();
    explicit String(const char* string);
    explicit String(const char* string, size_t size);
    explicit String(const uint8_t* string, size_t size) : String(reinterpret_cast<const char*>(string), size) { }
    explicit String(const char c);
    virtual ~String() { }

    bool equals(const sp<Object>& other) const override;
    size_t hashCode() const override;

    bool equals(const char* string) const;
    bool equals(const sp<String>& string) const;
    bool equalsIgnoreCase(const char* string) const;
    bool equalsIgnoreCase(const sp<String>& string) const;

    inline size_t length() const {
        return mStringBuffer->mSize;
    }

    inline bool isEmpty() const {
        return length() == 0;
    }

    inline const char* c_str() const {
        return mStringBuffer->mData;
    }

    inline operator const char*() const {
        return c_str();
    }

    inline char operator[](const size_t index) const {
        Assert::assertTrue(index < length());
        return mStringBuffer->mData[index];
    }

    inline char charAt(size_t index) const {
        Assert::assertTrue(index < length());
        return mStringBuffer->mData[index];
    }

    bool contains(const char* subString) const;
    bool contains(const sp<String>& subString) const;

    bool startsWith(const char* prefix) const;
    bool startsWith(const sp<String>& prefix) const;

    bool endsWith(const char* suffix) const;
    bool endsWith(const sp<String>& suffix) const;

    sp<String> substring(size_t beginIndex) const;
    sp<String> substring(size_t beginIndex, size_t endIndex) const;

    sp<String> toLowerCase() const;
    sp<String> toUpperCase() const;

    ssize_t indexOf(const char c) const;
    ssize_t indexOf(const char* string) const;
    ssize_t indexOf(const sp<String>& string) const;
    ssize_t indexOf(const char c, size_t fromIndex) const;
    ssize_t indexOf(const char* string, size_t fromIndex) const;
    ssize_t indexOf(const sp<String>& string, size_t fromIndex) const;

    ssize_t lastIndexOf(const char c) const;
    ssize_t lastIndexOf(const char* string) const;
    ssize_t lastIndexOf(const sp<String>& string) const;
    ssize_t lastIndexOf(const char c, size_t fromIndex) const;
    ssize_t lastIndexOf(const char* string, size_t fromIndex) const;
    ssize_t lastIndexOf(const sp<String>& string, size_t fromIndex) const;

    sp<String> trim() const;

    sp<ArrayList<sp<String>>> split(const char* separator) const;
    sp<ArrayList<sp<String>>> split(const sp<String>& separator) const;

    inline static sp<String> valueOf(const char* string) {
        return (string != nullptr) ? new String(string) : nullptr;
    }

    inline static sp<String> valueOf(const char* string, size_t size) {
        return (string != nullptr) ? new String(string, size) : nullptr;
    }

    inline static sp<String> valueOf(bool b) {
        return new String(b ? "true" : "false");
    }

    inline static sp<String> valueOf(char c) {
        return new String(c);
    }

    static sp<String> valueOf(int8_t value);
    static sp<String> valueOf(uint8_t value);
    static sp<String> valueOf(int16_t value);
    static sp<String> valueOf(uint16_t value);
    static sp<String> valueOf(int32_t value);
    static sp<String> valueOf(uint32_t value);
    static sp<String> valueOf(float value);
    static sp<String> valueOf(double value);

    static sp<String> format(const char* format, ...) __attribute__((format (printf, 1, 2)));

    static size_t length(const char* string) {
        return strlen(string);
    }

    sp<String> replace(char oldChar, char newChar);

    sp<String> append(const char* string) const {
        return append(string, strlen(string));
    }

    sp<String> append(const sp<String>& string) const {
        return append(string->c_str(), strlen(string->c_str()));
    }

    sp<String> append(const char* string, size_t size) const;

    sp<String> append(const char* string, size_t offset, size_t size) const;

    sp<String> append(const sp<String>& string, size_t offset, size_t size) const {
        Assert::assertTrue("IndexOutOfBoundsException", offset + size <= string->length());
        return append(string->c_str(), offset, size);
    }

    sp<String> appendFormatted(const char* format, ...) const __attribute__((format (printf, 2, 3)));

private:
    class StringBuffer : public LightweightObject<StringBuffer> {
    public:
        StringBuffer() :
                mData(nullptr),
                mSize(0) {
        }

        StringBuffer(size_t size);
        StringBuffer(const char* string, size_t size);
        StringBuffer(const char* string1, size_t size1, const char* string2, size_t size2);

        ~StringBuffer() {
            if (mData != nullptr) {
                free(mData);
            }
        }

    private:
        char* mData;
        size_t mSize;

        friend class String;
        friend class StringBuilder;
    };

    String(const sp<StringBuffer>& string) :
            mStringBuffer(string) {
    }

    sp<String> appendFormattedWithVarArgList(const char* format, va_list args) const;

    sp<StringBuffer> mStringBuffer;
    static const sp<StringBuffer> EMPTY_STRING_BUFFER;

    friend class StringBuilder;
};

} /* namespace mindroid */

#endif /* MINDROID_STRING_H_ */
