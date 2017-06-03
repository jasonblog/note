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

#include "mindroid/lang/String.h"
#include "mindroid/lang/Class.h"
#include <cstdio>

namespace mindroid {

/*
 * C++ Standard Core Language (http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_closed.html#269)
 * Objects with static storage duration defined in namespace scope in the same translation unit
 * and dynamically initialized shall be initialized in the order in which their definition appears
 * in the translation unit.
 */
const sp<String::StringBuffer> String::EMPTY_STRING_BUFFER = new String::StringBuffer("", 0);
const sp<String> String::EMPTY_STRING = new String();

String::String() {
    mStringBuffer = EMPTY_STRING_BUFFER;
}

String::String(const char* string) {
    if (string != nullptr) {
        size_t size = strlen(string);
        if (size > 0) {
            mStringBuffer = new StringBuffer(string, size);
        } else {
            mStringBuffer = EMPTY_STRING_BUFFER;
        }
    } else {
        mStringBuffer = EMPTY_STRING_BUFFER;
    }
}

String::String(const char* string, size_t size) {
    if (string != nullptr) {
        if (size > 0) {
            mStringBuffer = new StringBuffer(string, size);
        } else {
            mStringBuffer = EMPTY_STRING_BUFFER;
        }
    } else {
        mStringBuffer = EMPTY_STRING_BUFFER;
    }
}

String::String(const char c) {
    mStringBuffer = new StringBuffer(&c, 1);
}

bool String::equals(const sp<Object>& other) const {
    if (other == this) {
        return true;
    }

    if (Class<String>::isInstance(other)) {
        sp<String> s = Class<String>::cast(other);
        return this->equals(s);
    } else {
        return false;
    }
}

size_t String::hashCode() const {
    size_t hash = 0;
    for (size_t i = 0; i < mStringBuffer->mSize; i++) {
        hash = 31 * hash + mStringBuffer->mData[i];
    }
    return hash;
}

bool String::equals(const char* string) const {
    if (mStringBuffer->mData && string) {
        return strcmp(mStringBuffer->mData, string) == 0;
    } else {
        if (mStringBuffer->mData == nullptr && string == nullptr) {
            return true;
        } else {
            return false;
        }
    }
}

bool String::equals(const sp<String>& string) const {
    if (mStringBuffer->mData && string->mStringBuffer->mData) {
        return strcmp(mStringBuffer->mData, string->mStringBuffer->mData) == 0;
    } else {
        if (mStringBuffer->mData == nullptr && string->mStringBuffer->mData == nullptr) {
            return true;
        } else {
            return false;
        }
    }
}

bool String::equalsIgnoreCase(const char* string) const {
    sp<String> tmp = String::valueOf(string);
    return toLowerCase()->equals(tmp->toLowerCase());
}

bool String::equalsIgnoreCase(const sp<String>& string) const {
    return toLowerCase()->equals(string->toLowerCase());
}

bool String::contains(const char* subString) const {
    if (c_str() && subString) {
        return strstr(c_str(), subString) != nullptr;
    } else {
        return false;
    }
}

bool String::contains(const sp<String>& subString) const {
    return contains(subString->c_str());
}

bool String::startsWith(const char* prefix) const {
    size_t prefixSize = strlen(prefix);
    if (length() >= prefixSize) {
        return strncmp(c_str(), prefix, prefixSize) == 0;
    } else {
        return false;
    }
}

bool String::startsWith(const sp<String>& prefix) const {
    return startsWith(prefix->c_str());
}

bool String::endsWith(const char* suffix) const {
    size_t suffixSize = strlen(suffix);
    if (length() >= suffixSize) {
        return strncmp(c_str() + length() - suffixSize, suffix, suffixSize) == 0;
    } else {
        return false;
    }
}

bool String::endsWith(const sp<String>& suffix) const {
    return endsWith(suffix->c_str());
}

sp<String> String::substring(size_t beginIndex) const {
    if (beginIndex < length()) {
        return String::valueOf(c_str() + beginIndex, length() - beginIndex);
    } else {
        return EMPTY_STRING;
    }
}

sp<String> String::substring(size_t beginIndex, size_t endIndex) const {
    if (beginIndex < length()) {
        return String::valueOf(c_str() + beginIndex, endIndex - beginIndex);
    } else {
        return EMPTY_STRING;
    }
}

sp<String> String::toLowerCase() const {
    sp<StringBuffer> stringBuffer = new StringBuffer(length());
    for (size_t i = 0; i < length(); i++) {
        stringBuffer->mData[i] = tolower(mStringBuffer->mData[i]);
    }
    return new String(stringBuffer);
}

sp<String> String::toUpperCase() const {
    sp<StringBuffer> stringBuffer = new StringBuffer(length());
    for (size_t i = 0; i < length(); i++) {
        stringBuffer->mData[i] = toupper(mStringBuffer->mData[i]);
    }
    return new String(stringBuffer);
}

ssize_t String::indexOf(const char c) const {
    const char* substr = strchr(c_str(), c);
    if (substr != nullptr) {
        return substr - c_str();
    } else {
        return -1;
    }
}

ssize_t String::indexOf(const char* string) const {
    const char* substr = strstr(c_str(), string);
    if (substr != nullptr) {
        return substr - c_str();
    } else {
        return -1;
    }
}

ssize_t String::indexOf(const sp<String>& string) const {
    return indexOf(string->c_str());
}

ssize_t String::indexOf(const char c, size_t fromIndex) const {
    const char* substr = strchr(c_str() + fromIndex, c);
    if (substr != nullptr) {
        return substr - c_str();
    } else {
        return -1;
    }
}

ssize_t String::indexOf(const char* string, size_t fromIndex) const {
    const char* substr = strstr(c_str() + fromIndex, string);
    if (substr != nullptr) {
        return substr - c_str();
    } else {
        return -1;
    }
}

ssize_t String::indexOf(const sp<String>& string, size_t fromIndex) const {
    return indexOf(string->c_str() + fromIndex);
}

ssize_t String::lastIndexOf(const char c) const {
    const char* substr = strrchr(c_str(), c);
    if (substr != nullptr) {
        return substr - c_str();
    } else {
        return -1;
    }
}

ssize_t String::lastIndexOf(const char* string) const {
    return -1;
}

ssize_t String::lastIndexOf(const sp<String>& string) const {
    return -1;
}

ssize_t String::lastIndexOf(const char c, size_t fromIndex) const {
    const char* substr = strrchr(c_str() + fromIndex, c);
    if (substr != nullptr) {
        return substr - c_str();
    } else {
        return -1;
    }
}

ssize_t String::lastIndexOf(const char* string, size_t fromIndex) const {
    return -1;
}

ssize_t String::lastIndexOf(const sp<String>& string, size_t fromIndex) const {
    return -1;
}

sp<String> String::trim() const {
    size_t beginIndex;
    ssize_t endIndex;
    for (beginIndex = 0; beginIndex < length(); beginIndex++) {
        if (!isspace(mStringBuffer->mData[beginIndex])) {
            break;
        }
    }
    for (endIndex = length() - 1; endIndex >= 0; endIndex--) {
        if (!isspace(mStringBuffer->mData[endIndex])) {
            break;
        }
    }
    if (beginIndex == 0 && endIndex == (ssize_t) length() - 1) {
        return const_cast<String*>(this);
    } else {
        if (beginIndex != length()) {
            return new String(new StringBuffer(mStringBuffer->mData + beginIndex,
                    endIndex - beginIndex + 1));
        } else {
            return EMPTY_STRING;
        }
    }
}

sp<ArrayList<sp<String>>> String::split(const char* separator) const {
    sp<ArrayList<sp<String>>> strings = new ArrayList<sp<String>>();
    ssize_t curIndex = 0;
    ssize_t prevCurIndex;
    while (curIndex >= 0 && (size_t) curIndex < length()) {
        prevCurIndex = curIndex;
        curIndex = indexOf(separator, curIndex);
        if (curIndex >= 0) {
            strings->add(substring(prevCurIndex, curIndex));
            curIndex += strlen(separator);
        } else {
            strings->add(substring(prevCurIndex, length()));
        }
    }
    return strings;
}

sp<ArrayList<sp<String>>> String::split(const sp<String>& separator) const {
    return split(separator->c_str());
}

sp<String> String::valueOf(int8_t value) {
    size_t size = snprintf(NULL, 0, "%d", value);
    sp<StringBuffer> stringBuffer = new StringBuffer(size);
    snprintf(stringBuffer->mData, size + 1, "%d", value);
    return new String(stringBuffer);
}

sp<String> String::valueOf(uint8_t value) {
    size_t size = snprintf(NULL, 0, "%u", value);
    sp<StringBuffer> stringBuffer = new StringBuffer(size);
    snprintf(stringBuffer->mData, size + 1, "%u", value);
    return new String(stringBuffer);
}

sp<String> String::valueOf(int16_t value) {
    size_t size = snprintf(NULL, 0, "%d", value);
    sp<StringBuffer> stringBuffer = new StringBuffer(size);
    snprintf(stringBuffer->mData, size + 1, "%d", value);
    return new String(stringBuffer);
}

sp<String> String::valueOf(uint16_t value) {
    size_t size = snprintf(NULL, 0, "%u", value);
    sp<StringBuffer> stringBuffer = new StringBuffer(size);
    snprintf(stringBuffer->mData, size + 1, "%u", value);
    return new String(stringBuffer);
}

sp<String> String::valueOf(int32_t value) {
    size_t size = snprintf(NULL, 0, "%d", value);
    sp<StringBuffer> stringBuffer = new StringBuffer(size);
    snprintf(stringBuffer->mData, size + 1, "%d", value);
    return new String(stringBuffer);
}

sp<String> String::valueOf(uint32_t value) {
    size_t size = snprintf(NULL, 0, "%u", value);
    sp<StringBuffer> stringBuffer = new StringBuffer(size);
    snprintf(stringBuffer->mData, size + 1, "%u", value);
    return new String(stringBuffer);
}

sp<String> String::valueOf(float value) {
    size_t size = snprintf(NULL, 0, "%f", value);
    sp<StringBuffer> stringBuffer = new StringBuffer(size);
    snprintf(stringBuffer->mData, size + 1, "%f", value);
    return new String(stringBuffer);
}

sp<String> String::valueOf(double value) {
    size_t size = snprintf(NULL, 0, "%f", value);
    sp<StringBuffer> stringBuffer = new StringBuffer(size);
    snprintf(stringBuffer->mData, size + 1, "%f", value);
    return new String(stringBuffer);
}

sp<String> String::format(const char* format, ...) {
    va_list args;
    va_start(args, format);
    sp<String> formattedString = EMPTY_STRING->appendFormattedWithVarArgList(format, args);
    va_end(args);
    return formattedString;
}

sp<String> String::replace(char oldChar, char newChar) {
    sp<StringBuffer> stringBuffer = new StringBuffer(c_str(), length());
    for (size_t i = 0; i < length(); i++) {
        stringBuffer->mData[i] = (stringBuffer->mData[i] == oldChar) ? newChar : stringBuffer->mData[i];
    }
    return new String(stringBuffer);
}

sp<String> String::append(const char* string, size_t size) const {
    if (string != nullptr && size > 0) {
        return new String(new StringBuffer(mStringBuffer->mData, mStringBuffer->mSize, string, size));
    }
    return const_cast<String*>(this);
}

sp<String> String::append(const char* string, size_t offset, size_t size) const {
    if (string != nullptr && size > 0) {
        return new String(new StringBuffer(mStringBuffer->mData, mStringBuffer->mSize, string + offset, size));
    }
    return const_cast<String*>(this);
}

sp<String> String::appendFormatted(const char* format, ...) const {
    va_list args;
    va_start(args, format);
    sp<String> formattedString = appendFormattedWithVarArgList(format, args);
    va_end(args);
    return formattedString;
}

sp<String> String::appendFormattedWithVarArgList(const char* format, va_list args) const {
    // see http://stackoverflow.com/questions/9937505/va-list-misbehavior-on-linux
    va_list copyOfArgs;
    va_copy(copyOfArgs, args);
    int size = vsnprintf(nullptr, 0, format, copyOfArgs);
    va_end(copyOfArgs);

    if (size != 0) {
        sp<StringBuffer> stringBuffer = new StringBuffer(mStringBuffer->mSize + size);
        memcpy(stringBuffer->mData, mStringBuffer->mData, mStringBuffer->mSize);
        vsnprintf(stringBuffer->mData + mStringBuffer->mSize, size + 1, format, args);
        return new String(stringBuffer);
    }

    return const_cast<String*>(this);
}

String::StringBuffer::StringBuffer(size_t size) {
    mSize = size;
    if (size > 0) {
        mData = (char*) malloc(mSize + 1);
        mData[size] = '\0';
    } else {
        mData = nullptr;
    }
}

String::StringBuffer::StringBuffer(const char* string, size_t size) {
    mSize = size;
    if (string != nullptr) {
        mData = (char*) malloc(mSize + 1);
        memcpy(mData, string, size);
        mData[size] = '\0';
    } else {
        mData = nullptr;
    }
}

String::StringBuffer::StringBuffer(const char* string1, size_t size1,
        const char* string2, size_t size2) {
    mSize = size1 + size2;
    mData = (char*) malloc(mSize + 1);
    memcpy(mData, string1, size1);
    memcpy(mData + size1, string2, size2);
    mData[size1 + size2] = '\0';
}

} /* namespace mindroid */
