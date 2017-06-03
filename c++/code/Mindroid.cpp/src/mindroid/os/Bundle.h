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

#ifndef MINDROID_BUNDLE_H_
#define MINDROID_BUNDLE_H_

#include "mindroid/lang/Object.h"
#include "mindroid/util/HashMap.h"
#include "mindroid/util/Variant.h"

namespace mindroid {

class Bundle :
        public Object {
public:
    /**
     * Constructs a new, empty Bundle.
     */
    Bundle();
    virtual ~Bundle();

    /**
     * Constructs a Bundle containing a copy of the mappings from the given Bundle.
     *
     * @param b a Bundle to be copied.
     */
    Bundle(const sp<Bundle>& b);

    Bundle(const Bundle&) = delete;
    Bundle& operator=(const Bundle&) = delete;

    /**
     * Removes all elements from the mapping of this Bundle.
     */
    void clear() {
        mMap.clear();
    }

    /**
     * Returns the number of mappings contained in this Bundle.
     *
     * @return the number of mappings as an int.
     */
    size_t size() const {
        return mMap->size();
    }

    /**
     * Returns true if the mapping of this Bundle is empty, false otherwise.
     */
    bool isEmpty() const {
        return mMap->isEmpty();
    }

    /**
     * Returns a Set containing the Strings used as keys in this Bundle.
     * @return an Set<String> value, or null
     */
    sp<Set<sp<String>>> keySet() const;

    /**
     * Returns true if the given key is contained in the mapping
     * of this Bundle.
     *
     * @param key a String key
     * @return true if the key is part of the mapping, false otherwise
     */
    bool containsKey(const char* key) {
        return containsKey(String::valueOf(key));
    }
    bool containsKey(const sp<String>& key);

    /**
     * Returns the entry with the given key as an object.
     *
     * @param key a String key
     * @return an Variant, or null
     */
    sp<Variant> get(const char* key) const {
        return mMap->get(String::valueOf(key));
    }
    sp<Variant> get(const sp<String>& key) const {
        return mMap->get(key);
    }

    /**
     * Removes any entry with the given key from the mapping of this Bundle.
     *
     * @param key a String key
     */
    void remove(const char* key) {
        remove(String::valueOf(key));
    }
    void remove(const sp<String>& key);

    /**
     * Inserts all mappings from the given Bundle into this Bundle.
     *
     * @param bundle a Bundle
     */
    void putAll(const sp<Bundle>& bundle);

    /**
     * Inserts an {@link IBinder} value into the mapping of this Bundle, replacing any existing
     * value for the given key. Either key or value may be null.
     *
     * <p class="note">
     * You should be very careful when using this function. In many places where Bundles are used
     * (such as inside of Intent objects), the Bundle can live longer inside of another process than
     * the process that had originally created it. In that case, the IBinder you supply here will
     * become invalid when your process goes away, and no longer usable, even if a new process is
     * created for you later on.
     * </p>
     *
     * @param key a String, or null
     * @param value an IBinder object, or null
     */
    void putBinder(const char* key, const sp<IBinder>& value) {
        putBinder(String::valueOf(key), value);
    }
    void putBinder(const sp<String>& key, const sp<IBinder>& value);

    /**
     * Inserts a Boolean value into the mapping of this Bundle, replacing any existing value for the
     * given key. Either key or value may be null.
     *
     * @param key a String, or null
     * @param value a Boolean, or null
     */
    void putBoolean(const char* key, bool value) {
        putBoolean(String::valueOf(key), value);
    }
    void putBoolean(const sp<String>& key, bool value);

    /**
     * Inserts a Bundle value into the mapping of this Bundle, replacing any existing value for the
     * given key. Either key or value may be null.
     *
     * @param key a String, or null
     * @param value a Bundle object, or null
     */
    void putBundle(const char* key, const sp<Bundle>& value) {
        putBundle(String::valueOf(key), value);
    }
    void putBundle(const sp<String>& key, const sp<Bundle>& value);

    /**
     * Inserts a byte value into the mapping of this Bundle, replacing any existing value for the
     * given key.
     *
     * @param key a String, or null
     * @param value a byte
     */
    void putByte(const char* key, uint8_t value) {
        putByte(String::valueOf(key), value);
    }
    void putByte(const sp<String>& key, uint8_t value);

    /**
     * Inserts a char value into the mapping of this Bundle, replacing any existing value for the
     * given key.
     *
     * @param key a String, or null
     * @param value a char, or null
     */
    void putChar(const char* key, char value) {
        putChar(String::valueOf(key), value);
    }
    void putChar(const sp<String>& key, char value);

    /**
     * Inserts a double value into the mapping of this Bundle, replacing any existing value for the
     * given key.
     *
     * @param key a String, or null
     * @param value a double
     */
    void putDouble(const char* key, double value) {
        putDouble(String::valueOf(key), value);
    }
    void putDouble(const sp<String>& key, double value);

    /**
     * Inserts a float value into the mapping of this Bundle, replacing any existing value for the
     * given key.
     *
     * @param key a String, or null
     * @param value a float
     */
    void putFloat(const char* key, float value) {
        putFloat(String::valueOf(key), value);
    }
    void putFloat(const sp<String>& key, float value);

    /**
     * Inserts an int value into the mapping of this Bundle, replacing any existing value for the
     * given key.
     *
     * @param key a String, or null
     * @param value an int, or null
     */
    void putInt(const char* key, int32_t value) {
        putInt(String::valueOf(key), value);
    }
    void putInt(const sp<String>& key, int32_t value);
    void putUnsignedInt(const char* key, uint32_t value) {
        putUnsignedInt(String::valueOf(key), value);
    }
    void putUnsignedInt(const sp<String>& key, uint32_t value);

    /**
     * Inserts an ArrayList<Integer> value into the mapping of this Bundle, replacing any existing
     * value for the given key. Either key or value may be null.
     *
     * @param key a String, or null
     * @param value an ArrayList<Integer> object, or null
     */
    void putIntegerArrayList(const char* key, const sp<ArrayList<int32_t>>& value) {
        putIntegerArrayList(String::valueOf(key), value);
    }
    void putIntegerArrayList(const sp<String>& key, const sp<ArrayList<int32_t>>& value);

    /**
     * Inserts a long value into the mapping of this Bundle, replacing any existing value for the
     * given key.
     *
     * @param key a String, or null
     * @param value a long
     */
    void putLong(const char* key, int64_t value) {
        putLong(String::valueOf(key), value);
    }
    void putLong(const sp<String>& key, int64_t value);
    void putUnsignedLong(const char* key, uint64_t value) {
        putUnsignedLong(String::valueOf(key), value);
    }
    void putUnsignedLong(const sp<String>& key, uint64_t value);

    /**
     * Inserts an object value into the mapping of this Bundle, replacing any existing value for the
     * given key. Either key or value may be null.
     *
     * @param key a String, or null
     * @param value an object, or null
     */
    void putObject(const char* key, const sp<Object>& value) {
        putObject(String::valueOf(key), value);
    }
    void putObject(const sp<String>& key, const sp<Object>& value);

    /**
     * Inserts a short value into the mapping of this Bundle, replacing any existing value for the
     * given key.
     *
     * @param key a String, or null
     * @param value a short
     */
    void putShort(const char* key, int16_t value) {
        putShort(String::valueOf(key), value);
    }
    void putShort(const sp<String>& key, int16_t value);
    void putUnsignedShort(const char* key, uint16_t value) {
        putUnsignedShort(String::valueOf(key), value);
    }
    void putUnsignedShort(const sp<String>& key, uint16_t value);

    /**
     * Inserts a String value into the mapping of this Bundle, replacing any existing value for the
     * given key. Either key or value may be null.
     *
     * @param key a String, or null
     * @param value a String, or null
     */
    void putString(const char* key, const char* value) {
        putString(String::valueOf(key), String::valueOf(value));
    }
    void putString(const char* key, const sp<String>& value) {
        putString(String::valueOf(key), value);
    }
    void putString(const sp<String>& key, const sp<String>& value);

    /**
     * Inserts an ArrayList<String> value into the mapping of this Bundle, replacing any existing
     * value for the given key. Either key or value may be null.
     *
     * @param key a String, or null
     * @param value an ArrayList<String> object, or null
     */
    void putStringArrayList(const char* key, const sp<ArrayList<sp<String>>>& value) {
        putStringArrayList(String::valueOf(key), value);
    }
    void putStringArrayList(const sp<String>& key, const sp<ArrayList<sp<String>>>& value);

    /**
     * Returns the value associated with the given key, or null if
     * no mapping of the desired type exists for the given key or a null
     * value is explicitly associated with the key.
     *
     * @param key a String, or null
     * @return an IBinder value, or null
     */
    sp<IBinder> getBinder(const char* key) const {
        return getBinder(String::valueOf(key));
    }
    sp<IBinder> getBinder(const sp<String>& key) const;

    /**
     * Returns the value associated with the given key, or false if no mapping of the desired type
     * exists for the given key.
     *
     * @param key a String
     * @return a boolean value
     */
    bool getBoolean(const char* key) const {
        return getBoolean(String::valueOf(key), false);
    }
    bool getBoolean(const sp<String>& key) const {
        return getBoolean(key, false);
    }

    /**
     * Returns the value associated with the given key, or defaultValue if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String
     * @param defaultValue Value to return if key does not exist
     * @return a boolean value
     */
    bool getBoolean(const char* key, const bool defaultValue) const {
        return getBoolean(String::valueOf(key), defaultValue);
    }
    bool getBoolean(const sp<String>& key, const bool defaultValue) const;

    /**
     * Returns the value associated with the given key, or null if no mapping of the desired type
     * exists for the given key or a null value is explicitly associated with the key.
     *
     * @param key a String, or null
     * @return a Bundle value, or null
     */
    sp<Bundle> getBundle(const char* key) const {
        return getBundle(String::valueOf(key));
    }
    sp<Bundle> getBundle(const sp<String>& key) const;

    /**
     * Returns the value associated with the given key, or (byte) 0 if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String
     * @return a byte value
     */
    uint8_t getByte(const char* key) const {
        return getByte(String::valueOf(key), 0);
    }
    uint8_t getByte(const sp<String>& key) const {
        return getByte(key, 0);
    }

    /**
     * Returns the value associated with the given key, or defaultValue if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String
     * @param defaultValue Value to return if key does not exist
     * @return a byte value
     */
    uint8_t getByte(const char* key, const uint8_t defaultValue) const {
        return getByte(String::valueOf(key), defaultValue);
    }
    uint8_t getByte(const sp<String>& key, const uint8_t defaultValue) const;

    /**
     * Returns the value associated with the given key, or (char) 0 if no mapping of the desired type
     * exists for the given key.
     *
     * @param key a String
     * @return a char value
     */
    char getChar(const char* key) const {
        return getChar(String::valueOf(key), 0);
    }
    char getChar(const sp<String>& key) const {
        return getChar(key, 0);
    }

    /**
     * Returns the value associated with the given key, or defaultValue if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String
     * @param defaultValue Value to return if key does not exist
     * @return a char value
     */
    char getChar(const char* key, const char defaultValue) const {
        return getChar(String::valueOf(key), defaultValue);
    }
    char getChar(const sp<String>& key, const char defaultValue) const;

    /**
     * Returns the value associated with the given key, or 0.0 if no mapping of the desired type
     * exists for the given key.
     *
     * @param key a String
     * @return a double value
     */
    double getDouble(const char* key) const {
        return getDouble(String::valueOf(key), 0.0);
    }
    double getDouble(const sp<String>& key) const {
        return getDouble(key, 0.0);
    }

    /**
     * Returns the value associated with the given key, or defaultValue if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String
     * @param defaultValue Value to return if key does not exist
     * @return a double value
     */
    double getDouble(const char* key, const double defaultValue) const {
        return getDouble(String::valueOf(key), defaultValue);
    }
    double getDouble(const sp<String>& key, const double defaultValue) const;

    /**
     * Returns the value associated with the given key, or 0.0f if no mapping of the desired type
     * exists for the given key.
     *
     * @param key a String
     * @return a float value
     */
    float getFloat(const char* key) const {
        return getFloat(String::valueOf(key), 0.0f);
    }
    float getFloat(const sp<String>& key) const {
        return getFloat(key, 0.0f);
    }

    /**
     * Returns the value associated with the given key, or defaultValue if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String
     * @param defaultValue Value to return if key does not exist
     * @return a float value
     */
    float getFloat(const char* key, const float defaultValue) const {
        return getFloat(String::valueOf(key), defaultValue);
    }
    float getFloat(const sp<String>& key, const float defaultValue) const;

    /**
     * Returns the value associated with the given key, or 0 if no mapping of the desired type
     * exists for the given key.
     *
     * @param key a String
     * @return an int value
     */
    int32_t getInt(const char* key) const {
        return getInt(String::valueOf(key), 0);
    }
    int32_t getInt(const sp<String>& key) const {
        return getInt(key, 0);
    }
    uint32_t getUnsignedInt(const char* key) const {
        return getUnsignedInt(String::valueOf(key), 0);
    }
    uint32_t getUnsignedInt(const sp<String>& key) const {
        return getUnsignedInt(key, 0);
    }

    /**
     * Returns the value associated with the given key, or defaultValue if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String
     * @param defaultValue Value to return if key does not exist
     * @return an int value
     */
    int32_t getInt(const char* key, const int32_t defaultValue) const {
        return getInt(String::valueOf(key), defaultValue);
    }
    int32_t getInt(const sp<String>& key, const int32_t defaultValue) const;
    uint32_t getUnsignedInt(const char* key, const uint32_t defaultValue) const {
        return getUnsignedInt(String::valueOf(key), defaultValue);
    }
    uint32_t getUnsignedInt(const sp<String>& key, const uint32_t defaultValue) const;

    /**
     * Returns the value associated with the given key, or null if no mapping of the desired type
     * exists for the given key or a null value is explicitly associated with the key.
     *
     * @param key a String, or null
     * @return an ArrayList<String> value, or null
     */
    sp<ArrayList<int32_t>> getIntegerArrayList(const char* key) const {
        return getIntegerArrayList(String::valueOf(key));
    }
    sp<ArrayList<int32_t>> getIntegerArrayList(const sp<String>& key) const;

    /**
     * Returns the value associated with the given key, or 0L if no mapping of the desired type
     * exists for the given key.
     *
     * @param key a String
     * @return a long value
     */
    int64_t getLong(const char* key) const {
        return getLong(String::valueOf(key), 0);
    }
    int64_t getLong(const sp<String>& key) const {
        return getLong(key, 0);
    }
    uint64_t getUnsignedLong(const char* key) const {
        return getUnsignedLong(String::valueOf(key), 0);
    }
    uint64_t getUnsignedLong(const sp<String>& key) const {
        return getUnsignedLong(key, 0);
    }

    /**
     * Returns the value associated with the given key, or defaultValue if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String
     * @param defaultValue Value to return if key does not exist
     * @return a long value
     */
    int64_t getLong(const char* key, const int64_t defaultValue) const {
        return getLong(String::valueOf(key), defaultValue);
    }
    int64_t getLong(const sp<String>& key, const int64_t defaultValue) const;
    uint64_t getUnsignedLong(const char* key, const uint64_t defaultValue) const {
        return getUnsignedLong(String::valueOf(key), defaultValue);
    }
    uint64_t getUnsignedLong(const sp<String>& key, const uint64_t defaultValue) const;

    /**
     * Returns the value associated with the given key, or null if no mapping of the desired type
     * exists for the given key or a null value is explicitly associated with the key.
     *
     * @param key a String, or null
     * @return an object value, or null
     */
    sp<Object> getObject(const char* key) const {
        return getObject(String::valueOf(key));
    }
    sp<Object> getObject(const sp<String>& key) const;

    /**
     * Returns the value associated with the given key, or defaultValue if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String, or null
     * @param defaultValue Value to return if key does not exist
     * @return an object value, or null
     */
    sp<Object> getObject(const char* key, const sp<Object>& defaultValue) const {
        return getObject(String::valueOf(key), defaultValue);
    }
    sp<Object> getObject(const sp<String>& key, const sp<Object>& defaultValue) const;

    /**
     * Returns the value associated with the given key, or (short) 0 if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String
     * @return a short value
     */
    int16_t getShort(const char* key) const {
        return getShort(String::valueOf(key), 0);
    }
    int16_t getShort(const sp<String>& key) const {
        return getShort(key, 0);
    }
    uint16_t getUnsignedShort(const char* key) const {
        return getUnsignedShort(String::valueOf(key), 0);
    }
    uint16_t getUnsignedShort(const sp<String>& key) const {
        return getUnsignedShort(key, 0);
    }

    /**
     * Returns the value associated with the given key, or defaultValue if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String
     * @param defaultValue Value to return if key does not exist
     * @return a short value
     */
    int16_t getShort(const char* key, const int16_t defaultValue) const {
        return getShort(String::valueOf(key), defaultValue);
    }
    int16_t getShort(const sp<String>& key, const int16_t defaultValue) const;
    uint16_t getUnsignedShort(const char* key, const uint16_t defaultValue) const {
        return getShort(String::valueOf(key), defaultValue);
    }
    uint16_t getUnsignedShort(const sp<String>& key, const uint16_t defaultValue) const;

    /**
     * Returns the value associated with the given key, or null if no mapping of the desired type
     * exists for the given key or a null value is explicitly associated with the key.
     *
     * @param key a String, or null
     * @return a String value, or null
     */
    sp<String> getString(const char* key) const {
        return getString(String::valueOf(key));
    }
    sp<String> getString(const sp<String>& key) const;

    /**
     * Returns the value associated with the given key, or defaultValue if no mapping of the desired
     * type exists for the given key.
     *
     * @param key a String, or null
     * @param defaultValue Value to return if key does not exist
     * @return a String value, or null
     */
    sp<String> getString(const char* key, const char* defaultValue) const {
        return getString(String::valueOf(key), defaultValue);
    }
    sp<String> getString(const sp<String>& key, const char* defaultValue) const;

    /**
     * Returns the value associated with the given key, or null if no mapping of the desired type
     * exists for the given key or a null value is explicitly associated with the key.
     *
     * @param key a String, or null
     * @return an ArrayList<String> value, or null
     */
    sp<ArrayList<sp<String>>> getStringArrayList(const char* key) const {
        return getStringArrayList(String::valueOf(key));
    }
    sp<ArrayList<sp<String>>> getStringArrayList(const sp<String>& key) const;

    /**
     * Retain only basic types within the Bundle.
     *
     * @hide
     */
    void retainBasicTypes();

private:
    sp<HashMap<sp<String>, sp<Variant>>> mMap;
};

} /* namespace mindroid */

#endif /* MINDROID_BUNDLE_H_ */
